#ifndef __CDLLMANAGER_H__
#define __CDLLMANAGER_H__

#include "Definition.h"

#ifdef WINDOWS
typedef FARPROC	FarProc;
#elif   LINUX || ANDROID_LIUNX
typedef void*   HMODULE;
typedef void (*FarProc)(void *);
#endif // WINDOWS

namespace CDLLMANAGER
{
    class CCLASSINTERFACEAPI CDLLManager
    {
    public:
        CDLLManager();
        ~CDLLManager();
        void CLoadLibrary(const char* FileName);
        void CFreeLibrary();
        void CGetProcAddress(const char* cpProcName,FarProc* pProcAddr);

    protected:
        /// <summary>
        /// DLL库句柄
        /// </summary>
        HMODULE hDLL; 
    public:
        char dllName[MAX_PATH];
        /// <summary>
        /// 是否已经加载
        /// </summary>
        bool IsLoad;

    };

}

#endif // !__CDLLMANAGER_H__
