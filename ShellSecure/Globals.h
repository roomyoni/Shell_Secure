#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")

float dpi_scale = 2.56f;
float dpi_settings = 2.56f;
float childscroll = 0;

int tabs = 0;

bool menu_active = true;

ImVec2 WindowSize(590, 425);

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

namespace gui
{
    static POINTS					guiPosition = { 1920, 1060 };
    static int                      title_height = 60;
    static int                      sidebar_width = 40;
}

namespace Settings
{
    bool AudioChange = false;
    bool DesktopFile = false;
    bool Geolocation = false;
    bool TimeZone = false;
}

namespace Blacklists
{
    std::vector<std::string> FileHashes
    {
        "7983b41b7664ceaa5451ec32319d1dac"
    };

    std::vector<std::string> PDBBlacklist = { 
        "UniversalCleaner" 
    };

    int HashFlag = 0;
    int PDBFlag = 0;

    struct BlacklistType
    {
        std::string Message;
        ImColor Color;
    };

    std::vector<BlacklistType> BlacklistMessages
    {
        {"No File Selected", ImColor(220, 220, 220, 255)},
        {"Not Detected", ImColor(0, 220, 0, 255)},
        {"Detected", ImColor(220, 0, 0, 255)},
        {"Not Applicable", ImColor(222, 125, 7, 255)}
    };
}