#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CWall_Symbol final : public CGameObject
{
private:
	explicit CWall_Symbol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWall_Symbol(const CWall_Symbol& rhs);
	virtual ~CWall_Symbol();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

public:
	const _bool&	Get_CanNextStageOpen() { return m_bCanNextStage; }
	const _bool&	Get_ReadyToOpenPortal() { return m_bReadyToOpenPortal; }

public:
	void			Set_NextStagePortal() { m_bCanNextStage = true; }
	void			Set_ReadyOpenPortal() { m_bReadyToOpenPortal = true; }

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	HRESULT			Ready_Collider();

private:
	_bool			m_bCanNextStage = false;
	_bool			m_bReadyToOpenPortal = false;

	CRcTex*			m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static	CWall_Symbol*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free() override;
};

