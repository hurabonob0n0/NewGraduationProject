#include "DefaultObj.h"
#include "GameInstance.h"

CDefaultObj::CDefaultObj() : CRenderObject()
{
}

CDefaultObj::CDefaultObj(CDefaultObj& rhs) : CRenderObject(rhs)
{
}

HRESULT CDefaultObj::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CDefaultObj::Initialize(void* pArg)
{
    m_RG = CRenderer::RG_NONBLEND;

    __super::Initialize(pArg);

    MaterialData MD{};

    XMStoreFloat4x4( &MD.MatTransform, XMMatrixIdentity());
    MD.DiffuseMapIndex = m_GameInstance->Add_Texture("BrickDiffuse", CTexture::Create(L"../bin/Models/Basic/bricks.dds"));
    MD.NormalMapIndex = m_GameInstance->Add_Texture("BrickNormal", CTexture::Create(L"../bin/Models/Basic/bricks_nmap.dds"));

    Set_MatIndex(m_GameInstance->Add_Material("BrickMat", MD));

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
