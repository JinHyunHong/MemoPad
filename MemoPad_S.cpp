// MemoPad.cpp : ���ø����̼ǿ� ���� �������� �����մϴ�.
//

#include "framework.h"
#include "MemoPad.h"

#define MAX_LOADSTRING 100
#define MAX_LINE 20
#define MAX_NOUN_SIZE 10

// ���� ����:
HINSTANCE g_hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
RECT g_WindowRC = { 0, 0, 800, 500 };
static float g_fOffsetX;
static float g_fOffsetY;
WCHAR g_CH_SPELLING_Temp[MAX_NOUN_SIZE];

// ��� ó��
static HBRUSH g_WindowBrush;
static int iHour;


// ��Ʈ ���� ����
static COLORREF fColor;


// ���� ����
static WSADATA wsadata;
static SOCKET s, cs;
static SOCKADDR_IN addr = { 0 }, c_addr;
static int iSize, iMsgLen, iSocketCount;
static char socketbuffer[100];
static TCHAR socketstr[100];
static TCHAR socketmsg[200];
static bool bPaint;
static HWND g_hDlg_Talk;
static string g_sMyIp;




// �� �ڵ� ��⿡ ���Ե� �Լ��� ������ �����մϴ�:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK CH_SPELLING(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK LINESPACING(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ADDNOUNLIST(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TALK(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
vector<string> OutFromFile(TCHAR filename[], HWND hWnd, bool bTextout);
void PrintMessage(HWND hList);
string GetMyIpAddress();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MEMOPAD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���ø����̼� �ʱ�ȭ�� �����մϴ�:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEMOPAD));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�:
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
//  �Լ�: MyRegisterClass()
//
//  �뵵: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    SYSTEMTIME g_SystemTime;
    GetLocalTime(&g_SystemTime);
    iHour = g_SystemTime.wHour;
    iHour = 20;

    if (iHour > 5 && iHour < 19)
    {
        g_WindowBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        fColor = RGB(0, 0, 0);
    }

    else
    {
        g_WindowBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
        fColor = RGB(255, 255, 255);
    }

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(nullptr, IDC_IBEAM);
    wcex.hbrBackground = g_WindowBrush;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MEMOPAD);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));


    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   �뵵: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   �ּ�:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

    HWND hWnd = CreateWindowW(szWindowClass, L"MemoPad_Server", WS_OVERLAPPEDWINDOW,
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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �뵵: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���ø����̼� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static vector<string> vectext(1);
    static vector<string> vecStorageText(0);

    // iCaretPos -> ������ �� ī�� ��ġ
    static int iCount, iLine, iBackEmptyCount, iCaretPos;
    static bool bTextUpdate;
    static bool bInputAble;


    OPENFILENAME OFN, SFN;


    TCHAR lpstrFile[100] = _T("");
    static char filepath[100], filename[100];

    static SIZE size;
    // �ҷ����� ����
    TCHAR filter[] = _T("�ؽ�Ʈ ����(*.txt)\0*.txt\0��� ���� \0*.*\0");


    // ��Ʈ ���� ���� ����
    CHOOSEFONT FONT;
    HFONT hFont, OldFont = 0;
    static LOGFONT LogFont;

    //�� ���� ���� ����
    static bool g_bDraw = false;	//	���콺�� ���� �������� Ȯ���� �� �ִ� bool ����
    static int g_iX;				//	���콺�� x��ǥ��
    static int g_iY;				//	���콺�� y��ǥ��
    static int iDrawCount;

    HPEN MyPen = NULL;
    int P_Color = RGB(0, 255, 0);

    POINT pt;

    // �޴� checked�� ���� HMENU
    static HMENU hMenu = GetMenu(hWnd);



    switch (message)
    {
        // �����츦 �����ϱ� �� �޼��� ����
    case WM_CREATE:
        WSAStartup(MAKEWORD(2, 2), &wsadata);
        s = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_family = AF_INET;
        addr.sin_port = 20;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        bind(s, (LPSOCKADDR)&addr, sizeof(addr));
        WSAAsyncSelect(s, hWnd, WM_ASYNC, FD_ACCEPT);
        bPaint = false;
        if (listen(s, 5) == -1)
            return 0;

        iLine = 0;
        iCount = 0;
        g_fOffsetX = 5;
        g_fOffsetY = 20;
        iBackEmptyCount = 1;
        bTextUpdate = false;
        bInputAble = true;
        iDrawCount = -1;
        CreateCaret(hWnd, NULL, 3, 15);
        break;

    case WM_ASYNC:
        switch (lParam)
        {
        case FD_ACCEPT:
            iSize = sizeof(c_addr);
            cs = accept(s, (LPSOCKADDR)&c_addr, &iSize);
            WSAAsyncSelect(cs, hWnd, WM_ASYNC, FD_READ);
            break;
        case FD_READ:
            iMsgLen = recv(cs, socketbuffer, 100, 0);
            socketbuffer[iMsgLen] = NULL;
            bPaint = false;

#ifdef _UNICODE
            iMsgLen = MultiByteToWideChar(CP_ACP, 0, socketbuffer, strlen(socketbuffer), NULL, NULL);
            MultiByteToWideChar(CP_ACP, 0, socketbuffer, strlen(socketbuffer), socketmsg, iMsgLen);
            socketmsg[iMsgLen] = NULL;
#else
            strcpy_s(msg, buffer);
#endif      
            PrintMessage(g_hDlg_Talk);
            break;
        default:
            break;

        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �޴� ������ ���� �м��մϴ�:
        switch (wmId)
        {
        case ID_NEW:
        {
            // �����۾��� ����� ���ο� ���Ϸ� �����Ѵ�.
            int iInput = MessageBox(hWnd, L"�� ������ ���ڽ��ϱ�?", L"�� ���� ����", MB_OKCANCEL);

            switch (iInput)
            {
            case IDOK:
                vecStorageText.clear();
                vectext.clear();
                vecStorageText.resize(1);
                vectext.resize(1);
                iLine = 0;
                iCount = 0;
                InvalidateRect(hWnd, NULL, TRUE);
                SetCaretPos(g_fOffsetX, 0);
                break;
            case IDCANCEL:
                break;
            case IDCLOSE:
                break;
            default:
                break;
            }
            break;
        }
        case ID_OPEN:
        {
            // �� ������ ����.
            memset(&OFN, 0, sizeof(OPENFILENAME));
            OFN.lStructSize = sizeof(OPENFILENAME);
            OFN.hwndOwner = hWnd;
            OFN.lpstrFile = (LPWSTR)filepath;
            OFN.nMaxFileTitle = 100;
            OFN.lpstrFileTitle = (LPWSTR)filename;
            OFN.nMaxFile = 100;
            OFN.lpstrInitialDir = _T(".");
            OFN.lpstrFilter = filter;
            // ���� ��ȭ���ڸ� �����ش�.
            if (GetOpenFileName(&OFN))
            {
                vector<string> vecStrBuffer;
                // ���� ��� �ʱ�ȭ
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
            }

            InvalidateRect(hWnd, NULL, TRUE);
            CreateCaret(hWnd, NULL, 3, 15);
            bTextUpdate = true;
            UpdateWindow(hWnd);
        }
        break;

        case ID_SAVE:
        {
            // �۾��� ������ ���Ϸ� �����Ѵ�.
            memset(&SFN, 0, sizeof(OPENFILENAME));
            SFN.lStructSize = sizeof(OPENFILENAME);
            SFN.hwndOwner = hWnd;
            SFN.lpstrFilter = filter;
            SFN.lpstrFile = lpstrFile;
            SFN.nMaxFile = 256;
            SFN.lpstrInitialDir = _T(".");
            // ���� ��ȭ���ڸ� �����ش�.



            if (GetSaveFileName(&SFN) != 0)
            {
                int iLen = 0;

                // ��ο��� ���� �̸� ���� ���ϱ�
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


                // ���� �̸��� ���� ��ŭ �� ������ �����Ѵ�.
                for (int i = 1; i <= iLen; i++)
                {
                    FileName[iLen - i] = lpstrFile[(lstrlen(lpstrFile) - i)];
                }

                // .txt ��� ���ڸ� �ٿ��ش�.
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

                    // ����Ű�� ���� �� ���������, ���� ���� ���� �κ��� ���� �۾����� vectext�� �־��ش�.
                    vecStorageText.push_back(vectext.at(vectext.size() - 1));

                    // ���ۿ� ���� �о���̱�
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

            InvalidateRect(hWnd, NULL, TRUE);
            CreateCaret(hWnd, NULL, 3, 15);
            bTextUpdate = true;
            UpdateWindow(hWnd);
            break;
        }

        case ID_FONT:
            // ���ڿ� ��Ʈ�� �����Ѵ�.
            memset(&FONT, 0, sizeof(CHOOSEFONT));
            FONT.lStructSize = sizeof(CHOOSEFONT);
            FONT.hwndOwner = hWnd;
            FONT.lpLogFont = &LogFont;
            FONT.Flags = CF_EFFECTS | CF_SCREENFONTS;
            if (ChooseFont(&FONT) != 0)
            {
                HDC hdc = GetDC(hWnd);
                fColor = FONT.rgbColors;
            }
            InvalidateRgn(hWnd, NULL, TRUE);
            CreateCaret(hWnd, NULL, 3, 15);
            bTextUpdate = true;
            UpdateWindow(hWnd);
            break;

            // �ڵ� ��� ��ħ
        case ID_AUTONOUNFIX:
        {
            UINT state = GetMenuState(hMenu, ID_AUTONOUNFIX, MF_BYCOMMAND);
            if (state == MF_CHECKED)
            {
                CheckMenuItem(hMenu, ID_AUTONOUNFIX, MF_UNCHECKED);
            }

            else
            {
                CheckMenuItem(hMenu, ID_AUTONOUNFIX, MF_CHECKED);
            }
        }
        break;


        // ���� ��� ��ħ
        case ID_MANUALNOUNFIX:
        {
            // �ܾ �ٲ��ش�.
            TCHAR cFilePath[100];
            memset(cFilePath, 0, sizeof(cFilePath));

            if (GetCurrentDirectory(sizeof(cFilePath), cFilePath) > 0)
            {
                vector<string> vecNounBuffer;

                cFilePath[lstrlen(cFilePath)] = '\\';
                cFilePath[lstrlen(cFilePath)] = 'n';
                cFilePath[lstrlen(cFilePath)] = 'o';
                cFilePath[lstrlen(cFilePath)] = 'u';
                cFilePath[lstrlen(cFilePath)] = 'n';
                cFilePath[lstrlen(cFilePath)] = '.';
                cFilePath[lstrlen(cFilePath)] = 't';
                cFilePath[lstrlen(cFilePath)] = 'x';
                cFilePath[lstrlen(cFilePath)] = 't';

                // ��� ���Ͽ��� ��� ����� �ҷ��´�.
                vecNounBuffer = OutFromFile(cFilePath, hWnd, false);

                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CH_SPELLING), hWnd, CH_SPELLING);

                // �����ڵ带 ��ȯ
                wstring sText_Temp(g_CH_SPELLING_Temp);
                string sTextWideTemp(sText_Temp.begin(), sText_Temp.end());

                // �κ� ��ȯ �κ� ã��
                bool bFind;
                POINT StorReplacePos;
                POINT TextReplacePos;
                string sNoun;

                // ���� �ʱ�ȭ
                bFind = false;
                StorReplacePos.x = 0;
                StorReplacePos.y = 0;
                TextReplacePos.x = 0;
                TextReplacePos.y = 0;

                int iSameCount = 0;

                if (sTextWideTemp.size() > 0 && sTextWideTemp.size() <= 2)
                {
                    MessageBox(hWnd, L"���� ���� 3 �̻� �ܾ �˻��� �� �ֽ��ϴ�.", L"����", MB_OK);
                    InvalidateRect(hWnd, NULL, TRUE);
                    CreateCaret(hWnd, NULL, 3, 15);
                    bTextUpdate = true;
                    break;
                }


                if (sTextWideTemp.size() > 2)
                {
                    // ���� ����� ��縦 �˻��Ѵ�.
                    for (int i = 0; i < vecNounBuffer.size(); i++)
                    {
                        for (int j = 0; j < vecNounBuffer.at(i).size(); j++)
                        {
                            if (vecNounBuffer.at(i).at(j) == sTextWideTemp.at(iSameCount))
                            {
                                iSameCount++;

                                // ���� + 1 ��ŭ ���ٸ� �ٲ��ش�.
                                if (iSameCount == sTextWideTemp.size() / 2 + 1)
                                {
                                    sNoun = vecNounBuffer.at(i);
                                    bFind = true;
                                    break;
                                }
                            }

                            else
                                iSameCount = 0;
                        }

                        if (bFind)
                            break;
                    }

                    // ���� �ܾ ã�� ���ߴٸ� �������´�.
                    if (!bFind)
                    {
                        MessageBox(hWnd, L"�ؽ�Ʈ ����� ������ �ܾ ã�� �� �����ϴ�.", L"ã�� �� ����", MB_OK);
                        InvalidateRect(hWnd, NULL, TRUE);
                        CreateCaret(hWnd, NULL, 3, 15);
                        bTextUpdate = true;
                        break;
                    }

                    bFind = false;
                    iSameCount = 0;

                    // ���ۿ� �Է��� ���ڿ��� �ִ��� ������� �˻��Ѵ�.
                    for (int i = 0; i < vecStorageText.size(); i++)
                    {
                        for (int j = 0; j < vecStorageText.at(i).size(); j++)
                        {
                            if (vecStorageText.at(i).at(j) == sTextWideTemp.at(iSameCount))
                            {
                                iSameCount++;

                                if (iSameCount == sTextWideTemp.size())
                                {
                                    StorReplacePos.x = j - sTextWideTemp.size() + 1;
                                    StorReplacePos.y = i;
                                    bFind = true;
                                    break;
                                }
                            }

                            else
                                iSameCount = 0;
                        }

                        if (bFind)
                            break;
                    }

                    if (bFind)
                    {
                        vecStorageText.at(StorReplacePos.y).erase(StorReplacePos.x, sTextWideTemp.size());
                        vecStorageText.at(StorReplacePos.y).insert(StorReplacePos.x, sNoun);
                        InvalidateRect(hWnd, NULL, TRUE);
                        bTextUpdate = true;
                        CreateCaret(hWnd, NULL, 3, 15);
                        break;
                    }

                    // ���� ���ۿ� �����Ƿ� ��Ž���� ���� �ٽ� �ʱ�ȭ
                    else
                    {
                        iSameCount = 0;

                        // �ǽð� �Է� ���ۿ� �ش� �Է��� ���ڿ��� �ִ��� �˻��Ѵ�.
                        for (int i = 0; i < vectext.size(); i++)
                        {
                            for (int j = 0; j < vectext.at(i).size(); j++)
                            {
                                if (vectext.at(i).at(j) == sTextWideTemp.at(iSameCount))
                                {
                                    iSameCount++;

                                    if (iSameCount == sTextWideTemp.size())
                                    {
                                        TextReplacePos.x = j - sTextWideTemp.size() + 1;
                                        TextReplacePos.y = i;
                                        bFind = true;
                                        break;
                                    }
                                }

                                else
                                    iSameCount = 0;
                            }
                            if (bFind)
                                break;
                        }

                        if (bFind)
                        {
                            vectext.at(TextReplacePos.y).erase(TextReplacePos.x, sTextWideTemp.size());
                            vectext.at(TextReplacePos.y).insert(TextReplacePos.x, sNoun);
                            InvalidateRect(hWnd, NULL, TRUE);
                            bTextUpdate = true;
                            CreateCaret(hWnd, NULL, 3, 15);
                            break;
                        }


                        else
                        {
                            MessageBox(hWnd, L"�ؽ�Ʈ ����� ��ġ�ϴ� �ؽ�Ʈ�� �����ϴ�.", L"ã�� �� ����", MB_OK);
                        }


                    }

                }
            }
        }
        InvalidateRect(hWnd, NULL, TRUE);
        CreateCaret(hWnd, NULL, 3, 15);
        bTextUpdate = true;
        break;

        case ID_ADDNOUNLIST:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_NOUNLIST), hWnd, ADDNOUNLIST);
            break;

        case ID_HIGHPEN:
        {
            UINT state = GetMenuState(hMenu, ID_HIGHPEN, MF_BYCOMMAND);
            if (state == MF_CHECKED)
            {
                CheckMenuItem(hMenu, ID_HIGHPEN, MF_UNCHECKED);
                // ������ �׸��� �׸���. 
                iDrawCount = -1;
                InvalidateRect(hWnd, NULL, TRUE);
                bTextUpdate = true;
                g_bDraw = false;
            }

            else
            {
                CheckMenuItem(hMenu, ID_HIGHPEN, MF_CHECKED);
                // ������ �׸��� �׸���. 
                iDrawCount = 0;
                InvalidateRect(hWnd, NULL, TRUE);
                bTextUpdate = true;
            }

        }
        break;


        case ID_LineSpacing:
            // �� ������ �����Ѵ�.
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_LINESPACING), hWnd, LINESPACING);
            InvalidateRect(hWnd, NULL, TRUE);
            CreateCaret(hWnd, NULL, 3, 15);
            bTextUpdate = true;
            break;

        case ID_TALK:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TALK), hWnd, TALK);
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

            RECT rc = { tPos.x, tPos.y, tPos.x + 5, tPos.y + g_fOffsetY };

            // Caret �κ��� ���� �ʵ��� �����ش�.
            FillRect(hdc, &rc, g_WindowBrush);

            if (!(vecStorageText.empty() && vectext.empty() && iLine > MAX_LINE))
            {
                iLine++;
                iCount++;
                vecStorageText.push_back(vectext.at(vectext.size() - 1));
                vectext.clear();
                vectext.resize(1);
            }
        }

        if (wParam == VK_SPACE)
        {
            // �ڵ� ��� ����� üũ �Ǿ��� ��� ����� ����ȴ�.
            UINT state = GetMenuState(hMenu, ID_AUTONOUNFIX, MF_BYCOMMAND);
            if (state == MF_CHECKED)
            {
                // �ܾ �ٲ��ش�.
                TCHAR cFilePath[100];
                memset(cFilePath, 0, sizeof(cFilePath));
                
                if (GetCurrentDirectory(sizeof(cFilePath), cFilePath) > 0)
                {
                    vector<string> vecNounBuffer;

                    cFilePath[lstrlen(cFilePath)] = '\\';
                    cFilePath[lstrlen(cFilePath)] = 'n';
                    cFilePath[lstrlen(cFilePath)] = 'o';
                    cFilePath[lstrlen(cFilePath)] = 'u';
                    cFilePath[lstrlen(cFilePath)] = 'n';
                    cFilePath[lstrlen(cFilePath)] = '.';
                    cFilePath[lstrlen(cFilePath)] = 't';
                    cFilePath[lstrlen(cFilePath)] = 'x';
                    cFilePath[lstrlen(cFilePath)] = 't';

                    // ��� ���Ͽ��� ��� ����� �ҷ��´�.
                    vecNounBuffer = OutFromFile(cFilePath, hWnd, false);

                    // �����ڵ带 ��ȯ
                    wstring sText_Temp(g_CH_SPELLING_Temp);
                    string sTextWideTemp(sText_Temp.begin(), sText_Temp.end());

                    // �κ� ��ȯ �κ� ã��
                    bool bFind;
                    POINT StorReplacePos;
                    POINT TextReplacePos;
                    string sNoun;

                    // ���� �ʱ�ȭ
                    bFind = false;
                    StorReplacePos.x = 0;
                    StorReplacePos.y = 0;

                    int iSameCount = 0;

                    // �����̽� �� ���� ���� ���ڿ� ������. 
                    // ����� �����̽� ������ ���ڿ��� ������ �κ�
                    for (int i = vectext.at(0).size() - 1; i >= 0; i--)
                    {
                        if (vectext.at(0).at(i) != ' ')
                        {
                            sTextWideTemp.push_back(vectext.at(0).at(i));
                        }

                        else
                            break;
                    }

                    reverse(sTextWideTemp.begin(), sTextWideTemp.end());

                    // 3 ���� �̻� �����ϵ���
                    if (sTextWideTemp.size() > 2)
                    {
                        // ���� ����� ��縦 �˻��Ѵ�.
                        for (int i = 0; i < vecNounBuffer.size(); i++)
                        {
                            for (int j = 0; j < vecNounBuffer.at(i).size(); j++)
                            {
                                if (vecNounBuffer.at(i).at(j) == sTextWideTemp.at(iSameCount))
                                {
                                    iSameCount++;

                                    // ���� + 1 ��ŭ ���ٸ� �ٲ��ش�.
                                    if (iSameCount == sTextWideTemp.size() / 2 + 1)
                                    {
                                        sNoun = vecNounBuffer.at(i);
                                        bFind = true;
                                        break;
                                    }
                                }

                                else
                                    iSameCount = 0;
                            }

                            if (bFind)
                                break;
                        }

                        // ���� �ܾ ã�� ���ߴٸ� �������´�.
                        if (!bFind)
                        {
                            InvalidateRect(hWnd, NULL, TRUE);
                            CreateCaret(hWnd, NULL, 3, 15);
                            bTextUpdate = true;
                            break;
                        }

                        bFind = false;
                        iSameCount = sTextWideTemp.size() - 1;

                        // �ǽð� �Է� ���ۿ� �ش� �Է��� ���ڿ��� �ִ��� �˻��Ѵ�. �����̽� �� ���� �������� �˻��Ѵ�.
                        for (int i = 0; i < vectext.size(); i++)
                        {
                            for (int j = vectext.at(i).size() - 1; j >= 0; j--)
                            {
                                if (vectext.at(i).at(j) == sTextWideTemp.at(iSameCount))
                                {
                                    iSameCount--;

                                    if (iSameCount == -1)
                                    {
                                        StorReplacePos.x = j;
                                        StorReplacePos.y = i;
                                        bFind = true;
                                        break;
                                    }
                                }

                                else
                                    iSameCount = sTextWideTemp.size() - 1;
                            }
                            if (bFind)
                                break;
                        }

                        if (bFind)
                        {
                            vectext.at(StorReplacePos.y).erase(StorReplacePos.x, sTextWideTemp.size());
                            vectext.at(StorReplacePos.y).insert(StorReplacePos.x, sNoun);
                            InvalidateRect(hWnd, NULL, TRUE);
                            bTextUpdate = true;
                            CreateCaret(hWnd, NULL, 3, 15);
                            break;
                        }
                    }
                }
            }
        }


        break;
    }

    // �׸� �׸��� ���� 

    case WM_LBUTTONDOWN:
    {
        // ���콺 ���� 
        if (iDrawCount == 0)
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            //	��ǥ�� ���;� �Ѵ�.
            g_iY = HIWORD(lParam);
            g_iX = LOWORD(lParam);
            //	���콺�� ���� �ִ�.
            g_bDraw = true;

        }

        break;

    }
    break;

    case WM_PAINT:
    {
        ShowCaret(hWnd);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (iHour > 5 && iHour < 19)
        {
            SetBkColor(hdc, RGB(255, 255, 255));
        }

        else
        {
            SetBkColor(hdc, RGB(0, 0, 0));
        }


        // ��Ʈ ���� �Լ�
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
                    SetCaretPos(size.cx + g_fOffsetX, iLine * g_fOffsetY);
                    TextOutA(hdc, g_fOffsetX, g_fOffsetY * i, vecStorageText.at(i).c_str(), vecStorageText.at(i).length());
                }
                bTextUpdate = false;
            }
            GetTextExtentPointA(hdc, vectext.at(vectext.size() - 1).c_str(), vectext.at(vectext.size() - 1).size(), &size);
            SetCaretPos(size.cx + g_fOffsetX, iLine * g_fOffsetY);
            TextOutA(hdc, g_fOffsetX, g_fOffsetY * iLine, vectext.at(count).c_str(), vectext.at(count).length());
        }

        SelectObject(hdc, OldFont);
        DeleteObject(hFont);


        EndPaint(hWnd, &ps);
    }
    break;

    case WM_CHAR:
    {
        // �ִ� ������ �Ѿ��� ���
        if (iLine > MAX_LINE)
        {
            MessageBox(hWnd, L"�Է� ���� ���ڿ� �ʰ�", L"�޸���", MB_OK);
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

            // ����Ű�� ���ڴ� �����Ѵ�.
            if ((!(wParam == VK_RETURN || wParam == VK_BACK)) && bInputAble)
            {
                vectext.at(vectext.size() - 1).push_back(((TCHAR)wParam));
                iCount++;
            }

        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }

    //	�׸� �׸��� ��( ���콺�� ���� ���¿����� )
    //	���콺 �̵�
    case WM_MOUSEMOVE:
    {
        // ���콺�� �����ִ°�?
        if (g_bDraw == true)
        {
            // DC�� ���´�.
            HDC hDC = GetDC(hWnd);

            HPEN MyPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 121));
            HPEN hPrevPen = (HPEN)SelectObject(hDC, MyPen);

            //	���� �������� �����Ѵ�.( ���� ��ǥ���̾�� �Ѵ�. )
            hPrevPen = (HPEN)SelectObject(hDC, MyPen);
            MoveToEx(hDC, g_iX, g_iY, NULL);

            //	���� ���콺 ��ǥ�� ���´�.
            g_iY = HIWORD(lParam);
            g_iX = LOWORD(lParam);

            //	���� �ߴ´�.
            LineTo(hDC, g_iX, g_iY);

            //	DC�� release ��Ų��.
            ReleaseDC(hWnd, hDC);
        }

        break;
    }

    //	�׸� �׸��� ��
    case WM_LBUTTONUP:
    {
        //	���콺�� �����ִٰ� ���� ����̹Ƿ�, �׸��� ������ �������� �ǹ�
        g_bDraw = false;

        break;
    }

    case WM_DESTROY:
        closesocket(s);
        WSACleanup();
        HideCaret(hWnd);
        DestroyCaret();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;

}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
    UNREFERENCED_PARAMETER(lParam);
    switch (iMsg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_CHANGE:
            GetDlgItemText(hDlg, IDC_EDIT1, g_CH_SPELLING_Temp, 10);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case ID_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return(INT_PTR)TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK LINESPACING(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    TCHAR buffer[10];
    switch (iMsg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_OK:
            GetDlgItemText(hDlg, IDC_EDIT1, buffer, 10);
            g_fOffsetY = _wtoi(buffer);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case ID_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ADDNOUNLIST(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hList;
    static int iSelection = -1;
    static vector<string> vecNounBuffer;
    TCHAR name[20] = { 0, };
    UNREFERENCED_PARAMETER(lParam);
    switch (iMsg)
    {
    case WM_INITDIALOG:
        hList = GetDlgItem(hDlg, IDC_LIST_Name);

        // ������ �о���δ�.
        TCHAR cFilePath[100];
        memset(cFilePath, 0, sizeof(cFilePath));

        if (GetCurrentDirectory(sizeof(cFilePath), cFilePath) > 0)
        {

            cFilePath[lstrlen(cFilePath)] = '\\';
            cFilePath[lstrlen(cFilePath)] = 'n';
            cFilePath[lstrlen(cFilePath)] = 'o';
            cFilePath[lstrlen(cFilePath)] = 'u';
            cFilePath[lstrlen(cFilePath)] = 'n';
            cFilePath[lstrlen(cFilePath)] = '.';
            cFilePath[lstrlen(cFilePath)] = 't';
            cFilePath[lstrlen(cFilePath)] = 'x';
            cFilePath[lstrlen(cFilePath)] = 't';

            // ��� ���Ͽ��� ��� ����� �ҷ��´�.
            vecNounBuffer = OutFromFile(cFilePath, 0, false);



            for (int i = 0; i < vecNounBuffer.size(); i++)
            {
                TCHAR AddName[20] = { 0, };
                int nLen = MultiByteToWideChar(CP_ACP, 0, vecNounBuffer.at(i).c_str(), strlen(vecNounBuffer.at(i).c_str()), NULL, NULL);
                MultiByteToWideChar(CP_ACP, 0, vecNounBuffer.at(i).c_str(), strlen(vecNounBuffer.at(i).c_str()), AddName, nLen);
                SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)AddName);
            }
        }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SAVE:
            FILE* fSaveFile;
            fopen_s(&fSaveFile, "noun.txt", "wt");

            if (fSaveFile)
            {
                TCHAR buffer[500];
                memset(buffer, 0, sizeof(buffer));

                int ibufferLastIndex = 0;

                // ���ۿ� ���� �о���̱�
                for (int i = 0; i < vecNounBuffer.size(); ++i)
                {
                    for (int j = 0; j < vecNounBuffer.at(i).size(); ++j)
                    {

                        buffer[j + ibufferLastIndex] = vecNounBuffer.at(i).at(j);


                        if (j + 1 >= vecNounBuffer.at(i).size())
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
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case ID_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case ID_ADD:
            GetDlgItemText(hDlg, IDC_EDIT1, name, 20);
            if (_tcscmp(name, _T("")))
            {
                char AddStr[20] = { 0, };
                SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)name);
                int len = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
                WideCharToMultiByte(CP_ACP, 0, name, -1, AddStr, len, NULL, NULL);

                vecNounBuffer.push_back(AddStr);

                // ����Ʈ �ڽ� �ʱ�ȭ
                SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), 0);
            }
            return (INT_PTR)TRUE;
        case ID_EDIT:
            if (iSelection != -1)
            {
                // �ش� �ε��� ����
                GetDlgItemText(hDlg, IDC_EDIT1, name, 20);
                SendMessage(hList, LB_DELETESTRING, iSelection, 0);
                SendMessage(hList, LB_ADDSTRING, iSelection, (LPARAM)name);
                iSelection = -1;
                return (INT_PTR)TRUE;
            }
        case ID_DELETE:
        {
            char sCompare[20] = { 0, };
            SendMessage(hList, LB_GETTEXT, iSelection, (LPARAM)name);
            SendMessage(hList, LB_DELETESTRING, iSelection, 0);

            vector<string>::iterator iter;
            vector<string>::iterator iterEnd = vecNounBuffer.end();

            int len = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
            WideCharToMultiByte(CP_ACP, 0, name, -1, sCompare, len, NULL, NULL);


            for (iter = vecNounBuffer.begin(); iter != iterEnd; iter++)
            {
                if ((*iter) == sCompare)
                {
                    (*iter).erase();
                    vecNounBuffer.erase(iter);
                    break;
                }
            }
            return (INT_PTR)TRUE;
        }
        case IDC_LIST_Name:
            if (HIWORD(wParam) == LBN_SELCHANGE)
                iSelection = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
            // ���� �ش� �ε��� �ؽ�Ʈ�� �����´�.
            SendMessage(hList, LB_GETTEXT, iSelection, (LPARAM)name);
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), name);
            return (INT_PTR)TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;;
    }

    return (INT_PTR)FALSE;
}


INT_PTR CALLBACK TALK(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    static HWND hList;

    switch (iMsg)
    {
    case WM_INITDIALOG:
        hList = GetDlgItem(hDlg, IDC_LIST_TALK);
        g_hDlg_Talk = hList;
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SEND:
            if (cs == INVALID_SOCKET)
                return (INT_PTR)FALSE;
            else
            {
                GetDlgItemText(hDlg, IDC_EDIT1, socketstr, 20);
#ifdef _UNICODE
                iMsgLen = WideCharToMultiByte(CP_ACP, 0, socketstr, -1, NULL, 0, NULL, NULL);
                WideCharToMultiByte(CP_ACP, 0, socketstr, -1, socketbuffer, iMsgLen, NULL, NULL);
#else
                strcpy_s(socketbuffer, str);
#endif
                g_sMyIp = GetMyIpAddress();
                g_sMyIp += " : ";
                g_sMyIp += socketbuffer;
                memset(socketbuffer, 0, sizeof(socketbuffer));
                strcpy_s(socketbuffer, g_sMyIp.c_str());
                send(cs, (LPSTR)socketbuffer, strlen(socketbuffer) + 1, 0);
                iSocketCount = 0;
            }
            iMsgLen = MultiByteToWideChar(CP_ACP, 0, g_sMyIp.c_str(), strlen(g_sMyIp.c_str()), NULL, NULL);
            MultiByteToWideChar(CP_ACP, 0, g_sMyIp.c_str(), strlen(g_sMyIp.c_str()) , socketstr, iMsgLen);

            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)socketstr);
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), 0);

            memset(socketstr, 0, sizeof(socketstr));
            g_sMyIp = "";
            

            return (INT_PTR)TRUE;
        case ID_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}



// ������ �о�´�.
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

void PrintMessage(HWND hList)
{
    if (_tcscmp(socketmsg, _T("")))
     {
         SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)socketmsg);
         bPaint = true;
     }
}


string GetMyIpAddress()
{
    string strMyIp = "";
    vector<sockaddr_in> vecMyIp;

    char myaddr[256];

    PHOSTENT pHostInfo;


    gethostname(myaddr, sizeof(myaddr));
    pHostInfo = gethostbyname(myaddr);

    if (pHostInfo)
    {
        for (int i = 0; pHostInfo->h_addr_list[i] != NULL; i++)
        {
            memcpy(&addr.sin_addr, pHostInfo->h_addr_list[i],
                pHostInfo->h_length);
            vecMyIp.push_back(addr);
        }
    }

    for (int i = 0; i < vecMyIp.size(); i++)
    {
        strMyIp += inet_ntoa(vecMyIp.at(i).sin_addr);
        strMyIp += "\n";
    }

    return strMyIp;
}