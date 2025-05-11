#include "RenderObject.h"
#include "GameInstance.h"
#include "VIBuffer_Geos.h"


CRenderObject::CRenderObject() : CGameObject()
{
}

CRenderObject::CRenderObject(CRenderObject& rhs) : CGameObject(rhs)
{
	m_objCBIndex = rhs.m_objCBIndex;
}

void CRenderObject::Set_ObjCBIndex(const UINT& index)
{
	m_objCBIndex = index;
}

void CRenderObject::Set_RenderGroup(CRenderer::RENDERGROUP RG)
{
	m_RG = RG;
}

HRESULT CRenderObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CRenderObject::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_RendererCom = (CRenderer*)m_GameInstance->Get_Component("RendererCom", nullptr);

	m_TexCoordTransformCom = (CTransform*)m_GameInstance->Get_Component("TransformCom", nullptr);

	return S_OK;
}

void CRenderObject::Tick(float fTimeDelta)
{
}

void CRenderObject::LateTick(float fTimeDelta)
{
	m_RendererCom->AddtoRenderObjects(m_RG, this);

	auto FrameResource = m_GameInstance->Get_Current_FrameResource();
	auto currObjectCB = FrameResource->m_ObjectCB;

	XMMATRIX world = m_TransformCom->Get_WorldMatrix();
	_matrix textransform = m_TexCoordTransformCom->Get_WorldMatrix();

	ObjectConstants objConstants{};
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(world));
	//objConstants.MaterialIndex = m_MatIndex;

	currObjectCB->CopyData(m_objCBIndex, objConstants);
}

void CRenderObject::Render()
{
	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = m_GameInstance->Get_Current_FrameResource()->m_ObjectCB->Resource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress();

	objCBAddress += m_objCBIndex * objCBByteSize;

	m_GameInstance->Get_CommandList()->SetGraphicsRootConstantBufferView(0, objCBAddress);
}

void CRenderObject::Free()
{
	Safe_Release(m_RendererCom);
	Safe_Release(m_TexCoordTransformCom);
	__super::Free();
}

CRenderObject* CRenderObject::Create()
{
	CRenderObject* pInstance = new CRenderObject;
	pInstance->Initialize_Prototype();
	return pInstance;
}

CRenderObject* CRenderObject::Clone(void* pArg)
{
	CRenderObject* pInstance = new CRenderObject(*this);
	pInstance->Initialize(pArg);
	return pInstance;
}
