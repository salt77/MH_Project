#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CShader;

END

class CEfx_Bomb;

class CStickyBomb : public CGameObject
{
private:
	enum STICKY_MODE
	{
		CHEST, HEAD, RHAND,
		MODE_END
	};

private:
	explicit CStickyBomb(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStickyBomb(const CStickyBomb& rhs);
	virtual ~CStickyBomb();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	HRESULT			Ready_Collider();
	void			Movement(const _float& fTimeDelta);
	void			Bomb();
	void			Collision();

private:
	_bool			m_bSticky = false;

	_ulong			m_dwSurviveTime = GetTickCount();
	_ulong			m_dwSurviveDelay = 1500;

	_float			m_fSpeed;
	_float			m_fFallSpeed;
	_float			m_fTime;
	_float			m_fCamAngle;

	_vec3			m_vStartPos;
	_vec3			m_vMovePos;
	_vec3			m_vFlightDir;
	_vec3			m_vMin;
	_vec3			m_vMax;

	CEfx_Bomb*		m_pEfxBomb = nullptr;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	STICKY_MODE		m_eMode = MODE_END;

	_vec3		m_vDir;
	_bool		m_bColl = false;

public:
	static CStickyBomb*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();
};
