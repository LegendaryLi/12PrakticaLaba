#include "framework.h"
#include "WindowsProject1.h"
#include "windows.h"
#include "iostream"
#include "stdio.h"
#include <commdlg.h>
#include "resource.h"
#include <strsafe.h>
#include <WindowsX.h>

#define ID_Save 2001
#define IDB_CHECKBOX 2002
#define PC_Name 2003
#define ID_Write 2004
#define ID_Read 2005
#define CreateFileFormataINI 2006
#define EDIT1 2007

#define sizebuffer 260
#define PATH L"C:\Users\MemphisJuniorGod\Desktop\1.txt"

char buffer[] = "Какая-то инфа";

HWND hEdit;
HWND hwnd;
HANDLE hFile;
HWND editHwnd;
HWND fileHwnd;

DWORD dwBytesToWrite = strlen(buffer) * sizeof(char);
DWORD dwBytesWritten = 0;
DWORD dwNoByteToRead = strlen(buffer);
DWORD dwNoByteRead = 0;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

static OPENFILENAME ofn;
static char szFile[MAX_PATH];

DWORD Size = 1024;
wchar_t SystemInfo[1024];


BOOL WriteAsync(HANDLE file, LPCVOID buf, DWORD offset, DWORD size, LPOVERLAPPED overlap) {
    ZeroMemory(overlap, sizeof(OVERLAPPED));
    overlap->Offset = offset;
    overlap->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL ret = WriteFile(file, buf, size, NULL, overlap);
    if (ret == FALSE && GetLastError() != ERROR_IO_PENDING)
        CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
    return ret;
}

BOOL FinishIo(LPOVERLAPPED overlap) {
    if (overlap->hEvent != NULL) {
        DWORD res = WaitForSingleObject(overlap->hEvent, INFINITE);
        if (res == WAIT_OBJECT_0) {
            CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
            return TRUE;
        }
    }
    return FALSE;
}


BOOL ReadAsync(HANDLE file, LPVOID buf, DWORD offset, DWORD size, LPOVERLAPPED overlap) {
    ZeroMemory(overlap, sizeof(OVERLAPPED));
    overlap->Offset = offset;
    overlap->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL ret = ReadFile(file, buf, size, NULL, overlap);
    if (ret == FALSE && GetLastError() != ERROR_IO_PENDING)
        CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
    return ret;
}

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                bFile;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       GetPrivateProfileInt(TEXT("General"), TEXT("WINX"), CW_USEDEFAULT, TEXT("C:\\ProgramData\\app.ini")), GetPrivateProfileInt(TEXT("General"), TEXT("WINY"), 0, TEXT("C:\\ProgramData\\app.ini")), GetPrivateProfileInt(TEXT("General"), TEXT("WINW"), CW_USEDEFAULT, TEXT("C:\\ProgramData\\app.ini")), GetPrivateProfileInt(TEXT("General"), TEXT("WINH"), 0, TEXT("C:\\ProgramData\\app.ini")), nullptr, nullptr, hInstance, nullptr);

   GetUserName(SystemInfo, &Size);

   if (!hWnd)
   {
      return FALSE;
   }

   TCHAR buf[999];
   GetPrivateProfileString(TEXT("General"), TEXT("File"), TEXT("Файл"), buf, 999, TEXT("C:\\ProgramData\\app.ini"));

   fileHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), buf, ES_AUTOHSCROLL | ES_LEFT | WS_CHILD | WS_VISIBLE, 10, 10, 200, 25, hWnd, (HMENU)EDIT1, hInst, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_MOVE:
        {
            TCHAR s3[100];
            TCHAR s4[100];
            _stprintf_s(s3, _T("%i"), LOWORD(lParam));
            WritePrivateProfileString(TEXT("General"), TEXT("WINX"), s3, TEXT("C:\\ProgramData\\app.ini"));
            _stprintf_s(s4, _T("%i"), HIWORD(lParam));
            WritePrivateProfileString(TEXT("General"), TEXT("WINY"), s4, TEXT("C:\\ProgramData\\app.ini"));
        }
        case WM_SIZE:
        {
            TCHAR s[100];
            TCHAR s2[100];
            _stprintf_s(s, _T("%i"), LOWORD(lParam));
            WritePrivateProfileString(TEXT("General"), TEXT("WINW"), s, TEXT("C:\\ProgramData\\app.ini"));
            _stprintf_s(s2, _T("%i"), HIWORD(lParam));
            WritePrivateProfileString(TEXT("General"), TEXT("WINH"), s2, TEXT("C:\\ProgramData\\app.ini"));
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            OVERLAPPED overlap;
            TCHAR buf[999];
            HANDLE file;
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
                case ID_Save:
                {
                    wchar_t buf[255] = L"\0";
                    wchar_t filter[] = L"Data Files(*.txt)\0*.txt\0All Files\0*.*\0\0";
                    wchar_t filterExt[][6] = { L".txt" };
                    wchar_t cCustomFilter[256] = L"\0\0";
                    int nFilterIndex = 0;
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hWnd;
                    ofn.hInstance = hInst;
                    ofn.lpstrFilter = filter;
                    ofn.lpstrCustomFilter = cCustomFilter;
                    ofn.nMaxCustFilter = 256;
                    ofn.nFilterIndex = nFilterIndex;
                    ofn.lpstrFile = buf;
                    ofn.nMaxFile = 255;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = 0;
                    ofn.Flags = OFN_FILEMUSTEXIST;
                    ofn.nFileOffset = 0;
                    ofn.nFileExtension = 0;
                    ofn.lpstrDefExt = NULL;
                    ofn.lCustData = NULL;
                    ofn.lpfnHook = NULL;
                    ofn.lpTemplateName = NULL;


                    if (GetSaveFileName(&ofn))
                    {
                        MessageBox(hwnd, ofn.lpstrFile, L"Файл сохранен под именем:", MB_OK);
                        hFile = CreateFile(ofn.lpstrFile,
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
                    }

                    /*if (GetSaveFileName(&ofn))
                    {
                        strcpy(fileName,ofn.lpstrFile);
                        return 1;
                    }
                    return 0;*/

                    /*if (GetSaveFileName(&ofn))
                    {
                        HANDLE hf = CreateFileW(TEXT("1.txt"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                    }*/
                }break;
                case ID_Write:
                {
                    char buf3[999];
                    WCHAR buf4[999];
                    GetWindowText(fileHwnd, buf, 999);
                    GetWindowText(editHwnd, buf4, 999);
                    WideCharToMultiByte(0, 0, buf4, -1, buf3, 999, 0, 0);
                    file = CreateFile(buf, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                    WriteAsync(file, buf3, 0, 999, &overlap);
                    FinishIo(&overlap);
                    CloseHandle(file);
                    WriteFile(hFile, (void*)buffer/*buf3*/, dwBytesToWrite, &dwBytesWritten, NULL);
                }break;
                case ID_Read:
                {
                    bFile = ReadFile(
                        hFile,
                        buffer,
                        dwNoByteToRead,
                        &dwNoByteRead,
                        NULL);
                    if (dwNoByteToRead > 0)
                    {
                        MessageBox(hwnd, (LPWSTR)buffer, L"Уведомление", MB_OK | MB_ICONQUESTION);
                    }
                }break;

                case CreateFileFormataINI:
                {
                    char buf2[999];
                    GetWindowText(fileHwnd, buf, 999);
                    file = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                    WritePrivateProfileString(TEXT("General"), TEXT("File"), buf, TEXT("C:\\ProgramData\\app.ini"));
                    ReadAsync(file, buf2, 0, 999, &overlap);
                    FinishIo(&overlap);
                    CloseHandle(file);
                    MultiByteToWideChar(0, 0, buf2, 999, buf, 999);
                    SetWindowText(editHwnd, buf);
                }break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
    {
        HWND ButtonSave = CreateWindow(L"Button", L"Save",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            215, 0, 100, 35, hWnd, (HMENU)ID_Save, NULL, NULL);

        HWND ButtonWrite = CreateWindow(L"Button", L"Write",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            155, 50, 110, 35, hWnd, (HMENU)ID_Write, NULL, NULL);

        HWND ButtonRead = CreateWindow(L"Button", L"Read",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            105, 100, 100, 35, hWnd, (HMENU)ID_Read, NULL, NULL);

        HWND ButtonCreateFileFormataINI = CreateWindow(L"Button", L"CreateFileFormataINI",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            55, 50, 100, 35, hWnd, (HMENU)CreateFileFormataINI, NULL, NULL);  
    }break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
