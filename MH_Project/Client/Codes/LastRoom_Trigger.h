#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;

END

class CLastRoom_Trigger final : public CGameObject
{
private:
	explicit CLastRoom_Trigger(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLastRoom_Trigger(const CLastRoom_Trigger& rhs);
	virtual ~CLastRoom_Trigger();

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Collider();

private:
	CTransform*		m_pTransformCom = nullptr;

public:
	static	CLastRoom_Trigger*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void				Free() override;
};

