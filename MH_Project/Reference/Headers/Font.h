#ifndef Font_h__
#define Font_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFont : public CBase
{
private:
	explicit CFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont(void);

public:
	HRESULT		Ready_Font(const wstring pFontType, 
							const _uint& iWidth, 
							const _uint& iHeight,
							const _uint& iWeight);

	void		Render_Font(const wstring pString, 
							const _vec2* pPos,
							D3DXCOLOR Color);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	LPD3DXFONT				m_pFont;
	LPD3DXSPRITE			m_pSprite;

public:
	static CFont*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
								const wstring pFontType,
								const _uint& iWidth,
								const _uint& iHeight,
								const _uint& iWeight);

private:
	virtual void Free(void);
};

END
#endif // Font_h__
