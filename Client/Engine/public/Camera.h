#pragma once
#include "RenderObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CRenderObject
{
public:
	CCamera();
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(float fTimeDelta);
	virtual void LateTick(float fTimeDelta);
	virtual void Render();

public:
	static CCamera* Create();
	CRenderObject* Clone(void* pArg);
	void Free() override;
};

END