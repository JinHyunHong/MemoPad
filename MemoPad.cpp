// MemoPad.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "MemoPad.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
RECT WindowRC = { 0, 0, 800, 500 };

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MEMOPAD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEMOPAD));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MEMOPAD);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, L"MemoPad", WS_OVERLAPPEDWINDOW,
        50, 50, WindowRC.right - WindowRC.left, WindowRC.bottom - WindowRC.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static vector<string> vectext(1);
    static vector<string> vecStorageText(1);
    static float fOffsetX;
    static float fOffsetY;
    static int iCount, iLine, iBackEmptyCount;
    static POINT tCaretPos;

    OPENFILENAME OFN, SFN;
    TCHAR lpstrFile[100] = _T("");
    static char filepath[100], filename[100];

    static SIZE size;
    // 불러오기 필터
    TCHAR filter[] = _T("텍스트 문서(*.txt)\0*.txt\0모든 파일 \0*.*\0");

    switch (message)
    {
        // 윈도우를 생성하기 전 메세지 전달
    case WM_CREATE:
        iLine = 0;
        iCount = 0;
        fOffsetX = 5;
        fOffsetY = 20;
        iBackEmptyCount = 1;
        CreateCaret(hWnd, NULL, 3, 15);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case ID_NEW:
        {
            MessageBox(hWnd, L"새 파일을 열겠습니까?", L"새 파일 선택", MB_OKCANCEL);
            memset(&vectext, 0, vectext.size());
            iLine = 0;
            iCount = 0;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        case ID_OPEN:
            memset(&OFN, 0, sizeof(OPENFILENAME));
            OFN.lStructSize = sizeof(OPENFILENAME);
            OFN.hwndOwner = hWnd;
            OFN.lpstrFile = (LPWSTR)filepath;
            OFN.nMaxFileTitle = 100;
            OFN.lpstrFileTitle = (LPWSTR)filename;
            OFN.nMaxFile = 100;
            OFN.lpstrInitialDir = _T(".");
            OFN.lpstrFilter = filter;
            // 열기 대화상자를 보여준다.
            if (GetOpenFileName(&OFN) != 0)
            {
            }
            break;

        case ID_SAVE:
            memset(&SFN, 0, sizeof(OPENFILENAME));
            SFN.lStructSize = sizeof(OPENFILENAME);
            SFN.hwndOwner = hWnd;
            SFN.lpstrFilter = filter;
            SFN.lpstrFile = lpstrFile;
            SFN.nMaxFile = 256;
            SFN.lpstrInitialDir = _T(".");
            // 저장 대화상자를 보여준다.
            if (GetSaveFileName(&SFN) != 0)
            {
                FILE* fSaveFile;
                fopen_s(&fSaveFile, (const char*)SFN.lpstrFile, "w");
                fclose(fSaveFile);
            }
            break;

        case ID_CaptureSave:
        {
        }
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;


    case WM_KEYDOWN:
    {
        if (wParam == VK_BACK)
        {
            if (!vectext.at(vectext.size() - 1).empty())
            {
                RECT rc = { 0, 0, 1, 1};
                vectext.at(vectext.size() - 1).pop_back();
                iCount--;
                InvalidateRect(hWnd, &rc, TRUE);
            }

            else
            {
                if (iLine != 0)
                {
                    vectext.clear();
                    vectext.push_back(vecStorageText.at(vecStorageText.size() - iBackEmptyCount));
                    iBackEmptyCount++;
                    --iLine;
                }
                
            }
            
            
        }

        if (wParam == VK_RETURN)
        {
            HideCaret(hWnd);

            iCount++;
            iLine++;

            vecStorageText.push_back(vectext.at(vectext.size() - 1));
            vectext.clear();
            vectext.resize(1);

            
        }
        break;
    }

    case WM_PAINT:
    {
        ShowCaret(hWnd);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        GetTextExtentPointA(hdc, vectext.at(vectext.size() - 1).c_str(), vectext.at(vectext.size() - 1).size(), &size);

        
        for (int count = 0; count < vectext.size(); count++)
        {
            TextOutA(hdc, fOffsetX, fOffsetY * iLine, vectext.at(count).c_str(), vectext.at(count).length());
        }

        SetCaretPos(size.cx + fOffsetX, iLine * fOffsetY);
        tCaretPos = { size.cx + (LONG)fOffsetX , iLine * (LONG)fOffsetY };
        
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_CHAR:
    {
        if (iLine > 20)
        {
            MessageBox(hWnd, L"입력 가능 문자열 초과", L"메모장", MB_OK);
            iLine = 9;
        }
        else
        {
            if (vectext.size() - 5 <= iCount)
            {
                vectext.reserve(vectext.size() * 2);
            }

            // 엔터키의 숫자는 무시한다.
            if (!(wParam == VK_RETURN || wParam == VK_BACK))
            {
                vectext.at(vectext.size() - 1).push_back(((TCHAR)wParam));
                iCount++;
            }
            
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }

    case WM_DESTROY:
        HideCaret(hWnd);
        DestroyCaret();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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