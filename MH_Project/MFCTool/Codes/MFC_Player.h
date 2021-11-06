#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CDynamicMesh;
class CCollider;
class CBoxCollider;
class CNaviMesh;

END

class CMFC_Player : public CGameObject
{
private:
	explicit CMFC_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_Player();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

public:
	const map<const wstring, CCollider*>	Get_ColliderMap() { return m_mapColliderCom; }
	const map<const wstring, CBoxCollider*>	Get_BoxColliderMap() { return m_mapBoxColliderCom; }

public:
	const vector<CCell*>&	Get_CellVector();

public:
	void			Set_AniIndex(_uint iIndex) { m_iAniIndex = iIndex; }

private:
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);

public:
	HRESULT			Add_NaviMesh(_uint iCellCount, vector<_matrix> vecPoint);
	HRESULT			DeleteAll_NaviMesh();
	HRESULT			Add_Collider(_float fRadius, wstring cstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType = COLLIDERTYPE::COLTYPE_SPHERE_DAMAGED);
	HRESULT			Add_Collider(_float vMinX, _float vMinY, _float vMinZ,
								_float vMaxX, _float vMaxY, _float vMaxZ,
								wstring wstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType);
	HRESULT			Delete_Collider(wstring wstrName, COLLIDERTYPE eColliderType);
	HRESULT			Change_ColliderScale(_float fRadius, wstring cstrName);

private:
	_uint			m_iColliderNum = 0;
	_uint			m_iAniIndex = 31;

	_vec3			m_vDir;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CDynamicMesh*	m_pMeshCom = nullptr;
	CNaviMesh*		m_pNaviMeshCom = nullptr;
	
	//CCollider*		m_pColliderCom = nullptr;
	map<const wstring, CCollider*>		m_mapColliderCom;
	map<const wstring, CBoxCollider*>	m_mapBoxColliderCom;

public:
	static	CMFC_Player*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};

