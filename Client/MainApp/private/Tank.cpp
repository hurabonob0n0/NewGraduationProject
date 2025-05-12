#include "Tank.h"
#include "Client_Defines.h"
#include "GameInstance.h"



CTank::CTank() : CRenderObject()
{
}

CTank::CTank(CTank& rhs) : CRenderObject(rhs)
{
}

HRESULT CTank::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CTank::Initialize(void* pArg)
{
    m_RG = CRenderer::RG_BLEND;

    __super::Initialize(pArg);

    m_VIBuffer= (CModel*)m_GameInstance->Get_Component("ModelCom");

    return S_OK;
}

void CTank::Tick(float fTimeDelta)
{
    if (m_GameInstance->Key_Pressing(VK_UP)) {
        m_TransformCom->Go_Straight(fTimeDelta* 10.f);
    }

    if (m_GameInstance->Key_Pressing(VK_LEFT)) {
        m_TransformCom->Go_Left(fTimeDelta * 10.f);
    }

    if (m_GameInstance->Key_Pressing(VK_DOWN)) {
        m_TransformCom->Go_Backward(fTimeDelta * 10.f);
    }

    if (m_GameInstance->Key_Pressing(VK_RIGHT)) {
        m_TransformCom->Go_Right(fTimeDelta * 10.f);
    }

   /* _float4 Position;

    XMStoreFloat4(&Position, m_TransformCom->Get_State(CTransform::STATE_POSITION));

    float TerrainY = ((CVIBuffer_Terrain*)(m_GameInstance->GetPrototype("TerrainCom")))->Get_Terrain_Heights(Position.x, Position.z);

    m_TransformCom->Set_State(CTransform::STATE_POSITION, _vector{ Position.x,TerrainY,Position.z,1.f });*/

    __super::Tick(fTimeDelta);
}

void CTank::LateTick(float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
}

void CTank::Render()
{
    __super::Render();

    m_VIBuffer->Render();
}

void CTank::Free()
{
    __super::Free();
    Safe_Release(m_VIBuffer);
}

CTank* CTank::Create()
{
    CTank* pInstance = new CTank;
    pInstance->Initialize_Prototype();
    return pInstance;
}

CRenderObject* CTank::Clone(void* pArg)
{
    CTank* pInstance = new CTank(*this);
    pInstance->Initialize(pArg);
    return pInstance;
}
