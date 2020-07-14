#include "WaitChainTraverse.h"

typedef struct _STR_ARRAY
{
	CHAR Desc[32];
} STR_ARRAY;

/*Description of objects*/
STR_ARRAY STR_OBJECT_TYPE[] =
{
	{ "CriticalSection" },
	{ "SendMessage" },
	{ "Mutex" },
	{ "Alpc" },
	{ "Com" },
	{ "ThreadWait" },
	{ "ProcWait" },
	{ "Thread" },
	{ "ComActivation" },
	{ "Unknown" },
	{ "Max" }
};

BOOL WaitChainTraverse::GrantDebugPrivilege()
{
	BOOL             fSuccess = FALSE;
	HANDLE           TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges;

	/*
		Process Token contains:
		- ID
		- Session ID
		- User ID
		- Group ID
		- Privileges: sets some property for a process for operating with it (as SE_DEBUG_NAME)
	*/

	auto cleanup = [&TokenHandle]() { if (TokenHandle != nullptr) CloseHandle(TokenHandle); };

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&TokenHandle))
	{
		cleanup();
	}

	TokenPrivileges.PrivilegeCount = 1;

	if (!LookupPrivilegeValue(NULL,
		SE_DEBUG_NAME,
		&TokenPrivileges.Privileges[0].Luid))
	{
		printf("Couldn't lookup SeDebugPrivilege name");
		cleanup();
	}

	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(TokenHandle,
		FALSE,
		&TokenPrivileges,
		sizeof(TokenPrivileges),
		NULL,
		NULL))
	{
		printf("Could not revoke the debug privilege");
		cleanup();
	}

	fSuccess = TRUE;

	return fSuccess;
}

BOOL WaitChainTraverse::CheckBlockingThreads(__in DWORD ProcessId)
{
	if (!(!InitCOMAccess())) { }
	if (!GrantDebugPrivilege()) return FALSE;

	g_WctHandle = OpenThreadWaitChainSession(0, NULL);

	auto process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

	if (process)
	{
		WCHAR file[MAX_PATH];

		if (GetProcessImageFileName(process, file, ARRAYSIZE(file)) > 0)
		{
			PCWSTR filePart = wcsrchr(file, L'\\');
			if (filePart)
			{
				filePart++;
			}
			else
			{
				filePart = file;
			}

			processName = std::wstring(filePart);
		}

		// Get a snapshot of this process. This enables us to
		// enumerate its threads.
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, ProcessId);

		if (snapshot)
		{
			THREADENTRY32 thread;
			thread.dwSize = sizeof(thread);

			// Walk the thread list and print each wait chain
			if (Thread32First(snapshot, &thread))
			{
				do
				{
					/*Get all threads that belong to the process that is being considered*/
					if (thread.th32OwnerProcessID == ProcessId)
					{
						// Open a handle to this specific thread
						HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS,
							FALSE,
							thread.th32ThreadID);
						if (threadHandle)
						{
							// Check whether the thread is still running
							DWORD exitCode;
							GetExitCodeThread(threadHandle, &exitCode);

							if (exitCode == STILL_ACTIVE)
							{
								TraverseChain(thread.th32ThreadID);
							}

							CloseHandle(threadHandle);
						}
					}
				} while (Thread32Next(snapshot, &thread));
			}
			CloseHandle(snapshot);
		}
		CloseHandle(process);
	}

	CloseThreadWaitChainSession(g_WctHandle);

	return true;
}

BOOL WaitChainTraverse::TraverseChain(__in DWORD ThreadId)
{
	WAITCHAIN_NODE_INFO NodeInfoArray[WCT_MAX_NODE_COUNT];
	DWORD               Count;
	BOOL                IsCycle;

	//printf("%d: ", ThreadId);

	Count = WCT_MAX_NODE_COUNT;

	// Make a synchronous WCT call to retrieve the wait chain.
	if (!GetThreadWaitChain(g_WctHandle,
		NULL,
		WCTP_GETINFO_ALL_FLAGS,
		ThreadId,
		&Count,
		NodeInfoArray,
		&IsCycle))
	{
		printf("Error (0X%x)\n", GetLastError());
		return false;
	}

	// Check if the wait chain is too big for the array we passed in.
	if (Count > WCT_MAX_NODE_COUNT)
	{
		Count = WCT_MAX_NODE_COUNT;
	}

	std::vector<WAITCHAIN_NODE_INFO> traversingNodes(NodeInfoArray, NodeInfoArray + Count);

	for(auto node : traversingNodes)
	{
		if (node.ObjectStatus == WctStatusBlocked)
			blockingNodes.push_back(node);
	}

	/*WARNING DEADLOCK: IsCycle*/
}

BOOL WaitChainTraverse::InitCOMAccess()
/*++

Routine Description:

Register COM interfaces with WCT. This enables WCT to provide wait
information if a thread is blocked on a COM call.

--*/
{
	PCOGETCALLSTATE       CallStateCallback;
	PCOGETACTIVATIONSTATE ActivationStateCallback;

	// Get a handle to OLE32.DLL. You must keep this handle around
	// for the life time for any WCT session.
	g_Ole32Hnd = LoadLibrary(L"ole32.dll");
	if (!g_Ole32Hnd)
	{
		printf("ERROR: GetModuleHandle failed: 0x%X\n", GetLastError());
		return FALSE;
	}

	// Retrieve the function addresses for the COM helper APIs.
	CallStateCallback = (PCOGETCALLSTATE)
		GetProcAddress(g_Ole32Hnd, "CoGetCallState");
	if (!CallStateCallback)
	{
		printf("ERROR: GetProcAddress failed: 0x%X\n", GetLastError());
		return FALSE;
	}

	ActivationStateCallback = (PCOGETACTIVATIONSTATE)
		GetProcAddress(g_Ole32Hnd, "CoGetActivationState");
	if (!ActivationStateCallback)
	{
		printf("ERROR: GetProcAddress failed: 0x%X\n", GetLastError());
		return FALSE;
	}

	// Register these functions with WCT.
	RegisterWaitChainCOMCallback(CallStateCallback,
		ActivationStateCallback);
	return TRUE;
}