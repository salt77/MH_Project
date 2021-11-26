#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CTrap final : public CGameObject
{
private:
	enum TRAP_STATE
	{
		TRAP_FIRE, TRAP_IDLE, 
		TRAP_END
	};

private:
	explicit CTrap(LPDIRECT3DDEVICE9 pGraghicDev);
	explicit CTrap(const CTrap&	rhs);
	virtual ~CTrap();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	void			Fire();
	void			Animation_Control();

private:
	_bool			m_bFire = false;

	_uint			m_iAniIndex = TRAP_FIRE;

	_double			m_lfAniEnd = 0.f;
	_float			m_fAniTime = 0.f;

	TRAP_STATE		m_eCurState = TRAP_IDLE;
	TRAP_STATE		m_ePreState = TRAP_END;

	CTransform*		m_pPlayerTrans = nullptr;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CDynamicMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static	CTrap*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free() override;
};

