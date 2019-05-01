#include "main.h"

// XMPlay WAD archive plugin (c) 2006-2013 Ian Luck

#include <windows.h>

#include "xmplay/xmpdsp.h" // requires the XMPlay "DSP/general plugin SDK"

static XMPFUNC_MISC *xmpfmisc;

static HINSTANCE ghInstance;
static BOOL isUnicode;

static HWND xmpwin;
static HHOOK hook;

static BOOL CALLBACK DSPDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void WINAPI DSP_About(HWND win);
static void *WINAPI DSP_New(void);
static void WINAPI DSP_Free(void *inst);
static const char *WINAPI DSP_GetDescription(void *inst);
static void WINAPI DSP_Config(void *inst, HWND win);
static DWORD WINAPI DSP_GetConfig(void *inst, void *config);
static BOOL WINAPI DSP_SetConfig(void *inst, void *config, DWORD size);

// config structure
typedef struct {
    BOOL showCues;
    BOOL keepOnClose;
} MSNStuff;
static MSNStuff msnConf;

static XMPDSP dsp = {
    XMPDSP_FLAG_NODSP,
    "XMPlay2MSN",
    DSP_About,
    DSP_New,
    DSP_Free,
    DSP_GetDescription,
    DSP_Config,
    DSP_GetConfig,
    DSP_SetConfig,
};

static void WINAPI SetNowPlaying(BOOL close)
{
	COPYDATASTRUCT msndata;
    int strLen;
    wchar_t *lpMsn;
    HWND msnui=0;
	char *title=NULL;
	if (!close) {
		if (msnConf.showCues) title=xmpfmisc->GetTag(TAG_TRACK_TITLE); // get cue title
		if (!title) title=xmpfmisc->GetTag(TAG_FORMATTED_TITLE); // get track title
	}
    if (!title) {
		if (msnConf.keepOnClose) return;
        lpMsn = L"\\0Music\\00\\0{0}\\0\\0";
        strLen = 20;
    } else {
		lpMsn = (wchar_t*)xmpfmisc->Alloc(1024);
        // stuff for MSN before...
        memcpy(lpMsn, L"\\0Music\\01\\0{0}\\0", 17*2);
        // actual title...
		strLen=MultiByteToWideChar(isUnicode?CP_UTF8:CP_ACP,0,title,-1,lpMsn+17,492)-1;  /* 1024/2 - 20 */
        // stuff for MSN after...
        memcpy(lpMsn + 17 + strLen, L"\\0", 3*2);
        strLen += 20;
    }
    msndata.dwData = 0x547;
    msndata.lpData = (void*)lpMsn;
    msndata.cbData = strLen * 2;

    while (msnui = FindWindowEx(NULL, msnui, "MsnMsgrUIManager", NULL))
        SendMessage(msnui, WM_COPYDATA, (WPARAM)xmpwin, (LPARAM)&msndata);

    if (title) {
		xmpfmisc->Free(title);
		xmpfmisc->Free(lpMsn);
    }
}

static LRESULT CALLBACK HookProc(int n, WPARAM w, LPARAM l)
{
	if (n==HC_ACTION) {
		CWPSTRUCT *cwp=(CWPSTRUCT*)l;
		if (cwp->message==WM_SETTEXT && cwp->hwnd==xmpwin) // title change
			SetNowPlaying(FALSE);
	}
	return CallNextHookEx(hook,n,w,l);
}

static void WINAPI DSP_About(HWND win)
{
	MessageBox(win,
		"XMPlay to MSN-Now-Playing Plugin\nCopyright 2005 Elliott Sales de Andrade"
		"\n\nContributors:\nSvante Boberg\nIan Luck",
		"XMPlay2MSN (rev.7)",
		MB_ICONINFORMATION);
}

static const char *WINAPI DSP_GetDescription(void *inst)
{
    return dsp.name;
}

static void *WINAPI DSP_New()
{
	xmpwin=xmpfmisc->GetWindow();
    msnConf.showCues = TRUE;
    msnConf.keepOnClose = FALSE;

	SetNowPlaying(FALSE);

	// setup hook to catch title changes
	hook=SetWindowsHookEx(WH_CALLWNDPROC,&HookProc,NULL,GetWindowThreadProcessId(xmpwin,NULL));

    return (void*)1;
}

static void WINAPI DSP_Free(void *inst)
{
	UnhookWindowsHookEx(hook);
	SetNowPlaying(TRUE);
}

static void WINAPI DSP_Config(void *inst, HWND win)
{
    DialogBox(ghInstance, MAKEINTRESOURCE(1000), win, &DSPDialogProc);
}

static DWORD WINAPI DSP_GetConfig(void *inst, void *config)
{
    memcpy(config, &msnConf, sizeof(msnConf));
    return sizeof(msnConf); // return size of config info
}

static BOOL WINAPI DSP_SetConfig(void *inst, void *config, DWORD size)
{
    memcpy(&msnConf, config, min(size,sizeof(msnConf)));
	SetNowPlaying(FALSE);
    return TRUE;
}

#define MESS(id,m,w,l) SendDlgItemMessage(hWnd,id,m,(WPARAM)(w),(LPARAM)(l))

static BOOL CALLBACK DSPDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					msnConf.showCues = (BST_CHECKED==MESS(10, BM_GETCHECK, 0, 0));
					msnConf.keepOnClose = (BST_CHECKED==MESS(20, BM_GETCHECK, 0, 0));
					SetNowPlaying(FALSE);
				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
			break;
        case WM_INITDIALOG:
			if (xmpfmisc->GetVersion()>=0x03040100) // check for 3.4.1
				MESS(10, BM_SETCHECK, msnConf.showCues?BST_CHECKED:BST_UNCHECKED, 0);
			else
				EnableWindow(GetDlgItem(hWnd,10),FALSE);
			MESS(20, BM_SETCHECK, msnConf.keepOnClose?BST_CHECKED:BST_UNCHECKED, 0);
			return TRUE;
    }
	return FALSE;
}

// get the plugin's XMPDSP interface
XMPDSP *WINAPI XMPDSP_GetInterface2(DWORD face, InterfaceProc faceproc)
{
	if (face!=XMPDSP_FACE) return NULL;
	xmpfmisc=(XMPFUNC_MISC*)faceproc(XMPFUNC_MISC_FACE); // import "misc" functions
	return &dsp;
}

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD reason, LPVOID reserved)
{
    if (reason==DLL_PROCESS_ATTACH) {
        ghInstance=hDLL;
        DisableThreadLibraryCalls(hDLL);
		isUnicode=(int)GetVersion()>=0;
    }
    return 1;
}

/*

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
*/

