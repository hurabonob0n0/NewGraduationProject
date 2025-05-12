#pragma once
#include "Graphic_Dev.h"
#include "Timer.h"
#include "RawInput.h"
#include "FrameResourceMgr.h"
#include "ComponentMgr.h"
#include "RootSignatureMgr.h"
#include "Shader_Mgr.h"
#include "PSOMgr.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "TextureMgr.h"
#include "MaterialMgr.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	void Initialize(WindowInfo windowInfo,CRawInput* pRawInput);
	void Update(CTimer* pTimer);
	void Late_Update(CTimer* pTimer);
	void Draw();

public:		//For_GraphicDev
	ID3D12GraphicsCommandList* Get_CommandList() { return m_Graphic_Dev->Get_CommandList(); }
	ID3D12Device* Get_Device() { return m_Graphic_Dev->Get_Device(); }
	ID3D12CommandQueue* Get_CommandQueue() { return m_Graphic_Dev->Get_CommandQueue(); }
	void Execute_CommandList() { m_Graphic_Dev->Execute_CommandList(); }
	UINT Get_CBVUAVSRVHeapSize() { return m_Graphic_Dev->Get_CBVUAVSRVHeapSize(); }

public:		//For_InputDev
	bool Key_Down(UINT vkey) {return m_Input_Dev->Key_Down(vkey);}
	bool Key_Up(UINT vkey) { return m_Input_Dev->Key_Up(vkey); }
	bool Key_Pressing(UINT vkey) { return m_Input_Dev->Key_Pressing(vkey); }

	//bool Mouse_Down(MOUSEKEYSTATE button);
	//bool Mouse_Up(MOUSEKEYSTATE button);
	//bool Mouse_Pressing(MOUSEKEYSTATE button);

	SHORT Get_Mouse_XDelta() { return m_Input_Dev->Get_Mouse_XDelta(); }
	SHORT Get_Mouse_YDelta() { return m_Input_Dev->Get_Mouse_YDelta(); }

public:		//For_FrameResourceMgr
	void Reset_CommandList_and_Allocator(ID3D12PipelineState* PSO) { m_FrameResourceMgr->Reset_CommandList_and_Allocator(PSO); } //Todo : ���߿� �Ű������� PSO�޾Ƽ� �������ֱ�
	CFrameResource* Get_Current_FrameResource() { return m_FrameResourceMgr->Get_Current_FrameResource(); }
	void Flush_CommandQueue() { m_FrameResourceMgr->Flush_CommandQueue(); }

public: //For ComponentMgr
	HRESULT AddPrototype(const string& prototypeTag, CComponent* pPrototype) { return m_ComponentMgr->AddPrototype(prototypeTag, pPrototype); }
	CComponent* Get_Component(const string& prototypeTag, void* pArg = nullptr) const { return m_ComponentMgr->GetComponent(prototypeTag, pArg); }

public: //For ObjectMgr
	HRESULT AddObject(string PrototypeTag, string layerTag, void* pArg) { return m_ObjectMgr->AddObject(PrototypeTag, layerTag, pArg); }
	HRESULT Add_PrototypeObject(string Prototypename, CGameObject* pGameObject) { return m_ObjectMgr->Add_PrototypeObject(Prototypename, pGameObject); }

public: //For MaterialMgr
	int Add_Material(string matName, MaterialData matInstance) { return m_MaterialMgr->Add_Material(matName, matInstance); }

public:
	int Add_Texture(string texname, CTexture* texInstance, CTextureMgr::TEXTURETYPE TT = CTextureMgr::TT_TEXTURE2D) {
		return m_TextureMgr->Add_Texture(texname, texInstance, TT);
	}
	ID3D12DescriptorHeap* Get_DescriptorHeap() { return m_TextureMgr->Get_DescriptorHeap(); }

public:
	void Free();

public:
	void Release_Engine();


private:
	CGraphic_Dev*			m_Graphic_Dev;
	CRawInput*				m_Input_Dev;
	CFrameResourceMgr*		m_FrameResourceMgr;
	CComponentMgr*			m_ComponentMgr;
	CRootSignatureMgr*		m_RootSignatureMgr;
	CShader_Mgr*			m_ShaderMgr;
	CPSOMgr*				m_PSOMgr;
	CRenderer*				m_MainRenderer;
	CObjectMgr*				m_ObjectMgr;
	CMaterialMgr*			m_MaterialMgr;
	CTextureMgr*			m_TextureMgr;
};

END