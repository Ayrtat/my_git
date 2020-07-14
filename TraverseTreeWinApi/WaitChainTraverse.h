#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wct.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Advapi32.lib")


class WaitChainTraverse
{
public:
    inline std::vector<int> getBlockingProcesses(__in DWORD ProcessId)
    {
        CheckBlockingThreads(ProcessId);

        std::vector<decltype(decltype(decltype(blockingNodes)::value_type::ThreadObject)::ProcessId)> result_vector;
        std::copy(blockingNodes.begin(), blocking.end(), std::back_inserter(result_vector), [](decltype(blockingNodes)::value_type node) { return node.ThreadObject.ProcessId; });

        return result_vector;
    }
private:
    std::vector<WAITCHAIN_NODE_INFO> blockingNodes;
    std::wstring processName;

    // Global variable to store the WCT session handle
    HWCT g_WctHandle = NULL;
    // Global variable to store OLE32.DLL module handle.
    HMODULE g_Ole32Hnd = NULL;
    /*
        These specific functions are needed for work of the WCT API.
        These specific functions are not intended for common using
    */
    BOOL CheckBlockingThreads(__in DWORD ProcessId);  /*retrieve all threads of a process*/
    BOOL GrantDebugPrivilege();                      /*get additional info for threads of a process*/
    BOOL TraverseChain(__in DWORD ThreadId);        /*set up privilege for a process*/
    BOOL InitCOMAccess();                          /*specific function for WCT API*/
};
    