#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

END

class CMFC_Player : public CGameObject
{
private:
	explicit CMFC_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_Player();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT			Add_Component(void);

public:
	HRESULT			Add_Collider(_float fRadius);

private:
	_uint			m_iColliderNum = 0;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	
	//CCollider*		m_pColliderCom = nullptr;
	vector<CCollider*>		m_vecColliderCom;

public:
	static	CMFC_Player*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};

