using DLLManager.DataMember;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace DLLManager
{
    /// <summary>
    /// c++DLL处理器，负责加载与释放c++DLL
    /// </summary>
    public class DllManager
    {
        #region NativeDllImpoetModle
        [DllImport("Kernel32", EntryPoint = "GetProcAddress")]
        private static extern int Windows_GetProcAddress(int handle, string funcname);

        [DllImport("Kernel32", EntryPoint = "LoadLibrary")]
        private static extern int Windows_LoadLibrary(string dllFilePath);

        [DllImport("Kernel32", EntryPoint = "FreeLibrary")]
        private static extern int Windows_FreeLibrary(int handle);

        [DllImport("Kernel32", EntryPoint = "GetLastError")]
        private static extern ulong Windows_GetErrorMessage();

        [DllImport("dl", EntryPoint = "dlsym")]
        private static extern IntPtr Linux_GetProcAddress(IntPtr handle, string funcname);

        [DllImport("dl", EntryPoint = "dlopen")]
        private static extern IntPtr Linux_LoadLibrary(string dllFilePath, int mode);

        [DllImport("dl", EntryPoint = "dlclose")]
        private static extern int Linux_FreeLibrary(IntPtr handle);

        [DllImport("dl", EntryPoint = "dlerror")]
        private static extern IntPtr Linux_GetErrorMessage();

        /// <summary>
        /// 无参构造
        /// </summary>
        public DllManager() : this(Platform.Windows) { }
        /// <summary>
        /// 构造
        /// </summary>
        /// <param name="platform">使用平台</param>
        public DllManager(Platform platform) 
        {
            this.platform = platform;
        }

        /// <summary>
        /// DLL库句柄
        /// </summary>
        private int dllModule;
        private IntPtr dllModulePtr;
        /// <summary>
        /// 是否已经加载
        /// </summary>
        public bool IsLoad;
        private string dllName;
        private Platform platform;

        /// <summary>
        /// 获取库函数地址
        /// </summary>
        /// <param name="funcname">函数名</param>
        /// <returns></returns>
        private int GetProcAddress(string funcname)
        {
            if (!IsLoad)
            {
                return 0;
            }
            int address = Windows_GetProcAddress(dllModule, funcname);
            if (address == 0)
            {
                Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Windows_GetErrorMessage());
                return 0;
            }
            return address;
        }
        /// <summary>
        /// 获取DLL库句柄
        /// </summary>
        /// <param name="dllFilePath">DLL库文件路径</param>
        /// <returns>DLL库句柄</returns>
        public bool LoadLibrary(string dllFilePath)
        {
            if (IsLoad)
            {
                return true;
            }
            dllName = dllFilePath;

            if (platform == Platform.Windows)
            {
                dllModule = Windows_LoadLibrary(dllFilePath);
                if (dllModule == 0)
                {
                    Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Windows_GetErrorMessage());
                    return false;
                } 
            }
            else
            {
                dllModulePtr = Linux_LoadLibrary(dllFilePath, 0);
                if (dllModulePtr == IntPtr.Zero)
                {
                    Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Marshal.PtrToStringAuto(Linux_GetErrorMessage()));
                    return false;
                }
            }
            IsLoad = true;
            return true;
        }
        /// <summary>
        /// 释放DLL库句柄
        /// </summary>
        /// <returns></returns>
        public bool FreeLibrary()
        {
            if (!IsLoad)
            {
                return true;
            }
            if (platform == Platform.Windows)
            {
                int res = Windows_FreeLibrary(dllModule);
                if (res == 0)
                {
                    Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Windows_GetErrorMessage());
                    return false;
                }
            }
            else
            {
                int res = Linux_FreeLibrary(dllModulePtr);
                if (res != 0)
                {
                    Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Marshal.PtrToStringAuto(Linux_GetErrorMessage()));
                    return false;
                }
            }

            IsLoad = false;
            return true;
        }

        ///<summary> 
        ///通过非托管函数名转换为对应的委托, by jingzhongrong 
        ///</summary> 
        ///<param name="functionName">非托管函数名</param> 
        ///<param name="t">对应的委托类型</param> 
        ///<returns>委托实例，可强制转换为适当的委托类型</returns> 
        public Delegate GetFunctionAddress(string functionName, Type t)
        {
            if (platform == Platform.Windows)
            {
                int address = GetProcAddress(functionName);
                if (address == 0)
                    return null;
                else
                    return Marshal.GetDelegateForFunctionPointer(new IntPtr(address), t);
            }
            else
            {
                IntPtr address = Linux_GetProcAddress(functionName);
                if (address == IntPtr.Zero)
                    return null;
                else
                    return Marshal.GetDelegateForFunctionPointer(address, t);
            }
        }

        /// <summary>
        /// 获取库函数地址
        /// </summary>
        /// <param name="funcname">函数名</param>
        /// <returns></returns>
        private IntPtr Linux_GetProcAddress(string funcname)
        {
            if (!IsLoad)
            {
                return IntPtr.Zero;
            }
            IntPtr ptrProcAddr = Linux_GetProcAddress(dllModulePtr, funcname);
            if (ptrProcAddr == IntPtr.Zero)
            {
                Console.WriteLine("dllName:{0},dllerror:{1}", dllName, Marshal.PtrToStringAuto(Linux_GetErrorMessage()));
                return IntPtr.Zero;
            }
            return ptrProcAddr;
        }
        #endregion
    }
}
