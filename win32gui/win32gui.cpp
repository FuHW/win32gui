// win32gui.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "win32gui.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WCHAR szInvalidNumber[MAX_LOADSTRING];
WCHAR szGotoError[MAX_LOADSTRING];

HWND hwndGoto = NULL;	// Window handle of dialog box

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32GUI, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDS_GOTO_INVALIDNUMBER, szInvalidNumber, MAX_LOADSTRING);
	LoadStringW(hInstance, IDS_GOTO_ERROR, szGotoError, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32GUI));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)
			|| !IsWindow(hwndGoto)
			|| !IsDialogMessage(hwndGoto, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

/* Test UI elements below start */
int DisplayConfirmSaveAsMessageBox() {
	int msgboxID = MessageBox(
		NULL,
		L"temp.txt already exist.\nDo you want to replace it?",
		L"Confirm Save As",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	if (msgboxID == IDYES) {
		OutputDebugString(L"Yes Clicked");
	} else {
		OutputDebugString(L"No Clicked");
	}

	return msgboxID;
}

WCHAR szItemName[80];	// receives name of item to delete

BOOL CALLBACK DeleteItemProc(HWND hwndDlg,
							 UINT message,
							 WPARAM wParam,
							 LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					if (!GetDlgItemText(hwndDlg, ID_ITEMNAME, szItemName, 80))
						*szItemName = 0;
					OutputDebugString(szItemName);
					break;
				case IDCANCEL:
					EndDialog(hwndDlg, wParam);
					break;
			}
			break;
	}

	return FALSE;
}

int iLine;			// Receives line number.
BOOL fRelative;		// Receives check box status.

BOOL CALLBACK GoToProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	BOOL fSuccess;

	switch (message) {
		case WM_INITDIALOG:
			CheckDlgButton(hwndDlg, ID_ABSREL, fRelative);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					fRelative = IsDlgButtonChecked(hwndDlg, ID_ABSREL);
					iLine = GetDlgItemInt(hwndDlg, ID_LINE, &fSuccess, fRelative);
					if (!fSuccess) {
						MessageBox(hwndDlg, szInvalidNumber, szGotoError, MB_OK);
						SendDlgItemMessage(hwndDlg, ID_LINE, EM_SETSEL, 0, -1L);
					} else {
						
					}
					return TRUE;
					break;
				case IDCANCEL:
					DestroyWindow(hwndDlg);
					hwndGoto = NULL;
					return TRUE;
					break;
			}
			break;
	}
	return FALSE;
}
/* Test UI elements below end */

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32GUI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32GUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
							  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId) {
				case IDM_ABOUT:
					//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					DisplayConfirmSaveAsMessageBox();
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				case IDM_DELETEITEM:
					if (DialogBox(hInst,
						MAKEINTRESOURCE(IDD_DELETEITEM),
						hWnd,
						(DLGPROC)DeleteItemProc) == IDOK) {
						OutputDebugString(L"DELETE ITEM YES");
					} else {
						OutputDebugString(L"DELETE ITEM NO");
					}
					break;
				case IDM_GOTO:
					if (!IsWindow(hwndGoto)) {
						hwndGoto = CreateDialog(hInst,
												MAKEINTRESOURCE(IDD_GOTO),
												hWnd,
												(DLGPROC)GoToProc);
						ShowWindow(hwndGoto, SW_SHOW);
					}
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}

