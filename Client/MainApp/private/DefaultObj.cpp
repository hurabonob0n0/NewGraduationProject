#include "Client_pch.h"
#include "DefaultObj.h"
#include "GameInstance.h"

CDefaultObj::CDefaultObj() : CRenderObject()
{
}

CDefaultObj::CDefaultObj(CDefaultObj& rhs) : CRenderObject(rhs)
{
    m_RG = rhs.m_RG;
}

HRESULT CDefaultObj::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    m_RG = CRenderer::RG_BLEND;

    return S_OK;
}

HRESULT CDefaultObj::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    m_VIBuffer = (CVIBuffer_Geos*)m_GameInstance->Get_Component("VIBuffer_GeosCom");

    return S_OK;
}

void CDefaultObj::Tick(float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CDefaultObj::LateTick(float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
}

void CDefaultObj::Render()
{
    __super::Render();

    m_VIBuffer->Render();
}

void CDefaultObj::Free()
{
    Safe_Release(m_VIBuffer);

    __super::Free();
}

CDefaultObj* CDefaultObj::Create()
{
    CDefaultObj* pInstance = new CDefaultObj;
    pInstance->Initialize_Prototype();
    return pInstance;
}

CRenderObject* CDefaultObj::Clone(void* pArg)
{
    CDefaultObj* pInstance = new CDefaultObj(*this);
    pInstance->Initialize(pArg);
    return pInstance;
}
