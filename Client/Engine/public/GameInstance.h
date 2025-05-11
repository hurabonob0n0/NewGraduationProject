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
	void Reset_CommandList_and_Allocator(ID3D12PipelineState* PSO) { m_FrameResourceMgr->Reset_CommandList_and_Allocator(PSO); } //Todo : 나중에 매개변수로 PSO받아서 설정해주기
	CFrameResource* Get_Current_FrameResource() { return m_FrameResourceMgr->Get_Current_FrameResource(); }

public: //For ComponentMgr
	HRESULT AddPrototype(const string& prototypeTag, CComponent* pPrototype) { return m_ComponentMgr->AddPrototype(prototypeTag, pPrototype); }
	CComponent* Get_Component(const string& prototypeTag, void* pArg = nullptr) const { return m_ComponentMgr->GetComponent(prototypeTag, pArg); }

public:
	HRESULT AddObject(string PrototypeTag, string layerTag, void* pArg) { return m_ObjectMgr->AddObject(PrototypeTag, layerTag, pArg); }
	HRESULT Add_PrototypeObject(string Prototypename, CGameObject* pGameObject) { return m_ObjectMgr->Add_PrototypeObject(Prototypename, pGameObject); }

public:
	void Free();


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

};

END