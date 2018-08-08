#pragma once 

#ifndef __DEFINITION_H__
#define __DEFINITION_H__

// 判断哪个平台
#if WIN32
#define WINDOWS 1
#elif defined(__APPLE__)
#if defined(__arm__)
#define IPHONE 1
#else
#define OSX 1
#endif
//#elif defined(__linux__)
//	#define LINUX 1
#elif defined(METRO) || defined(ANDROID_LIUNX)
// these are defined externally
#else
#define ANDROID_LIUNX 1
#endif

#if METRO
#define CINTERFACEAPI extern "C" __declspec(dllexport) __stdcall
#elif WINDOWS
#define CINTERFACEAPI extern "C" __declspec(dllexport)
#define CCLASSINTERFACEAPI __declspec(dllexport)
#else
#define CINTERFACEAPI extern "C"
#define CCLASSINTERFACEAPI 
#endif

#if WINDOWS
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>  
#include <time.h>
#include <vector>
#include <errno.h>
#define  LOGI(...)  printf(__VA_ARGS__) 
#define  STRLEN(...) lstrlen(__VA_ARGS__)
#define  STRCMP(...) lstrcmp(__VA_ARGS__)
#elif LINUX
#include <stdio.h>
#include <stdarg.h>

#elif ANDROID_LIUNX

#define  STRLEN(...) strlen(__VA_ARGS__)
#define  STRCMP(...) strcmp(__VA_ARGS__)
typedef int                 BOOL;
#ifndef MAX_PATH
#define MAX_PATH 260
#endif // !MAX_PATH

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdexcept>
//#include <exception>
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h>
//#include <stdarg.h>
#include <iostream>
#include <string>
using namespace std;
#include <vector>

//#define ANDROID_JNI 1

#if ANDROID_JNI || ANDROID_JNI_LOG

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "TAG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__) 

#else
#define  LOGI(...)  printf(__VA_ARGS__) 

#endif //ANDROID_JNI


#if Test

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "TAG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);printf(__VA_ARGS__)  

#endif // Test

#endif

#endif
