#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTexture;
class CRenderer;
class CTransform;
class CShader;
class CBalistaTrail;

END

class CDynamicCamera;

class CBalista final : public CGameObject
{
private:
	explicit CBalista(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBalista(const CBalista& rhs);
	virtual ~CBalista();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();
	HRESULT			Add_Component_ForTrail();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	HRESULT			SetUp_ConstantTable_ForTrail(LPD3DXEFFECT& pEffect);

private:
	HRESULT			Ready_Collider();
	void			Movement(const _float& fTimeDelta);

public:
	const _bool&	Get_AlreadyCollision() { return m_bCollision; }
	const _bool&	Get_AlreadyEnemyHit() { return m_bEnemyHit; }

public:
	void			Set_EnableBalista(_vec3 vPos, _vec3 vDir);
	void			Set_CollisionWall() { m_bCollision = true; m_dwSurviveTime = GetTickCount(); }
	void			Set_EnemyHit() { m_bEnemyHit = true; }
	void			Set_StickBox(_matrix matBoxWorld) { m_matStick = matBoxWorld; }
	void			Set_ReturnPoolingPos();

private:
	_bool			m_bCollision = false;
	_bool			m_bEnemyHit = false;

	_ulong			m_dwSurviveTime = GetTickCount();
	_ulong			m_dwSurviveDelay = 6000;

	_float			m_fTimeDelta = 0.f;
	_float			m_fAlphaValue = 1.f;
	_float			m_fAlphaInterpol = 0.25f;
	_float			m_fSpeed = 75.f;
	_float			m_fFallingSpeed;
	_float			m_fOriginFallingSpeed;
	_float			m_fSpeedDown;

	_vec3			m_vDir;
	_vec3			m_vOriginScale;

	_matrix			m_matStick;

	CDynamicCamera*	m_pMainCam = nullptr;
	CTransform*		m_pPlayerTrans = nullptr;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	// Trail Component
	CBalistaTrail*	m_pTrailCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CShader*		m_pTrailShaderCom = nullptr;

	// Sound 
	_bool			m_bSoundCheck = false;
	_bool			m_bFlySoundCheck = false;

public:
	static	CBalista*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free() override;
};

