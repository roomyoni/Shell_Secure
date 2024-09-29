//
//   ____  ____  ___   ________ 
//  / __ \/ __ \/   | / ____/ / 
// / / / / /_/ / /| |/ /   / /  
/// /_/ / _, _/ ___ / /___/ /___
//\____/_/ |_/_/  |_\____/_____/
//
//
//  SHELLSECURE ANTIVIRUS FOR SHELLHACKS 2024  
//  JADON MAEDER

#include <Windows.h>
#include <dwmapi.h>
#include <chrono>
#include <thread>
#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_freetype.h"
#include "FA.h"
#include "Functions.h"
#include <fstream>
#include "Communication.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool WriteRegistry(const std::string& keyPath, const std::string& valueName, const std::string& newValue) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)newValue.c_str(), newValue.size() + 1) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        RegCloseKey(hKey);
    }
    std::cerr << "Failed to write to registry key: " << keyPath << std::endl;
    return false;
}

std::string ReadRegistry(const std::string& keyPath, const std::string& valueName) {
    HKEY hKey;
    DWORD dataType;
    char data[256];
    DWORD dataSize = sizeof(data);
    std::string result = "";

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, valueName.c_str(), NULL, &dataType, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
            if (dataType == REG_SZ) {
                result = std::string(data, dataSize - 1);
            }
        }
        RegCloseKey(hKey);
    }
    else {
        std::cerr << "Failed to open registry key: " << keyPath << std::endl;
    }

    return result;
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        RECT wRect;
        if (::GetWindowRect(hWnd, &wRect)) {
        }

        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {

            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();

        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_LBUTTONDOWN:
        gui::guiPosition = MAKEPOINTS(lParam);
        break;

    case WM_MOUSEMOVE:
        if (wParam == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(lParam);
            auto rect = ::RECT{ };

            GetWindowRect(hWnd, &rect);

            rect.left += points.x - gui::guiPosition.x;
            rect.top += points.y - gui::guiPosition.y;

            if (gui::guiPosition.x >= 0 && gui::guiPosition.x >= 0 && gui::guiPosition.x <= (WindowSize.x * dpi_scale - (gui::title_height + (ImGui::GetStyle().WindowBorderSize * 2))) && gui::guiPosition.y >= 0 && gui::guiPosition.y <= gui::title_height)
            {
                Sleep(12);
                SetWindowPos(
                    hWnd,
                    HWND_TOPMOST,
                    rect.left, rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
            }

        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void MonitorRegistry()
{
    while (true)
    {
        std::string keyPath = "Software\\ShellSecure";
        std::string valueName1 = "Alert1";
        std::string valueName2 = "Alert2";
        std::string valueName3 = "Alert3";
        std::string valueName4 = "Alert4";

        std::string Alert1 = ReadRegistry(keyPath, valueName1);
        std::string Alert2 = ReadRegistry(keyPath, valueName2);
        std::string Alert3 = ReadRegistry(keyPath, valueName3);
        std::string Alert4 = ReadRegistry(keyPath, valueName4);

        if (Alert1 == "APPLAUNCH")
        {
            WriteRegistry(keyPath, valueName1, "null");
            MessageBox(NULL, L"WATCHDOG", L"NEW APP LAUNCHED", MB_OK | MB_TOPMOST);
        }
        else if (Alert2 == "AUDIOLAUNCH")
        {
            WriteRegistry(keyPath, valueName2, "null");
            MessageBox(NULL, L"WATCHDOG", L"AUDIO IS BEING RECORDED", MB_OK | MB_TOPMOST);
        }        
        else if (Alert3 == "GEOLOCATION")
        {
            WriteRegistry(keyPath, valueName3, "null");
            MessageBox(NULL, L"WATCHDOG", L"GEOLOCATION IS BEING ACCESSED", MB_OK | MB_TOPMOST);
        }        
        else if (Alert4 == "TIMEZONE")
        {
            WriteRegistry(keyPath, valueName4, "null");
            MessageBox(NULL, L"WATCHDOG", L"TIMEZONE IS BEING MODIFIED", MB_OK | MB_TOPMOST);
        }
        Sleep(200);
    }
}

int main()
{
    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    float screen_width = devMode.dmPelsWidth;
    float screen_height = devMode.dmPelsHeight;
    float screen_center_x = screen_width / 2;
    float screen_center_y = screen_height / 2;
    dpi_scale = (screen_width / 1180);
    dpi_settings = (screen_width / 1180);
    Minimalist_Driver->InitComm();

    std::thread MonitorThread(MonitorRegistry);
    MonitorThread.detach();

    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"MainWindow", NULL };
    ::RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Minimalist", WS_EX_LAYERED | WS_POPUP, 300, 300, WindowSize.x * dpi_scale, WindowSize.y * dpi_scale, NULL, NULL, wc.hInstance, NULL);
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }


    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplDX9_Init(g_pd3dDevice);

    auto config = ImFontConfig();
    config.FontDataOwnedByAtlas = false;
    config.OversampleH = config.OversampleV = 1;
    config.MergeMode = true;
    config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
    static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true; icons_config.OversampleV = 1;

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 12 * dpi_scale);
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 9 * dpi_scale, &config, ranges);
    io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 11 * dpi_scale, &icons_config, icons_ranges);


    ImFont* Segoemedium = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 15.0f * dpi_scale);
    Segoemedium = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 12 * dpi_scale, &config, ranges);
    Segoemedium = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 23 * dpi_scale, &icons_config, icons_ranges);

    ImFont* Segoebold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 22.0f * dpi_scale);
    Segoebold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 20 * dpi_scale, &config, ranges);
    Segoebold = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 28 * dpi_scale, &icons_config, icons_ranges);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui_ImplWin32_Init(hwnd);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;

    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();


        ImGui::NewFrame();

        style.Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255, 255);
        style.Colors[ImGuiCol_ChildBg] = ImColor(241, 245, 249, 255);
        style.Colors[ImGuiCol_Button] = ImColor(0, 122, 255, 255);
        style.Colors[ImGuiCol_ButtonHovered] = ImColor(0, 100, 210, 255);
        style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 88, 185, 255);
        style.Colors[ImGuiCol_Border] = ImColor(72, 136, 223, 0);
        style.Colors[ImGuiCol_Text] = ImColor(58, 59, 64, 255);

        style.WindowPadding = ImVec2(0, 0);
        style.WindowBorderSize = 0;
        style.FrameBorderSize = 0.5 * dpi_scale;
        style.WindowRounding = 6 * dpi_scale;
        style.ChildRounding = 3 * dpi_scale;
        style.FrameRounding = 3;
        style.ItemInnerSpacing = ImVec2(6, 6);
        style.ScrollbarSize = 5.6f * dpi_scale;
        style.ItemSpacing = ImVec2(27 * dpi_scale, 0);
        style.ButtonTextAlign = ImVec2(0.5, 0.5);

        ImGui::SetNextWindowSize(ImVec2(WindowSize.x * dpi_scale, WindowSize.y * dpi_scale));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("SHELLSECURE", &menu_active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        const auto& p = ImGui::GetWindowPos();
        const auto& pWindowDrawList = ImGui::GetWindowDrawList();
        const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
        const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();

        ImGui::SetCursorPos(ImVec2(15 * dpi_scale, 10 * dpi_scale));
        ImGui::PushFont(Segoebold);
        ImGui::TextColored(ImColor(0, 0, 0, 255), "ShellSecure");
        ImGui::PopFont();
        ImGui::SetCursorPos(ImVec2(15 * dpi_scale, 35 * dpi_scale));
        ImGui::TextColored(ImColor(154, 153, 156), "Shellhacks 2024");

        TabButton(ICON_FA_FILE "   File Scanner", ImVec2(15 * dpi_scale, 80 * dpi_scale), ImVec2(115, 20), 0, tabs);
        TabButton(ICON_FA_DOG "  Watch-Dog", ImVec2(15 * dpi_scale, 110 * dpi_scale), ImVec2(115, 20), 1, tabs);

        ImGui::SetCursorPos(ImVec2(150 * dpi_scale, 0));
        ImGui::BeginChild("##ChildWindow", ImVec2(440, 425 ), false);

        if (tabs == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255, 255, 255, 255));
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 70 * dpi_scale));
            ImGui::PushFont(Segoemedium);
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 70 * dpi_scale));
            ImGui::Text("File Report");
            ImGui::PopFont();
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 90 * dpi_scale));

            ImGui::BeginChild("##ChildWindow2", ImVec2(380, 200), false);

            // File Hash Blacklist
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 10 * dpi_scale));
            ImGui::Text("File Hash Blacklist");
            ImGui::SetCursorPos(ImVec2((365 * dpi_scale - (ImGui::CalcTextSize(Blacklists::BlacklistMessages[Blacklists::HashFlag].Message.c_str()).x)), 10 * dpi_scale));
            ImGui::TextColored(Blacklists::BlacklistMessages[Blacklists::HashFlag].Color, Blacklists::BlacklistMessages[Blacklists::HashFlag].Message.c_str());
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 30 * dpi_scale));
            ImGui::Separator();

            // PDB Blacklist
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
            ImGui::Text("PDB Blacklist");
            ImGui::SetCursorPos(ImVec2((365 * dpi_scale - (ImGui::CalcTextSize(Blacklists::BlacklistMessages[Blacklists::PDBFlag].Message.c_str()).x)), 40 * dpi_scale));
            ImGui::TextColored(Blacklists::BlacklistMessages[Blacklists::PDBFlag].Color, Blacklists::BlacklistMessages[Blacklists::PDBFlag].Message.c_str());
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 60 * dpi_scale));
            ImGui::Separator();


            ImGui::EndChild();


            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 300 * dpi_scale));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
            if (ImGui::Button("Upload File", ImVec2(40 * dpi_scale, 10 * dpi_scale)))
            {
                result = openFile();
                if (result)
                {
                    std::ifstream inBigArrayfile;
                    inBigArrayfile.open(sFilePath.c_str(), std::ios::binary | std::ios::in);

                    inBigArrayfile.seekg(0, std::ios::end);
                    long Length = inBigArrayfile.tellg();
                    inBigArrayfile.seekg(0, std::ios::beg);

                    char* InFileData = new char[Length];
                    inBigArrayfile.read(InFileData, Length);

                    std::string Temp = md5(InFileData, Length);


                    for (auto& MD5HASH : Blacklists::FileHashes) {
                        if (MD5HASH == Temp)
                        {
                            Blacklists::HashFlag = 2;
                            break;
                        }
                        Blacklists::HashFlag = 1;
                    }
                    delete[] InFileData;

                    ScanPDBPath(sFilePath.c_str());
                }
                else
                {
                    MessageBox(hwnd, L"Failed to Open File!", L"ERROR!", MB_OK);
                }
            }
            ImGui::PopStyleColor(2);
        }
        
        if (tabs == 1)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255, 255, 255, 255));
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 70 * dpi_scale));
            ImGui::PushFont(Segoemedium);
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 70 * dpi_scale));
            ImGui::Text("Watch-Dog");
            ImGui::PopFont();
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 90 * dpi_scale));

            ImGui::BeginChild("##ChildWindow2", ImVec2(380, 200), false);
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 10 * dpi_scale));
            ImGui::Checkbox("Alert for Audio Access", &Settings::AudioChange);
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 30 * dpi_scale));
            ImGui::Separator();

            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
            ImGui::Checkbox("Alert for Executed Files", &Settings::DesktopFile);
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 60 * dpi_scale));
            ImGui::Separator();

            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 70 * dpi_scale));
            ImGui::Checkbox("Alert for Geolocation", &Settings::Geolocation);
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 90 * dpi_scale));
            ImGui::Separator();

            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 100 * dpi_scale));
            ImGui::Checkbox("Alert for Time Zone Changes", &Settings::TimeZone);
            ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 120 * dpi_scale));
            ImGui::Separator();

            ImGui::EndChild();
            ImGui::SetCursorPos(ImVec2(30 * dpi_scale, 300 * dpi_scale));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
            if (ImGui::Button("Apply Changes", ImVec2(40 * dpi_scale, 10 * dpi_scale)))
            {
                Minimalist_Driver->ApplyChanges(Settings::AudioChange, Settings::DesktopFile, Settings::Geolocation, Settings::TimeZone);
            }
            ImGui::PopStyleColor(2);
        }

        ImGui::EndChild();

        ImGui::End();
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    exit(0);
}