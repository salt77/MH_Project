#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CStaticMesh;
class CShader;

END

class CStickyBomb : public CGameObject
{
private:
	explicit CStickyBomb(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStickyBomb(const CStickyBomb& rhs);
	virtual ~CStickyBomb(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual HRESULT LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void			Movement(const _float& fTimeDelta);

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	_vec3		m_vDir;
	_bool		m_bColl = false;

public:
	static CStickyBomb*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};
