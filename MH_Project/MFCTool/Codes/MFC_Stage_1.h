#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CStaticMesh;
class CShader;

END

class CMFC_Stage_1 : public CGameObject
{
private:
	explicit CMFC_Stage_1(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMFC_Stage_1(const CMFC_Stage_1& rhs);
	virtual ~CMFC_Stage_1(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	_vec3		m_vDir;
	_bool		m_bColl = false;

public:
	static CMFC_Stage_1*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};
