#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free() : CCamera()
{
}

CCamera_Free::CCamera_Free(CCamera_Free& rhs) : CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Set_RenderGroup(CRenderer::RG_PRIORITY);

	Set_MatIndex(m_GameInstance->Add_Texture("SkyBox", CTexture::Create(L"../bin/Models/SkyBox/desertcube1024.dds"), CTextureMgr::TT_TEXTURECUBE));

	CVIBuffer_Geos::BASIC_SUBMESHES BS = CVIBuffer_Geos::BS_SPHERE;

	m_VIBuffer = (CVIBuffer_Geos*)m_GameInstance->Get_Component("VIBuffer_GeosCom", &BS);



	return S_OK;
}

void CCamera_Free::Tick(float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCamera_Free::LateTick(float fTimeDelta)
{
	m_RendererCom->AddtoRenderObjects(m_RG, this);

	auto FrameResource = m_GameInstance->Get_Current_FrameResource();
	auto currObjectCB = FrameResource->m_ObjectCB;

	XMMATRIX world = m_TransformCom->Get_WorldMatrix();
	_matrix textransform = m_TexCoordTransformCom->Get_WorldMatrix();

	world.r[0] *= 100.f;
	world.r[1] *= 100.f;
	world.r[2] *= 100.f;

	ObjectConstants objConstants{};
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(textransform));
	objConstants.MaterialIndex = m_MatIndex;

	currObjectCB->CopyData(m_objCBIndex, objConstants);

	__super::LateTick(fTimeDelta);
}

void CCamera_Free::Render()
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(m_GameInstance->Get_DescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(m_MatIndex, m_GameInstance->Get_CBVUAVSRVHeapSize());
	GETCOMMANDLIST->SetGraphicsRootDescriptorTable(3, skyTexDescriptor);

	m_VIBuffer->Render();
}

void CCamera_Free::Free()
{
	Safe_Release(m_VIBuffer);
	__super::Free();
}

CCamera_Free* CCamera_Free::Create()
{
	CCamera_Free* pInstance = new CCamera_Free;
	pInstance->Initialize_Prototype();
	return pInstance;
}

CCamera_Free* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);
	pInstance->Initialize(pArg);
	return pInstance;
}
