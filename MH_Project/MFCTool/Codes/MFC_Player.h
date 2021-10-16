#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CDynamicMesh;
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

public:
	void			Set_AniIndex(_uint iIndex) { m_iAniIndex = iIndex; }

private:
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);

public:
	HRESULT			Add_Collider(_float fRadius, CString cstrName);
	HRESULT			Change_ColliderScale(_float fRadius, CString cstrName);

private:
	_uint			m_iColliderNum = 0;
	_uint			m_iAniIndex = 0;

	_vec3			m_vDir;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CDynamicMesh*	m_pMeshCom = nullptr;
	
	//CCollider*		m_pColliderCom = nullptr;
	map<const wstring, CCollider*>		m_mapColliderCom;

public:
	static	CMFC_Player*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};

