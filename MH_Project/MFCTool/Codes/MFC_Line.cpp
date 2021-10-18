#include "stdafx.h"
#include "MFC_Line.h"
#include "MFC_Camera.h"


CMFC_Line::CMFC_Line(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMFC_Line::~CMFC_Line()
{
}


HRESULT CMFC_Line::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//D3DXCreateLine(m_pGraphicDev, &m_pLine);
	//m_pLine->SetWidth(10.f);
	//m_pLine->SetAntialias(TRUE);
	//_vec3 vTemp[2];
	//vTemp[0] = _vec3(0.f, 0.f, 0.f);
	//vTemp[1] = _vec3(200.f, 200.f, 200.f);
	//CMFC_Camera* pMainCam = static_cast<CMFC_Camera*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));
	//m_pLine->DrawTransform(vTemp, 2, &(*m_pTransformCom->Get_WorldMatrix() * pMainCam->Get_MatView() * pMainCam->Get_MatProj()), D3DXCOLOR(0.f, 1.f, 1.f, 1.f));

	return S_OK;
}

_int CMFC_Line::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMFC_Line::Render_Object(void)
{
}

HRESULT CMFC_Line::Add_Component()
{


	return S_OK;
}

CMFC_Line * CMFC_Line::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMFC_Line*	pInstance = new CMFC_Line(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Line::Free()
{
	CGameObject::Free();
}
