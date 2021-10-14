#ifndef Texture_h__
#define Texture_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void);

public:
	HRESULT		Ready_Texture(const _tchar* pPath, 
								TEXTURETYPE eType, 
								const _uint& iCnt);

	void		Set_Texture(const _uint& iIndex = 0);		// 텍스처 그리기 함수

private:
	vector<IDirect3DBaseTexture9*>			m_vecTexture;
		
public:
	static CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _tchar* pPath, 
									TEXTURETYPE eType, 
									const _uint& iCnt = 1);

	virtual CComponent*		Clone(void);
	virtual void			Free(void);

};

END
#endif // Texture_h__
