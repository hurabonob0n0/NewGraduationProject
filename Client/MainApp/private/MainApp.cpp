#include "MainApp.h"
#include "DefaultObj.h"

IMPLEMENT_SINGLETON(CMainApp)

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// CreateWindow�� �Ϸ�Ǳ� ������ mhMainWnd�� ��ȿ���� �ʽ��ϴ�.
	// ������ CreateWindow�� �Ϸ�Ǳ� ���� �޼��� (���� ��� WM_CREATE)��
	// ���� �� �ֱ� ������ hwnd�� �����մϴ�.
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
		// ó���ؾ��� ������ �޼������� �ִ��� Ȯ���մϴ�.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ó���ؾ��� �޼����� ���� ���, ���ϸ��̼ǰ� ������ ó���մϴ�.
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
		// WM_ACTIVATE�� �����찡 Ȱ��ȭ �ǰų� ��Ȱ��ȭ �ɶ� �������ϴ�.
		// �����찡 ��Ȱ��ȭ �Ǿ��� ���� ������ ������Ű��
		// �ٽ� Ȱ��ȭ �Ǿ��� ���� ������ �ٽ� ����մϴ�.
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

		// WM_SIZE�� ����ڰ� ������ ũ�⸦ ������ �� �������ϴ�.
	case WM_SIZE:
		// ���ο� ������ ũ�⸦ �����մϴ�.
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		return 0;

		// �����찡 �ı��� �� WM_DESTORY�� �������ϴ�.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// WM_MENUCAR�� �޴��� Ȱ��ȭ �ɶ� �������ϴ�.
		// ����ڰ� �ϸ��, ���ӱ� Ű�� �ش����� �ʴ� Ű�� �����ϴ�.
	case WM_MENUCHAR:
		// ��Ʈ + ���͸� �Է½� �������� �߻��ϴ°��� �����մϴ�.
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

	// Ŭ���̾�Ʈ�� ũ�⸦ ������� ������ �簢���� ����մϴ�.
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

	// 1�� ������ ����� ����մϴ�.
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

		// ���� ����� ���� �ʱ�ȭ �մϴ�.
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
