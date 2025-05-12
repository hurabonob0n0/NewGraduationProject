#pragma once

/* 어떤 모델이든 다수의 메시로 구성되어 있다. */
/* 모델 = 메시 + 메시 + 메시... */
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END};
private:
	CModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render();

public:
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);


private:
	/* .fbx파일로부터 읽어온 정보를 1차적으로 저장한다.*/
	Assimp::Importer			m_Importer;

	// m_pAIScene = m_Importer.ReadFile( 경로, 플래그 );
	const aiScene*				m_pAIScene = { nullptr };

private:
	TYPE						m_eModelType = { TYPE_END };
	_float4x4					m_PivotMatrix;

	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	vector<class CBone*>			m_Bones;

private:	
	HRESULT Ready_Meshes();
	HRESULT Ready_Bones(aiNode* pNode, _int iParentBoneIndex);

private:
	void Set_Matrix_to_Bone(_uint iBoneIndex,_matrix mat);

public:
	static CModel* Create(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, TYPE eModelType, const string& strModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	
};

END

