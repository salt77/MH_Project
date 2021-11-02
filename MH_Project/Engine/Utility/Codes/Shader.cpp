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

	// 1. ���� m_pEffect ���� �ְ� m_pErrMsg ���� ���� ��� : �ڵ尡 ���������� �ۼ��Ǿ� �����Ͽ� ������ ���� ���
	// 2. ���� m_pEffect ���� ���� m_pErrMsg ���� �ִ� ��� : ������� �ڵ�� ���� �������� ������ ���
	// 3. ���� m_pEffect ���� �ְ� m_pErrMsg ���� �ִ� ��� : ������ �󿡴� ������ ������ �ۼ��� �ڵ� �� ��� �޼����� �߻��� ���

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

