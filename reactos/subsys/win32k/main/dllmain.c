/*
 *  ReactOS W32 Subsystem
 *  Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 ReactOS Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/* $Id: dllmain.c,v 1.76.12.4 2004/09/14 01:00:43 weiden Exp $
 *
 *  Entry Point for win32k.sys
 */
#include <w32k.h>

#define NDEBUG
#include <debug.h>

#ifdef __USE_W32API
typedef NTSTATUS (STDCALL *PW32_PROCESS_CALLBACK)(
   struct _EPROCESS *Process,
   BOOLEAN Create);

typedef NTSTATUS (STDCALL *PW32_THREAD_CALLBACK)(
   struct _ETHREAD *Thread,
   BOOLEAN Create);

VOID STDCALL
PsEstablishWin32Callouts(
   PW32_PROCESS_CALLBACK W32ProcessCallback,
   PW32_THREAD_CALLBACK W32ThreadCallback,
   PVOID Param3,
   PVOID Param4,
   ULONG W32ThreadSize,
   ULONG W32ProcessSize);
#endif

extern SSDT Win32kSSDT[];
extern SSPT Win32kSSPT[];
extern ULONG Win32kNumberOfSysCalls;

NTSTATUS STDCALL
Win32kProcessCallback (struct _EPROCESS *Process,
		     BOOLEAN Create)
{
  PW32PROCESS Win32Process;
  NTSTATUS Status;

  DPRINT("Win32kProcessCallback() called\n");

  Win32Process = Process->Win32Process;
  if (Create)
    {
      DPRINT("W32k: Create process\n");
      
      InitializeListHead(&Win32Process->ClassListHead);     

      InitializeListHead(&Win32Process->PrivateFontListHead);
      ExInitializeFastMutex(&Win32Process->PrivateFontListLock);
      
      InitializeListHead(&Win32Process->CursorIconListHead);
      ExInitializeFastMutex(&Win32Process->CursorIconListLock);

      Win32Process->KeyboardLayout = W32kGetDefaultKeyLayout();
      Win32Process->WindowStation = NULL;
      if (Process->Win32WindowStation != NULL)
	{
	  Status = 
	    IntValidateWindowStationHandle(Process->Win32WindowStation,
					   UserMode,
					   GENERIC_ALL,
					   &Win32Process->WindowStation);
	  if (!NT_SUCCESS(Status))
	    {
	      DPRINT1("Win32K: Failed to reference a window station for process.\n");
	    }
	}
      
      /* setup process flags */
      Win32Process->Flags = 0;
    }
  else
    {
      DPRINT("W32k: Destroy process, IRQ level: %lu\n", KeGetCurrentIrql ());

      CleanupForProcess(Process, Process->UniqueProcessId);

      IntGraphicsCheck(FALSE);
      
      /*
       * Deregister logon application automatically
       */
      if(LogonProcess == Win32Process)
      {
        LogonProcess = NULL;
      }
    }

  return STATUS_SUCCESS;
}


NTSTATUS STDCALL
Win32kThreadCallback (struct _ETHREAD *Thread,
		    BOOLEAN Create)
{
  struct _EPROCESS *Process;
  PW32THREAD Win32Thread;
  NTSTATUS Status;

  DPRINT("Win32kThreadCallback() called\n");

  Process = Thread->ThreadsProcess;
  Win32Thread = Thread->Win32Thread;
  if (Create)
    {
      DPRINT("W32k: Create thread\n");

      Win32Thread->IsExiting = FALSE;
      /* FIXME - destroy caret */
      Win32Thread->MessageQueue = MsqCreateMessageQueue(Thread);
      Win32Thread->KeyboardLayout = W32kGetDefaultKeyLayout();
      Win32Thread->MessagePumpHookValue = 0;
      InitializeListHead(&Win32Thread->WindowListHead);
      ExInitializeFastMutex(&Win32Thread->WindowListLock);
      InitializeListHead(&Win32Thread->W32CallbackListHead);
      ExInitializeFastMutex(&Win32Thread->W32CallbackListLock);

      /* By default threads get assigned their process's desktop. */
      Win32Thread->Desktop = NULL;
      Win32Thread->hDesktop = NULL;
      if (Process->Win32Desktop != NULL)
	{
	  Status = ObReferenceObjectByHandle(Process->Win32Desktop,
					     GENERIC_ALL,
					     ExDesktopObjectType,
					     UserMode,
					     (PVOID*)&Win32Thread->Desktop,
					     NULL);
	  if (!NT_SUCCESS(Status))
	    {
	      DPRINT1("Win32K: Failed to reference a desktop for thread.\n");
	    }
	  
	  Win32Thread->hDesktop = Process->Win32Desktop;
	}
    }
  else
    {
      DPRINT1("=== W32k: Destroy thread ===\n");

      Win32Thread->IsExiting = TRUE;
      #if 0
      HOOK_DestroyThreadHooks(Thread);
      #endif
      RemoveTimersThread(Thread->Cid.UniqueThread);
      DestroyThreadWindows(Thread);
      IntBlockInput(Win32Thread, FALSE);
      MsqDestroyMessageQueue(Win32Thread->MessageQueue);
      IntCleanupThreadCallbacks(Win32Thread);
    }

  return STATUS_SUCCESS;
}


/*
 * This definition doesn't work
 */
// BOOL STDCALL DllMain(VOID)
NTSTATUS STDCALL
DllMain (
  IN	PDRIVER_OBJECT	DriverObject,
  IN	PUNICODE_STRING	RegistryPath)
{
  NTSTATUS Status;
  BOOLEAN Result;

  /*
   * Register user mode call interface
   * (system service table index = 1)
   */
  Result = KeAddSystemServiceTable (Win32kSSDT,
				    NULL,
				    Win32kNumberOfSysCalls,
				    Win32kSSPT,
				    1);
  if (Result == FALSE)
    {
      DbgPrint("Adding system services failed!\n");
      return STATUS_UNSUCCESSFUL;
    }

  IntInitUserResourceLocks();

  /*
   * Register our per-process and per-thread structures.
   */
  PsEstablishWin32Callouts (Win32kProcessCallback,
			    Win32kThreadCallback,
			    0,
			    0,
			    sizeof(W32THREAD),
			    sizeof(W32PROCESS));
  
  Status = InitWindowStationImpl();
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to initialize window station implementation!\n");
    return STATUS_UNSUCCESSFUL;
  }
  
  Status = InitDesktopImpl();
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to initialize window station implementation!\n");
    return STATUS_UNSUCCESSFUL;
  }
  
  Status = InitTimerImpl();
  if (!NT_SUCCESS(Status))
  {
    DbgPrint("Failed to initialize timer implementation!\n");
    return STATUS_UNSUCCESSFUL;
  }
  
  Status = InitInputImpl();
  if (!NT_SUCCESS(Status))
    {
      DbgPrint("Failed to initialize input implementation.\n");
      return(Status);
    }

  Status = InitKeyboardImpl();
  if (!NT_SUCCESS(Status))
    {
      DbgPrint("Failed to initialize keyboard implementation.\n");
      return(Status);
    }

  Status = MsqInitializeImpl();
  if (!NT_SUCCESS(Status))
    {
      DbgPrint("Failed to initialize message queue implementation.\n");
      return(Status);
    }
  
  Status = InitGuiCheckImpl();
  if (!NT_SUCCESS(Status))
    {
      DbgPrint("Failed to initialize GUI check implementation.\n");
      return(Status);
    }
  
  InitGdiObjectHandleTable ();

  /* Initialize FreeType library */
  if (! InitFontSupport())
    {
      DPRINT1("Unable to initialize font support\n");
      return STATUS_UNSUCCESSFUL;
    }

  /* Create stock objects, ie. precreated objects commonly
     used by win32 applications */
  CreateStockObjects();
  CreateSysColorObjects();
  
  PREPARE_TESTS

  return STATUS_SUCCESS;
}


BOOLEAN STDCALL
Win32kInitialize (VOID)
{
  return TRUE;
}

NTSTATUS
IntConvertProcessToGUIProcess(PEPROCESS Process)
{
  /* FIXME - Convert process to GUI process! */
  DPRINT1("FIXME: Convert Process to GUI Process!!!!\n");
  return STATUS_UNSUCCESSFUL;
}

inline NTSTATUS
IntConvertThreadToGUIThread(PETHREAD Thread)
{
  NTSTATUS Status;
  
  /* FIXME - do this atomic!!! */
  
  if(Thread->Win32Thread != NULL)
  {
    return STATUS_SUCCESS;
  }
  
  /* FIXME - Convert thread to GUI thread! */
  Status = STATUS_UNSUCCESSFUL;
  DPRINT1("FIXME: Convert Thread to GUI Thread!!!!\n");
  
  if(NT_SUCCESS(Status) && Thread->ThreadsProcess->Win32Process == NULL)
  {
    /* We also need to convert the process */
    return IntConvertProcessToGUIProcess(Thread->ThreadsProcess);
  }
  
  return Status;
}

/* EOF */
