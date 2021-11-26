#ifndef HitBox_Pos_h__
#define HitBox_Pos_h__

#include "GameObject.h"

BEGIN(Engine)

class CTransform;

END

class CHitBox_Pos : public CGameObject
{
private:
	explicit CHitBox_Pos(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHitBox_Pos(const CHitBox_Pos& rhs);
	virtual ~CHitBox_Pos(void);

public:
	virtual HRESULT Ready_Object() override;
	virtual HRESULT	LateReady_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual _int	LateUpdate_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

private:
	HRESULT			Add_Component();

private:
	CTransform*		m_pTransformCom = nullptr;

public:
	static CHitBox_Pos*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};

#endif // HitBox_Pos_h__
