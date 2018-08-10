#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

#include "Definition.h"

//获取操作系统版本号
CINTERFACEAPI void GetSystemVersionName(char* VersionName);

//获取计算机CPU型号
CINTERFACEAPI void GetSystemCPUType(char* CPUType);

//获取当前用户名
CINTERFACEAPI void GetSystemUserName(char* UserName);

//获取电脑主机名
CINTERFACEAPI void GetSystemHostName(char* HostName);

//获取电脑IP
CINTERFACEAPI void GetSystemHostIP(char* IP);

#endif // !__SYSTEMINFO_H__
