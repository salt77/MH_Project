#ifndef UI_h__
#define UI_h__

#include "GameObject.h"

#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CUI abstract : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI(void);

public:
	virtual HRESULT Ready_Object();
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object() override;

protected:
	//HRESULT			Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

public:
	const _float&	Get_PosX() { return m_fX; }
	const _float&	Get_PosY() { return m_fY; }

public:
	void			Set_ValueRatio(_float fValue) { m_fValueRatio = fValue; }
	void			Set_MaxValueRatio(_float fValue) { m_fFullValueRatio = fValue; }

protected:
	_matrix			m_matProj;

	_float			m_fValueRatio = 1.f;
	_float			m_fFullValueRatio = 1.f;

	_float			m_fX, m_fY;
	_float			m_fSizeX, m_fSizeY;

protected:
	CRcTex*			m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

public:
	//static CUI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual void	Free(void) PURE;
};

#endif // UI_h__
