#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;

END

class CMFC_CamAt : public CGameObject
{
public:
	explicit CMFC_CamAt(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_CamAt();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;

public:
	void			Set_Selected(COLTYPE eColType) { m_eColType = eColType; }

private:
	HRESULT			Add_Component(void);

public:
	static	CMFC_CamAt*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free();

private:
	COLTYPE			m_eColType = COL_FALSE;

	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;

	LPD3DXMESH		m_pSphere;

#ifdef _DEBUG
	LPDIRECT3DTEXTURE9			m_pTexture[COLTYPE::COL_END];
#endif
};

