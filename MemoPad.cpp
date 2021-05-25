// MemoPad.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "MemoPad.h"

#define MAX_LOADSTRING 100
#define MAX_LINE 20

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
RECT g_WindowRC = { 0, 0, 800, 500 };

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK CH_SPELLING(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
vector<string> OutFromFile(TCHAR filename[], HWND hWnd, bool bTextout);

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
    wcex.hCursor = LoadCursor(nullptr, IDC_IBEAM);
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
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, L"MemoPad", WS_OVERLAPPEDWINDOW,
        50, 50, g_WindowRC.right - g_WindowRC.left, 
        g_WindowRC.bottom - g_WindowRC.top, nullptr, nullptr, hInstance, nullptr);

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
    static vector<string> vecStorageText(0);
    static float fOffsetX;
    static float fOffsetY;
    // iCaretPos -> 움직일 시 카렛 위치
    static int iCount, iLine, iBackEmptyCount, iCaretPos;
    static bool bTextUpdate;
    static bool bInputAble;


    OPENFILENAME OFN, SFN;
    

    TCHAR lpstrFile[100] = _T("");
    static char filepath[100], filename[100];

    static SIZE size;
    // 불러오기 필터
    TCHAR filter[] = _T("텍스트 문서(*.txt)\0*.txt\0모든 파일 \0*.*\0");


    // 폰트 관련 변수 선언
    CHOOSEFONT FONT;
    static COLORREF fColor;
    HFONT hFont, OldFont = 0;
    static LOGFONT LogFont;


    switch (message)
    {
        // 윈도우를 생성하기 전 메세지 전달
    case WM_CREATE:
        iLine = 0;
        iCount = 0;
        fOffsetX = 5;
        fOffsetY = 20;
        iBackEmptyCount = 1;
        bTextUpdate = false;
        bInputAble = true;
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
            vecStorageText.clear();
            vectext.clear();
            vecStorageText.resize(1);
            vectext.resize(1);
            iLine = 0;
            iCount = 0;
            InvalidateRect(hWnd, NULL, TRUE);
            SetCaretPos(fOffsetX, 0);
            break;
        }
        case ID_OPEN:
        {
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
                vector<string> vecStrBuffer;
                // 기존 모두 초기화
                vecStorageText.clear();
                vectext.clear();
                iLine = 0;
                iCount = 0;

                TCHAR* CBuffer;

                vecStrBuffer = OutFromFile(OFN.lpstrFile, hWnd, false);

                iLine = vecStrBuffer.size() - 1;

                for (int i = 0; i < vecStrBuffer.size(); i++)
                {
                    for (int j = 0; j < vecStrBuffer.at(i).size(); j++)
                    {
                        iCount++;
                    }
                }

                for (int i = 0; i < vecStrBuffer.size(); i++)
                {
                    if (i == vecStrBuffer.size() - 1)
                    {
                        vectext.push_back(vecStrBuffer.at(i));
                    }

                    else
                        vecStorageText.push_back(vecStrBuffer.at(i));
                }

                InvalidateRect(hWnd, NULL, TRUE);
                CreateCaret(hWnd, NULL, 3, 15);
                bTextUpdate = true;
                UpdateWindow(hWnd);
            }
        }
            break;

        case ID_SAVE:
        {
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
                int iLen = 0;

                // 경로에서 파일 이름 길이 구하기
                for (int i = lstrlen(lpstrFile); i > 0; i--)
                {
                    if (lpstrFile[i - 1] == '\\')
                    {
                        break;
                    }

                    else
                    {
                        iLen++;
                    }
                }

                char FileName[15];
                memset(FileName, 0, sizeof(FileName));


                // 파일 이름을 길이 만큼 새 변수에 저장한다.
                for (int i = 1; i <= iLen; i++)
                {
                    FileName[iLen - i] = lpstrFile[(lstrlen(lpstrFile) - i)];
                }

                // .txt 출력 글자를 붙여준다.
                FileName[iLen] = '.';
                FileName[iLen + 1] = 't';
                FileName[iLen + 2] = 'x';
                FileName[iLen + 3] = 't';


                FILE* fSaveFile;
                fopen_s(&fSaveFile, FileName, "wt");

                if (fSaveFile)
                {
                    TCHAR buffer[500];
                    memset(buffer, 0, sizeof(buffer));

                    int ibufferLastIndex = 0;

                    // 엔터키를 누를 시 저장되지만, 저장 되지 못한 부분인 현재 작업중인 vectext를 넣어준다.
                    vecStorageText.push_back(vectext.at(vectext.size() - 1));

                    // 버퍼에 문자 읽어들이기
                    for (int i = 0; i < vecStorageText.size(); ++i)
                    {
                        for (int j = 0; j < vecStorageText.at(i).size(); ++j)
                        {

                            buffer[j + ibufferLastIndex] = vecStorageText.at(i).at(j);


                            if (j + 1 >= vecStorageText.at(i).size())
                            {
                                ibufferLastIndex = j + ibufferLastIndex + 1;
                            }

                        }

                        buffer[ibufferLastIndex] = CHAR('\n');
                        ibufferLastIndex += 1;

                    }
                    _fputts(buffer, fSaveFile);
                }

                fclose(fSaveFile);
            }
            break;
        }

        case ID_FONT:
            memset(&FONT, 0, sizeof(CHOOSEFONT));
            FONT.lStructSize = sizeof(CHOOSEFONT);
            FONT.hwndOwner = hWnd;
            FONT.lpLogFont = &LogFont;
            FONT.Flags = CF_EFFECTS | CF_SCREENFONTS;
            if (ChooseFont(&FONT) != 0)
            {
                HDC hdc = GetDC(hWnd);
                fColor = FONT.rgbColors;
                InvalidateRgn(hWnd, NULL, TRUE);
                bTextUpdate = true;
            }

            
            break;
            // 문법을 바꿔준다.
        case ID_CH_SPELLING:
        {
            TCHAR cFilePath[100];
            memset(cFilePath, 0, sizeof(cFilePath));

            if (GetCurrentDirectory(sizeof(cFilePath), cFilePath) > 0)
            {
                vector<string> vecVerbBuffer;
                vector<string> vecNounBuffer;

                cFilePath[lstrlen(cFilePath)] = '\\';
                cFilePath[lstrlen(cFilePath)] = 'v';
                cFilePath[lstrlen(cFilePath)] = 'e';
                cFilePath[lstrlen(cFilePath)] = 'r';
                cFilePath[lstrlen(cFilePath)] = 'b';
                cFilePath[lstrlen(cFilePath)] = '.';
                cFilePath[lstrlen(cFilePath)] = 't';
                cFilePath[lstrlen(cFilePath)] = 'x';
                cFilePath[lstrlen(cFilePath)] = 't';


                // 동사 파일에서 동사 목록을 불러온다.
                vecVerbBuffer = OutFromFile(cFilePath, hWnd, false);

                // cFilePath verb.txt를 지워준다.
                for (int i = 1; i < 9; i++)
                {
                    cFilePath[lstrlen(cFilePath) - 1] = 0;
                }

                cFilePath[lstrlen(cFilePath)] = 'n';
                cFilePath[lstrlen(cFilePath)] = 'o';
                cFilePath[lstrlen(cFilePath)] = 'u';
                cFilePath[lstrlen(cFilePath)] = 'n';
                cFilePath[lstrlen(cFilePath)] = '.';
                cFilePath[lstrlen(cFilePath)] = 't';
                cFilePath[lstrlen(cFilePath)] = 'x';
                cFilePath[lstrlen(cFilePath)] = 't';

                // 명사 파일에서 명사 목록을 불러온다.
                vecNounBuffer = OutFromFile(cFilePath, hWnd, false);

                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CH_SPELLING), hWnd, CH_SPELLING);

                // 전체 문법 수정
                //for (int i = 0; i < vecStorageText.size(); i++)
                //{
                //    for (int k = 0; k < vecStorageText.at(i).size(); k++)
                //    {
                //        if (vecStorageText.at(i).at(k) == vecVerbBuffer.at(i).at(0))
                //        {
                //
                //        }
                //    }
                //}
                // 부분 문법 수정

            }
        }
        break;

        case IDM_ABOUT:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
                vectext.at(vectext.size() - 1).pop_back();
                iCount--;

                InvalidateRect(hWnd, NULL, TRUE);

                bTextUpdate = true;
            }

            else
            {
                if (iLine != 0)
                {
                    vectext.clear();
                    vectext.push_back(vecStorageText.at(vecStorageText.size() - 1));
                    vecStorageText.pop_back();
                    --iLine;
                    InvalidateRect(hWnd, NULL, TRUE);
                    bTextUpdate = true;
                }

            }


        }

        if (wParam == VK_RETURN)
        {
            HDC hdc = GetDC(hWnd);

            POINT tPos;
            GetCaretPos(&tPos);

            RECT rc = { tPos.x, tPos.y, tPos.x + 5, tPos.y + fOffsetY };

            // Caret 부분이 남지 않도록 지워준다.
            FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

            if (!(vecStorageText.empty() && vectext.empty() && iLine > MAX_LINE))
            {
                iLine++;
                iCount++;
                vecStorageText.push_back(vectext.at(vectext.size() - 1));
                vectext.clear();
                vectext.resize(1);
            }
        }


        break;
    }

    case WM_LBUTTONDOWN:
    {

    }
    break;


    case WM_PAINT:
    {
        ShowCaret(hWnd);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 폰트 관련 함수
        hFont = CreateFontIndirect(&LogFont);
        OldFont = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, fColor);

        for (int count = 0; count < vectext.size(); count++)
        {
            if (bTextUpdate)
            {
                for (int i = 0; i < vecStorageText.size(); i++)
                {
                    GetTextExtentPointA(hdc, vecStorageText.at(i).c_str(), vecStorageText.at(i).size(), &size);
                    SetCaretPos(size.cx + fOffsetX, iLine * fOffsetY);
                    TextOutA(hdc, fOffsetX, fOffsetY * i, vecStorageText.at(i).c_str(), vecStorageText.at(i).length());
                }
                bTextUpdate = false;
            }
            GetTextExtentPointA(hdc, vectext.at(vectext.size() - 1).c_str(), vectext.at(vectext.size() - 1).size(), &size);
            SetCaretPos(size.cx + fOffsetX, iLine * fOffsetY);
            TextOutA(hdc, fOffsetX, fOffsetY * iLine, vectext.at(count).c_str(), vectext.at(count).length());
        }


        SelectObject(hdc, OldFont);
        DeleteObject(hFont);


        EndPaint(hWnd, &ps);
    }
    break;

    case WM_CHAR:
    {
        if (iLine > MAX_LINE)
        {
            MessageBox(hWnd, L"입력 가능 문자열 초과", L"메모장", MB_OK);
            iLine = MAX_LINE;
            bInputAble = false;
            DestroyCaret();
        }
        else
        {
            if (vectext.size() - 5 <= iCount)
            {
                vectext.reserve(vectext.size() * 2);
            }

            // 엔터키의 숫자는 무시한다.
            if ((!(wParam == VK_RETURN || wParam == VK_BACK)) && bInputAble)
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

INT_PTR CALLBACK CH_SPELLING(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_EDITSTR:
            break;
        case ID_CHANGE:
            break;
        case ID_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        break;
    }
    return (INT_PTR)FALSE;
}

// 파일을 읽어온다.
vector<string> OutFromFile(TCHAR filename[], HWND hWnd, bool bTextout)
{
    FILE* fReadFile;
    HDC hdc;
    int iLine;
    TCHAR buffer[500];
    vector<string> vecbufferStorage;
    iLine = 0;
    hdc = GetDC(hWnd);
#ifdef _UNICODE
    _tfopen_s(&fReadFile, filename, _T("r, ccs = UNICODE"));
#else
    _tfopen_s(&fReadFile, filename, _T("r"));
#endif
    while (_fgetts(buffer, 100, fReadFile) != NULL)
    {
        if (buffer[_tcslen(buffer) - 1] == _T('\n'))
            buffer[_tcslen(buffer) - 1] = NULL;
        if (bTextout)
        {
            TextOut(hdc, 0, iLine * 20, buffer, _tcslen(buffer));
            iLine++;
        }
        char cTemp[500];
        WideCharToMultiByte(CP_ACP, 0, buffer, 500, cTemp, 500, NULL, NULL);
        vecbufferStorage.resize(vecbufferStorage.size() + 1);
        vecbufferStorage.at(vecbufferStorage.size() - 1).append(cTemp);
        
    }


    fclose(fReadFile);
    ReleaseDC(hWnd, hdc);

    return vecbufferStorage;
}


