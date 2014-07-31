// WinMsgLoop.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WinMsgLoop.h"

#include <functional>

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HANDLE g_Sig1 = NULL;
HANDLE g_Sig2 = NULL;
// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass( HINSTANCE hInstance );
BOOL				InitInstance( HINSTANCE,int );
LRESULT CALLBACK	WndProc( HWND,UINT,WPARAM,LPARAM );
INT_PTR CALLBACK	About( HWND,UINT,WPARAM,LPARAM );
void                SigTest1( HANDLE hSig );
void                SigTest2( HANDLE hSig );

int APIENTRY _tWinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

    HANDLE hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL,0,0 );

 	// TODO: �ڴ˷��ô��롣
	HACCEL hAccelTable;
    WinMsgLoop GuiMsgLoop;
    g_Sig1 = CreateEvent( 0,TRUE,FALSE,nullptr );
    g_Sig2 = CreateEvent( 0,TRUE,FALSE,nullptr );

    GuiMsgLoop.AddHandle( g_Sig1,std::bind( SigTest1,std::placeholders::_1 ) );
    GuiMsgLoop.AddHandle( g_Sig2,std::bind( SigTest2,std::placeholders::_1 ) );


	// ��ʼ��ȫ���ַ���
	LoadString( hInstance,IDS_APP_TITLE,szTitle,MAX_LOADSTRING );
	LoadString( hInstance,IDC_WINMSGLOOP,szWindowClass,MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators( hInstance,MAKEINTRESOURCE(IDC_WINMSGLOOP) );

	// ����Ϣѭ��:
	const int iRetCode=GuiMsgLoop.RunMsgLoop( hAccelTable );

    CloseHandle( g_Sig1 );
    CloseHandle( g_Sig2 );
	return iRetCode;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance,MAKEINTRESOURCE(IDI_WINMSGLOOP) );
	wcex.hCursor		= LoadCursor( NULL,IDC_ARROW );
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE( IDC_WINMSGLOOP );
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon( wcex.hInstance,MAKEINTRESOURCE(IDI_SMALL) );

	return RegisterClassEx( &wcex );
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow( szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,hInstance,NULL );

   if ( !hWnd )
   {
      return FALSE;
   }

   ShowWindow( hWnd,nCmdShow );
   UpdateWindow( hWnd );

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
	int wmId(0), wmEvent(0);
    PAINTSTRUCT ps = {0};
	HDC hdc        = NULL;

	switch( message )
	{
	case WM_COMMAND:
		wmId    = LOWORD( wParam );
		wmEvent = HIWORD( wParam );
		// �����˵�ѡ��:
		switch( wmId )
		{
		case IDM_ABOUT:
			DialogBox( hInst,MAKEINTRESOURCE(IDD_ABOUTBOX),hWnd,About );
			break;
		case IDM_EXIT:
			DestroyWindow( hWnd );
            break;
        case ID_SIGNAL_1:
            SetEvent( g_Sig1 );
			break;
        case ID_SIGNAL_2:
            SetEvent( g_Sig2 );
            break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint( hWnd,&ps );
		// TODO: �ڴ���������ͼ����...
		EndPaint( hWnd,&ps );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd,message,wParam,lParam );
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	switch( message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if ( LOWORD(wParam)==IDOK || 
             LOWORD(wParam)==IDCANCEL )
		{
			EndDialog( hDlg,LOWORD(wParam) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void SigTest1( HANDLE hSig )
{
    MessageBox( NULL,_T("�������ź�1"),L"�ź���ʾ",MB_OK );
    ResetEvent( hSig );
}

void SigTest2( HANDLE hSig )
{
    MessageBox( NULL,_T("�������ź�2"),L"�ź���ʾ",MB_OK );
    ResetEvent( hSig );
}