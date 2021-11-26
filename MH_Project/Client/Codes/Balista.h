#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

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
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	HRESULT			Ready_Collider();
	void			Movement(const _float& fTimeDelta);

public:
	const _bool&	Get_AlreadyCollision() { return m_bCollision; }

public:
	void			Set_EnableBalista(_vec3 vPos, _vec3 vDir);
	void			Set_CollisionWall() { m_bCollision = true; m_dwSurviveTime = GetTickCount(); }

private:
	_bool			m_bCollision = false;

	_ulong			m_dwSurviveTime = GetTickCount();
	_ulong			m_dwSurviveDelay = 500;

	_float			m_fSpeed = 75.f;

	_vec3			m_vDir;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static	CBalista*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free() override;
};

