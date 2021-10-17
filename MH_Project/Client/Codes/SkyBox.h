#ifndef SkyBox_h__
#define SkyBox_h__

#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CRenderer;
class CTransform;

END

class CSkyBox : public CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT			Add_Component(void);

private:
	CCubeTex*	m_pBufferCom = nullptr;
	CTexture*	m_pTextureCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;

public:
	static CSkyBox*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};

#endif // SkyBox_h__
