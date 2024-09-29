	#pragma once
#include <Windows.h>
#include <string>
#include <shobjidl.h> 
#include "MD5.h"
#include <dbghelp.h>
#include "Globals.h"
#pragma comment(lib, "dbghelp.lib")

//credits to https://stackoverflow.com/questions/68601080/how-do-you-open-a-file-explorer-dialogue-in-c
std::string sSelectedFile = "None";
std::string sFilePath;
bool openFile()
{
    //  CREATE FILE OBJECT INSTANCE
    HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(f_SysHr))
        return FALSE;

    // CREATE FileOpenDialog OBJECT
    IFileOpenDialog* f_FileSystem;
    f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
    if (FAILED(f_SysHr)) {
        CoUninitialize();
        return FALSE;
    }

    //  SHOW OPEN FILE DIALOG WINDOW
    f_SysHr = f_FileSystem->Show(NULL);
    if (FAILED(f_SysHr)) {
        f_FileSystem->Release();
        CoUninitialize();
        return FALSE;
    }

    //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
    IShellItem* f_Files;
    f_SysHr = f_FileSystem->GetResult(&f_Files);
    if (FAILED(f_SysHr)) {
        f_FileSystem->Release();
        CoUninitialize();
        return FALSE;
    }

    //  STORE AND CONVERT THE FILE NAME
    PWSTR f_Path;
    f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
    if (FAILED(f_SysHr)) {
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();
        return FALSE;
    }

    //  FORMAT AND STORE THE FILE PATH
    std::wstring path(f_Path);
    std::string c(path.begin(), path.end());
    sFilePath = c;

    //  FORMAT STRING FOR EXECUTABLE NAME
    const size_t slash = sFilePath.find_last_of("/\\");
    sSelectedFile = sFilePath.substr(slash + 1);

    //  SUCCESS, CLEAN UP
    CoTaskMemFree(f_Path);
    f_Files->Release();
    f_FileSystem->Release();
    CoUninitialize();
    return TRUE;
}


bool IsBlacklisted(const std::string& pdbPath, const std::vector<std::string>& blacklist) {
    for (const auto& keyword : blacklist) {
        if (pdbPath.find(keyword) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void ScanPDBPath(const char* binaryPath) {
    HANDLE hFile = CreateFileA(binaryPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open binary file." << std::endl;
        return;
    }

    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapping == NULL) {
        std::cerr << "Failed to create file mapping." << std::endl;
        CloseHandle(hFile);
        return;
    }

    LPVOID pBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (pBase == NULL) {
        std::cerr << "Failed to map view of file." << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pBase;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Invalid DOS signature." << std::endl;
        Blacklists::PDBFlag = 3;
        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)pBase + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        std::cerr << "Invalid NT signature." << std::endl;
        Blacklists::PDBFlag = 3;
        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    DWORD debugDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
    if (debugDirRVA == 0) {
        std::cerr << "No debug directory found." << std::endl;
        Blacklists::PDBFlag = 3;
        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
    DWORD sectionCount = ntHeaders->FileHeader.NumberOfSections;
    DWORD debugDirOffset = 0;

    for (DWORD i = 0; i < sectionCount; ++i, ++section) {
        DWORD sectionStart = section->VirtualAddress;
        DWORD sectionEnd = sectionStart + section->SizeOfRawData;

        if (debugDirRVA >= sectionStart && debugDirRVA < sectionEnd) {
            debugDirOffset = debugDirRVA - sectionStart + section->PointerToRawData;
            break;
        }
    }

    if (debugDirOffset == 0) {
        std::cerr << "Failed to locate debug directory." << std::endl;
        Blacklists::PDBFlag = 3;
        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return;
    }

    PIMAGE_DEBUG_DIRECTORY debugDir = (PIMAGE_DEBUG_DIRECTORY)((BYTE*)pBase + debugDirOffset);

    struct RSDS_DEBUG_INFO {
        DWORD signature;
        GUID guid;
        DWORD age;
        char pdbFileName[1];  
    };

    if (debugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
        DWORD* signature = (DWORD*)((BYTE*)pBase + debugDir->PointerToRawData);
        if (*signature == 'SDSR') { 
            RSDS_DEBUG_INFO* rsdsInfo = (RSDS_DEBUG_INFO*)signature;
            if (IsBlacklisted(rsdsInfo->pdbFileName, Blacklists::PDBBlacklist))
            {
                Blacklists::PDBFlag = 2;
            }
            else
            {
                Blacklists::PDBFlag = 1;
            }
        }
        else {
            std::cerr << "No RSDS signature found." << std::endl;
            Blacklists::PDBFlag = 3;
        }
    }
    else {
        std::cerr << "No CodeView debug information found." << std::endl;
        Blacklists::PDBFlag = 3;
    }

    UnmapViewOfFile(pBase);
    CloseHandle(hMapping);
    CloseHandle(hFile);
}

bool result = FALSE;

void TabButton(const char* fmt, ImVec2 Pos, ImVec2 Size, int desired, int& range)
{

    ImGui::SetCursorPos(Pos);
    float Alpha = 0;
    const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();
    const auto& CurrentWindowPos = ImGui::GetWindowPos();
    ImVec2 P1, P2;
    ImDrawList* pDrawList;
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.10f, 0.48f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0 * dpi_scale);
    if (desired == range)
    {
        ImGui::ButtonMulticolored(fmt, Size, ImGuiButtonFlags_None, ImColor(246, 248, 250), ImColor(246, 248, 250), ImColor(246, 248, 250), ImColor(246, 248, 250), 2 * dpi_scale);
    }
    else
    {
        ImGui::ButtonMulticolored(fmt, Size, ImGuiButtonFlags_None, ImColor(18, 19, 31, (int)Alpha), ImColor(18, 19, 31, (int)Alpha), ImColor(18, 19, 31, (int)Alpha), ImColor(18, 19, 31, (int)Alpha), 2 * dpi_scale);
    }
    //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
    ImGui::PopStyleVar(2);
    if (ImGui::IsItemClicked())
    {
        //PlaySound(TEXT("pop.wav"), NULL, SND_FILENAME | SND_ASYNC);
        range = desired;
    }

}

bool IsExecutable(void* module) { //scanning for sig wont work otherwise
    auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return false; 
    }

    auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return false; 
    }

    return true;  
}


