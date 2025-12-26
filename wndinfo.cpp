#include "include.h"

HWND hwnd;
HFONT hTextFont;
HWND hLabel;
POINT mousePos;

int currentLanguage = LANG__ENGLISH;
int currentCorner = 0;
int currentFontSize = 0;
int currentTransparency = 0;
BOOL isWindows11OrGreater = FALSE;
BOOL isRoundCorner = FALSE;
BOOL isTopMost = TRUE;

BOOL isConfigFileExistsBefore = TRUE;

BOOL IsWindows11OrGreater()
{
    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtDll)
        return FALSE;

    pRtlGetVersion RtlGetVersion = (pRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
    if (!RtlGetVersion)
        return FALSE;

    OSVERSIONINFOW osvi = {0};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (RtlGetVersion(&osvi) != 0)
        return FALSE;

    return (osvi.dwBuildNumber >= 22000);
}

void CenterWindow(HWND hwnd)
{
    RECT rcWork;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);

    RECT rcWnd;
    GetWindowRect(hwnd, &rcWnd);
    int winWidth = rcWnd.right - rcWnd.left;
    int winHeight = rcWnd.bottom - rcWnd.top;

    int x = rcWork.left + (rcWork.right - rcWork.left - winWidth) / 2;
    int y = rcWork.top + (rcWork.bottom - rcWork.top - winHeight) / 2;

    SetWindowPos(hwnd, NULL, x, y, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void MoveWindowToCorner(HWND hwnd, int corner)
{
    RECT rcWork;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);

    int x = 0, y = 0;

    switch (corner)
    {
    case IDM_MOVE_TOPLEFT:
        x = rcWork.left + MARGIN;
        y = rcWork.top + MARGIN;
        break;
    case IDM_MOVE_TOPRIGHT:
        x = rcWork.right - WIN_WIDTH - MARGIN;
        y = rcWork.top + MARGIN;
        break;
    case IDM_MOVE_BOTTOMLEFT:
        x = rcWork.left + MARGIN;
        y = rcWork.bottom - WIN_HEIGHT - MARGIN;
        break;
    case IDM_MOVE_BOTTOMRIGHT:
        x = rcWork.right - WIN_WIDTH - MARGIN;
        y = rcWork.bottom - WIN_HEIGHT - MARGIN;
        break;
    default:
        return;
    }

    SetWindowPos(hwnd, NULL, x, y, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void SetWindowRoundCorner(HWND hwnd)
{
    int cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
    isRoundCorner = TRUE;
}

void SetWindowSquareCorner(HWND hwnd)
{
    int cornerPreference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
    isRoundCorner = FALSE;
}

void SetFontSize(int fontSize)
{
    if (fontSize == currentFontSize)
        return;

    if (hTextFont)
    {
        DeleteObject(hTextFont);
        hTextFont = NULL;
    }

    hTextFont = CreateFontW(
        -fontSize,
        0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Microsoft YaHei");

    if (hTextFont && hLabel)
    {
        SendMessageW(hLabel, WM_SETFONT, (WPARAM)hTextFont, TRUE);
    }

    currentFontSize = fontSize;
    InvalidateRect(hwnd, NULL, TRUE);
}

BOOL IsConfigFileExists()
{
    DWORD attr = GetFileAttributesW(CONFIG_FILE_NAME);
    return (attr != INVALID_FILE_ATTRIBUTES);
}

void SaveConfig()
{
    wchar_t buf[32];

    _itow_s(currentLanguage, buf, _countof(buf), 10);
    WritePrivateProfileStringW(CONFIG_SECTION, L"currentLanguage", buf, CONFIG_FILE_NAME);

    _itow_s(currentCorner, buf, _countof(buf), 10);
    WritePrivateProfileStringW(CONFIG_SECTION, L"currentCorner", buf, CONFIG_FILE_NAME);

    _itow_s(currentFontSize, buf, _countof(buf), 10);
    WritePrivateProfileStringW(CONFIG_SECTION, L"currentFontSize", buf, CONFIG_FILE_NAME);

    _itow_s(currentTransparency, buf, _countof(buf), 10);
    WritePrivateProfileStringW(CONFIG_SECTION, L"currentTransparency", buf, CONFIG_FILE_NAME);

    WritePrivateProfileStringW(CONFIG_SECTION, L"isTopMost", isTopMost ? L"1" : L"0", CONFIG_FILE_NAME);

    WritePrivateProfileStringW(CONFIG_SECTION, L"isRoundCorner", isRoundCorner ? L"1" : L"0", CONFIG_FILE_NAME);
}

void LoadConfig()
{
    if (!IsConfigFileExists())
    {
        isConfigFileExistsBefore = FALSE;

        currentLanguage = DEFAULT_LANGUAGE;
        currentCorner = DEFAULT_CORNER;
        currentFontSize = DEFAULT_FONT_SIZE;
        currentTransparency = DEFAULT_TRANSPARENCY;
        isTopMost = DEFAULT_ISTOPMOST;
        isRoundCorner = DEFAULT_ISROUNDCORNER;

        SaveConfig();
        return;
    }

    wchar_t buffer[64] = {0};

    // Language
    GetPrivateProfileStringW(CONFIG_SECTION, L"currentLanguage", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    currentLanguage = (buffer[0] ? _wtoi(buffer) : DEFAULT_LANGUAGE);

    // Corner
    GetPrivateProfileStringW(CONFIG_SECTION, L"currentCorner", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    currentCorner = (buffer[0] ? _wtoi(buffer) : DEFAULT_CORNER);

    // Font size
    GetPrivateProfileStringW(CONFIG_SECTION, L"currentFontSize", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    currentFontSize = (buffer[0] ? _wtoi(buffer) : DEFAULT_FONT_SIZE);

    // Transparency
    GetPrivateProfileStringW(CONFIG_SECTION, L"currentTransparency", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    currentTransparency = (buffer[0] ? _wtoi(buffer) : DEFAULT_TRANSPARENCY);

    // TopMost
    GetPrivateProfileStringW(CONFIG_SECTION, L"isTopMost", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    isTopMost = (buffer[0] ? (_wtoi(buffer) != 0) : DEFAULT_ISTOPMOST);

    // RoundCorner
    GetPrivateProfileStringW(CONFIG_SECTION, L"isRoundCorner", L"", buffer, _countof(buffer), CONFIG_FILE_NAME);
    isRoundCorner = (buffer[0] ? (_wtoi(buffer) != 0) : DEFAULT_ISROUNDCORNER);
}

std::wstring GetProcessNameByID(DWORD processID)
{
    std::wstring procssName = L"";
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        MessageBoxW(NULL, L"Unable to create snapshot of processes.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnapshot, &pe32))
    {
        MessageBoxW(NULL, L"Unable to get process information.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        CloseHandle(hSnapshot);
        exit(1);
    }

    while (Process32NextW(hSnapshot, &pe32))
    {
        if (pe32.th32ProcessID == processID)
        {
            procssName = pe32.szExeFile;
            break;
        }
    }

    CloseHandle(hSnapshot);
    return procssName;
}

std::wstring GetDpiAwarenessByHwnd(HWND hwnd)
{
    DPI_AWARENESS_CONTEXT awareness = GetWindowDpiAwarenessContext(hwnd);

    if (!awareness)
    {
        if (currentLanguage == LANG__ENGLISH)
            return L"Failed to get DPI information";
        else if (currentLanguage == LANG_CHINESE_SIMPLIFIED)
            return L"获取DPI信息失败";
    }
    else
    {
        if (AreDpiAwarenessContextsEqual(awareness, DPI_AWARENESS_CONTEXT_UNAWARE))
        {
            return L"Unaware";
        }
        else if (AreDpiAwarenessContextsEqual(awareness, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return L"System Aware";
        }
        else if (AreDpiAwarenessContextsEqual(awareness, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return L"Per-Monitor Aware";
        }
        else if (AreDpiAwarenessContextsEqual(awareness, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        {
            return L"Per-Monitor Aware V2";
        }
        else if (AreDpiAwarenessContextsEqual(awareness, DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED))
        {
            return L"Unaware (GDI Scaled)";
        }
        else
        {
            if (currentLanguage == LANG__ENGLISH)
                return L"Unknown";
            else if (currentLanguage == LANG_CHINESE_SIMPLIFIED)
                return L"未知";
        }
    }

    return L"???";
}

std::wstring AnsiToWide(const char *ansi)
{
    if (!ansi)
        return L"";
    int len = static_cast<int>(strlen(ansi));
    int size = MultiByteToWideChar(CP_ACP, 0, ansi, len, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_ACP, 0, ansi, len, &wstr[0], size);
    return wstr;
}

void ShowAboutMessage()
{
    std::wstring aboutMessage = L"";

    aboutMessage += L"wndinfo ";
    aboutMessage += WNDINFO_VERSION;
    aboutMessage += L"\n";

    if (currentLanguage == LANG__ENGLISH)
    {
        aboutMessage += L"Window information viewing tool for researching and debugging on Windows\n\n";
        aboutMessage += L"Repository link: https://github.com/SJEllipses/wndinfo\n";

        aboutMessage += L"Build time: " + AnsiToWide(__DATE__) + L" " + AnsiToWide(__TIME__) + L"\n";
#if defined(_MSC_VER)
        aboutMessage += L"Compiler: MSVC " + std::to_wstring(_MSC_VER) + L"\n";
#elif defined(__GNUC__)
        aboutMessage += L"Compiler: GCC " + std::to_wstring(__GNUC__) + L"." + std::to_wstring(__GNUC_MINOR__) + L"." + std::to_wstring(__GNUC_PATCHLEVEL__) + L"\n";
#endif

        MessageBoxW(hwnd, aboutMessage.c_str(), L"About wndinfo", MB_OK);
    }
    else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
    {
        aboutMessage += L"Windows 系统下的窗口信息查看工具，用于学习和问题分析\n\n";
        aboutMessage += L"仓库链接: https://github.com/SJEllipses/wndinfo\n";

        aboutMessage += L"构建时间: " + AnsiToWide(__DATE__) + L" " + AnsiToWide(__TIME__) + L"\n";
#if defined(_MSC_VER)
        aboutMessage += L"编译器: MSVC " + std::to_wstring(_MSC_VER) + L"\n";
#elif defined(__GNUC__)
        aboutMessage += L"编译器: GCC " + std::to_wstring(__GNUC__) + L"." + std::to_wstring(__GNUC_MINOR__) + L"." + std::to_wstring(__GNUC_PATCHLEVEL__) + L"\n";
#endif

        MessageBoxW(hwnd, aboutMessage.c_str(), L"关于 wndinfo", MB_OK);
    }
}

std::wstring GetForegroundWindowText()
{
    std::wstring windowTextInf = L"";
    if (currentLanguage == LANG__ENGLISH)
    {
        windowTextInf = L"Window information of foreground window:\n";
    }
    else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
    {
        windowTextInf = L"前景窗口信息:\n";
    }

    HWND hForeground = GetForegroundWindow();
    if (hForeground)
    {
        wchar_t title[256];
        GetWindowTextW(hForeground, title, sizeof(title));

        wchar_t className[256];
        GetClassNameW(hForeground, className, sizeof(className));

        DWORD processId = 0;
        GetWindowThreadProcessId(hForeground, &processId);
        std::wstring procssName = GetProcessNameByID(processId);

        if (currentLanguage == LANG__ENGLISH)
        {
            windowTextInf += L"Window Title: " + std::wstring(title) + L"\n";
            windowTextInf += L"Window Handle: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hForeground)) + L"\n";
            windowTextInf += L"Window Class: " + std::wstring(className) + L"\n";
            windowTextInf += L"Window PID: " + std::to_wstring(processId) + L"\n";
            windowTextInf += L"Window Process Name: " + procssName + L"\n";
            windowTextInf += L"Window DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hForeground)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
            windowTextInf += L"Window DPI Awareness: " + GetDpiAwarenessByHwnd(hForeground) + L"\n";
        }
        else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
        {
            windowTextInf += L"窗口标题: " + std::wstring(title) + L"\n";
            windowTextInf += L"窗口句柄: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hForeground)) + L"\n";
            windowTextInf += L"窗口类名: " + std::wstring(className) + L"\n";
            windowTextInf += L"窗口进程ID: " + std::to_wstring(processId) + L"\n";
            windowTextInf += L"窗口进程名: " + procssName + L"\n";
            windowTextInf += L"窗口DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hForeground)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
            windowTextInf += L"窗口DPI感知: " + GetDpiAwarenessByHwnd(hForeground) + L"\n";
        }
    }
    else
    {
        if (currentLanguage == LANG__ENGLISH)
        {
            windowTextInf += L"No Window\n";
        }
        else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
        {
            windowTextInf += L"无窗口\n";
        }
    }

    return windowTextInf;
}

std::wstring GetMousePosWindowText(HWND hwnd)
{
    std::wstring windowTextInf = L"";
    if (currentLanguage == LANG__ENGLISH)
    {
        windowTextInf = L"\nWindow information of mouse pointing:\n";
    }
    else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
    {
        windowTextInf = L"\n鼠标指针所处窗口信息:\n";
    }

    if (GetCursorPos(&mousePos))
    {
        HWND hTarget = WindowFromPoint(mousePos);

        if (hTarget == hLabel)
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(hTarget, &processId);
            std::wstring procssName = GetProcessNameByID(processId);

            if (currentLanguage == LANG__ENGLISH)
            {
                windowTextInf += L"Window Title: " + std::wstring(L"wndinfo") + L"\n";
                windowTextInf += L"Window Handle: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hTarget)) + L"\n";
                windowTextInf += L"Window Class: " + std::wstring(L"Static") + L"\n";
                windowTextInf += L"Window PID: " + std::to_wstring(processId) + L"\n";
                windowTextInf += L"Window Process Name: " + std::wstring(procssName) + L"\n";
                windowTextInf += L"Window DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hTarget)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
                windowTextInf += L"Window DPI Awareness: " + GetDpiAwarenessByHwnd(hTarget) + L"\n";
            }
            else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
            {
                windowTextInf += L"窗口标题: " + std::wstring(L"wndinfo") + L"\n";
                windowTextInf += L"窗口句柄: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hTarget)) + L"\n";
                windowTextInf += L"窗口类名: " + std::wstring(L"Static") + L"\n";
                windowTextInf += L"窗口进程ID: " + std::to_wstring(processId) + L"\n";
                windowTextInf += L"窗口进程名: " + std::wstring(procssName) + L"\n";
                windowTextInf += L"窗口DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hTarget)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
                windowTextInf += L"窗口DPI感知: " + GetDpiAwarenessByHwnd(hTarget) + L"\n";
            }

            return windowTextInf;
        }
        else if (!hTarget)
        {
            if (currentLanguage == LANG__ENGLISH)
            {
                windowTextInf += L"No Window";
            }
            else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
            {
                windowTextInf += L"无窗口";
            }

            return windowTextInf;
        }
        else
        {
            wchar_t title[256];
            GetWindowTextW(hTarget, title, sizeof(title));
            wchar_t className[256];
            GetClassNameW(hTarget, className, sizeof(className));

            DWORD processId = 0;
            GetWindowThreadProcessId(hTarget, &processId);
            std::wstring procssName = GetProcessNameByID(processId);

            if (currentLanguage == LANG__ENGLISH)
            {
                windowTextInf += L"Window Title: " + std::wstring(title) + L"\n";
                windowTextInf += L"Window Handle: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hTarget)) + L"\n";
                windowTextInf += L"Window Class: " + std::wstring(className) + L"\n";
                windowTextInf += L"Window PID: " + std::to_wstring(processId) + L"\n";
                windowTextInf += L"Window Process Name: " + std::wstring(procssName) + L"\n";
                windowTextInf += L"Window DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hTarget)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
                windowTextInf += L"Window DPI Awareness: " + GetDpiAwarenessByHwnd(hTarget) + L"\n";
            }
            else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
            {
                windowTextInf += L"窗口标题: " + std::wstring(title) + L"\n";
                windowTextInf += L"窗口句柄: 0x" + std::to_wstring(reinterpret_cast<uintptr_t>(hTarget)) + L"\n";
                windowTextInf += L"窗口类名: " + std::wstring(className) + L"\n";
                windowTextInf += L"窗口进程ID: " + std::to_wstring(processId) + L"\n";
                windowTextInf += L"窗口进程名: " + std::wstring(procssName) + L"\n";
                windowTextInf += L"窗口DPI: " + std::to_wstring(static_cast<int>(std::round(static_cast<double>(GetDpiForWindow(hTarget)) / USER_DEFAULT_SCREEN_DPI * 100.0))) + L"%\n";
                windowTextInf += L"窗口DPI感知: " + GetDpiAwarenessByHwnd(hTarget) + L"\n";
            }
        }
    }

    return windowTextInf;
}

void BackgroundThread(HWND hwnd)
{
    for (;;)
    {
        std::wstring Texts = GetForegroundWindowText() + GetMousePosWindowText(hwnd);
        SendMessageW(hwnd, WM_UPDATE_STATIC, 0, reinterpret_cast<LPARAM>(Texts.c_str()));
        Sleep(100);
    }
}

LRESULT WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_DESTROY:
    {
        DeleteObject(hTextFont);
        PostQuitMessage(0);
        break;
    }

    case WM_CREATE:
    {
        hTextFont = CreateFontW(
            -currentFontSize,
            0, 0, 0,
            FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            L"Microsoft YaHei");

        hLabel = CreateWindowW(
            L"STATIC",
            L"",
            WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY,
            20, 20,
            460, 540,
            hwnd,
            NULL,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL);

        SendMessage(hLabel, WM_SETFONT, (WPARAM)hTextFont, TRUE);

        std::thread(BackgroundThread, hwnd).detach();
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }

    case WM_UPDATE_STATIC:
    {
        const wchar_t *text = (const wchar_t *)lParam;

        HDC hdc = GetDC(hLabel);
        RECT rect;
        GetClientRect(hLabel, &rect);

        HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); // Use white brush
        FillRect(hdc, &rect, hBrush);

        SetWindowTextW(hLabel, text);

        ReleaseDC(hLabel, hdc);
        break;
    }

    case WM_CONTEXTMENU:
    {
        if (lParam == -1)
        {
            return 0;
        }

        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        HMENU hMenu = CreatePopupMenu();

        AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"wndinfo");

        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

        AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== Languages 语言 ===");
        AppendMenuW(hMenu, MF_STRING | (currentLanguage == LANG__ENGLISH ? MF_CHECKED : 0), IDM_LANG_ENG, L"English");
        AppendMenuW(hMenu, MF_STRING | (currentLanguage == LANG_SIMPLIFIED_CHINESE ? MF_CHECKED : 0), IDM_LANG_SIMPLIFIED_CHINESE, L"简体中文");

        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

        if (currentLanguage == LANG__ENGLISH)
        {
            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== Position ===");

            HMENU hMoveMenu = CreatePopupMenu();
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_TOPLEFT ? MF_CHECKED : 0), IDM_MOVE_TOPLEFT, L"Top Left");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_TOPRIGHT ? MF_CHECKED : 0), IDM_MOVE_TOPRIGHT, L"Top Right");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_BOTTOMLEFT ? MF_CHECKED : 0), IDM_MOVE_BOTTOMLEFT, L"Left Bottom");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_BOTTOMRIGHT ? MF_CHECKED : 0), IDM_MOVE_BOTTOMRIGHT, L"Right Bottom");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMoveMenu, L"Move to");

            AppendMenuW(hMenu, MF_STRING | (isTopMost ? MF_CHECKED : 0), IDM_TOPMOST, L"Always Top Most");

            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== Styles ===");
            AppendMenuW(hMenu, MF_STRING | (isWindows11OrGreater ? 0 : MF_DISABLED) | (isRoundCorner ? MF_CHECKED : 0), IDM_ROUND_CORNER, L"Rounded Corner Border");

            HMENU hFontMenu = CreatePopupMenu();
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_BIG ? MF_CHECKED : 0), IDM_FONT_BIG, L"Big");
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_MEDIUM ? MF_CHECKED : 0), IDM_FONT_MEDIUM, L"Medium");
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_SMALL ? MF_CHECKED : 0), IDM_FONT_SMALL, L"Small");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFontMenu, L"Font Size");

            HMENU hTransparencyMenu = CreatePopupMenu();
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_HIGH ? MF_CHECKED : 0), IDM_TRANSPARENCY_HIGH, L"High");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_MEDIUM ? MF_CHECKED : 0), IDM_TRANSPARENCY_MEDIUM, L"Medium");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_LOW ? MF_CHECKED : 0), IDM_TRANSPARENCY_LOW, L"Low");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_OPAQUE ? MF_CHECKED : 0), IDM_TRANSPARENCY_OPAQUE, L"Opaque");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hTransparencyMenu, L"Transparency");

            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== Actions ===");
            AppendMenuW(hMenu, MF_STRING, IDM_MOVE, L"Move");
            AppendMenuW(hMenu, MF_STRING, IDM_MINIMIZE, L"Minimize");
            AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Close");
        }
        else if (currentLanguage == LANG_SIMPLIFIED_CHINESE)
        {
            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== 位置 ===");

            HMENU hMoveMenu = CreatePopupMenu();
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_TOPLEFT ? MF_CHECKED : 0), IDM_MOVE_TOPLEFT, L"左上角");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_TOPRIGHT ? MF_CHECKED : 0), IDM_MOVE_TOPRIGHT, L"右上角");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_BOTTOMLEFT ? MF_CHECKED : 0), IDM_MOVE_BOTTOMLEFT, L"左下角");
            AppendMenuW(hMoveMenu, MF_STRING | (currentCorner == IDM_MOVE_BOTTOMRIGHT ? MF_CHECKED : 0), IDM_MOVE_BOTTOMRIGHT, L"右下角");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMoveMenu, L"移动到");

            AppendMenuW(hMenu, MF_STRING | (isTopMost ? MF_CHECKED : 0), IDM_TOPMOST, L"保持在最前端");

            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== 样式 ===");
            AppendMenuW(hMenu, MF_STRING | (isWindows11OrGreater ? 0 : MF_DISABLED) | (isRoundCorner ? MF_CHECKED : 0), IDM_ROUND_CORNER, L"圆角边框");

            HMENU hFontMenu = CreatePopupMenu();
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_BIG ? MF_CHECKED : 0), IDM_FONT_BIG, L"大");
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_MEDIUM ? MF_CHECKED : 0), IDM_FONT_MEDIUM, L"中");
            AppendMenuW(hFontMenu, MF_STRING | (currentFontSize == FONT_SIZE_SMALL ? MF_CHECKED : 0), IDM_FONT_SMALL, L"小");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFontMenu, L"字体大小");

            HMENU hTransparencyMenu = CreatePopupMenu();
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_HIGH ? MF_CHECKED : 0), IDM_TRANSPARENCY_HIGH, L"高");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_MEDIUM ? MF_CHECKED : 0), IDM_TRANSPARENCY_MEDIUM, L"中");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_LOW ? MF_CHECKED : 0), IDM_TRANSPARENCY_LOW, L"低");
            AppendMenuW(hTransparencyMenu, MF_STRING | (currentTransparency == TRANSPARENCY_OPAQUE ? MF_CHECKED : 0), IDM_TRANSPARENCY_OPAQUE, L"不透明");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hTransparencyMenu, L"透明度");

            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

            AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"=== 操作 ===");
            AppendMenuW(hMenu, MF_STRING, IDM_MOVE, L"移动");
            AppendMenuW(hMenu, MF_STRING, IDM_MINIMIZE, L"最小化");
            AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"关闭");
        }

        SetForegroundWindow(hwnd);
        TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);

        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDM_ABOUT:
            ShowAboutMessage();
            break;

        case IDM_LANG_ENG:
            currentLanguage = LANG__ENGLISH;
            break;
        case IDM_LANG_SIMPLIFIED_CHINESE:
            currentLanguage = LANG_SIMPLIFIED_CHINESE;
            break;

        case IDM_MINIMIZE:
            ShowWindow(hwnd, SW_MINIMIZE);
            break;
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        case IDM_MOVE:
            RECT rcBefore;
            GetWindowRect(hwnd, &rcBefore);

            SendMessageW(hwnd, WM_SYSCOMMAND, SC_MOVE, 0);

            RECT rcAfter;
            GetWindowRect(hwnd, &rcAfter);

            if (rcBefore.left != rcAfter.left || rcBefore.top != rcAfter.top)
            {
                currentCorner = 0;
            }
            break;

        case IDM_MOVE_TOPLEFT:
        case IDM_MOVE_TOPRIGHT:
        case IDM_MOVE_BOTTOMLEFT:
        case IDM_MOVE_BOTTOMRIGHT:
            MoveWindowToCorner(hwnd, LOWORD(wParam));
            currentCorner = LOWORD(wParam);
            break;

        case IDM_FONT_SMALL:
            SetFontSize(FONT_SIZE_SMALL);
            break;
        case IDM_FONT_MEDIUM:
            SetFontSize(FONT_SIZE_MEDIUM);
            break;
        case IDM_FONT_BIG:
            SetFontSize(FONT_SIZE_BIG);
            break;

        case IDM_TOPMOST:
            if (isTopMost)
            {
                SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                isTopMost = FALSE;
            }
            else
            {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                isTopMost = TRUE;
            }
            break;

        case IDM_ROUND_CORNER:
            if (isRoundCorner)
            {
                SetWindowSquareCorner(hwnd);
                isRoundCorner = FALSE;
            }
            else
            {
                SetWindowRoundCorner(hwnd);
                isRoundCorner = TRUE;
            }
            break;

        case IDM_TRANSPARENCY_OPAQUE:
            currentTransparency = TRANSPARENCY_OPAQUE;
            SetLayeredWindowAttributes(hwnd, 0, currentTransparency, LWA_ALPHA);
            break;
        case IDM_TRANSPARENCY_HIGH:
            currentTransparency = TRANSPARENCY_HIGH;
            SetLayeredWindowAttributes(hwnd, 0, currentTransparency, LWA_ALPHA);
            break;
        case IDM_TRANSPARENCY_MEDIUM:
            currentTransparency = TRANSPARENCY_MEDIUM;
            SetLayeredWindowAttributes(hwnd, 0, currentTransparency, LWA_ALPHA);
            break;
        case IDM_TRANSPARENCY_LOW:
            currentTransparency = TRANSPARENCY_LOW;
            SetLayeredWindowAttributes(hwnd, 0, currentTransparency, LWA_ALPHA);
            break;
        }
        break;
    }

    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    MSG msg;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

    LoadConfig(); // Load configuration from file

    if (!RegisterClassEx(&wc))
    {
        MessageBoxW(NULL, L"Failed to register window!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowExW(
        WS_EX_APPWINDOW | WS_EX_LAYERED,
        L"WindowClass",
        L"wndinfo",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIN_WIDTH,
        WIN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Failed to create window!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Set position
    if (currentCorner == 0)
        CenterWindow(hwnd);
    else
        MoveWindowToCorner(hwnd, currentCorner);

    // Set transparency
    SetLayeredWindowAttributes(hwnd, 0, currentTransparency, LWA_ALPHA);

    // Set round corner border
    isWindows11OrGreater = IsWindows11OrGreater();

    if (isWindows11OrGreater && !isConfigFileExistsBefore)
    {
        isRoundCorner = TRUE;
        SetWindowRoundCorner(hwnd);
    }
    else
    {
        if (isRoundCorner)
            SetWindowRoundCorner(hwnd);
    }

    // Set topmost
    if (isTopMost)
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SaveConfig();
    return msg.wParam;
}