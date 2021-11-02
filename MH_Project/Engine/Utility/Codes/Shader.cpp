#include "Shader.h"

USING(Engine)

Engine::CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

Engine::CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	m_pEffect->AddRef();
}

Engine::CShader::~CShader(void)
{
}


HRESULT Engine::CShader::Ready_Shader(const wstring pFilePath)
{

	// 1. 만약 m_pEffect 값이 있고 m_pErrMsg 값이 없는 경우 : 코드가 정상적으로 작성되어 컴파일에 문제가 없는 경우
	// 2. 만약 m_pEffect 값이 없고 m_pErrMsg 값이 있는 경우 : 비상적인 코드로 인해 컴파일이 실패한 경우
	// 3. 만약 m_pEffect 값이 있고 m_pErrMsg 값도 있는 경우 : 컴파일 상에는 문제가 없으나 작성한 코드 중 경고성 메세지가 발생한 경우

	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, pFilePath.c_str(),
										NULL,
										NULL,
										D3DXSHADER_DEBUG,
										NULL, 
										&m_pEffect,
										&m_pErrMsg)))
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_Error", MB_OK);
		return E_FAIL;
	}
	
	else if (nullptr != m_pErrMsg)
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_Warning", MB_OK);
	}

	return S_OK;
}

Engine::CShader* Engine::CShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath)
{
	CShader*		pInstance = new CShader(pGraphicDev);

	if (FAILED(pInstance->Ready_Shader(pFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::CShader::Clone(void)
{
	return new CShader(*this);
}

void Engine::CShader::Free(void)
{
	Safe_Release(m_pEffect);

	CComponent::Free();
}

