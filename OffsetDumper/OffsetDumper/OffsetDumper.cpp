// OffsetDumper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>

#include <string>
#include <vector>
#include "mem.h"
using namespace std;

DWORD pid = 0;
string SaveFileName;
string ProcessName;

inline bool FileExist(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
string GetStr(string linha)
{
    string s = linha.substr(linha.find(":"),linha.size() );
    replaceAll(s, ":", "");
    replaceAll(s, " ", "");
    return s;
}
char* ConvertStringToChar(string str)
{
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    // do stuff
    return cstr;

}
unsigned char* ConvertBytesStrToString(string s1)
{
    unsigned char* val = new unsigned char[s1.length() + 1];
    strcpy((char*)val, s1.c_str());
    return val;
}
vector<string>Split(string s, string spliter)
{
    std::vector<string> ret;
    char* token = strtok(ConvertStringToChar(s), ConvertStringToChar(spliter));

    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL)
    {
        ret.push_back(string(token));
        //printf("%s\n", token);
        token = strtok(NULL, ConvertStringToChar(spliter));
    }
    return ret;
}

const wchar_t* ConvertStrToWchar_t(string str )
{
    std::wstring widestr = std::wstring(str.begin(), str.end());
    return widestr.c_str();
}
bool GetNextByte(char** pszString, unsigned char& rByte, bool& isWhiteSpace)
{
    do
    {
        if (*(*pszString) == '?')
        {
            rByte = 0;
            isWhiteSpace = true;
            *(*pszString)++;

            if (*(*pszString) == '?')
                *(*pszString)++;

            return true;
        }
        else if (isxdigit(**pszString))
        {
            isWhiteSpace = false;
            rByte = (unsigned char)(strtoul(*pszString, pszString, 16) & 0xFF);
            return true;
        }
    } while (*(*pszString)++);

    return false;
}

int Text2Hex(const char* pszString, unsigned char* pbArray, char* pszMask)
{
    int Count = 0;
    bool isWhiteSpace = false;

    if (pszMask)
        *pszMask = 0;

    if (GetNextByte(const_cast<char**>(&pszString), pbArray[Count], isWhiteSpace))
    {
        do
        {
            Count++;

            if (pszMask)
                strncat(pszMask, (isWhiteSpace) ? "?" : "x", sizeof(pszMask));

        } while (GetNextByte(const_cast<char**>(&pszString), pbArray[Count], isWhiteSpace));
    }

    return Count;
}
std::pair<std::vector<BYTE>, std::string> IDAToCode(std::string in_ida_sig)
{
    // variables
    char mask[MAXCHAR];
    std::vector<BYTE> vec_of_byte;
    unsigned char byte_array[MAXCHAR];

    // get ByteArray and Mask
    int count = Text2Hex(in_ida_sig.c_str(), byte_array, mask);

    // create array of bytes
    for (int i = 0; i < count; i++)
        vec_of_byte.push_back(byte_array[i]);

    return std::pair<std::vector<BYTE>, std::string>({ vec_of_byte, mask });
}

int main(int argc,char** argv)
{
    if (argc != 2)
    {
        std::cout << "[-] Incorrect usage" << endl;
        std::cout << "[-] OffsetDumper.exe Config.txt" << endl;
        system("pause");
        return -1;
    }
    std::string str(argv[1]);

    if (str.find(".txt") == string::npos)
    {
        std::cout << "[-] Config file must be .txt" << endl;
        system("pause");
        return -1;
    }

    if (!FileExist(str))
    {
        wcout << "[-] Could not open " << argv[1] << endl;
        system("pause");
        return -1;
    }

    std::vector<string>texto;
    std::string linha;
    std::ifstream file(argv[1]);
    while (std::getline(file, linha))
    {
        // Process str
        texto.push_back(linha);
    }

    SaveFileName = GetStr(texto[0]);
    ProcessName = GetStr(texto[1]);

    //Convert String to wchar_t*
    pid = mem::get_pid(ConvertStrToWchar_t(ProcessName));
    HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

    if (pid == 0)
    {
        std::cout << "[-] " << ProcessName << " not found" << endl;
        system("pause");
        return -1;
    }

    std::ofstream outfile(SaveFileName);

    time_t now = time(0);
    char* dt = ctime(&now);
    outfile << "#pragma once\n" << std::endl;

    outfile << "//"<<dt << std::endl;
    for (int i = 2; i < texto.size(); i++)
    {
        int index = texto[i].find("FindPattern(");
       
        if (index == string::npos)
        {
            outfile << texto[i] << std::endl;
            continue;
        }
         

        string final = texto[i].substr(index, texto[i].size() - index);
    
        string tempf = final;
        replaceAll(tempf, "FindPattern(", "");
        replaceAll(tempf, ")", "");


        //std::cout << "f "<< final << endl;
        vector<string> parameters = Split(tempf, ",");
        parameters = Split(tempf, ",");

     /*
        for (int i = 0; i < parameters.size(); i++)
        {
            std::cout << "["<< i << "] " << parameters[i] << endl;
        }
       */



        PatternSearch type;

        if (parameters[2] == "address")
            type = PatternSearch::address;
        else if (parameters[2] == "offset")
            type = PatternSearch::offset;
        else if (parameters[2] == "normal")
            type = PatternSearch::normal;
        else if (parameters[2] == "call_func")
            type = PatternSearch::call_func;
        else
            continue;

        int sizeb = 1;

        if (parameters.size() > 3)
            sizeb = std::stoi(parameters[4]);


        auto code = IDAToCode(parameters[1]);
        PatternStruct Ps = PatternStruct(
            (char*)code.first.data(),
            (char*)code.second.c_str(),
            type,
            std::stoi(parameters[3]), sizeb);

        const wchar_t* t = ConvertStrToWchar_t(parameters[0]);

        char hex[40];
        if(type == PatternSearch::normal || type == PatternSearch::call_func)
        {
            uint64_t ad = mem::FindPattern(proc, (wchar_t*)t, Ps);
            sprintf(hex, "%x", ad);
        }
        else
        {
            DWORD ad = mem::FindPattern(proc, (wchar_t*)t, Ps);
            sprintf(hex, "%x", ad);
        }
  
 


        string str = string(hex);

        replaceAll(texto[i], final, str);
        std::cout << texto[i] << endl;

        outfile << texto[i]<< std::endl;
    }




    system("pause");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
