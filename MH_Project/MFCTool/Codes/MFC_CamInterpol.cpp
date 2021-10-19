#include "stdafx.h"
#include "MFC_CamInterpol.h"
#include "MFC_Camera.h"


CMFC_CamInterpol::CMFC_CamInterpol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_CamInterpol::~CMFC_CamInterpol()
{
}


HRESULT CMFC_CamInterpol::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	CTransform* pCamTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Camera", L"Com_Transform", ID_DYNAMIC));

	m_vOriginPos = *pCamTrans->Get_Info(INFO_POS);
	m_pTransformCom->Set_Pos(&m_vOriginPos);

	return S_OK;
}

_int CMFC_CamInterpol::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	//CMFC_Camera* pCamera = static_cast<CMFC_Camera*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));
	////CTransform* pCamTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Camera", L"Com_Transform", ID_DYNAMIC));

	//m_fCamSpeed = pCamera->Get_Speed();
	//pCamera->Set_StalkTarget(*m_pTransformCom->Get_Info(INFO_POS));

	//if (pCamera->Get_ActionCam() && _vec3(0.f, 0.f, 0.f) != m_vNextPos)
	//{
	//	_vec3 vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
	//	_vec3 vDir = m_vNextPos - vMyPos;
	//	D3DXVec3Normalize(&vDir, &vDir);

	//	vMyPos += vDir * m_fCamSpeed;

	//	m_pTransformCom->Set_Pos(&vMyPos);
	//}

	//if (_vec3(0.f, 0.f, 0.f) == m_vNextPos)
	//	m_vNextPos = m_vPrePos;

	return iExit;
}

void CMFC_CamInterpol::Render_Object()
{
	return;
}

HRESULT CMFC_CamInterpol::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}


CMFC_CamInterpol * CMFC_CamInterpol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_CamInterpol*	pInstance = new CMFC_CamInterpol(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_CamInterpol::Free()
{
	CGameObject::Free();
}
