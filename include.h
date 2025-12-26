#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <string>
#include <thread>
#include <cmath>

#define WM_UPDATE_STATIC (WM_USER + 1)

// Version
#define WNDINFO_VERSION L"0.0.1"

// Actions
#define IDM_MINIMIZE 1001
#define IDM_EXIT 1002
#define IDM_MOVE 1003

// Positions
#define IDM_MOVE_TOPLEFT 2001
#define IDM_MOVE_TOPRIGHT 2002
#define IDM_MOVE_BOTTOMLEFT 2003
#define IDM_MOVE_BOTTOMRIGHT 2004
#define IDM_TOPMOST 2005

// Styles
#define IDM_ROUND_CORNER 3001

// Font size
#define IDM_FONT_BIG 3002
#define IDM_FONT_MEDIUM 3003
#define IDM_FONT_SMALL 3004
const int FONT_SIZE_BIG = 20;
const int FONT_SIZE_MEDIUM = 18;
const int FONT_SIZE_SMALL = 16;

// TRANSPARENCY value
#define IDM_TRANSPARENCY_OPAQUE 4001
#define IDM_TRANSPARENCY_HIGH 4002
#define IDM_TRANSPARENCY_MEDIUM 4003
#define IDM_TRANSPARENCY_LOW 4004
const int TRANSPARENCY_OPAQUE = 255;
const int TRANSPARENCY_HIGH = 160;
const int TRANSPARENCY_MEDIUM = 190;
const int TRANSPARENCY_LOW = 220;

// Window
const int MARGIN = 20;
const int WIN_WIDTH = 500;
const int WIN_HEIGHT = 580;

// Language
#define LANG__ENGLISH 110
#define LANG_SIMPLIFIED_CHINESE 111
#define IDM_LANG_ENG 5001
#define IDM_LANG_SIMPLIFIED_CHINESE 5002

// About
#define IDM_ABOUT 6001

// Config file
#define CONFIG_FILE_NAME L".\\wndinfo.ini"
#define CONFIG_SECTION L"Settings"

#define DEFAULT_LANGUAGE LANG__ENGLISH
#define DEFAULT_CORNER IDM_MOVE_TOPRIGHT
#define DEFAULT_FONT_SIZE 18     // FONT_SIZE_MEDIUM
#define DEFAULT_TRANSPARENCY 190 // TRANSPARENCY_MEDIUM
#define DEFAULT_ISROUNDCORNER FALSE
#define DEFAULT_ISTOPMOST TRUE

typedef LONG(NTAPI *pRtlGetVersion)(LPOSVERSIONINFOW);