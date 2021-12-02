#include "stdafx.h"
#include "Wall_Symbol.h"

#include "Export_Function.h"


CWall_Symbol::CWall_Symbol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CWall_Symbol::CWall_Symbol(const CWall_Symbol & rhs)
	: CGameObject(rhs)
{
}

CWall_Symbol::~CWall_Symbol()
{
}


HRESULT CWall_Symbol::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	m_pTransformCom->Set_Pos(&WALL_SYMBOL_POS);
	m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);

	return S_OK;
}

HRESULT CWall_Symbol::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CWall_Symbol::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

_int CWall_Symbol::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapBoxColliderCom.empty())
		m_mapBoxColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());

	return iExit;
}

void CWall_Symbol::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	if (!m_bCanNextStage)
	{
		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(0);

		m_pBufferCom->Render_Buffer();
		m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

HRESULT CWall_Symbol::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Prototype(L"Proto_Buffer_RcTex"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_Symbol"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Symbol"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CWall_Symbol::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlphaValue", 0.7f);

	return S_OK;
}

HRESULT CWall_Symbol::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Collider(-2.f, -0.5f, -0.1f, 2.f, 0.5f, 0.1f, L"Wall_Symbol_Collider", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_OTHER), E_FAIL);

	return S_OK;
}


CWall_Symbol * CWall_Symbol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWall_Symbol*	pInstance = new CWall_Symbol(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CWall_Symbol::Free()
{
	CGameObject::Free();
}
