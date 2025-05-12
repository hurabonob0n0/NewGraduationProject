#include "Renderer.h"
#include "RenderObject.h"
#include "GameInstance.h"
#include "FrameResourceMgr.h"

CRenderer::CRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) : CComponent(pDevice, pCommandList)
{
}

CRenderer::CRenderer(CRenderer& rhs) : CComponent(rhs)
{
}

void CRenderer::AddtoRenderObjects(RENDERGROUP RG, CRenderObject* pRenderObject)
{
    pRenderObject->Set_ObjCBIndex(m_vRenderObjects[RG].size());
    Safe_AddRef(pRenderObject);
    m_vRenderObjects[RG].push_back(pRenderObject);
}

void CRenderer::ResetRenderObjects()
{
    for (auto& pRenderObject : m_vRenderObjects[RG_PRIORITY]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_PRIORITY].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_NONLIGHT]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_NONLIGHT].clear();


    for (auto& pRenderObject : m_vRenderObjects[RG_NONBLEND]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_NONBLEND].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_BLEND]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_BLEND].clear();

    for (auto& pRenderObject : m_vRenderObjects[RG_UI]) {
        pRenderObject->Set_ObjCBIndex(-1);
        Safe_Release(pRenderObject);
    }
    m_vRenderObjects[RG_UI].clear();
}

void CRenderer::Resize_RenderGroups()
{
    for (int i = 0; i < (int)RG_UI; ++i)
    {
        m_vRenderObjects[i].reserve(1000);
    }
}

HRESULT CRenderer::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Resize_RenderGroups();
    return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void CRenderer::Render()
{
    Render_Priority();
    Render_NonLight();
    Render_NonBlend();
    Render_Blend();
    Render_UI();
}

void CRenderer::Render_Priority()
{
    for (auto& pGameObject : m_vRenderObjects[RG_PRIORITY])
    {
        pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_vRenderObjects[RG_PRIORITY].clear();
}

void CRenderer::Render_NonLight()
{
    for (auto& pGameObject : m_vRenderObjects[RG_NONLIGHT])
    {
        pGameObject->Render();

        Safe_Release(pGameObject);
    }

    m_vRenderObjects[RG_NONLIGHT].clear();
}

void CRenderer::Render_NonBlend()
{
    for (auto& pGameObject : m_vRenderObjects[RG_NONBLEND])
    {
        pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_vRenderObjects[RG_NONBLEND].clear();
}

void CRenderer::Render_Blend()
{
    for (auto& pGameObject : m_vRenderObjects[RG_BLEND])
    {
        pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_vRenderObjects[RG_BLEND].clear();
}

void CRenderer::Render_UI()
{
    for (auto& pGameObject : m_vRenderObjects[RG_UI])
    {
        pGameObject->Render();

        Safe_Release(pGameObject);
    }
    m_vRenderObjects[RG_UI].clear();
}

CRenderer* CRenderer::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
    CRenderer* pInstance = new CRenderer(pDevice, pCommandList);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to create Renderer prototype.");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
    AddRef();
    return this;
}

void CRenderer::Free()
{
    ResetRenderObjects();
    __super::Free();
}
