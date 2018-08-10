#define ANDROID_JNI 1
#include "SystemInfoTool.h"
#ifdef WINDOWS
#elif LINUX
#include <unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <netdb.h>
#include <net/if.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#elif ANDROID_LIUNX
#include <unistd.h>
#include <pwd.h>
#include <netdb.h>
#include <net/if.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // WINDOWS


CINTERFACEAPI void GetSystemVersionName(char* VersionName)
{
#ifdef WINDOWS
    std::string vname = "Windows ";
    
    //用SYSTEM_INFO结构判断64位AMD处理器
    SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器  
    GetNativeSystemInfo(&info);            //调用GetSystemInfo函数填充结构  

    //先判断是否为win8.1或win10
    typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
    HINSTANCE hinst = LoadLibrary(L"ntdll.dll");
    DWORD dwMajor, dwMinor, dwBuildNumber;
    NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers"); 
    proc(&dwMajor, &dwMinor, &dwBuildNumber); 
    //if (dwMajor >= 6 && dwMinor >= 2)	//win 8.1
    if (dwMajor >= 6)	//win 8.1
    {
        vname = vname.append("NT ");
        char version[8];
        sprintf(version,"%wd.%wd",dwMajor,dwMinor);
        vname = vname.append(version);
        if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        {
            vname.append("; Win64; x64;");
        }else
        {
            vname.append("; Win32; x86;");
        }
        strcpy(VersionName,const_cast<char*>(vname.c_str()));
        return;
    }

    //下面判断不能Win Server，因为本人还未有这种系统的机子，暂时不给出

    OSVERSIONINFOEX os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    //#pragma warning(disable:4996)
    if (GetVersionEx((OSVERSIONINFO *)&os))
    {
        if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
        {
            vname = vname.append("NT ");
        }
        char version[8];
        sprintf(version,"%wd.%wd",os.dwMajorVersion,os.dwMinorVersion);
        vname = vname.append(version);
        if (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        {
            vname.append("; Win64; x64;");
        }else
        {
            vname.append("; Win32; x86;");
        }
        strcpy(VersionName,const_cast<char*>(vname.c_str()));
    }

#elif LINUX
    
    char szTest[256] = {0};  
    std::string vname;

    FILE *fp = fopen("/proc/version", "r");  
    if(NULL == fp)
    {
        LOGI("failed to open version\n");   
    }
    else
    {
        while(!feof(fp))  
        {  
            memset(szTest, 0, sizeof(szTest));  
            fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n
            //printf("%s", szTest);  
            vname.append(szTest);
        } 
    }
    
    fclose(fd); 
    vname = vname.substr(vname.find("Linux version"),vname.find("(gcc version") - vname.find("Linux Version") - 1);

#ifdef __x86_64__
    vname.append("; Linux64; x64;");
#elif   __i386__
    vname.append("; Linux32; x86;");
#endif // __x86_64__

    strcpy(VersionName,const_cast<char*>(vname.c_str()));

#elif ANDROID_LIUNX

    char szTest[1000] = {0};  
    std::string vname = "Android ";

    FILE *fp = fopen("/system/build.prop", "r");  
    if(NULL == fp)   
    {
        LOGI("failed to open /system/build.prop\n");   
    }
    else
    {
        while(!feof(fp))  
        {  
            memset(szTest, 0, sizeof(szTest));  
            fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n

            string release = szTest;
            if (release.find("ro.build.version.release=") != string::npos)
            {
                vname.append(release.substr(release.find("=")+1,release.length() - release.find("=") - 2));
            }
        }  
    }

    fclose(fp); 

    FILE *fd = fopen("/proc/cpuinfo", "r");  
	if(NULL == fd)   
    {
		printf("failed to open /proc/cpuinfo\n");   
    }
    memset(szTest, 0, sizeof(szTest));  
    fgets(szTest, sizeof(szTest) - 1, fd); // leave out \n
    string Hardware = szTest;
    if (Hardware.find("AArch64") != string::npos)
    {
        vname.append("; Android64; x64;");
    }
    else
    {
        vname.append("; Android32; x86;");
    }

	fclose(fd); 

    strcpy(VersionName,const_cast<char*>(vname.c_str()));
#endif // WINDOWS
}

#ifdef WINDOWS
// ---- get cpu info ---- //
// save 4 register variables
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;

// init cpu in assembly language
void initCpu(DWORD veax)  
{
	__asm
	{
		mov eax, veax
		cpuid
			mov deax, eax
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
	}
}
 
std::string getCpuType()
{
	const DWORD id = 0x80000002; // start 0x80000002 end to 0x80000004
	char cpuType[49];
	memset(cpuType, 0, sizeof(cpuType));
 
	for (DWORD t = 0; t < 3; t++)
	{
		initCpu(id + t);
		
		memcpy(cpuType + 16 * t + 0, &deax, 4);
		memcpy(cpuType + 16 * t + 4, &debx, 4);
		memcpy(cpuType + 16 * t + 8, &decx, 4);
		memcpy(cpuType + 16 * t + 12, &dedx, 4);
	}
 
	return cpuType;
}
#endif // WINDOWS

CINTERFACEAPI void GetSystemCPUType(char* CPUType)
{
#ifdef WINDOWS
    std::string cputype = getCpuType();
    strcpy(CPUType,const_cast<char*>(cputype.c_str()));
#elif   LINUX || ANDROID_LIUNX
	FILE *fp = fopen("/proc/cpuinfo", "r");  
	if(NULL == fp)   
		printf("failed to open cpuinfo\n");   
	char szTest[1000] = {0};   
    // read file line by line 
    std::string cpuType;
	while(!feof(fp))  
	{  
		memset(szTest, 0, sizeof(szTest));  
        fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n
        string Hardware = szTest;
        if (Hardware.find("model name") != string::npos)
        {
            cpuType = Hardware.substr(Hardware.find(":")+1,Hardware.length() - Hardware.find(":") - 2);
        }

        if (Hardware.find("Hardware") != string::npos)
        {
            cpuType = Hardware.substr(Hardware.find(":")+1,Hardware.length() - Hardware.find(":") - 2);
        }
    }  
    strcpy(CPUType,const_cast<char*>(cpuType.c_str()));
	fclose(fp); 
#endif // WINDOWS
}

CINTERFACEAPI void GetSystemUserName(char* UserName)
{
#ifdef WINDOWS
    WCHAR strBuffer[256] ={0};
    DWORD dwSize = 256;
    GetUserName(strBuffer,&dwSize);
    char userName[1024] = {0};
    sprintf(userName,"%ws",strBuffer);
    strcpy(UserName, userName);
#elif LINUX || ANDROID_LIUNX
    uid_t userid;
    struct passwd* pwd;
    userid=getuid();
    pwd=getpwuid(userid);
    strcpy(UserName,pwd->pw_name);
#endif // WINDOWS

}

CINTERFACEAPI void GetSystemHostName(char* HostName)
{
#ifdef WINDOWS
    WSADATA WSAData;                                //WSADATA结构被用来储存调用AfxSocketInit全局函数返回的Windows Sockets初始化信息。
    if (WSAStartup(MAKEWORD(2, 0),&WSAData))        // 初始化Windows sockets API
    {  
        printf("WSAStartup failed %s\n", WSAGetLastError());
        exit(-1);        //异常退出 
    }  

    char ihostName[256];
    if(gethostname(ihostName,sizeof(ihostName)))        //获取主机名
    {
        printf("Error: %u\n", WSAGetLastError());
        exit(-1);        //异常退出 
    }
    strcpy(HostName, ihostName);

    WSACleanup();  
#elif LINUX || ANDROID_LIUNX
    char ihostName[256];
    gethostname(ihostName,sizeof(ihostName));
    strcpy(HostName, ihostName);
#endif // WINDOWS
}

CINTERFACEAPI void GetSystemHostIP(char* IP)
{
#ifdef WINDOWS
    char buf[256]="";

    WSADATA WSAData;

    WSAStartup(MAKEWORD(2, 0), &WSAData);

    struct hostent *ph = 0;

    gethostname(buf, 256);

    ph = gethostbyname(buf);

    const char *pIP =inet_ntoa(*((struct in_addr *)ph->h_addr_list[0]));

    std::string strIp = pIP; 

    strcpy(IP,const_cast<char*>(strIp.c_str()));

    WSACleanup();
#elif LINUX || ANDROID_LIUNX
    int sock_get_ip;  
  
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;     
  
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
        printf("socket create failse...GetLocalIp!\n"); 
        return;
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {
        printf("socket ioctl failse...GetLocalIp!\n");  
        return;
    }  
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
      
    strcpy(IP,inet_ntoa(sin->sin_addr));
    close( sock_get_ip );  
#endif // WINDOWS
}
