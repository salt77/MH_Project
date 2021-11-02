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
	LPD3DXEFFECT			m_pEffect;		// ���̴��� �����ϰ� ���� �İ�ü
	LPD3DXBUFFER			m_pErrMsg;		// �ۼ��� hlsl �ڵ尡 �ùٸ��� �˻� ��, �߻��� �����޼����� �����ϱ� ���� ����

public:
	static CShader*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);
};

END
#endif // Shader_h__
