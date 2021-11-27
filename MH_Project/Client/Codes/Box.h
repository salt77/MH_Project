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

class CBox final : public CGameObject
{
private:
	enum TRAP_STATE
	{
		TRAP_FIRE, TRAP_IDLE, 
		TRAP_END
	};

private:
	explicit CBox(LPDIRECT3DDEVICE9 pGraghicDev);
	explicit CBox(const CBox&	rhs);
	virtual ~CBox();

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

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	static	CBox*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void	Free() override;
};

