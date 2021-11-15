#ifndef BackGround_h__
#define BackGround_h__

#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;

END

class CBackGround : public CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround(void);

public:
	virtual HRESULT Ready_Object(_uint iTexIndex);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT			Add_Component(void);

private:
	_uint		m_iTexIndex = 0;

	CRcTex*		m_pBufferCom = nullptr;
	CTexture*	m_pTextureCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;

public:
	static CBackGround*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexIndex = 0);
	virtual void			Free(void);
};

#endif // BackGround_h__
