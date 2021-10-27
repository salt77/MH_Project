#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CStaticMesh;

END

class CMFC_Stage : public CGameObject
{
private:
	explicit CMFC_Stage(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMFC_Stage(const CMFC_Stage& rhs);
	virtual ~CMFC_Stage(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_Material();

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;

	_vec3		m_vDir;
	_bool		m_bColl = false;

public:
	static CMFC_Stage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};
