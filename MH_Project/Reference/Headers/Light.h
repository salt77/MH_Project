#ifndef Light_h__
#define Light_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight : public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight(void);

public:
	const D3DLIGHT9*			Get_Light() { return &m_tLightInfo; }
	const _float&				Get_LightPower() { return m_fLightPower; }

public:
	HRESULT			Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex, const _float& fLightPower = 10.f);
	void			Render_Light(LPD3DXEFFECT& pEffect);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	D3DLIGHT9					m_tLightInfo;

	_float						m_fLightPower = 10.f;

	_uint						m_iIndex = 0;

	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

public:
	static CLight*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
							   const D3DLIGHT9* pLightInfo,
							   const _uint& iIndex);

private:
	virtual void	Free(void);
};

END
#endif // Light_h__
