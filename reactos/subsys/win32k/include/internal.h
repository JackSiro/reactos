#ifndef _NTUSER_H
#define _NTUSER_H

/*
 * Forward declarations to win32k internal structures and types
 */
struct _ACCELERATOR_TABLE;
struct _THRDCARETINFO;
struct _CLASS_OBJECT;
struct _CURSOR_OBJECT;
struct _CURSORCLIP_INFO;
struct _SYSTEM_CURSORINFO;
struct tagDCE;
struct tagHOOK;
struct tagHOOKTABLE;
struct _HOT_KEY_ITEM;
struct _MENU_ITEM;
struct _MENU_OBJECT;
struct _USER_MESSAGE;
struct _USER_SENT_MESSAGE;
struct _USER_SENT_MESSAGE_NOTIFY;
struct _USER_MESSAGE_QUEUE;
struct _PROPERTY;
struct _WINDOW_SCROLLINFO;
struct _MSG_TIMER_ENTRY;
struct _INTERNALPOS;
struct _INTERNAL_WINDOWPOS;
struct _DOSENDMESSAGE;
struct tagMSGMEMORY;

/*
 * Pointer types
 */

typedef struct _ACCELERATOR_TABLE *PACCELERATOR_TABLE;
typedef struct _THRDCARETINFO *PTHRDCARETINFO;
typedef struct _CLASS_OBJECT *PCLASS_OBJECT;
typedef struct _CURSOR_OBJECT *PCURSOR_OBJECT;
typedef struct _CURSORCLIP_INFO *PCURSORCLIP_INFO;
typedef struct _SYSTEM_CURSORINFO *PSYSTEM_CURSORINFO;
typedef struct tagDCE *PDCE;
typedef struct tagHOOK *PHOOK;
typedef struct tagHOOKTABLE *PHOOKTABLE;
typedef struct _HOT_KEY_ITEM *PHOT_KEY_ITEM;
typedef struct _MENU_ITEM *PMENU_ITEM;
typedef struct _MENU_OBJECT *PMENU_OBJECT;
typedef struct _USER_MESSAGE *PUSER_MESSAGE;
typedef struct _USER_SENT_MESSAGE *PUSER_SENT_MESSAGE;
typedef struct _USER_SENT_MESSAGE_NOTIFY *PUSER_SENT_MESSAGE_NOTIFY;
typedef struct _USER_MESSAGE_QUEUE *PUSER_MESSAGE_QUEUE;
typedef struct _PROPERTY *PPROPERTY;
typedef struct _WINDOW_SCROLLINFO *PWINDOW_SCROLLINFO;
typedef struct _MSG_TIMER_ENTRY *PMSG_TIMER_ENTRY;
typedef struct _INTERNALPOS *PINTERNALPOS;
typedef struct _WINDOW_OBJECT *PWINDOW_OBJECT;
typedef struct _INTERNAL_WINDOWPOS *PINTERNAL_WINDOWPOS;
typedef struct _DOSENDMESSAGE *PDOSENDMESSAGE;
typedef struct tagMSGMEMORY *PMSGMEMORY;

/*
 * Other types
 */

typedef HANDLE HDCE;

/*
 * Include the ex.h here, it contains the definitions for
 * the WINSTATION_OBJECT and DESKTOP_OBJECT structures
 */

#include <internal/ex.h>

/*
 * win32k internal
 */

/* ACCELERATORS ***************************************************************/

typedef struct _ACCELERATOR_TABLE
{
  int Count;
  LPACCEL Table;
} ACCELERATOR_TABLE;

/* CARETS *********************************************************************/

#define IDCARETTIMER (0xffff)

/* a copy of this structure is in lib/user32/include/user32.h */
typedef struct _THRDCARETINFO
{
  PWINDOW_OBJECT Window;
  HBITMAP Bitmap;
  POINT Pos;
  SIZE Size;
  BYTE Visible;
  BYTE Showing;
} THRDCARETINFO;

/* WINDOW CLASSES *************************************************************/

#define IS_ATOM(x) \
  (((ULONG_PTR)(x) > 0x0) && ((ULONG_PTR)(x) < 0x10000))

#define ClassReferenceObject(ClassObj) \
  InterlockedIncrement((LONG*)&((ClassObj)->RefCount))

#define ClassDereferenceObject(ClassObj) \
  if(InterlockedDecrement((LONG*)&((ClassObj)->RefCount)) == 0) \
  { \
    ExFreePool(ClassObj); \
  }

typedef struct _CLASS_OBJECT
{
  LONG    RefCount;
  
  UINT    cbSize;
  UINT    style;
  WNDPROC lpfnWndProcA;
  WNDPROC lpfnWndProcW;
  int     cbClsExtra;
  int     cbWndExtra;
  HANDLE  hInstance;
  HICON   hIcon;
  HCURSOR hCursor;
  HBRUSH  hbrBackground;
  UNICODE_STRING lpszMenuName;
  RTL_ATOM Atom;
  HICON   hIconSm;
  BOOL Unicode;
  BOOL Global;
  LIST_ENTRY ListEntry;
  PBYTE   ExtraData;
  /* list of windows */
  LIST_ENTRY ClassWindowsListHead;
} CLASS_OBJECT;

PCLASS_OBJECT INTERNAL_CALL IntCreateClass(CONST WNDCLASSEXW *lpwcx,
                                      DWORD Flags,
                                      WNDPROC wpExtra,
                                      PUNICODE_STRING MenuName,
                                      RTL_ATOM Atom);
PCLASS_OBJECT INTERNAL_CALL IntRegisterClass(CONST WNDCLASSEXW *lpwcx, PUNICODE_STRING ClassName,
                                        PUNICODE_STRING MenuName, WNDPROC wpExtra, DWORD Flags);
ULONG         INTERNAL_CALL IntGetClassLong(PWINDOW_OBJECT WindowObject, ULONG Offset, BOOL Ansi);
ULONG         INTERNAL_CALL IntSetClassLong(PWINDOW_OBJECT WindowObject, ULONG Offset, LONG dwNewLong, BOOL Ansi);
BOOL          INTERNAL_CALL IntGetClassName(PWINDOW_OBJECT WindowObject, PUNICODE_STRING ClassName, ULONG nMaxCount);
BOOL          INTERNAL_CALL IntReferenceClassByNameOrAtom(PCLASS_OBJECT *Class, PUNICODE_STRING ClassNameOrAtom, HINSTANCE hInstance);
BOOL          INTERNAL_CALL IntReferenceClassByName(PCLASS_OBJECT *Class, PUNICODE_STRING ClassName, HINSTANCE hInstance);
BOOL          INTERNAL_CALL IntReferenceClassByAtom(PCLASS_OBJECT* Class, RTL_ATOM Atom, HINSTANCE hInstance);
BOOL          INTERNAL_CALL IntGetClassInfo(HINSTANCE hInstance, PUNICODE_STRING ClassName, LPWNDCLASSEXW lpWndClassEx, BOOL Ansi);

/* CURSORS AND ICONS **********************************************************/

#define IntGetSysCursorInfo(WinStaObj) \
  (PSYSTEM_CURSORINFO)((WinStaObj)->SystemCursor)

#define IntLockProcessCursorIcons(W32Process) \
  ExAcquireFastMutex(&W32Process->CursorIconListLock)

#define IntUnLockProcessCursorIcons(W32Process) \
  ExReleaseFastMutex(&W32Process->CursorIconListLock)

typedef struct _CURSOR_OBJECT
{
  HANDLE Handle;
  LIST_ENTRY ListEntry;
  PW32PROCESS Process;
  HMODULE hModule;
  HRSRC hRsrc;
  HRSRC hGroupRsrc;
  SIZE Size;
  BYTE Shadow;
  ICONINFO IconInfo;
} CURSOR_OBJECT;

typedef struct _CURSORCLIP_INFO
{
  BOOL IsClipped;
  UINT Left;
  UINT Top;
  UINT Right;
  UINT Bottom;
} CURSORCLIP_INFO;

typedef struct _SYSTEM_CURSORINFO
{
  BOOL Enabled;
  BOOL SwapButtons;
  UINT ButtonsDown;
  LONG x, y;
  BOOL SafetySwitch;
  UINT SafetyRemoveCount;
  LONG PointerRectLeft;
  LONG PointerRectTop;
  LONG PointerRectRight;
  LONG PointerRectBottom;
  FAST_MUTEX CursorMutex;
  CURSORCLIP_INFO CursorClipInfo;
  PCURSOR_OBJECT CurrentCursorObject;
  BYTE ShowingCursor;
  UINT DblClickSpeed;
  UINT DblClickWidth;
  UINT DblClickHeight;
  DWORD LastBtnDown;
  LONG LastBtnDownX;
  LONG LastBtnDownY;
  HANDLE LastClkWnd;
} SYSTEM_CURSORINFO;

PCURSOR_OBJECT INTERNAL_CALL IntSetCursor(PCURSOR_OBJECT NewCursor, BOOL ForceChange);
PCURSOR_OBJECT INTERNAL_CALL IntCreateCursorObject(HANDLE *Handle);
VOID           INTERNAL_CALL IntCleanupCurIcons(struct _EPROCESS *Process, PW32PROCESS Win32Process);
VOID           INTERNAL_CALL IntGetCursorIconInfo(PCURSOR_OBJECT Cursor, PICONINFO IconInfo);
BOOL           INTERNAL_CALL IntGetCursorIconSize(PCURSOR_OBJECT Cursor, BOOL *fIcon, SIZE *Size);
BOOL           INTERNAL_CALL IntGetCursorInfo(PCURSORINFO pci);
BOOL           INTERNAL_CALL IntDestroyCursorObject(PCURSOR_OBJECT Object, BOOL RemoveFromProcess);
PCURSOR_OBJECT INTERNAL_CALL IntFindExistingCursorObject(HMODULE hModule, HRSRC hRsrc, LONG cx, LONG cy);
VOID           INTERNAL_CALL IntGetClipCursor(RECT *lpRect);
VOID           INTERNAL_CALL IntSetCursorIconData(PCURSOR_OBJECT Cursor, BOOL *fIcon, POINT *Hotspot,
                                                  HMODULE hModule, HRSRC hRsrc, HRSRC hGroupRsrc);
BOOL           INTERNAL_CALL IntDrawIconEx(HDC hdc, int xLeft, int yTop, PCURSOR_OBJECT Cursor, int cxWidth, int cyWidth,
                                           UINT istepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT diFlags);

/* DCES ***********************************************************************/

/* DC hook codes */
#define DCHC_INVALIDVISRGN      0x0001
#define DCHC_DELETEDC           0x0002

#define DCHF_INVALIDATEVISRGN   0x0001
#define DCHF_VALIDATEVISRGN     0x0002

#define  DCEOBJ_AllocDCE()  \
  ((HDCE) GDIOBJ_AllocObj (GDI_OBJECT_TYPE_DCE))
#define  DCEOBJ_FreeDCE(hDCE)  GDIOBJ_FreeObj((HGDIOBJ)hDCE, GDI_OBJECT_TYPE_DCE)
#define  DCEOBJ_LockDCE(hDCE) ((PDCE)GDIOBJ_LockObj((HGDIOBJ)hDCE, GDI_OBJECT_TYPE_DCE))
#define  DCEOBJ_UnlockDCE(DCE) GDIOBJ_UnlockObj((PGDIOBJ)DCE)

typedef enum
{
    DCE_CACHE_DC,   /* This is a cached DC (allocated by USER) */
    DCE_CLASS_DC,   /* This is a class DC (style CS_CLASSDC) */
    DCE_WINDOW_DC   /* This is a window DC (style CS_OWNDC) */
} DCE_TYPE, *PDCE_TYPE;

typedef struct tagDCE
{
    struct tagDCE  *next;
    HANDLE         Handle;
    HDC            hDC;
    PWINDOW_OBJECT CurrentWindow;
    PWINDOW_OBJECT DCWindow;
    HRGN           hClipRgn;
    DCE_TYPE       type;
    DWORD          DCXFlags;
} DCE;  /* PDCE already declared at top of file */

PDCE           INTERNAL_CALL DceAllocDCE(PWINDOW_OBJECT Window, DCE_TYPE Type);
PDCE           INTERNAL_CALL DCE_FreeDCE(PDCE dce);
VOID           INTERNAL_CALL DCE_FreeWindowDCE(PWINDOW_OBJECT);
BOOL           INTERNAL_CALL DCE_Cleanup(PDCE pDce);
HRGN           INTERNAL_CALL DceGetVisRgn(PWINDOW_OBJECT Window, ULONG Flags, PWINDOW_OBJECT Child, ULONG CFlags);
INT            INTERNAL_CALL DCE_ExcludeRgn(HDC, PWINDOW_OBJECT, HRGN);
BOOL           INTERNAL_CALL DCE_InvalidateDCE(PWINDOW_OBJECT, const PRECTL);
PWINDOW_OBJECT INTERNAL_CALL IntWindowFromDC(HDC hDc);
PDCE           INTERNAL_CALL DceFreeDCE(PDCE dce);
void           INTERNAL_CALL DceFreeWindowDCE(PWINDOW_OBJECT Window);
void           INTERNAL_CALL DceEmptyCache(void);
VOID           INTERNAL_CALL DceResetActiveDCEs(PWINDOW_OBJECT Window, int DeltaX, int DeltaY);
HDC            INTERNAL_CALL IntGetDCEx(PWINDOW_OBJECT Window, HRGN ClipRegion, ULONG Flags);
INT            INTERNAL_CALL IntReleaseDC(PWINDOW_OBJECT Window, HDC hDc);

/* DESKTOPS *******************************************************************/

extern PCLASS_OBJECT DesktopWindowClass;
extern HDC ScreenDeviceContext;

#define IntIsActiveDesktop(Desktop) \
  ((Desktop)->WindowStation->ActiveDesktop == (Desktop))

NTSTATUS            INTERNAL_CALL InitDesktopImpl(VOID);
NTSTATUS            INTERNAL_CALL CleanupDesktopImpl(VOID);
VOID                INTERNAL_CALL IntGetDesktopWorkArea(PDESKTOP_OBJECT Desktop, PRECT Rect);
HDC                 INTERNAL_CALL IntGetScreenDC(VOID);
PWINDOW_OBJECT      INTERNAL_CALL IntGetDesktopWindow (VOID);
PWINDOW_OBJECT      INTERNAL_CALL IntGetCurrentThreadDesktopWindow(VOID);
PUSER_MESSAGE_QUEUE INTERNAL_CALL IntGetActiveMessageQueue(VOID);
PUSER_MESSAGE_QUEUE INTERNAL_CALL IntSetActiveMessageQueue(PUSER_MESSAGE_QUEUE NewQueue);
PDESKTOP_OBJECT     INTERNAL_CALL IntGetInputDesktop(VOID);
NTSTATUS            INTERNAL_CALL IntShowDesktop(PDESKTOP_OBJECT Desktop, ULONG Width, ULONG Height);
NTSTATUS            INTERNAL_CALL IntHideDesktop(PDESKTOP_OBJECT Desktop);
HDESK               INTERNAL_CALL IntGetDesktopObjectHandle(PDESKTOP_OBJECT DesktopObject);
NTSTATUS            INTERNAL_CALL IntValidateDesktopHandle(HDESK Desktop, KPROCESSOR_MODE AccessMode, ACCESS_MASK DesiredAccess, PDESKTOP_OBJECT *Object);
BOOL                INTERNAL_CALL IntPaintDesktop(HDC hDC);

/* FOCUS **********************************************************************/

PWINDOW_OBJECT INTERNAL_CALL IntGetCaptureWindow(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntGetFocusWindow(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntGetThreadFocusWindow(VOID);
BOOL           INTERNAL_CALL IntMouseActivateWindow(PWINDOW_OBJECT DesktopWindow, PWINDOW_OBJECT Window);
BOOL           INTERNAL_CALL IntSetForegroundWindow(PWINDOW_OBJECT Window);
PWINDOW_OBJECT INTERNAL_CALL IntSetActiveWindow(PWINDOW_OBJECT Window);
PWINDOW_OBJECT INTERNAL_CALL IntGetForegroundWindow(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntGetActiveWindow(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntSetFocus(PWINDOW_OBJECT Window);
PWINDOW_OBJECT INTERNAL_CALL IntSetCapture(PWINDOW_OBJECT Window);

/* HOOKS **********************************************************************/

#define HOOK_THREAD_REFERENCED	(0x1)
#define NB_HOOKS (WH_MAXHOOK-WH_MINHOOK+1)

typedef struct tagHOOK
{
  LIST_ENTRY Chain;          /* Hook chain entry */
  HHOOK      Self;           /* user handle for this hook */
  PETHREAD   Thread;         /* Thread owning the hook */
  int        HookId;         /* Hook table index */
  HOOKPROC   Proc;           /* Hook function */
  BOOLEAN    Ansi;           /* Is it an Ansi hook? */
  ULONG      Flags;          /* Some internal flags */
  UNICODE_STRING ModuleName; /* Module name for global hooks */
} HOOK;

typedef struct tagHOOKTABLE
{
  FAST_MUTEX Lock;
  LIST_ENTRY Hooks[NB_HOOKS];  /* array of hook chains */
  UINT       Counts[NB_HOOKS]; /* use counts for each hook chain */
} HOOKTABLE;

/* HOTKEYS ********************************************************************/

typedef struct _HOT_KEY_ITEM
{
  LIST_ENTRY ListEntry;
  struct _ETHREAD *Thread;
  PWINDOW_OBJECT Window;
  int id;
  UINT fsModifiers;
  UINT vk;
} HOT_KEY_ITEM;

/* INPUT **********************************************************************/

#define ThreadHasInputAccess(W32Thread) \
  (TRUE)

NTSTATUS            INTERNAL_CALL InitInputImpl(VOID);
NTSTATUS            INTERNAL_CALL InitKeyboardImpl(VOID);
PUSER_MESSAGE_QUEUE INTERNAL_CALL W32kGetPrimitiveMessageQueue(VOID);
PKBDTABLES          INTERNAL_CALL W32kGetDefaultKeyLayout(VOID);
VOID                INTERNAL_CALL W32kKeyProcessMessage(LPMSG Msg, PKBDTABLES KeyLayout);
BOOL                INTERNAL_CALL IntBlockInput(PW32THREAD W32Thread, BOOL BlockIt);
BOOL                INTERNAL_CALL IntMouseInput(MOUSEINPUT *mi);
BOOL                INTERNAL_CALL IntKeyboardInput(KEYBDINPUT *ki);
UINT                INTERNAL_CALL IntSendInput(UINT nInputs, LPINPUT pInput, INT cbSize);

/* MENUS **********************************************************************/

#define IS_ATOM(x) \
  (((ULONG_PTR)(x) > 0x0) && ((ULONG_PTR)(x) < 0x10000))
  
#define MENU_ITEM_TYPE(flags) \
  ((flags) & (MF_STRING | MF_BITMAP | MF_OWNERDRAW | MF_SEPARATOR))
  
#ifndef MF_END
#define MF_END             (0x0080)
#endif

typedef struct _MENU_ITEM
{
  struct _MENU_ITEM *Next;
  UINT fType;
  UINT fState;
  UINT wID;
  HMENU hSubMenu;
  HBITMAP hbmpChecked;
  HBITMAP hbmpUnchecked;
  ULONG_PTR dwItemData;
  UNICODE_STRING Text;
  HBITMAP hbmpItem;
  RECT Rect;
  UINT XTab;
} MENU_ITEM;

typedef struct _MENU_OBJECT
{
  PW32PROCESS W32Process;
  LIST_ENTRY ListEntry;
  FAST_MUTEX MenuItemsLock;
  PMENU_ITEM MenuItemList;
  ROSMENUINFO MenuInfo;
  BOOL RtoL;
} MENU_OBJECT;

/* MESSAGE QUEUE **************************************************************/

#define MSQ_HUNG        5000

#define IntLockMessageQueue(MsgQueue) \
  ExAcquireFastMutex(&(MsgQueue)->Lock)

#define IntUnLockMessageQueue(MsgQueue) \
  ExReleaseFastMutex(&(MsgQueue)->Lock)

#define IntLockHardwareMessageQueue(MsgQueue) \
  KeWaitForMutexObject(&(MsgQueue)->HardwareLock, UserRequest, KernelMode, FALSE, NULL)

#define IntUnLockHardwareMessageQueue(MsgQueue) \
  KeReleaseMutex(&(MsgQueue)->HardwareLock, FALSE)

#define IntReferenceMessageQueue(MsgQueue) \
  InterlockedIncrement(&(MsgQueue)->References)

#define IntDereferenceMessageQueue(MsgQueue) \
  do { \
    if(InterlockedDecrement(&(MsgQueue)->References) == 0) \
    { \
      DPRINT1("Free message queue 0x%x\n", (MsgQueue)); \
      ExFreePool((MsgQueue)); \
    } \
  } while(0)

/* check the queue status */
#define MsqIsSignaled(MsgQueue) \
  (((MsgQueue)->WakeBits & (MsgQueue)->WakeMask) || ((MsgQueue)->ChangedBits & (MsgQueue)->ChangedMask))

#define IS_BTN_MESSAGE(message,code) \
  ((message) == WM_LBUTTON##code || \
   (message) == WM_MBUTTON##code || \
   (message) == WM_RBUTTON##code || \
   (message) == WM_XBUTTON##code || \
   (message) == WM_NCLBUTTON##code || \
   (message) == WM_NCMBUTTON##code || \
   (message) == WM_NCRBUTTON##code || \
   (message) == WM_NCXBUTTON##code )

#define MAKE_LONG(x, y) ((((y) & 0xFFFF) << 16) | ((x) & 0xFFFF))

#define MsgCopyKMsgToMsg(msg, kmsg) \
 (msg)->hwnd = ((kmsg)->Window ? (kmsg)->Window->Handle : NULL); \
 (msg)->message = (kmsg)->message; \
 (msg)->wParam = (kmsg)->wParam; \
 (msg)->lParam = (kmsg)->lParam; \
 (msg)->time = (kmsg)->time; \
 (msg)->pt = (kmsg)->pt

#define MsgCopyMsgToKMsg(kmsg, msg, MsgWindow) \
 (kmsg)->Window = (MsgWindow); \
 (kmsg)->message = (msg)->message; \
 (kmsg)->wParam = (msg)->wParam; \
 (kmsg)->lParam = (msg)->lParam; \
 (kmsg)->time = (msg)->time; \
 (kmsg)->pt = (msg)->pt

typedef struct _KMSG
{
  PWINDOW_OBJECT Window;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD time;
  POINT pt;
} KMSG, *PKMSG;

typedef struct _USER_MESSAGE
{
  LIST_ENTRY ListEntry;
  BOOLEAN FreeLParam;
  KMSG Msg;
} USER_MESSAGE;

typedef struct _USER_SENT_MESSAGE
{
  LIST_ENTRY ListEntry;
  KMSG Msg;
  PKEVENT volatile CompletionEvent;
  LRESULT volatile * volatile Result;
  struct _USER_MESSAGE_QUEUE* SenderQueue;
  SENDASYNCPROC CompletionCallback;
  ULONG_PTR CompletionCallbackContext;
  /* entry in the dispatching list of the sender's message queue */
  LIST_ENTRY DispatchingListEntry;
} USER_SENT_MESSAGE;

typedef struct _USER_SENT_MESSAGE_NOTIFY
{
  SENDASYNCPROC CompletionCallback;
  ULONG_PTR CompletionCallbackContext;
  LRESULT Result;
  PWINDOW_OBJECT Window;
  UINT Msg;
  LIST_ENTRY ListEntry;
} USER_SENT_MESSAGE_NOTIFY;

typedef struct _USER_MESSAGE_QUEUE
{
  /* Reference counter, only access this variable with interlocked functions! */
  LONG References;
  
  /* Owner of the message queue */
  struct _ETHREAD *Thread;
  /* Queue of messages sent to the queue. */
  LIST_ENTRY SentMessagesListHead;
  /* Queue of messages posted to the queue. */
  LIST_ENTRY PostedMessagesListHead;
  /* Queue of sent-message notifies for the queue. */
  LIST_ENTRY NotifyMessagesListHead;
  /* Queue for hardware messages for the queue. */
  LIST_ENTRY HardwareMessagesListHead;
  /* Lock for the hardware message list. */
  KMUTEX HardwareLock;
  /* Lock for the queue. */
  FAST_MUTEX Lock;
  /* Pointer to the current WM_MOUSEMOVE message */
  PUSER_MESSAGE MouseMoveMsg;
  /* True if a WM_QUIT message is pending. */
  BOOLEAN QuitPosted;
  /* The quit exit code. */
  ULONG QuitExitCode;
  /* Set if there are new messages in any of the queues. */
  KEVENT NewMessages;  
  /* Last time PeekMessage() was called. */
  ULONG LastMsgRead;
  /* True if a window needs painting. */
  BOOLEAN PaintPosted;
  /* Count of paints pending. */
  ULONG PaintCount;
  /* Current window with focus (ie. receives keyboard input) for this queue. */
  PWINDOW_OBJECT FocusWindow;
  /* Current active window for this queue. */
  PWINDOW_OBJECT ActiveWindow;
  /* Current capture window for this queue. */
  PWINDOW_OBJECT CaptureWindow;
  /* Current move/size window for this queue */
  PWINDOW_OBJECT MoveSize;
  /* Current menu owner window for this queue */
  PWINDOW_OBJECT MenuOwner;
  /* Identifes the menu state */
  BYTE MenuState;
  /* Caret information for this queue */
  PTHRDCARETINFO CaretInfo;
  
  /* Window hooks */
  PHOOKTABLE Hooks;

  /* queue state tracking */
  WORD WakeBits;
  WORD WakeMask;
  WORD ChangedBits;
  WORD ChangedMask;
  
  /* extra message information */
  LPARAM ExtraInfo;

  /* messages that are currently dispatched by other threads */
  LIST_ENTRY DispatchingMessagesHead;
  /* messages that are currently dispatched by this message queue, required for cleanup */
  LIST_ENTRY LocalDispatchingMessagesHead;
} USER_MESSAGE_QUEUE;

typedef struct _DOSENDMESSAGE
{
  UINT uFlags;
  UINT uTimeout;
  ULONG_PTR Result;
} DOSENDMESSAGE;

typedef struct tagMSGMEMORY
{
  UINT Message;
  UINT Size;
  INT Flags;
} MSGMEMORY;

BOOL                INTERNAL_CALL MsqIsHung(PUSER_MESSAGE_QUEUE MessageQueue);
NTSTATUS            INTERNAL_CALL MsqSendMessage(PUSER_MESSAGE_QUEUE MessageQueue,
                                                 PWINDOW_OBJECT Window, UINT Msg, WPARAM wParam, LPARAM lParam,
                                                 UINT uTimeout, BOOL Block, ULONG_PTR *uResult);
PUSER_MESSAGE       INTERNAL_CALL MsqCreateMessage(PKMSG Msg, BOOLEAN FreeLParam);
VOID                INTERNAL_CALL MsqDestroyMessage(PUSER_MESSAGE Message);
VOID                INTERNAL_CALL MsqPostMessage(PUSER_MESSAGE_QUEUE MessageQueue, PKMSG Msg, BOOLEAN FreeLParam);
VOID                INTERNAL_CALL MsqPostQuitMessage(PUSER_MESSAGE_QUEUE MessageQueue, ULONG ExitCode);
BOOLEAN             INTERNAL_CALL MsqFindMessage(IN PUSER_MESSAGE_QUEUE MessageQueue,
                                                 IN BOOLEAN Hardware,
                                                 IN BOOLEAN Remove,
                                                 IN PWINDOW_OBJECT FilterWindow,
                                                 IN UINT MsgFilterLow,
                                                 IN UINT MsgFilterHigh,
                                                 OUT PUSER_MESSAGE* Message);
VOID                INTERNAL_CALL MsqInitializeMessageQueue(struct _ETHREAD *Thread, PUSER_MESSAGE_QUEUE MessageQueue);
VOID                INTERNAL_CALL MsqCleanupMessageQueue(PUSER_MESSAGE_QUEUE MessageQueue);
PUSER_MESSAGE_QUEUE INTERNAL_CALL MsqCreateMessageQueue(struct _ETHREAD *Thread);
VOID                INTERNAL_CALL MsqDestroyMessageQueue(PUSER_MESSAGE_QUEUE MessageQueue);
PUSER_MESSAGE_QUEUE INTERNAL_CALL MsqGetHardwareMessageQueue(VOID);
NTSTATUS            INTERNAL_CALL MsqWaitForNewMessage(PUSER_MESSAGE_QUEUE MessageQueue);
NTSTATUS            INTERNAL_CALL MsqInitializeImpl(VOID);
BOOLEAN             INTERNAL_CALL MsqDispatchOneSentMessage(PUSER_MESSAGE_QUEUE MessageQueue);
NTSTATUS            INTERNAL_CALL MsqWaitForNewMessages(PUSER_MESSAGE_QUEUE MessageQueue);
VOID                INTERNAL_CALL MsqSendNotifyMessage(PUSER_MESSAGE_QUEUE MessageQueue, PUSER_SENT_MESSAGE_NOTIFY NotifyMessage);
VOID                INTERNAL_CALL MsqIncPaintCountQueue(PUSER_MESSAGE_QUEUE Queue);
VOID                INTERNAL_CALL MsqDecPaintCountQueue(PUSER_MESSAGE_QUEUE Queue);
NTSTATUS            INTERNAL_CALL CopyMsgToKernelMem(PKMSG KernelModeMsg, MSG *UserModeMsg, PMSGMEMORY MsgMemoryEntry, PWINDOW_OBJECT MsgWindow);
NTSTATUS            INTERNAL_CALL CopyMsgToUserMem(MSG *UserModeMsg, PKMSG KernelModeMsg);
LRESULT             INTERNAL_CALL IntSendMessage(PWINDOW_OBJECT Window, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT             INTERNAL_CALL IntSendMessageTimeout(PWINDOW_OBJECT Window, UINT Msg, WPARAM wParam, LPARAM lParam,
                                                        UINT uFlags, UINT uTimeout, ULONG_PTR *uResult);
BOOL                INTERNAL_CALL IntPostThreadMessage(PW32THREAD W32Thread, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL                INTERNAL_CALL IntPostMessage(PWINDOW_OBJECT Window, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL                INTERNAL_CALL IntWaitMessage(PWINDOW_OBJECT Window, UINT MsgFilterMin, UINT MsgFilterMax);
BOOL                INTERNAL_CALL IntTranslateKbdMessage(PKMSG lpMsg, HKL dwhkl);
inline VOID         INTERNAL_CALL MsqSetQueueBits(PUSER_MESSAGE_QUEUE queue, WORD bits);
inline VOID         INTERNAL_CALL MsqClearQueueBits(PUSER_MESSAGE_QUEUE queue, WORD bits);
BOOL                INTERNAL_CALL IntInitMessagePumpHook(VOID);
BOOL                INTERNAL_CALL IntUninitMessagePumpHook(VOID);
PHOOKTABLE          INTERNAL_CALL MsqGetHooks(PUSER_MESSAGE_QUEUE Queue);
VOID                INTERNAL_CALL MsqSetHooks(PUSER_MESSAGE_QUEUE Queue, PHOOKTABLE Hooks);
LPARAM              INTERNAL_CALL MsqSetMessageExtraInfo(LPARAM lParam);
LPARAM              INTERNAL_CALL MsqGetMessageExtraInfo(VOID);

/* MESSAGES *******************************************************************/

BOOL       INTERNAL_CALL IntPeekMessage(PUSER_MESSAGE Msg, PWINDOW_OBJECT FilterWindow,
                                        UINT MsgFilterMin, UINT MsgFilterMax, UINT RemoveMsg);
BOOL       INTERNAL_CALL IntTranslateMouseMessage(PUSER_MESSAGE_QUEUE ThreadQueue, PKMSG Msg, USHORT *HitTest, BOOL Remove);
PMSGMEMORY INTERNAL_CALL FindMsgMemory(UINT Msg);
UINT       INTERNAL_CALL MsgMemorySize(PMSGMEMORY MsgMemoryEntry, WPARAM wParam, LPARAM lParam);
BOOL       INTERNAL_CALL IntGetMessage(PUSER_MESSAGE Msg, PWINDOW_OBJECT FilterWindow,
                                       UINT MsgFilterMin, UINT MsgFilterMax, BOOL *GotMessage);
LRESULT    INTERNAL_CALL IntDispatchMessage(PNTUSERDISPATCHMESSAGEINFO MsgInfo, PWINDOW_OBJECT Window);

/* KEYBOARD *******************************************************************/

VOID           INTERNAL_CALL MsqPostKeyboardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID           INTERNAL_CALL MsqPostHotKeyMessage(PVOID Thread, PWINDOW_OBJECT Window, WPARAM wParam, LPARAM lParam);
VOID           INTERNAL_CALL MsqInsertSystemMessage(PKMSG Msg);
BOOL           INTERNAL_CALL MsqIsDblClk(PKMSG Msg, BOOL Remove);
PWINDOW_OBJECT INTERNAL_CALL MsqSetStateWindow(PUSER_MESSAGE_QUEUE MessageQueue, ULONG Type, PWINDOW_OBJECT Window);

/* PAINTING *******************************************************************/

#define DCX_DCEEMPTY		0x00000800
#define DCX_DCEBUSY		0x00001000
#define DCX_DCEDIRTY		0x00002000
#define DCX_USESTYLE		0x00010000
#define DCX_WINDOWPAINT		0x00020000
#define DCX_KEEPCLIPRGN		0x00040000
#define DCX_NOCLIPCHILDREN      0x00080000

#if 0
#define IntLockWindowUpdate(Window) \
  ExAcquireFastMutex(&Window->UpdateLock)

#define IntUnLockWindowUpdate(Window) \
  ExReleaseFastMutex(&Window->UpdateLock)
#else
#define IntLockWindowUpdate(Window)
#define IntUnLockWindowUpdate(Window)
#endif

#define IntInvalidateRect(WndObj, Rect, Erase) \
   IntRedrawWindow((WndObj), (LPRECT)(Rect), 0, RDW_INVALIDATE | ((Erase) ? RDW_ERASE : 0))

#define IntInvalidateRgn(WndObj, Rgn, Erase) \
   IntRedrawWindow((WndObj), NULL, (Rgn), RDW_INVALIDATE | ((Erase) ? RDW_ERASE : 0))

#define IntValidateRect(WndObj, Rect) \
   IntRedrawWindow((WndObj), (Rect), NULL, RDW_VALIDATE | RDW_NOCHILDREN)

#define IntValidateRgn(WndObj, hRgn) \
   IntRedrawWindow((WndObj), NULL, (hRgn), RDW_VALIDATE | RDW_NOCHILDREN)

#define IntUpdateWindow(WndObj) \
   IntRedrawWindow((WndObj), NULL, 0, RDW_UPDATENOW | RDW_ALLCHILDREN)

#define IntIsWindowDirty(Window) \
  ((Window->Style & WS_VISIBLE) && \
      ((Window->UpdateRegion != NULL) || \
       (Window->Flags & WINDOWOBJECT_NEED_INTERNALPAINT) || \
       (Window->Flags & WINDOWOBJECT_NEED_NCPAINT)))

VOID INTERNAL_CALL IntValidateParent(PWINDOW_OBJECT Child, HRGN ValidRegion);
BOOL INTERNAL_CALL IntRedrawWindow(PWINDOW_OBJECT Window, LPRECT UpdateRect, HRGN UpdateRgn, UINT Flags);
BOOL INTERNAL_CALL IntGetPaintMessage(PWINDOW_OBJECT Window, UINT MsgFilterMin, UINT MsgFilterMax,
                                      PW32THREAD Thread, PKMSG Message, BOOL Remove);
HDC  INTERNAL_CALL IntBeginPaint(PWINDOW_OBJECT Window, PAINTSTRUCT* lPs);
BOOL INTERNAL_CALL IntEndPaint(PWINDOW_OBJECT Window, CONST PAINTSTRUCT* lPs);
BOOL INTERNAL_CALL IntGetUpdateRect(PWINDOW_OBJECT Window, LPRECT Rect, BOOL Erase);
INT  INTERNAL_CALL IntGetUpdateRgn(PWINDOW_OBJECT Window, HRGN hRgn, BOOL bErase);

/* WINDOW PROPERTIES **********************************************************/

typedef struct _PROPERTY
{
  LIST_ENTRY PropListEntry;
  HANDLE Data;
  ATOM Atom;
} PROPERTY;

BOOL      INTERNAL_CALL IntSetProp(PWINDOW_OBJECT WindowObject, ATOM Atom, HANDLE Data);
BOOL      INTERNAL_CALL IntRemoveProp(PWINDOW_OBJECT WindowObject, ATOM Atom, HANDLE *Data);
PPROPERTY INTERNAL_CALL IntGetProp(PWINDOW_OBJECT WindowObject, ATOM Atom);

/* SCROLL BARS ****************************************************************/

#define IntGetScrollbarInfoFromWindow(Window, i) \
  ((PSCROLLBARINFO)(&((Window)->Scroll + i)->ScrollBarInfo))

#define IntGetScrollInfoFromWindow(Window, i) \
  ((LPSCROLLINFO)(&((Window)->Scroll + i)->ScrollInfo))

#define SBOBJ_TO_SBID(Obj)	((Obj) - OBJID_HSCROLL)
#define SBID_IS_VALID(id)	(id == SB_HORZ || id == SB_VERT || id == SB_CTL)

typedef struct _WINDOW_SCROLLINFO
{
  SCROLLBARINFO ScrollBarInfo;
  SCROLLINFO ScrollInfo;
} WINDOW_SCROLLINFO;

BOOL  INTERNAL_CALL IntCreateScrollBars(PWINDOW_OBJECT Window);
BOOL  INTERNAL_CALL IntDestroyScrollBars(PWINDOW_OBJECT Window);
BOOL  INTERNAL_CALL IntGetScrollBarInfo(PWINDOW_OBJECT Window, LONG idObject, PSCROLLBARINFO psbi);
BOOL  INTERNAL_CALL IntEnableScrollBar(BOOL Horz, PSCROLLBARINFO Info, UINT wArrows);
BOOL  INTERNAL_CALL IntEnableWindowScrollBar(PWINDOW_OBJECT Window, UINT wSBflags, UINT wArrows);
BOOL  INTERNAL_CALL IntSetScrollBarInfo(PWINDOW_OBJECT Window, LONG idObject, SETSCROLLBARINFO *info);
DWORD INTERNAL_CALL IntShowScrollBar(PWINDOW_OBJECT Window, int wBar, DWORD bShow);
BOOL  INTERNAL_CALL IntGetScrollBarRect (PWINDOW_OBJECT Window, INT nBar, PRECT lprect);
DWORD INTERNAL_CALL IntSetScrollInfo(PWINDOW_OBJECT Window, INT nBar, LPCSCROLLINFO lpsi, BOOL bRedraw);
BOOL  INTERNAL_CALL IntGetScrollInfo(PWINDOW_OBJECT Window, INT nBar, LPSCROLLINFO lpsi);

/* TIMERS *********************************************************************/

typedef struct _MSG_TIMER_ENTRY{
   LIST_ENTRY     ListEntry;
   LARGE_INTEGER  Timeout;
   HANDLE         ThreadID;
   UINT           Period;
   KMSG           Msg;
} MSG_TIMER_ENTRY;

NTSTATUS         INTERNAL_CALL InitTimerImpl(VOID);
VOID             INTERNAL_CALL RemoveTimersThread(HANDLE ThreadID);
VOID             INTERNAL_CALL RemoveTimersWindow(PWINDOW_OBJECT Window);
PMSG_TIMER_ENTRY INTERNAL_CALL IntRemoveTimer(PWINDOW_OBJECT Window, UINT_PTR IDEvent, HANDLE ThreadID, BOOL SysTimer);
UINT_PTR         INTERNAL_CALL IntSetTimer(PWINDOW_OBJECT WindowObject, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc, BOOL SystemTimer);

/* VISIBLE REGION *************************************************************/

HRGN INTERNAL_CALL VIS_ComputeVisibleRegion(PWINDOW_OBJECT Window, BOOLEAN ClientArea,
                                            BOOLEAN ClipChildren, BOOLEAN ClipSiblings);
VOID INTERNAL_CALL VIS_WindowLayoutChanged(PWINDOW_OBJECT Window, HRGN UncoveredRgn);

/* WINDOWS ********************************************************************/

/* same as HWND_BOTTOM */
#define WINDOW_BOTTOM	((PWINDOW_OBJECT)1)
/* same as HWND_NOTOPMOST */
#define WINDOW_NOTOPMOST	((PWINDOW_OBJECT)-2)
/* same as HWND_TOP */
#define WINDOW_TOP	((PWINDOW_OBJECT)0)
/* same as HWND_TOPMOST */
#define WINDOW_TOPMOST	((PWINDOW_OBJECT)(-1))
/* same as HWND_MESSAGE */
#define WINDOW_MESSAGE	((PWINDOW_OBJECT)-3)

/* Window flags. */
#define WINDOWOBJECT_NEED_SIZE            (0x00000001)
#define WINDOWOBJECT_NEED_ERASEBKGND      (0x00000002)
#define WINDOWOBJECT_NEED_NCPAINT         (0x00000004)
#define WINDOWOBJECT_NEED_INTERNALPAINT   (0x00000008)
#define WINDOWOBJECT_RESTOREMAX           (0x00000020)

#define WINDOWSTATUS_DESTROYING         (0x1)
#define WINDOWSTATUS_DESTROYED          (0x2)

#define HAS_DLGFRAME(Style, ExStyle) \
            (((ExStyle) & WS_EX_DLGMODALFRAME) || \
            (((Style) & WS_DLGFRAME) && (!((Style) & WS_THICKFRAME))))

#define HAS_THICKFRAME(Style, ExStyle) \
            (((Style) & WS_THICKFRAME) && \
            (!(((Style) & (WS_DLGFRAME | WS_BORDER)) == WS_DLGFRAME)))

#define HAS_THINFRAME(Style, ExStyle) \
            (((Style) & WS_BORDER) || (!((Style) & (WS_CHILD | WS_POPUP))))

#define IntIsDesktopWindow(WndObj) \
  ((WndObj)->Parent == NULL)

#define IntIsBroadcastHwnd(hWnd) \
  (hWnd == HWND_BROADCAST || hWnd == HWND_TOPMOST)

#define IntWndBelongsToThread(WndObj, W32Thread) \
  ((WndObj)->MessageQueue->Thread->Win32Thread == W32Thread)

#define IntGetWndThreadId(WndObj) \
  ((WndObj)->MessageQueue->Thread->Cid.UniqueThread)

#define IntGetWndProcessId(WndObj) \
  ((WndObj)->MessageQueue->Thread->ThreadsProcess->UniqueProcessId)

#define IntGetParentObject(WndObj) \
  ((WndObj)->Parent)

#define IntIsWindow(WndObj) \
  ((WndObj) != NULL && ObmObjectDeleted(WndObj) == FALSE)

#define IntReferenceWindowObject(WndObj) \
  ObmReferenceObject(WndObj)

#define IntReleaseWindowObject(WndObj) \
  ObmDereferenceObject(WndObj)

#define IntIsWindowInDestroy(WndObj) \
  (((Window)->Status & WINDOWSTATUS_DESTROYING) == WINDOWSTATUS_DESTROYING)

#define IntMoveWindow(WndObj, X, Y, nWidth, nHeight, bRepaint) \
  IntSetWindowPos((WndObj), 0, (X), (Y), (nWidth), (nHeight), \
                  ((bRepaint) ? SWP_NOZORDER | SWP_NOACTIVATE : SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW))

typedef struct _INTERNALPOS
{
  RECT NormalRect;
  POINT IconPos;
  POINT MaxPos;
} INTERNALPOS;

typedef struct _WINDOW_OBJECT
{
  /* Handle for the window. */
  HANDLE Handle;
  /* Entry in the list of thread windows. */
  LIST_ENTRY ThreadListEntry;
  /* Pointer to the window class. */
  PCLASS_OBJECT Class;
  /* entry in the window list of the class object */
  LIST_ENTRY ClassListEntry;
  /* Extended style. */
  DWORD ExStyle;
  /* Window name. */
  UNICODE_STRING WindowName;
  /* Window ID */
  ULONG WindowID;
  /* Style. */
  DWORD Style;
  /* Context help id */
  DWORD ContextHelpId;
  /* Handle of the module that created the window. */
  HINSTANCE Instance;
  /* Entry in the thread's list of windows. */
  LIST_ENTRY ListEntry;
  /* Pointer to the extra data associated with the window. */
  PBYTE ExtraData;
  /* Size of the extra data associated with the window. */
  ULONG ExtraDataSize;
  /* Position of the window. */
  RECT WindowRect;
  /* Position of the window's client area. */
  RECT ClientRect;
  /* Window flags. */
  ULONG Flags;
  /* Handle of region of the window to be updated. */
  HRGN UpdateRegion;
  HRGN NCUpdateRegion;
  /* Handle of the window region. */
  HRGN WindowRegion;
  /* Lock to be held when manipulating (NC)UpdateRegion */
  FAST_MUTEX UpdateLock;
  /* Pointer to the owning thread's message queue. */
  PUSER_MESSAGE_QUEUE MessageQueue;
  /* First child window */
  struct _WINDOW_OBJECT* FirstChild;
  /* Last Child Window */
  struct _WINDOW_OBJECT* LastChild;
  /* Next sibling */
  struct _WINDOW_OBJECT* NextSibling;
  /* Previous sibling */
  struct _WINDOW_OBJECT* PrevSibling;
  /* parent window. */
  struct _WINDOW_OBJECT* Parent;
  /* owner window. */
  struct _WINDOW_OBJECT* Owner;
  /* DC Entries (DCE) */ 
  PDCE Dce;
  /* Property list head.*/
  LIST_ENTRY PropListHead;
  /* Count of properties */
  ULONG PropListItems;
  /* Scrollbar info */
  PWINDOW_SCROLLINFO Scroll;
  LONG UserData;
  BOOL Unicode;
  WNDPROC WndProcA;
  WNDPROC WndProcW;
  struct _WINDOW_OBJECT* LastPopup; /* last active popup window */
  PINTERNALPOS InternalPos;
  ULONG Status;
  /* counter for tiled child windows */
  ULONG TiledCounter;
} WINDOW_OBJECT; /* PWINDOW_OBJECT already declared at top of file */

PWINDOW_OBJECT INTERNAL_CALL IntCreateWindow(DWORD dwExStyle, PUNICODE_STRING ClassName, PUNICODE_STRING WindowName,
                                             ULONG dwStyle, LONG x, LONG y, LONG nWidth, LONG nHeight,
                                             PWINDOW_OBJECT Parent, PMENU_OBJECT Menu, ULONG WindowID, HINSTANCE hInstance,
                                             LPVOID lpParam, DWORD dwShowMode, BOOL bUnicodeWindow);
BOOL           INTERNAL_CALL IntDestroyWindow(PWINDOW_OBJECT Window, PW32PROCESS ProcessData, PW32THREAD ThreadData, BOOL SendMessages);
NTSTATUS       INTERNAL_CALL InitWindowImpl (VOID);
NTSTATUS       INTERNAL_CALL CleanupWindowImpl (VOID);
VOID           INTERNAL_CALL IntGetClientRect (PWINDOW_OBJECT WindowObject, PRECT Rect);
PWINDOW_OBJECT INTERNAL_CALL IntGetActiveWindow (VOID);
BOOL           INTERNAL_CALL IntIsWindowVisible (PWINDOW_OBJECT Wnd);
BOOL           INTERNAL_CALL IntIsChildWindow (PWINDOW_OBJECT Parent, PWINDOW_OBJECT Child);
VOID           INTERNAL_CALL IntUnlinkWindow(PWINDOW_OBJECT Wnd);
VOID           INTERNAL_CALL IntLinkWindow(PWINDOW_OBJECT Wnd, PWINDOW_OBJECT WndParent, PWINDOW_OBJECT WndPrevSibling);
PWINDOW_OBJECT INTERNAL_CALL IntGetAncestor(PWINDOW_OBJECT Wnd, UINT Type);
PWINDOW_OBJECT INTERNAL_CALL IntGetParent(PWINDOW_OBJECT Wnd);
INT            INTERNAL_CALL IntGetWindowRgn(PWINDOW_OBJECT Window, HRGN hRgn);
INT            INTERNAL_CALL IntGetWindowRgnBox(PWINDOW_OBJECT Window, RECT *Rect);
BOOL           INTERNAL_CALL IntGetWindowInfo(PWINDOW_OBJECT WindowObject, PWINDOWINFO pwi);
LONG           INTERNAL_CALL IntGetWindowLong(PWINDOW_OBJECT WindowObject, INT Index, BOOL Ansi);
LONG           INTERNAL_CALL IntSetWindowLong(PWINDOW_OBJECT WindowObject, INT Index, LONG NewValue, BOOL Ansi);
VOID           INTERNAL_CALL IntGetWindowBorderMeasures(PWINDOW_OBJECT WindowObject, INT *cx, INT *cy);
BOOL           INTERNAL_CALL IntAnyPopup(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntGetShellWindow(VOID);
PWINDOW_OBJECT INTERNAL_CALL IntSetParent(PWINDOW_OBJECT Window, PWINDOW_OBJECT WndNewParent);
BOOL           INTERNAL_CALL IntSetShellWindowEx(PWINDOW_OBJECT Shell, PWINDOW_OBJECT ListView);
WORD           INTERNAL_CALL IntSetWindowWord(PWINDOW_OBJECT WindowObject, INT Index, WORD NewValue);
ULONG          INTERNAL_CALL IntGetWindowThreadProcessId(PWINDOW_OBJECT Window, ULONG *Pid);
PWINDOW_OBJECT INTERNAL_CALL IntGetWindow(PWINDOW_OBJECT Window, UINT uCmd);
BOOL           INTERNAL_CALL IntDefSetText(PWINDOW_OBJECT WindowObject, PUNICODE_STRING WindowText);
INT            INTERNAL_CALL IntInternalGetWindowText(PWINDOW_OBJECT WindowObject, LPWSTR lpString, INT nMaxCount);
BOOL           INTERNAL_CALL IntDereferenceWndProcHandle(WNDPROC wpHandle, WndProcHandle *Data);
DWORD          INTERNAL_CALL IntRemoveWndProcHandle(WNDPROC Handle);
DWORD          INTERNAL_CALL IntRemoveProcessWndProcHandles(HANDLE ProcessID);
DWORD          INTERNAL_CALL IntAddWndProcHandle(WNDPROC WindowProc, BOOL IsUnicode);

/* WINPOS *********************************************************************/

/* Undocumented flags. */
#define SWP_NOCLIENTMOVE          0x0800
#define SWP_NOCLIENTSIZE          0x1000

#define IntPtInWindow(WndObject,x,y) \
  ((x) >= (WndObject)->WindowRect.left && \
   (x) < (WndObject)->WindowRect.right && \
   (y) >= (WndObject)->WindowRect.top && \
   (y) < (WndObject)->WindowRect.bottom && \
   (!(WndObject)->WindowRegion || ((WndObject)->Style & WS_MINIMIZE) || \
    NtGdiPtInRegion((WndObject)->WindowRegion, (INT)((x) - (WndObject)->WindowRect.left), \
                    (INT)((y) - (WndObject)->WindowRect.top))))

#define HWNDValidateWindowObject(hwnd) \
  ((PWINDOW_OBJECT)(hwnd) > WINDOW_BOTTOM && (PWINDOW_OBJECT)(hwnd) < WINDOW_MESSAGE)

typedef struct _INTERNAL_WINDOWPOS
{
  PWINDOW_OBJECT Window;
  PWINDOW_OBJECT InsertAfter;
  int x;
  int y;
  int cx;
  int cy;
  UINT flags;
} INTERNAL_WINDOWPOS;

BOOL         INTERNAL_CALL IntGetClientOrigin(PWINDOW_OBJECT Window, LPPOINT Point);
LRESULT      INTERNAL_CALL WinPosGetNonClientSize(PWINDOW_OBJECT Window, RECT* WindowRect, RECT* ClientRect);
UINT         INTERNAL_CALL WinPosGetMinMaxInfo(PWINDOW_OBJECT Window, POINT* MaxSize, POINT* MaxPos, POINT* MinTrack, POINT* MaxTrack);
UINT         INTERNAL_CALL WinPosMinMaximize(PWINDOW_OBJECT WindowObject, UINT ShowFlag, RECT* NewPos);
BOOLEAN      INTERNAL_CALL WinPosSetWindowPos(PWINDOW_OBJECT Window, PWINDOW_OBJECT InsertAfter, INT x, INT y, INT cx, INT cy, UINT flags);
BOOLEAN      INTERNAL_CALL WinPosShowWindow(PWINDOW_OBJECT Window, INT Cmd);
USHORT       INTERNAL_CALL WinPosWindowFromPoint(PWINDOW_OBJECT ScopeWin, PUSER_MESSAGE_QUEUE OnlyHitTests, POINT *WinPoint, PWINDOW_OBJECT* Window);
VOID         INTERNAL_CALL WinPosActivateOtherWindow(PWINDOW_OBJECT Window);
PINTERNALPOS INTERNAL_CALL WinPosInitInternalPos(PWINDOW_OBJECT WindowObject, POINT *pt, PRECT RestoreRect);
VOID         INTERNAL_CALL WinPosSetupInternalPos(VOID);

/* WINDOW STATION *************************************************************/

extern WINSTATION_OBJECT *InputWindowStation;
extern PW32PROCESS LogonProcess;

#define WINSTA_ROOT_NAME	L"\\Windows\\WindowStations"
#define WINSTA_ROOT_NAME_LENGTH	23

/* Window Station Status Flags */
#define WSS_LOCKED	(1)
#define WSS_NOINTERACTIVE	(2)

#define PROCESS_WINDOW_STATION() \
  ((HWINSTA)(IoGetCurrentProcess()->Win32WindowStation))

#define SET_PROCESS_WINDOW_STATION(WinSta) \
  ((IoGetCurrentProcess()->Win32WindowStation) = (PVOID)(WinSta))

NTSTATUS INTERNAL_CALL InitWindowStationImpl(VOID);
NTSTATUS INTERNAL_CALL CleanupWindowStationImpl(VOID);
NTSTATUS INTERNAL_CALL IntValidateWindowStationHandle(HWINSTA WindowStation,
                                                      KPROCESSOR_MODE AccessMode,
                                                      ACCESS_MASK DesiredAccess,
                                                      PWINSTATION_OBJECT *Object);

BOOL     INTERNAL_CALL IntGetWindowStationObject(PWINSTATION_OBJECT Object);
BOOL     INTERNAL_CALL IntInitializeDesktopGraphics(VOID);
VOID     INTERNAL_CALL IntEndDesktopGraphics(VOID);
BOOL     INTERNAL_CALL IntGetFullWindowStationName(OUT PUNICODE_STRING FullName,
                                                   IN PUNICODE_STRING WinStaName,
                                                   IN OPTIONAL PUNICODE_STRING DesktopName);

/* MOUSE **********************************************************************/

#ifndef XBUTTON1
#define XBUTTON1	(0x01)
#endif
#ifndef XBUTTON2
#define XBUTTON2	(0x02)
#endif
#ifndef MOUSEEVENTF_XDOWN
#define MOUSEEVENTF_XDOWN	(0x80)
#endif
#ifndef MOUSEEVENTF_XUP
#define MOUSEEVENTF_XUP	(0x100)
#endif

BOOL INTERNAL_CALL IntCheckClipCursor(LONG *x, LONG *y, PSYSTEM_CURSORINFO CurInfo);
BOOL INTERNAL_CALL IntSwapMouseButton(PWINSTATION_OBJECT WinStaObject, BOOL Swap);
INT  INTERNAL_CALL MouseSafetyOnDrawStart(SURFOBJ *SurfObj, LONG HazardX1, LONG HazardY1, LONG HazardX2, LONG HazardY2);
INT  INTERNAL_CALL MouseSafetyOnDrawEnd(SURFOBJ *SurfObj);
BOOL INTERNAL_CALL MouseMoveCursor(LONG X, LONG Y);
VOID INTERNAL_CALL EnableMouse(HDC hDisplayDC);
VOID INTERNAL_CALL SetPointerRect(PSYSTEM_CURSORINFO CurInfo, PRECTL PointerRect);

/* MISC FUNCTIONS *************************************************************/

NTSTATUS  INTERNAL_CALL InitGuiCheckImpl(VOID);
NTSTATUS  INTERNAL_CALL CsrInit(VOID);
NTSTATUS  INTERNAL_CALL CsrNotify(PCSRSS_API_REQUEST Request, PCSRSS_API_REPLY Reply);
PEPROCESS INTERNAL_CALL CsrAttachToCsrss(VOID);
VOID      INTERNAL_CALL CsrDetachFromCsrss(PEPROCESS PrevProcess);

INT  INTERNAL_CALL IntGetSystemMetrics(INT nIndex);
BOOL INTERNAL_CALL IntSystemParametersInfo(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
BOOL INTERNAL_CALL IntRegisterLogonProcess(DWORD dwProcessId, BOOL bRegister);
void INTERNAL_CALL W32kRegisterPrimitiveMessageQueue();

#endif /* _NTUSER_H */
