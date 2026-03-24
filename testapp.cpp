#include <windows.h>
#include <stdio.h>

void WriteLog(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);
    HANDLE hLog = CreateFileA("log.txt", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLog != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hLog, buffer, (DWORD)strlen(buffer), &written, NULL);
        WriteFile(hLog, "\r\n", 2, &written, NULL);
        CloseHandle(hLog);
    }
}

DWORD WINAPI MyTestThread(LPVOID lpParam) {
    WriteLog("[Thread] Thread is running");
    printf("hehe\n");
    Sleep(500);
    WriteLog("[Thread] Thread complete");
    return 0;
}

int main() {
    //DeleteFileA("log.txt");
    WriteLog("[Main] Program is starting");
    LARGE_INTEGER frequency, startTime, endTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    const char* fileName = "New_Text_Document.txt";
    HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        const char* writeData = "D34dCh1ck3n";
        DWORD bytesWritten = 0;
        if (WriteFile(hFile, writeData, (DWORD)strlen(writeData), &bytesWritten, NULL)) {
            WriteLog("[File] WriteFile complete: %lu bytes to file.", bytesWritten);
        }
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        char readBuffer[100] = { 0 };
        DWORD bytesRead = 0;
        if (ReadFile(hFile, readBuffer, sizeof(readBuffer) - 1, &bytesRead, NULL)) {
            WriteLog("[File] ReadFile complete, content: '%s'", readBuffer);
        }
        CloseHandle(hFile);
    }
    else {
        WriteLog("[File] CreateFile error: %lu", GetLastError());
    }

    const char* mutexName = "Global\\Group1SigmaMutex";
    HANDLE hMutex = CreateMutexA(NULL, FALSE, mutexName);
    if (hMutex != NULL) {
        WriteLog("[Mutex] CreateMutex complete: %s", mutexName);

        HANDLE hOpenedMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, mutexName);
        if (hOpenedMutex != NULL) {
            WriteLog("[Mutex] OpenMutex complete.");
            CloseHandle(hOpenedMutex);
        }
        else {
            WriteLog("[Mutex] OpenMutex error: %lu", GetLastError());
        }
        CloseHandle(hMutex);
    }
    else {
        WriteLog("[Mutex] CreateMutex error %lu", GetLastError());
    }

    HKEY hKey;
    LSTATUS regStatus = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Wallpapers", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (regStatus == ERROR_SUCCESS) {
        WriteLog("[Registry] RegCreateKeyExA complete.");

        DWORD testValue = 2026;
        regStatus = RegSetValueExA(hKey, "DeadChicken", 0, REG_DWORD, (const BYTE*)&testValue, sizeof(testValue));
        if (regStatus == ERROR_SUCCESS) {
            WriteLog("[Registry] RegSetValueExA (DeadChicken = %lu).", testValue);
        }
        else {
            WriteLog("[Registry] RegSetValueExA error: %ld", regStatus);
        }
        RegCloseKey(hKey);
    }
    else {
        WriteLog("[Registry] RegCreateKeyExA error: %ld", regStatus);
    }

    HANDLE hThread = CreateThread(NULL, 0, MyTestThread, NULL, 0, NULL);
    if (hThread != NULL) {
        WriteLog("[Thread] CreateThread complete.");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
    else {
        WriteLog("[Thread] CreateThread error: %lu", GetLastError());
    }

    QueryPerformanceCounter(&endTime);
    double elapsedTimeMs = (double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
    WriteLog("[Performance] Total time of execution: %.2f ms", elapsedTimeMs);

    return 0;
}