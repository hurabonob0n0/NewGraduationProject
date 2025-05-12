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
	m_Graphic_Dev->OnResize();
	m_FrameResourceMgr->Flush_CommandQueue();
	m_FrameResourceMgr->Reset_CommandList_and_Allocator(nullptr);

	//ComponentMgr
	m_ComponentMgr = CComponentMgr::Get_Instance();

	//RootSignatureMgr
	m_RootSignatureMgr = CRootSignatureMgr::Get_Instance();
	m_RootSignatureMgr->Register("DefaultRS", CRootSignature::Create()->CreateDefaultGraphicsRootSignature());

	//ShaderMgr
	m_ShaderMgr = CShader_Mgr::Get_Instance();
	m_ShaderMgr->AddShader("DefaultVS", CShader::ST_VS, L"../Bin/Shaders/Default.hlsl", nullptr);
	m_ShaderMgr->AddShader("DefaultPS", CShader::ST_PS, L"../Bin/Shaders/Default.hlsl", nullptr);

	m_ShaderMgr->AddShader("SkyVS", CShader::ST_VS, L"../Bin/Shaders/Sky.hlsl", nullptr);
	m_ShaderMgr->AddShader("SkyPS", CShader::ST_PS, L"../Bin/Shaders/Sky.hlsl", nullptr);

	m_ShaderMgr->AddShader("PosNorVS", CShader::ST_VS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);
	m_ShaderMgr->AddShader("PosNorPS", CShader::ST_PS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);

	//m_ShaderMgr->AddShader("DefaultVS", CShader::ST_VS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);
	//m_ShaderMgr->AddShader("DefaultPS", CShader::ST_PS, L"../Bin/ShaderFiles/Default.hlsl", nullptr);

	//PSOMgr
	m_PSOMgr = CPSOMgr::Get_Instance();
	m_PSOMgr->AddPSO("DefaultPSO", m_RootSignatureMgr->Get("DefaultRS"),
		m_ShaderMgr->GetShaderObj("DefaultVS"), m_ShaderMgr->GetShaderObj("DefaultPS"),
		Get_Device(), CPSO::IT_MESH);

	m_PSOMgr->AddPSO("SkyPSO", m_RootSignatureMgr->Get("DefaultRS"),
		m_ShaderMgr->GetShaderObj("SkyVS"), m_ShaderMgr->GetShaderObj("SkyPS"),
		Get_Device(), CPSO::IT_POS_NOR_TEX);

	m_PSOMgr->AddPSO("PosNorPSO", m_RootSignatureMgr->Get("DefaultRS"),
		m_ShaderMgr->GetShaderObj("PosNorVS"), m_ShaderMgr->GetShaderObj("PosNorPS"),
		Get_Device(), CPSO::IT_POS_NOR);

	//Renderer
	m_MainRenderer = CRenderer::Create(Get_Device(),Get_CommandList());
	m_ComponentMgr->AddPrototype("RendererCom", m_MainRenderer);

	//ObjectMgr
	m_ObjectMgr = CObjectMgr::Get_Instance();

	//MaterialMgr
	m_MaterialMgr = CMaterialMgr::Get_Instance();
	m_MaterialMgr->Resize();

	//TextureMgr
	m_TextureMgr = CTextureMgr::Get_Instance();
	m_TextureMgr->Resize_TexMap();
	m_TextureMgr->Make_DescriptorHeap();
	
	m_Graphic_Dev->Execute_CommandList();

	m_FrameResourceMgr->Flush_CommandQueue();

	m_FrameResourceMgr->Reset_CommandList_and_Allocator(nullptr);

}

void CGameInstance::Update(CTimer* pTimer)
{
	m_FrameResourceMgr->BeginFrame();


	m_ObjectMgr->Update(pTimer->DeltaTime());
	m_MaterialMgr->Update_Mats();
}

void CGameInstance::Late_Update(CTimer* pTimer)
{
	m_ObjectMgr->LateUpdate(pTimer->DeltaTime());
}

void CGameInstance::Draw()
{
	m_FrameResourceMgr->Reset_CommandList_and_Allocator(m_PSOMgr->Get("SkyPSO"));

	m_TextureMgr->Set_DescriptorHeap();

	Get_CommandList()->SetGraphicsRootSignature(m_RootSignatureMgr->Get("DefaultRS"));

	auto passCB = m_FrameResourceMgr->Get_Current_FrameResource()->m_PassCB->Resource();
	Get_CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	m_MaterialMgr->Set_Materials();

	Get_CommandList()->SetGraphicsRootDescriptorTable(4, m_TextureMgr->Get_DescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	m_Graphic_Dev->Set_BackBuffer_and_DSV();

	m_MainRenderer->Render_Priority();

	Get_CommandList()->SetPipelineState(m_PSOMgr->Get("DefaultPSO"));

	m_MainRenderer->Render_NonBlend();

	Get_CommandList()->SetPipelineState(m_PSOMgr->Get("PosNorPSO"));

	m_MainRenderer->Render_Blend();

	m_Graphic_Dev->Present();

	m_FrameResourceMgr->SignalAndAdvance();

	//m_MainRenderer->ResetRenderObjects();
}

void CGameInstance::Free()
{
	
}

void CGameInstance::Release_Engine()
{
	m_FrameResourceMgr->Flush_CommandQueue();
	Safe_Release(m_ObjectMgr);
	Safe_Release(m_MainRenderer);
	Safe_Release(m_ComponentMgr);
	Safe_Release(m_ShaderMgr);
	Safe_Release(m_PSOMgr);
	Safe_Release(m_RootSignatureMgr);
	Safe_Release(m_FrameResourceMgr);
	Safe_Release(m_Graphic_Dev);
	Safe_Release(m_Input_Dev);
}
