#include "CDLLManager.h"

namespace CDLLMANAGER
{
    CDLLManager::CDLLManager()
    {
        hDLL = NULL;
        IsLoad = false;
    }
    
    CDLLManager::~CDLLManager()
    {
    }

    void CDLLManager::CLoadLibrary(const char* FileName)
    {
        memcpy(dllName, FileName, MAX_PATH);
#ifdef WINDOWS
        hDLL = LoadLibraryA(dllName);
#elif ANDROID_LIUNX || LINUX
        //以 RTLD_NOW 立即决定 模式 打开指定的动态连接库文件
        hDLL = dlopen(dllName, RTLD_NOW);
#endif // WINDOWS

        if(hDLL == NULL)
        {
            IsLoad = false;
#ifdef WINDOWS
            LOGI("dllName:%s, hDLL:%s, GetLastError:%d\n", dllName, hDLL == NULL ? "false" : "true", GetLastError());
#elif ANDROID_LIUNX || LINUX
            LOGI("dllName:%s,hDLL:%s,dlerror:%s\n", dllName, hDLL == NULL ? "false" : "true", dlerror());
#endif // WINDOWS
        }
        else
        {
            IsLoad = true;
        }
    }

    void CDLLManager::CGetProcAddress(const char* cpProcName,FarProc* pProcAddr)
    {
#ifdef WINDOWS
        *pProcAddr = GetProcAddress(hDLL, cpProcName);
#elif ANDROID_LIUNX || LINUX
        //通过句柄和连接符名称获取函数名或者变量名
        *pProcAddr = (FarProc)dlsym(hDLL, cpProcName); 
#endif // WINDOWS

        if(pProcAddr == NULL)
        {
            LOGI("dllName:%s,cpProcName:%s,dlerror:pProcAddr == NULL\n", dllName, cpProcName);
        }
    }//先按ESC，saveas /home/rain/hello.cpp即可存盘
    //:18,188 w>> ~/yourname.txt 
    // g++ one.cpp two.cpp three.cpp -fPIC -shared -o libtest.so
    void CDLLManager::CFreeLibrary()
    {
        if (dllName != NULL && IsLoad)
        {
#ifdef WINDOWS
            FreeLibrary(hDLL); 
#elif ANDROID_LIUNX || LINUX
            //卸载打开的动态链接库
            int res = dlclose(hDLL);
            if (res != 0)
            {
                LOGI("dllName:%s,hDLL:%d,dlerror:%s\n", dllName, hDLL == NULL ? 0 : 1, dlerror());
            }    
#endif // WINDOWS
            IsLoad = false;
        }
    }
}
