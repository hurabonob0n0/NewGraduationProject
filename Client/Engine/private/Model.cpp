#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
//#include "Animation.h"

CModel::CModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: CComponent(pDevice, pCommandList)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_eModelType(rhs.m_eModelType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	/* aiProcess_PreTransformVertices : */
	/* 이 모델을 구성하는 메시들이 혹여 지정된 상태대로 그려져야한다면.(메시의 이름과 같은 이름을 가진 뼈가 존재한다면) */
	/* 그 지정된 상태행렬응ㄹ 이 메시의 정점에 미리 곱해서 로드한다. */
	/* aiProcess_PreTransformVertices옵션을 넣어서 로드하게되면 이미 정점에 뼈의 상태가 적용되어있는것이다. */
	/* 추후에 뼈를 움직이게되면 정점에 들어가있는 뼈 상태를 빼서 넣는다라는 복잡한과정. */
	/* 그래서 해당 옵션을 주고 로드하게되면 어심프자체에서 애니메이션에 관련된 정보를 삭제해버린다. */
	/* 이 옵션을 통해 로드하는 경우는 반드시 애니메이션이 없는 경우에만 처리를 해야되겠다. */
	_uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	/* m_pAIScene안에 .fbx파일에 담겨있던 정보들이 담긴다.  */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* m_pAIScene안에 들어가있는 정보들을 우리가 사용하기 좋은 형태로 생성, 저장해주는 작업을 수행하면 되겠다. */
	/* mRootNode : 모든 뼈의 가장 시작이되는 뼈다. */
	/* 내 모델의 모든 뼈를 구분없이 다 로드한다. */
	/* WHY? 뼈들의 상태정보를 가진 뼈를 만들어내는 과정. */
	/* 뼈의 상태정보 = 내뼈의 상태 * 부모뼈의 상태 */
	/* 내뼈의 정보만 필요한게아니라 이 뼈의 부모가 대체 누구였는가?! */
	/* 부모자식간의 뼈의 관계 + 뼈의 상태를 로드하는 작업이 Ready_Bones함수의 역활이다. */
	/* aiNode */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

/* 정점들을 그린다. */
HRESULT CModel::Render(_uint iMeshIndex)
{
	/* iMeshIndex에 해당하는 메시에 영향을 주는 뼈들을 모아서 셰이더로 전달한다. */
	m_Meshes[iMeshIndex]->Render();
	return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& mesh : m_Meshes)
		mesh->Render();
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	/* iMeshIndex번째 메시에 접근한다. */
	/* 우리는 메시마다 VB, iB를 만든다. */
	/* 메시마다 그려낸다. */
	/* 한번 그린다. == 특정 메시의 정점들만 셰이더를 통과한다. */
	/* 이 때에 전달되는 뼈들은 특정 메시에 영향을 주는 뼈들만 전달되어야할 필요가 있다. */
	/* + 정점들이 가지고 있는 BlendIndices는 전체뼈의 인덱스가 아니라. 이 메시에 영향을 주는 뼈들읜 인덱스를 의미하는거. */
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CModel::Ready_Meshes()
{
	/* 현재 모델을 구성하는 메시의 갯수. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		/* VB, IB를 만든다. */
		CMesh* pMesh = CMesh::Create(m_Device, m_CommandList, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParentIndex);
	}
	return S_OK;
}


void CModel::Set_Matrix_to_Bone(_uint iBoneIndex, _matrix Mat)
{
	m_Bones[iBoneIndex]->Mul_TransformationMatrix(Mat);
}

CModel* CModel::Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pCommandList);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, strModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Importer.FreeScene();

}
