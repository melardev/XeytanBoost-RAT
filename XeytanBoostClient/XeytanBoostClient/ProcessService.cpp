#include "ProcessService.h"
#include "appdefs.h"

#ifdef XEYTAN_WINDOWS
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

#elif defined(XEYTAN_LINUX)
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()

#define PROC_DIRECTORY "/proc/"
#include <list>
#include <locale>
#include <codecvt>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif


#if defined(XEYTAN_WINDOWS)
BOOL CALLBACK onEnumWindowProc(HWND hwnd, LPARAM lp)
{
	Process* processModel = (Process*)lp;
	processModel->hasUi = IsWindowVisible(hwnd);

	int bufferLength = GetWindowTextLengthW(hwnd);
	wchar_t* buffer = new wchar_t[bufferLength + 1];
	GetWindowTextW(hwnd, buffer, bufferLength + 1);
	processModel->title = buffer;
	delete[] buffer;
	return TRUE;
}

void ProcessService::getProcesses(std::list<Process>& processes)
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W); // If not set the call Process32FirstW fails

	std::list<Process>::const_iterator it = processes.cbegin();

	if (Process32First(handle, &pe))
	{
		do
		{
			Process process;
			process.pid = pe.th32ProcessID;

			process.path = pe.szExeFile;
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
			if (hProcess != NULL)
			{
				// We have successfully opened the process for query
				wchar_t filePath[MAX_PATH];
				if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH))
				{
					process.path = filePath;
				}

				CloseHandle(hProcess);
			}


			EnumWindows(onEnumWindowProc, (LPARAM)&process);
			processes.push_back(process);
		}
		while (Process32NextW(handle, &pe));
	}
}
#elif  defined(XEYTAN_LINUX)

typedef std::codecvt_utf8<wchar_t> convert_type;

std::wstring stringToWString(const std::string& t_str)
{
	//setup converter

	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

std::wstring stringToWstring(const char* utf8Bytes)
{
	//setup converter
	using convert_type = std::codecvt_utf8<typename std::wstring::value_type>;
	std::wstring_convert<convert_type, typename std::wstring::value_type> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(utf8Bytes);
}

void search(Process& process, Window w)
{
	Atom type;
	int format;
	unsigned long nItems;
	unsigned long bytesAfter;
	unsigned char* propPID = 0;

	if (Success == XGetWindowProperty(display, w, atomPID, 0, 1, False, XA_CARDINAL,
	                                  &type, &format, &nItems, &bytesAfter, &propPID))
	{
		if (propPID != 0)
		{
			// If the PID matches, add this window to the result set.
			uint64_t foundId = *((uint64_t*)propPID);
			if (process.pid == foundId)
			{
				// _result.push_back(w);
				unsigned char* list;
				Atom prop = XInternAtom(display, "WM_NAME", False), type;
				int form;
				unsigned long remain, len;

				if (XGetWindowProperty(display, w, prop, 0, 1024, False, AnyPropertyType,
				                       &type, &form, &len, &remain, &list) != Success)
				{
					// XA_STRING

					// Error
				}
				else
				{
					if ((char*)list != NULL)
					{
						process.title = stringToWstring((char*)list);
					}
				}
			}

			XFree(propPID);
		}
	}


	// Recurse into child windows.
	Window wRoot;
	Window wParent;
	Window* wChild;
	unsigned nChildren;
	if (0 != XQueryTree(display, w, &wRoot, &wParent, &wChild, &nChildren))
	{
		for (unsigned i = 0; i < nChildren; i++)
			search(process, wChild[i]);
	}
}

void getWindowTitle(Process& process)
{
	Atom atomPID = XInternAtom(display, "_NET_WM_PID", True);
	if (atomPID == None)
	{
		return;
	}

	Window w = XDefaultRootWindow(display);
	std::list<Window> _result;

	search(process, w);
}

void ProcessService::getProcesses(std::list<Process>& processes)
{
	std::list<Process> processes;
	char cmdLinePath[100];
	char processPath[PATH_MAX];
	char* processName = NULL;

	struct dirent* de_DirEntity = NULL;
	DIR* dir_proc = NULL;


	dir_proc = opendir(PROC_DIRECTORY);
	if (dir_proc == NULL)
	{
		perror("Couldn't open the " PROC_DIRECTORY " directory");
		return;
	}

	// Loop while not NULL
	while ((de_DirEntity = readdir(dir_proc)))
	{
		if (de_DirEntity->d_type == DT_DIR)
		{
			Process process;

			if (isNumeric(de_DirEntity->d_name))
				process.pid = atoi(de_DirEntity->d_name);
			else
				process.pid = -1;

			strcpy(cmdLinePath, PROC_DIRECTORY);
			strcat(cmdLinePath, de_DirEntity->d_name);
			strcat(cmdLinePath, "/cmdline");
			FILE* fd_CmdLineFile = fopen(cmdLinePath, "rt"); // open the file for reading text
			if (fd_CmdLineFile)
			{
				fscanf(fd_CmdLineFile, "%s", processPath); // read from /proc/<NR>/cmdline
				fclose(fd_CmdLineFile); // close the file prior to exiting the routine

				if (strrchr(processPath, '/')) // if not found returns NULL, otherwise a ptr to the last occurrence
					processName = strrchr(processPath, '/') + 1;
				else
					processName = processPath;

				process.path = stringToWstring(processPath);
				// printf("Process name: %s\n", processPath);
// printf("Pure Process name: %s\n", processName);

// long int pid = strtol(de_DirEntity->d_name);
				// printf("%d\n", pid);


				getWindowTitle(process);

				process.hasUi = !process.title.empty();

				processes.push_back(process);
			}
		}
	}

	closedir(dir_proc);
}
#endif
