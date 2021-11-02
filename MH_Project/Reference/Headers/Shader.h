#ifndef Shader_h__
#define Shader_h__

#include "Engine_Define.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShader(const CShader& rhs);
	virtual ~CShader(void);

public:
	LPD3DXEFFECT	Get_EffectHandle(void) { return m_pEffect; }

public:
	HRESULT			Ready_Shader(const wstring pFilePath);

private:
	LPD3DXEFFECT			m_pEffect;		// 쉐이더를 제어하가 위한 컴객체
	LPD3DXBUFFER			m_pErrMsg;		// 작성한 hlsl 코드가 올바른지 검사 후, 발생한 에러메세지를 저장하기 위한 버퍼

public:
	static CShader*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);
};

END
#endif // Shader_h__
