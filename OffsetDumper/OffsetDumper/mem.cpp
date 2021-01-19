#include "mem.h"
#include <iostream>


DWORD  mem::get_pid(const wchar_t* process_name)
{
    DWORD  pid = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, process_name))
                {
                    pid = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));

        }
    }
    CloseHandle(hSnap);
    return pid;
}

DWORD mem::get_ModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

MODULEENTRY32  mem::get_Module(uintptr_t dwProcID, wchar_t* moduleName)
{
    MODULEENTRY32 modEntry = { 0 };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        modEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &modEntry))
        {
            do
            {
                if (wcscmp(modEntry.szModule, moduleName) == 0)
                {
                    break;
                }
            } while (Module32Next(hSnapshot, &modEntry));
        }
        CloseHandle(hSnapshot);
    }
    return modEntry;
}

//Internal Pattern scan, external pattern scan is just a wrapper around this
uintptr_t mem::FindPattern(char* base, unsigned int size, char* pattern, char* mask)
{
    size_t patternLength = strlen(mask);

    for (uintptr_t i = 0; i < size - patternLength; i++)
    {
        bool found = true;
        for (uintptr_t j = 0; j < patternLength; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(char*)(base + i + j))
            {
                found = false;
                break; // yeah that's right, stop iterating when pattern is bad.  Looking at you fleep...
            }
        }

        if (found)
        {
            return (uintptr_t)base + i;
        }
    }
    return 0;
}

//Scan just one module
uintptr_t mem::FindPatternEx(HANDLE hProcess, wchar_t* module, char* pattern, char* mask)
{
    //Grab module information from External Process
    MODULEENTRY32 modEntry = mem::get_Module(GetProcessId(hProcess), module);
    uintptr_t start = (uintptr_t)modEntry.modBaseAddr;
    uintptr_t end = start + modEntry.modBaseSize;

    uintptr_t currentChunk = start;
    SIZE_T bytesRead;

    while (currentChunk < end)
    {
        //make data accessible to ReadProcessMemory
        DWORD oldprotect;
        VirtualProtectEx(hProcess, (void*)currentChunk, 4096, PROCESS_VM_READ, &oldprotect);

        //Copy chunk of external memory into local storage
        byte buffer[4096];
        ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, 4096, &bytesRead);

        //if readprocessmemory failed, return
        if (bytesRead == 0)
        {
            return 0;
        }

        //Find pattern in local buffer, if pattern is found return address of matching data
        uintptr_t InternalAddress = FindPattern((char*)&buffer, bytesRead, pattern, mask);

        //if Find Pattern returned an address
        if (InternalAddress != 0)
        {
            //convert internal offset to external address and return
            uintptr_t offsetFromBuffer = InternalAddress - (uintptr_t)&buffer;
            return currentChunk + offsetFromBuffer;
        }

        //pattern not found in this chunk
        else
        {
            //advance to next chunk
            currentChunk = currentChunk + bytesRead;
        }
    }
    return 0;
}



uintptr_t mem::FindPattern(HANDLE hProcess, wchar_t* module, PatternStruct Struct)
{
    uintptr_t address = FindPatternEx(hProcess, module, Struct.Sig, Struct.Mask);

    if (address == 0)
        return 0;

    address += Struct.AddOffset;

    DWORD base = mem::get_ModuleBaseAddress(GetProcessId(hProcess), module);

    if (Struct.Search == PatternSearch::call_func)
    {
        if (mem::ReadMemory<byte>(hProcess, address) == 0xE8) //E8  == Call
        {
            DWORD rel_function = mem::ReadMemory<DWORD>(hProcess, address + 1);


            return (rel_function + address + 5) - base;
        }
    }

    if (Struct.Search == PatternSearch::address)
    {
        uintptr_t relative = mem::ReadMemory<uintptr_t>(hProcess, (uintptr_t)address);

        return ((relative + address) - base) + 4;
        ///return mem::ReadMemory<uintptr_t>(hProcess,(uintptr_t)address);


    }

    if (Struct.Search == PatternSearch::offset)
    {
        if (Struct.Size == 1)
            return mem::ReadMemory<int8_t>(hProcess, address);
        else if (Struct.Size == 2)
            return mem::ReadMemory<int16_t>(hProcess, address);
        else if (Struct.Size == 4)
            return mem::ReadMemory<int32_t>(hProcess, address);
    }

    return address - base;

}