#include "MainApp.h"
#include "DefaultObj.h"

IMPLEMENT_SINGLETON(CMainApp)

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// CreateWindow가 완료되기 전에는 mhMainWnd는 유효하지 않습니다.
	// 하지만 CreateWindow가 완료되기 전에 메세지 (예를 들어 WM_CREATE)를
	// 받을 수 있기 때문에 hwnd를 전달합니다.
	return CMainApp::Get_Instance()->MsgProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	CMainApp* mainApp = CMainApp::Get_Instance();
	if (FAILED(mainApp->Initialize(hInstance)))
		return 0;

	return mainApp->Run();
}

CMainApp::CMainApp() : m_GameInstance(CGameInstance::Get_Instance()), m_Timer(CTimer::Get_Instance()), m_Input_Dev(CRawInput::Get_Instance())
{
}

HRESULT CMainApp::Initialize(HINSTANCE g_hInstance)
{
	Initialize_MainWindow(g_hInstance);
	m_Input_Dev->Initialize(m_hMainWnd);

	WindowInfo WI;
	WI.Wincx = m_ClientWidth;
	WI.Wincy = m_ClientHeight;
	WI.windowHandle = m_hMainWnd;
	WI.isFullScreen = m_FullscreenState;

	m_GameInstance->Initialize(WI, m_Input_Dev);

	m_GameInstance->AddPrototype("TransformCom", CTransform::Create(GETDEVICE,GETCOMMANDLIST));
	m_GameInstance->AddPrototype("VIBuffer_GeosCom", CVIBuffer_Geos::Create(GETDEVICE, GETCOMMANDLIST));

	m_GameInstance->Add_PrototypeObject("Camera", CCamera::Create());
	m_GameInstance->Add_PrototypeObject("BoxObject", CDefaultObj::Create());

	_matrix mat = XMMatrixTranslation(0.f, 5.f, -5.f);
	m_GameInstance->AddObject("Camera", "Camera", &mat);

	_matrix mat1 = XMMatrixTranslation(0.f, 5.f, 10.f);
	m_GameInstance->AddObject("BoxObject", "BoxObject", &mat1);

	return S_OK;
}

int CMainApp::Run()
{
	MSG msg = { 0 };

	m_Timer->Reset();

	while (msg.message != WM_QUIT)
	{
		// 처리해야할 윈도우 메세지들이 있는지 확인합니다.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 처리해야할 메세지가 없는 경우, 에니메이션과 게임을 처리합니다.
		else
		{
			m_Timer->Tick();

			if (!m_AppPaused)
			{

				CalculateFrameStats();
				Update(m_Timer);
				Late_Update(m_Timer);
				Draw();

				m_Input_Dev->ResetPerFrame();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	RELEASE_INSTANCE(CMainApp);

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return (int)msg.wParam;
}

LRESULT CMainApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE는 윈도우가 활성화 되거나 비활성화 될때 보내집니다.
		// 윈도우가 비활성화 되었을 때는 게임을 중지시키고
		// 다시 활성화 되었을 때는 게임을 다시 재게합니다.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_Timer->Stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer->Start();
		}
		return 0;

		// WM_SIZE는 사용자가 윈도우 크기를 변경할 때 보내집니다.
	case WM_SIZE:
		// 새로운 윈도우 크기를 저장합니다.
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		return 0;

		// 윈도우가 파괴될 때 WM_DESTORY가 보내집니다.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// WM_MENUCAR는 메뉴가 활성화 될때 보내집니다.
		// 사용자가 니모닉, 가속기 키에 해당하지 않는 키를 누릅니다.
	case WM_MENUCHAR:
		// 알트 + 엔터를 입력시 비프음이 발생하는것을 방지합니다.
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_INPUT:
		m_Input_Dev->Update_InputDev(lParam);
		break;

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


HRESULT CMainApp::Initialize_MainWindow(HINSTANCE g_hInstance)
{
	m_hAppInst = g_hInstance;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// 클라이언트의 크기를 기반으로 윈도우 사각형을 계산합니다.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"MainWnd", m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}



void CMainApp::Update(const CTimer* Timer)
{
	m_GameInstance->Update(m_Timer);
}

void CMainApp::Late_Update(const CTimer* Timer)
{
	m_GameInstance->Late_Update(m_Timer);
}

void CMainApp::Draw()
{
	m_GameInstance->Draw();
}

void CMainApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	++frameCnt;

	// 1초 동안의 평균을 계산합니다.
	if ((m_Timer->TotalTime() - timeElapsed >= 1.0f))
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1;
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mpsfStr = to_wstring(mspf);

		wstring windowText = m_MainWndCaption +
			L"    fps: " + fpsStr +
			L"   mfps: " + mpsfStr;

		SetWindowText(m_hMainWnd, windowText.c_str());

		// 다음 평균을 위해 초기화 합니다.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void CMainApp::Free()
{
	Safe_Release(m_Timer);
	Safe_Release(m_Input_Dev);
	Safe_Release(m_GameInstance);
}
