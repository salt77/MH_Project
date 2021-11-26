#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;

END

class CWall_Collision final : public CGameObject
{
private:
	explicit CWall_Collision(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWall_Collision(const CWall_Collision& rhs);
	virtual ~CWall_Collision();

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
	CRenderer*		m_pRendererCom = nullptr;

public:
	static	CWall_Collision*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void				Free() override;
};

