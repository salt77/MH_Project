#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;

END

class CMFC_Line : public CGameObject
{
private:
	explicit CMFC_Line(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMFC_Line();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component();

private:
	LPD3DXLINE	m_pLine = nullptr;

public:
	static	CMFC_Line*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();
};

