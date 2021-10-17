#include "stdafx.h"
#include "MFC_CamAt.h"


CMFC_CamAt::CMFC_CamAt(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_CamAt::~CMFC_CamAt()
{
}


HRESULT CMFC_CamAt::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXCreateSphere(m_pGraphicDev, 1.f, 10, 10, &m_pSphere, nullptr);

	for (_uint i = 0; i < COL_END; ++i)
	{
		D3DXCreateTexture(m_pGraphicDev,
			1,
			1,
			1,  // miplevel
			0,	// 텍스처의 용도, D3DUSAGE_RENDERTARGET
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&m_pTexture[i]);

		D3DLOCKED_RECT LockRect;
		m_pTexture[i]->LockRect(0, &LockRect, NULL, 0);

		*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * i, 1.f - i, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}

	CTransform* pTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Camera", L"Com_Transform", ID_DYNAMIC));
	if (!pTrans)
		return E_FAIL;

	m_pTransformCom->Set_Pos(pTrans->Get_Info(INFO_POS));

	return S_OK;
}

_int CMFC_CamAt::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	return iExit;
}

void CMFC_CamAt::Render_Object()
{
	//Add_RenderGroup(RENDER_NONALPHA, this);

#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTexture(0, m_pTexture[m_eColType]);
	m_pSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif
}

HRESULT CMFC_CamAt::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}


CMFC_CamAt * CMFC_CamAt::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_CamAt* pInstance = new CMFC_CamAt(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_CamAt::Free()
{
	for (_uint i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

	CGameObject::Free();
}
