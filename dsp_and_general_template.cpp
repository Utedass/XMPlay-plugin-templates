#include "xmpdsp.h"

#define DLL_EXPORT __declspec(dllexport)


#define DEBUG_FUNCTION_ENTRY(A,B,C,D) MessageBox(A,B,C,D)
//#define DEBUG_FUNCTION_ENTRY(A,B,C,D)

// Globals for using API-functions
static XMPFUNC_MISC *xmpfmisc;
static XMPFUNC_FILE *xmpffile;
static XMPFUNC_TEXT *xmpftext;
static DWORD xmpver;

// OPTIONAL
static void WINAPI DSP_About(HWND win)
{

    DEBUG_FUNCTION_ENTRY(0, "DSP_About", "DSP plugin", MB_ICONEXCLAMATION);
}

// create new plugin instance (return new instance handle)
static void *WINAPI DSP_New()
{
    // Gör initierande saker här? Returnera 1? wtf..

    DEBUG_FUNCTION_ENTRY(0, "DSP_New", "DSP plugin", MB_ICONEXCLAMATION);
    return (void*)1;
}

// free plugin instance
static void WINAPI DSP_Free(void *inst)
{

    DEBUG_FUNCTION_ENTRY(0, "DSP_Free", "DSP plugin", MB_ICONEXCLAMATION);
}

static const char * WINAPI DSP_GetDescription(void *inst)
{

    DEBUG_FUNCTION_ENTRY(0, "DSP_GetDescription", "DSP plugin", MB_ICONEXCLAMATION);
    return "Simply the best";
}

// present config options to user (OPTIONAL)
static void WINAPI DSP_Config(void *inst, HWND win)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_Config", "DSP plugin", MB_ICONEXCLAMATION);

}

// get config (return size of config data)
static DWORD WINAPI DSP_GetConfig(void *inst, void *config)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_GetConfig", "DSP plugin", MB_ICONEXCLAMATION);
    return 0;
}

// apply config
static BOOL WINAPI DSP_SetConfig(void *inst, void *config, DWORD size)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_SetConfig", "DSP plugin", MB_ICONEXCLAMATION);
    return true;
}

// a track has been opened or closed (OPTIONAL)
static void WINAPI DSP_NewTrack(void *inst, const char *file)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_NewTrack", "DSP plugin", MB_ICONEXCLAMATION);

}

// === the following are optional with the XMPDSP_FLAG_NODSP flag ===

// set sample format (if form=NULL output stopped)
static void WINAPI DSP_SetFormat(void *inst, const XMPFORMAT *form)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_SetFormat", "DSP plugin", MB_ICONEXCLAMATION);

}

// reset DSP after seeking
static void WINAPI DSP_Reset(void *inst)
{
    DEBUG_FUNCTION_ENTRY(0, "DSP_Reset", "DSP plugin", MB_ICONEXCLAMATION);

}

// process samples (return number of samples processed)
static DWORD WINAPI DSP_Process(void *inst, float *data, DWORD count)
{
    // The Process function currently must return the same amount of data as it is given - it can't
    // shorten/stretch the sound. This restriction may (or may not ;)) be lifted in future.

    DEBUG_FUNCTION_ENTRY(0, "DSP_Process", "DSP plugin", MB_ICONEXCLAMATION);
    return count;
}

// the title has changed (OPTIONAL)
static void WINAPI DSP_NewTitle(void *inst, const char *title)
{

    DEBUG_FUNCTION_ENTRY(0, "DSP_NewTitle", "DSP plugin", MB_ICONEXCLAMATION);
}


XMPDSP plugin_interface = {

    // XMPDSP_FLAG_MULTI    supports multiple instances
    // XMPDSP_FLAG_TAIL     effect has a tail
    // XMPDSP_FLAG_NODSP    no DSP processing (a "general" plugin), exclude from "DSP" saved settings
    // XMPDSP_FLAG_TITLE    want title change notifications (NewTitle)
    XMPDSP_FLAG_NODSP | XMPDSP_FLAG_TITLE,

    // Plugin name
    "DSP Plugin Template",
    DSP_About, // OPTIONAL
    DSP_New,
    DSP_Free,
    DSP_GetDescription,
    0, //DSP_Config, // OPTIONAL
    DSP_GetConfig,
    DSP_SetConfig,

    // The rest is optional. SetFormat, Reset and Process must be specified unless XMPDSP_FLAG_NODSP flag is supplied
    0, //DSP_NewTrack,
    0, //DSP_SetFormat,
    0, //DSP_Reset,
    0, //DSP_Process,
    0, //DSP_NewTitle
};


extern "C" XMPDSP * DLL_EXPORT WINAPI XMPDSP_GetInterface2(DWORD face, InterfaceProc faceproc)
{
    if (face!=XMPDSP_FACE) { // unsupported version

        DEBUG_FUNCTION_ENTRY(0,"Failed face (version?) check",0,MB_ICONEXCLAMATION);

		return NULL;
	}

	// Acquire handles to API-function structs
	xmpfmisc=(XMPFUNC_MISC*)faceproc(XMPFUNC_MISC_FACE);
	xmpffile=(XMPFUNC_FILE*)faceproc(XMPFUNC_FILE_FACE);
	xmpftext=(XMPFUNC_TEXT*)faceproc(XMPFUNC_TEXT_FACE);
	xmpver=xmpfmisc->GetVersion();

    return &plugin_interface;
}

extern "C" BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
			DisableThreadLibraryCalls(hinstDLL);
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
