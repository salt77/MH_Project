#include "stdafx.h"
#include "MFC_CamEye.h"
#include "MFC_Camera.h"


CMFC_CamEye::CMFC_CamEye(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_CamEye::~CMFC_CamEye()
{
}


HRESULT CMFC_CamEye::Ready_Object(void)
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

		*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f - i, 0.f, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}

	CTransform* pTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Camera", L"Com_Transform", ID_DYNAMIC));
	if (!pTrans)
		return E_FAIL;

	m_pTransformCom->Set_Pos(pTrans->Get_Info(INFO_POS));

	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(10.f);
	m_pLine->SetAntialias(TRUE);

	return S_OK;
}

_int CMFC_CamEye::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Interpolation_Traffic();

	CMFC_Camera* pCamera = static_cast<CMFC_Camera*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));
	//CMFC_CamInterpol* vCamInterpol = static_cast<CMFC_CamInterpol*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_CamInterpol"));

	if (pCamera && 
		pCamera->Get_ActionCam())
	{
		CTransform* pCamTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Camera", L"Com_Transform", ID_DYNAMIC));
		//CTransform*	pCamInterpolTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_CamInterpol", L"Com_Transform", ID_DYNAMIC));

		if (D3DXVec3Length(&(*pCamTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))) < 0.5f)
			pCamera->Set_StalkTarget(m_vTargetEye);
	}

	return iExit;
}

void CMFC_CamEye::Render_Object()
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

HRESULT CMFC_CamEye::Add_Component(void)
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

void CMFC_CamEye::Interpolation_Traffic()
{
	if (_vec3(0.f, 0.f, 0.f) != m_vNextEye)
	{
		if (_vec3(0.f, 0.f, 0.f) != m_vPreEye)
			D3DXVec3CatmullRom(&m_vTargetEye, &m_vPreEye, m_pTransformCom->Get_Info(INFO_POS), &m_vNextEye, &m_vNextEye, 0.5f);
		else
			D3DXVec3CatmullRom(&m_vTargetEye, m_pTransformCom->Get_Info(INFO_POS), m_pTransformCom->Get_Info(INFO_POS), &m_vNextEye, &m_vNextEye, 0.5f);

		_vec3 vTemp[2];
		vTemp[0] = *m_pTransformCom->Get_Info(INFO_POS);
		vTemp[1] = m_vTargetEye;

		_matrix		matWorld, matView, matProj;
		m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		m_pLine->DrawTransform(vTemp, 2, &(matWorld * matView * matProj), D3DXCOLOR(0.f, 1.f, 1.f, 1.f));
	}
}


CMFC_CamEye * CMFC_CamEye::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_CamEye* pInstance = new CMFC_CamEye(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_CamEye::Free()
{
	for (_uint i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

	CGameObject::Free();
}
