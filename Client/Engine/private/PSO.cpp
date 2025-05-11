#include "PSO.h"
#include "Shader.h"

CPSO::CPSO()
{
    ZeroMemory(&m_PsoDesc, sizeof(m_PsoDesc));
}

void CPSO::SetupInputLayout(INPUTLAYOUT_TYPE eLayout)
{
    switch (eLayout)
    {
    case IT_POS:
        m_PsoDesc.InputLayout = { VTXPOS::VertexElements, VTXPOS::iNumElements };
        break;
    case IT_POS_NOR:
        m_PsoDesc.InputLayout = { VTXPOSNOR::VertexElements, VTXPOSNOR::iNumElements };
        break;
    case IT_POS_TEX:
        m_PsoDesc.InputLayout = { VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements };
        break;
    case IT_POS_NOR_TEX:
        m_PsoDesc.InputLayout = { VTXNORTEX::VertexElements, VTXNORTEX::iNumElements };
        break;
    case IT_CUBE_TEX:
        m_PsoDesc.InputLayout = { VTXCUBETEX::VertexElements, VTXCUBETEX::iNumElements };
        break;
    case IT_MESH:
        m_PsoDesc.InputLayout = { VTXMESH::VertexElements, VTXMESH::iNumElements };
        break;
    case IT_ANIM_MESH:
        m_PsoDesc.InputLayout = { VTXANIMMESH::VertexElements, VTXANIMMESH::iNumElements };
        break;
    default:
        m_PsoDesc.InputLayout = { nullptr, 0 };
        break;
    }
}

HRESULT CPSO::Initialize(ID3D12RootSignature* pRootSignature, CShader* VS, CShader* PS, ID3D12Device* pDevice, INPUTLAYOUT_TYPE eLayout)
{
    SetupInputLayout(eLayout);

    m_PsoDesc.pRootSignature = pRootSignature;
    m_PsoDesc.VS = { reinterpret_cast<BYTE*>(VS->Get()), VS->Get_Buffer_Size() };
    m_PsoDesc.PS = { reinterpret_cast<BYTE*>(PS->Get()), PS->Get_Buffer_Size() };
    m_PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    m_PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    m_PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    m_PsoDesc.SampleMask = UINT_MAX;
    m_PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_PsoDesc.NumRenderTargets = 1;
    m_PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_PsoDesc.SampleDesc.Count = 1;
    m_PsoDesc.SampleDesc.Quality = 0;
    m_PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    switch (eLayout)
    {
    case Engine::CPSO::IT_POS:
        break;
    case Engine::CPSO::IT_POS_TEX:
        break;
    case Engine::CPSO::IT_POS_NOR:
        break;
    case Engine::CPSO::IT_POS_NOR_TEX:
        break;
    case Engine::CPSO::IT_CUBE_TEX:
        break;
    case Engine::CPSO::IT_MESH:
        break;
    case Engine::CPSO::IT_ANIM_MESH:
        break;
    case Engine::CPSO::IT_END:
        break;
    default:
        break;
    }

   

    return pDevice->CreateGraphicsPipelineState(&m_PsoDesc, IID_PPV_ARGS(&m_PSO));
}

CPSO* CPSO::Create(ID3D12RootSignature* pRootSignature, CShader* VS, CShader* PS, ID3D12Device* pDevice, INPUTLAYOUT_TYPE eLayout)
{
    CPSO* pInstance = new CPSO;
    if (FAILED(pInstance->Initialize(pRootSignature, VS, PS, pDevice, eLayout)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CPSO::Free()
{
    Safe_Release(m_PSO);
}