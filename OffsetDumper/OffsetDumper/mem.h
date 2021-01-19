#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <string>

enum PatternSearch
{
	normal = 0,
	call_func = 1, // call Csgo.exe + 0x2D1C // vai buscar o 2D1C
	address = 2, // mov [rax] , 0xDEADBEEF // vai buscar o deadbeef
	offset = 3 // mov eax , [ebx + 0xA0] // vai buscar o 0xA0
};

struct PatternStruct
{
public:
	char* Sig;
	char* Mask;
	int AddOffset;
	PatternSearch Search;
	int Size;

	PatternStruct(char* sig, char* mask)
	{
		Sig = sig;
		Mask = mask;
		AddOffset = 0;
		Search = PatternSearch::normal;
		Size = 1;
	}

	PatternStruct(char* sig, char* mask, PatternSearch search)
	{
		Sig = sig;
		Mask = mask;
		AddOffset = 0;
		Search = search;
		Size = 1;
	}
	PatternStruct(char* sig, char* mask, PatternSearch search, int add)
	{
		Sig = sig;
		Mask = mask;
		AddOffset = add;
		Search = search;
		Size = 1;
	}

	PatternStruct(char* sig, char* mask, PatternSearch search, int add, int size)
	{
		Sig = sig;
		Mask = mask;
		AddOffset = add;
		Search = search;
		Size = size;
	}


};

class mem
{
public:
	static DWORD get_pid(const wchar_t* process_name);
	static MODULEENTRY32 get_Module(uintptr_t dwProcID, wchar_t* moduleName);
	static DWORD get_ModuleBaseAddress(DWORD procId, const wchar_t* modName);

	static uintptr_t FindPatternEx(HANDLE hProcess, wchar_t* module, char* pattern, char* mask);
	static uintptr_t FindPattern(char* base, unsigned int size, char* pattern, char* mask);
	static uint64_t FindPattern(HANDLE hProcess, wchar_t* module, PatternStruct Struct);

	template<typename T>
	static T ReadMemory(HANDLE proc, uint64_t address) {
		T buffer = T();
		ReadProcessMemory(proc, (LPCVOID)address, &buffer, sizeof(buffer), 0);
		return buffer;
	}

	template<typename T>
	static void WriteMemory(HANDLE proc, uint64_t address, T dataToWrite) {
		T buffer = dataToWrite;
		WriteProcessMemory(proc, (LPVOID)address, &buffer, sizeof(buffer), 0);
	}

	static std::string ReadString(HANDLE proc, DWORD address)
	{
		std::string String;
		char _string[MAX_PATH];

		ReadProcessMemory(proc, (LPVOID)address, &_string, sizeof(_string), 0);

		String = std::string(_string);

		CloseHandle(proc);
		return String;
	}


};

