#include "framework.h"
#include "Protect the Earth.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "D2BMPLOADER.h"
#include "errh.h"
#include "fcheck.h"
#include "spaceapi.h"
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"d2d1.lib")
#pragma comment (lib,"dwrite.lib")
#pragma comment (lib,"d2bmploader.lib")
#pragma comment (lib,"errh.lib")
#pragma comment (lib,"fcheck.lib")
#pragma comment (lib,"spaceapi.lib")

#define bWinClassName L"ProtectingEarth"

#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define save_file L".\\res\\data\\save.dat"
#define rec_file L".\\res\\data\\record.dat"
#define help_file L".\\res\\data\\help.dat"
#define sound_file L".\\res\\snd\\main.wav"

#define mNew 1001
#define mSpeed 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define record 2001
#define no_record 2002
#define first_record 2003

#define keyW 0x57
#define keyX 0x58
#define keyA 0x41
#define keyD 0x44
#define keyQ 0x51
#define keyE 0x45
#define keyC 0x43
#define keyZ 0x5A

WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
POINT cur_pos = { 0 };
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HWND bHwnd = nullptr;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint = { 0 };
UINT bTimer = 0;

MSG bMsg = { 0 };
BOOL bRet = 0;

D2D1_RECT_F b1R = { 0, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2R = { scr_width / 3, 0, scr_width / 3 + scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3R = { scr_width - scr_width / 3, 0, scr_width, 50.0f };

D2D1_RECT_F Txtb1R = { 10.0f, 5.0f, scr_width / 3 - 60.0f, 50.0f };
D2D1_RECT_F Txtb2R = { scr_width / 3 + 10.0f, 5.0f, scr_width / 3 + scr_width / 3 - 60.0f, 50.0f };
D2D1_RECT_F Txtb3R = { scr_width - scr_width / 3 + 10.0f, 5.0f, scr_width - 10.0f, 50.0f };

bool pause = false;
bool sound = true;
bool show_help = false;
bool in_client = true;
bool set_name = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

float speed = 1;
int score = 0;
int mins = 0;
int secs = 0;

wchar_t current_player[16] = L"A PILOT";

////////////////////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

ID2D1RadialGradientBrush* butBckg = nullptr;
ID2D1SolidColorBrush* txtBrush = nullptr;
ID2D1SolidColorBrush* hgltBrush = nullptr;
ID2D1SolidColorBrush* inactBrush = nullptr;

ID2D1SolidColorBrush* Star1Brush = nullptr;
ID2D1SolidColorBrush* Star2Brush = nullptr;
ID2D1SolidColorBrush* Star3Brush = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmTxt = nullptr;
IDWriteTextFormat* midTxt = nullptr;
IDWriteTextFormat* bigTxt = nullptr;

ID2D1Bitmap* bmpHeroU = nullptr;
ID2D1Bitmap* bmpHeroUR = nullptr;
ID2D1Bitmap* bmpHeroUL = nullptr;
ID2D1Bitmap* bmpHeroD = nullptr;
ID2D1Bitmap* bmpHeroDR = nullptr;
ID2D1Bitmap* bmpHeroDL = nullptr;
ID2D1Bitmap* bmpHeroL = nullptr;
ID2D1Bitmap* bmpHeroR = nullptr;

ID2D1Bitmap* bmpHeroBulU = nullptr;
ID2D1Bitmap* bmpHeroBulUR = nullptr;
ID2D1Bitmap* bmpHeroBulUL = nullptr;
ID2D1Bitmap* bmpHeroBulD = nullptr;
ID2D1Bitmap* bmpHeroBulDR = nullptr;
ID2D1Bitmap* bmpHeroBulDL = nullptr;
ID2D1Bitmap* bmpHeroBulL = nullptr;
ID2D1Bitmap* bmpHeroBulR = nullptr;

ID2D1Bitmap* bmpEvil1 = nullptr;
ID2D1Bitmap* bmpEvil2 = nullptr;
ID2D1Bitmap* bmpEvil3 = nullptr;

ID2D1Bitmap* bmpEvilBul = nullptr;

//////////////////////////////////////////

struct BULLETS
{
    space::OBJECT Dims;
    dirs dir;
};

space::Person Hero = nullptr;
dirs hero_prev_dir = dirs::right;

std::vector<space::Person> vEvils;
std::vector<BULLETS> vMyBullets;
std::vector<BULLETS>vEnemyBullets;

std::vector<space::OBJECT> vStars;

space::LINEDATA OneLine;
space::AIDATA AI_Input;
space::AIOUTPUT AI_Output;

//////////////////////////////////////////////

template <typename OBJ> bool Swipe(OBJ** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream rec(L".\\res\\data\\error.log", std::ios::app);
    rec << what << L" ! Time stamp: " << std::chrono::system_clock::now() << std::endl;
    rec.close();
}
void ClearResources()
{
    Swipe(&iFactory);
    Swipe(&Draw);
    Swipe(&butBckg);
    Swipe(&txtBrush);
    Swipe(&hgltBrush);
    Swipe(&inactBrush);
    Swipe(&Star1Brush);
    Swipe(&Star2Brush);
    Swipe(&Star3Brush);
    Swipe(&iWriteFactory);
    Swipe(&nrmTxt);
    Swipe(&midTxt);
    Swipe(&bigTxt);

    Swipe(&bmpHeroU);
    Swipe(&bmpHeroUR);
    Swipe(&bmpHeroUL);
    Swipe(&bmpHeroD);
    Swipe(&bmpHeroDR);
    Swipe(&bmpHeroDL);
    Swipe(&bmpHeroR);
    Swipe(&bmpHeroL);

    Swipe(&bmpHeroBulU);
    Swipe(&bmpHeroBulUR);
    Swipe(&bmpHeroBulUL);
    Swipe(&bmpHeroBulD);
    Swipe(&bmpHeroBulDR);
    Swipe(&bmpHeroBulDL);
    Swipe(&bmpHeroBulR);
    Swipe(&bmpHeroBulL);

    Swipe(&bmpEvil1);
    Swipe(&bmpEvil2);
    Swipe(&bmpEvil3);
    Swipe(&bmpEvilBul);
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    std::remove(tmp_file);
    ClearResources();
    exit(1);
}
void InitGame()
{
    speed = 1;
    score = 0;
    mins = 0;
    secs = 0;
    wcscpy_s(current_player, L"A PILOT");
    set_name = false;

    Swipe(&Hero);

    if (!vEvils.empty())
        for (int i = 0; i < vEvils.size(); i++)Swipe(&vEvils[i]);
    vEvils.clear();
    vMyBullets.clear();
    vEnemyBullets.clear();
    vStars.clear();
    //////////////////////////////////////////////////////

    Hero = space::iCreatePerson(types::hero, 50.0f, scr_height - 150.0f);
    if (Hero)Hero->dir = dirs::right;

}

void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return (INT_PTR)(TRUE);
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 15) < 1)
            {
                if (sound)MessageBeep(MB_ICONEXCLAMATION);
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                wcscpy_s(current_player, L"A PILOT");
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
            break;
        }
    }

    return (INT_PTR)FALSE;
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, bTimer, 1000, NULL);
        srand((unsigned int)(time(NULL)));

        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();

        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mSpeed, L"Турбо режим");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(50, 50, 50)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        secs++;
        mins = (int)(floor(secs / 60));
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }
            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= Txtb1R.left && cur_pos.x <= Txtb1R.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                    }
                    if (b2Hglt || b3Hglt)
                    {
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= Txtb2R.left && cur_pos.x <= Txtb2R.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                    }
                    if (b1Hglt || b3Hglt)
                    {
                        b1Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= Txtb3R.left && cur_pos.x <= Txtb3R.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                    }
                    if (b1Hglt || b2Hglt)
                    {
                        b1Hglt = false;
                        b2Hglt = false;
                    }
                }
                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                    b2Hglt = false;
                    b3Hglt = false;
                }
                SetCursor(mainCursor);
                return true;
            }
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Ако рестартираш, ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mSpeed:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            MessageBox(hwnd, L"Турбо режим включен !\n\nУвеличавам скоростта !",
                L"Турбо режим !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
            pause = false;
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        }
        break;

    case WM_KEYDOWN:
        if (!Hero)break;
        hero_prev_dir = Hero->dir;
        switch (wParam)
        {
        case keyW:
            Hero->dir = dirs::up;
            break;

        case keyX:
            Hero->dir = dirs::down;
            break;

        case keyA:
            Hero->dir = dirs::left;
            break;

        case keyD:
            Hero->dir = dirs::right;
            break;

        case keyQ:
            Hero->dir = dirs::u_l;
            break;

        case keyE:
            Hero->dir = dirs::u_r;
            break;

        case keyZ:
            Hero->dir = dirs::d_l;
            break;

        case keyC:
            Hero->dir = dirs::d_r;
            break;

        default:Hero->dir = dirs::stop;

        }
        break;


    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream tmp(Ltmp_file);
        tmp << L"Igrata e v action !";
        tmp.close();
    }

    int winx = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2));
    if (GetSystemMetrics(SM_CXSCREEN) < winx + (int)(scr_width) || GetSystemMetrics(SM_CYSCREEN) < 20 + (int)(scr_height))
        ErrExit(eScreen);
    
    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\bmain.ico", IMAGE_ICON, 255, 186, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFile(L".\\res\\main.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCursor || !outCursor)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &WinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));
    bWin.hIcon = mainIcon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ЗВЕЗДНА ВОЙНА !", WS_CAPTION | WS_SYSMENU, winx, 20, (int)(scr_width), (int)scr_height,
        NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else ShowWindow(bHwnd, SW_SHOWDEFAULT);
    ////////////////////////////////////////////////////////////////////////////////

    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 Factory");
        ErrExit(eD2D);
    }
    if (iFactory && bHwnd)
        hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
            D2D1::SizeU((UINT32)(scr_width), (UINT)(scr_height))), &Draw);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 HwndRenderTarget");
        ErrExit(eD2D);
    }

    D2D1_GRADIENT_STOP gStops[2] = { 0 };
    ID2D1GradientStopCollection* gColl = nullptr;

    gStops[0].position = 0;
    gStops[0].color = D2D1::ColorF(D2D1::ColorF::Orange);
    gStops[1].position = 1.0f;
    gStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);

    if (Draw)
        hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 GradientStopCollection");
        ErrExit(eD2D);
    }
    if (Draw && gColl)
        hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
            D2D1::Point2F(0, 0), scr_width / 2, 25.0f), gColl, &butBckg);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 RadialGradientBrush");
        ErrExit(eD2D);
    }
    Swipe(&gColl);

    if (Draw)
    {
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::ForestGreen), &txtBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &hgltBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateGray), &inactBrush);
    }
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 Text Brushes");
        ErrExit(eD2D);
    }

    if (Draw)
    {
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::AliceBlue), &Star1Brush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &Star2Brush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &Star3Brush);
    }
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 Field & Stars Brushes");
        ErrExit(eD2D);
    }

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 WriteFactory");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
    {
        hr = iWriteFactory->CreateTextFormat(L"CANDARA", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"", &nrmTxt);
        hr = iWriteFactory->CreateTextFormat(L"CANDARA", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"", &midTxt);
        hr = iWriteFactory->CreateTextFormat(L"CANDARA", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigTxt);
    }
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 TextFormats");
        ErrExit(eD2D);
    }
    ///////////////////////////////////////////////////////////////
    bmpHeroBulD = Load(L".\\res\\img\\hero\\bullet\\d.png", Draw);
    if (!bmpHeroBulD)
    {
        LogError(L"Error loading bmpHeroBulD");
        ErrExit(eD2D);
    }
    bmpHeroBulDR = Load(L".\\res\\img\\hero\\bullet\\dr.png", Draw);
    if (!bmpHeroBulDR)
    {
        LogError(L"Error loading bmpHeroBulDR");
        ErrExit(eD2D);
    }
    bmpHeroBulDL = Load(L".\\res\\img\\hero\\bullet\\dl.png", Draw);
    if (!bmpHeroBulDL)
    {
        LogError(L"Error loading bmpHeroBulDL");
        ErrExit(eD2D);
    }
    bmpHeroBulU = Load(L".\\res\\img\\hero\\bullet\\u.png", Draw);
    if (!bmpHeroBulU)
    {
        LogError(L"Error loading bmpHeroBulU");
        ErrExit(eD2D);
    }
    bmpHeroBulUR = Load(L".\\res\\img\\hero\\bullet\\ur.png", Draw);
    if (!bmpHeroBulUR)
    {
        LogError(L"Error loading bmpHeroBulUR");
        ErrExit(eD2D);
    }
    bmpHeroBulUL = Load(L".\\res\\img\\hero\\bullet\\Ul.png", Draw);
    if (!bmpHeroBulUL)
    {
        LogError(L"Error loading bmpHeroBulUL");
        ErrExit(eD2D);
    }
    bmpHeroBulL = Load(L".\\res\\img\\hero\\bullet\\l.png", Draw);
    if (!bmpHeroBulL)
    {
        LogError(L"Error loading bmpHeroBulL");
        ErrExit(eD2D);
    }
    bmpHeroBulR = Load(L".\\res\\img\\hero\\bullet\\r.png", Draw);
    if (!bmpHeroBulR)
    {
        LogError(L"Error loading bmpHeroBulR");
        ErrExit(eD2D);
    }

    bmpHeroD = Load(L".\\res\\img\\hero\\d.png", Draw);
    if (!bmpHeroD)
    {
        LogError(L"Error loading bmpHeroD");
        ErrExit(eD2D);
    }
    bmpHeroDR = Load(L".\\res\\img\\hero\\dr.png", Draw);
    if (!bmpHeroDR)
    {
        LogError(L"Error loading bmpHeroDR");
        ErrExit(eD2D);
    }
    bmpHeroDL = Load(L".\\res\\img\\hero\\dl.png", Draw);
    if (!bmpHeroDL)
    {
        LogError(L"Error loading bmpHeroDL");
        ErrExit(eD2D);
    }
    bmpHeroU = Load(L".\\res\\img\\hero\\u.png", Draw);
    if (!bmpHeroU)
    {
        LogError(L"Error loading bmpHeroU");
        ErrExit(eD2D);
    }
    bmpHeroUR = Load(L".\\res\\img\\hero\\ur.png", Draw);
    if (!bmpHeroUR)
    {
        LogError(L"Error loading bmpHeroUR");
        ErrExit(eD2D);
    }
    bmpHeroUL = Load(L".\\res\\img\\hero\\Ul.png", Draw);
    if (!bmpHeroUL)
    {
        LogError(L"Error loading bmpHeroUL");
        ErrExit(eD2D);
    }
    bmpHeroL = Load(L".\\res\\img\\hero\\l.png", Draw);
    if (!bmpHeroL)
    {
        LogError(L"Error loading bmpHeroL");
        ErrExit(eD2D);
    }
    bmpHeroR = Load(L".\\res\\img\\hero\\r.png", Draw);
    if (!bmpHeroR)
    {
        LogError(L"Error loading bmpHeroR");
        ErrExit(eD2D);
    }

    bmpEvil1 = Load(L".\\res\\img\\evil\\evil1.png", Draw);
    if (!bmpEvil1)
    {
        LogError(L"Error loading bmpEvil1");
        ErrExit(eD2D);
    }
    bmpEvil2 = Load(L".\\res\\img\\evil\\evil2.png", Draw);
    if (!bmpEvil2)
    {
        LogError(L"Error loading bmpEvil1");
        ErrExit(eD2D);
    }
    bmpEvil3 = Load(L".\\res\\img\\evil\\evil3.png", Draw);
    if (!bmpEvil3)
    {
        LogError(L"Error loading bmpEvil3");
        ErrExit(eD2D);
    }
    bmpEvilBul = Load(L".\\res\\img\\evil\\bullet.png", Draw);
    if (!bmpEvilBul)
    {
        LogError(L"Error loading bmpEvilBul");
        ErrExit(eD2D);
    }
    ////////////////////////////////////////////////////////////

    D2D1_RECT_F up_txtR = { 0, -150.0f , 800.0f, 50.0f };
    D2D1_RECT_F down_txtR = { scr_width, scr_height, scr_width + 400.0f, scr_height + 250.0f };

    mciSendString(L"play .\\res\\snd\\intro.wav", NULL, NULL, NULL);
    bool up_ok = false;
    bool down_ok = false;

    while (!up_ok || !down_ok)
    {
        if (Draw && bigTxt && nrmTxt)
        {
            if (!up_ok)
            {
                if (up_txtR.right < scr_width - 100.0f)
                {
                    up_txtR.left += 2.5f;
                    up_txtR.right += 2.5f;
                }
                if (up_txtR.top < scr_height / 2 - 100.0f)
                {
                    up_txtR.top += 2.5f;
                    up_txtR.bottom += 2.5f;
                }
                if (up_txtR.right >= scr_width - 100.0f && up_txtR.top >= scr_height / 2 - 100.0f)up_ok = true;
            }
            if (!down_ok)
            {
                if (down_txtR.left > scr_width / 2)
                {
                    down_txtR.left -= 2.5f;
                    down_txtR.right -= 2.5f;
                }
                if (down_txtR.top > scr_height / 2 + 100.0f)
                {
                    down_txtR.top -= 2.5f;
                    down_txtR.bottom -= 2.5f;
                }
                if (down_txtR.left <= scr_width / 2 && down_txtR.top <= scr_height / 2 + 100.0f)down_ok = true;
            }

            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::Silver));
            Draw->DrawTextW(L"ИЗВЪНЗЕМНИ ГАДОВЕ !", 20, bigTxt, up_txtR, txtBrush);
            Draw->DrawTextW(L"dev. Daniel !", 14, bigTxt, down_txtR, txtBrush);
            Draw->EndDraw();
        }
    }
    Sleep(1500);

    float next_x = 20.0f;
    float next_y = 55.0f;

    for (int i = 0; i < 150; i++)
    {
        int choice = rand() % 3;

        next_x += 10.0f + (float)(rand() % 40);
        next_y += 15.0f + (float)(rand() % 60);
        if (next_x > scr_width - 20.0f) next_x = 20.0f + (float)(rand() % 20);
        if (next_y > 650.0f) next_y = 55.0f + (float)(rand() % 20);

        switch (choice)
        {
        case 0:
            vStars.push_back(space::OBJECT(next_x, next_y, 1, 1.5));
            break;

        case 1:
            vStars.push_back(space::OBJECT(next_x, next_y, 2, 2.5));
            break;

        case 2:
            vStars.push_back(space::OBJECT(next_x, next_y, 3, 2.5));
            break;
        }
    }
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);
    CreateResources();
    ////////////////////////////////////////

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg); 
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::Silver));
            Draw->DrawTextW(L"ПАУЗА", 6, bigTxt, D2D1::RectF(scr_width / 2 - 50.0f, scr_height / 2 - 50.0f, scr_width, scr_height), 
                txtBrush);
            Draw->EndDraw();
            continue;
        }

        ///////////////////////////////////

        //HERO ENGINE *********************

        if (Hero)
            if (Hero->dir != dirs::stop)Hero->Move(speed);

        //ENEMIES ENGINE ********************************

        if (rand() % (200 - 10 * (int)(speed)) == 0)
        {
            int edir = rand() % 8;
            int etype = rand() % 3 + 2;
            switch (edir)
            {
            case 0:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), -50.0f, 0));
                vEvils.back()->dir = dirs::d_r;
                break;

            case 1:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), 
                    (float)(rand() % ((int)(scr_width - 100)) + 100), 0));
                vEvils.back()->dir = dirs::down;
                break;

            case 2:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), scr_width, 0));
                vEvils.back()->dir = dirs::d_l;
                break;

            case 3:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), -50.0f, scr_height));
                vEvils.back()->dir = dirs::u_r;
                break;

            case 4:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), (float)(rand() % ((int)(scr_width - 100)) + 100), 
                    scr_height));
                vEvils.back()->dir = dirs::up;
                break;

            case 5:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), scr_width, scr_height));
                vEvils.back()->dir = dirs::u_l;
                break;

            case 6:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), 0, (float)(rand() % ((int)(scr_height - 200)) + 50)));
                vEvils.back()->dir = dirs::right;
                break;

            case 7:
                vEvils.push_back(space::iCreatePerson(static_cast<types>(etype), scr_width, 
                    (float)(rand() % ((int)(scr_height - 200)) + 50)));
                vEvils.back()->dir = dirs::left;
                break;


            }
        }

        if (!vEvils.empty() && Hero)
        {
            for (std::vector<space::Person>::iterator ship = vEvils.begin(); ship < vEvils.end(); ship++)
            {
                AI_Input.enemy_x = Hero->x;
                AI_Input.enemy_y = Hero->y;
                AI_Input.enemy_ex = Hero->ex;
                AI_Input.enemy_ey = Hero->ey;
                AI_Input.my_x = (*ship)->x;
                AI_Input.my_y = (*ship)->y;
                AI_Input.my_ex = (*ship)->ex;
                AI_Input.my_ey = (*ship)->ey;
                if (((Hero->x >= (*ship)->x - 50.0f && Hero->x <= (*ship)->ex + 50.0f) ||
                    (Hero->ex >= (*ship)->x - 50.0f && Hero->ex <= (*ship)->ex + 50.0f))
                    && ((Hero->y >= (*ship)->y - 50.0f && Hero->y <= (*ship)->ey + 50.0f) ||
                        (Hero->ey >= (*ship)->y - 50.0f && Hero->ey <= (*ship)->ey + 50.0f)))
                    AI_Input.enemy_in_range = true;
                else AI_Input.enemy_in_range = false;

                AI_Output = space::AIToDo(AI_Input);
                if (AI_Output.new_action == choices::shoot)
                {
                    (*ship)->Shoot();
                    (*ship)->dir = dirs::stop;
                }
                else if (AI_Output.new_action == choices::move) (*ship)->dir = AI_Output.new_dir;
                    
                if((*ship)->dir!=dirs::stop)
                    if ((*ship)->Move(speed) == DLL_FAIL)
                    {
                        switch ((*ship)->dir)
                        {
                        case dirs::up:
                            (*ship)->dir = dirs::down;
                            break;

                        case dirs::down:
                            (*ship)->dir = dirs::up;
                            break;

                        case dirs::left:
                            (*ship)->dir = dirs::right;
                            break;

                        case dirs::right:
                            (*ship)->dir = dirs::left;
                            break;

                        case dirs::u_r:
                            (*ship)->dir = dirs::d_l;
                            break;

                        case dirs::u_l:
                            (*ship)->dir = dirs::d_r;
                            break;

                        case dirs::d_r:
                            (*ship)->dir = dirs::u_l;
                            break;

                        case dirs::d_l:
                            (*ship)->dir = dirs::u_r;
                            break;
                        }
                    }
            }
        }





        //DRAW THINGS **********************

        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        if(butBckg) Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), butBckg);
        if (nrmTxt && txtBrush && inactBrush && hgltBrush)
        {
            if (set_name) Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmTxt, Txtb1R, inactBrush);
            else
            {
                if (b1Hglt) Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmTxt, Txtb1R, hgltBrush);
                else Draw->DrawTextW(L"ИМЕ НА ПИЛОТ", 13, nrmTxt, Txtb1R, txtBrush);
            }
            if (b2Hglt) Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTxt, Txtb2R, hgltBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTxt, Txtb2R, txtBrush);
            if (b3Hglt) Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTxt, Txtb3R, hgltBrush);
            else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTxt, Txtb3R, txtBrush);
        }
        if (!vStars.empty())
        {
            for (std::vector<space::OBJECT>::iterator star = vStars.begin(); star < vStars.end(); star++)
            {
                switch ((int)(star->GetWidth()))
                {
                case 1:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(star->x + star->GetWidth() / 2, star->y + star->GetHeight() / 2),
                        star->GetWidth(), star->GetHeight()), Star1Brush);
                    break;

                case 2:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(star->x + star->GetWidth() / 2, star->y + star->GetHeight() / 2),
                        star->GetWidth(), star->GetHeight()), Star2Brush);
                    break;

                case 3:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(star->x + star->GetWidth() / 2, star->y + star->GetHeight() / 2),
                        star->GetWidth(), star->GetHeight()), Star3Brush);
                    break;
                }
            }
        }

        //HERO ******************************

        if (Hero)
        {
            switch (Hero->dir)
            {
            case dirs::down:
                Draw->DrawBitmap(bmpHeroD, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::up:
                Draw->DrawBitmap(bmpHeroU, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::left:
                Draw->DrawBitmap(bmpHeroL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::right:
                Draw->DrawBitmap(bmpHeroR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::d_r:
                Draw->DrawBitmap(bmpHeroDR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::d_l:
                Draw->DrawBitmap(bmpHeroDL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::u_r:
                Draw->DrawBitmap(bmpHeroUR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::u_l:
                Draw->DrawBitmap(bmpHeroUL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::stop:
                switch (hero_prev_dir)
                {
                case dirs::down:
                    Draw->DrawBitmap(bmpHeroD, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::up:
                    Draw->DrawBitmap(bmpHeroU, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::left:
                    Draw->DrawBitmap(bmpHeroL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::right:
                    Draw->DrawBitmap(bmpHeroR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::d_r:
                    Draw->DrawBitmap(bmpHeroDR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::d_l:
                    Draw->DrawBitmap(bmpHeroDL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::u_r:
                    Draw->DrawBitmap(bmpHeroUR, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;

                case dirs::u_l:
                    Draw->DrawBitmap(bmpHeroUL, D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                    break;
                }
                break;
            }
        }


        //ENEMIES ***************************

        if (!vEvils.empty())
        {
            for (std::vector<space::Person>::iterator it = vEvils.begin(); it < vEvils.end(); ++it)
            {
                switch ((*it)->type)
                {
                case types::evil1:
                    Draw->DrawBitmap(bmpEvil1, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    break;

                case types::evil2:
                    Draw->DrawBitmap(bmpEvil2, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    break;

                case types::evil3:
                    Draw->DrawBitmap(bmpEvil3, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    break;
                }
            }
        }

        ////////////////////////////////////

        Draw->EndDraw();
    }

    ////////////////////////////////////////
    std::remove(tmp_file);
    ClearResources();
    return (int) bMsg.wParam;
}