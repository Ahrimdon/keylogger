#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>

using namespace std;

bool deleteRegistryEntry() {
    HKEY hKey;
    bool result = false;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (RegDeleteValue(hKey, _T("Keylogger")) == ERROR_SUCCESS) {
            result = true;
        }
        RegCloseKey(hKey);
    }
    return result;
}

bool stopKeylogger() {
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, _T("UHmXv0oMgHXiqghTDiM6klEQi"));
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return true;
    }
    return false;
}

int main() {
    bool deleted = deleteRegistryEntry();
    bool stopped = stopKeylogger();

    if (deleted) {
        cout << "Registry entry deleted successfully." << endl;
    } else {
        cout << "Error deleting registry entry." << endl;
    }

    if (stopped) {
        cout << "Keylogger program stopped successfully." << endl;
    } else {
        cout << "Error stopping keylogger program." << endl;
    }

    return 0;
}