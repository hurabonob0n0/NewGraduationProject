#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance() : m_Graphic_Dev(CGraphic_Dev::Get_Instance()),m_FrameResourceMgr(CFrameResourceMgr::Get_Instance())
{
}

void CGameInstance::Initialize(WindowInfo windowInfo, CRawInput* pRawInput)
{
	ID3D12Debug* debugController;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	debugController->EnableDebugLayer();

	//InputDev
	m_Input_Dev = pRawInput;
	Safe_AddRef(m_Input_Dev);

	//GraphicDev
	m_Graphic_Dev->Initialize(windowInfo);
	
	//FrameResourceMgr + GraphicDev
	m_FrameResourceMgr->Initialize(m_Graphic_Dev->Get_Device(), m_Graphic_Dev->Get_CommandList(), m_Graphic_Dev->Get_CommandQueue(), g_NumFrameResources);
	m_FrameResourceMgr->Flush_CommandQueue();
	m_Graphic_Dev->OnResize();
	m_FrameResourceMgr->Flush_CommandQueue();

	//ComponentMgr
	m_ComponentMgr = CComponentMgr::Get_Instance();

	//RootSignatureMgr
	m_RootSignatureMgr = CRootSignatureMgr::Get_Instance();
	m_RootSignatureMgr->Register("DefaultRS", CRootSignature::Create()->Push(D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_ALL)->
		Push(D3D12_ROOT_PARAMETER_TYPE_CBV, 1, 0, D3D12_SHADER_VISIBILITY_ALL));

	//ShaderMgr
	m_ShaderMgr = CShader_Mgr::Get_Instance();
	m_ShaderMgr->AddShader("DefaultVS", CShader::ST_VS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);
	m_ShaderMgr->AddShader("DefaultPS", CShader::ST_PS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);

	//PSOMgr
	m_PSOMgr = CPSOMgr::Get_Instance();
	m_PSOMgr->AddPSO("DefaultPSO", m_RootSignatureMgr->Get("DefaultRS"),
		m_ShaderMgr->GetShaderObj("DefaultVS"), m_ShaderMgr->GetShaderObj("DefaultPS"),
		Get_Device(), CPSO::IT_POS_NOR);

	//Renderer
	m_MainRenderer = CRenderer::Create(Get_Device(),Get_CommandList());
	m_ComponentMgr->AddPrototype("RendererCom", m_MainRenderer);

	//ObjectMgr
	m_ObjectMgr = CObjectMgr::Get_Instance();

	m_FrameResourceMgr->Flush_CommandQueue();

}

void CGameInstance::Update(CTimer* pTimer)
{
	m_FrameResourceMgr->BeginFrame();


	m_ObjectMgr->Update(pTimer->DeltaTime());
}

void CGameInstance::Late_Update(CTimer* pTimer)
{
	m_ObjectMgr->LateUpdate(pTimer->DeltaTime());
}

void CGameInstance::Draw()
{

	auto pso = m_PSOMgr->Get("DefaultPSO");
	m_FrameResourceMgr->Reset_CommandList_and_Allocator(m_PSOMgr->Get("DefaultPSO"));


	auto rs = m_RootSignatureMgr->Get("DefaultRS");
	Get_CommandList()->SetGraphicsRootSignature(m_RootSignatureMgr->Get("DefaultRS"));


	auto fr = m_FrameResourceMgr->Get_Current_FrameResource();

	auto passCB = m_FrameResourceMgr->Get_Current_FrameResource()->m_PassCB->Resource();

	D3D12_GPU_VIRTUAL_ADDRESS PassCBAddress = passCB->GetGPUVirtualAddress();

	Get_CommandList()->SetGraphicsRootConstantBufferView(1, PassCBAddress);

	m_Graphic_Dev->Set_BackBuffer_and_DSV();
	m_MainRenderer->Render();
	m_Graphic_Dev->Present();

	m_FrameResourceMgr->SignalAndAdvance();

	m_MainRenderer->ResetRenderObjects();
}

void CGameInstance::Free()
{
	m_FrameResourceMgr->Flush_CommandQueue();

	Safe_Release(m_FrameResourceMgr);
	Safe_Release(m_Input_Dev);
	Safe_Release(m_Graphic_Dev);
	Safe_Release(m_RootSignatureMgr);
	Safe_Release(m_PSOMgr);
	Safe_Release(m_ShaderMgr);
	Safe_Release(m_MainRenderer);
	Safe_Release(m_ComponentMgr);
}
