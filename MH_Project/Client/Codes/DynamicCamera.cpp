#include "stdafx.h"
#include "DynamicCamera.h"
#include "Player.h"

#include "Export_Function.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{

}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs)
	: CCamera(rhs)
{

}

CDynamicCamera::~CDynamicCamera(void)
{

}

HRESULT CDynamicCamera::Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_Object(), E_FAIL);

	return S_OK;
}

Engine::_int CDynamicCamera::Update_Object(const _float& fTimeDelta)
{
	_int	iExit = CCamera::Update_Object(fTimeDelta);

	Key_Input(fTimeDelta);
	Movement(fTimeDelta);

	if (true == m_bFix)
	{
		Mouse_Fix();
		Mouse_Move();
	}	

	//m_matWorld = *m_pTransformCom->Get_WorldMatrix();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return iExit;
}

const _vec3 & CDynamicCamera::Get_CamDirVector(DIR eDir)
{
	_vec3 vRight = {};

	_vec3 vTempEye = m_vEye;
	vTempEye.y = m_vAt.y;
	_vec3 vDirToPlayer = m_vAt - vTempEye;

	D3DXVec3Normalize(&vDirToPlayer, &vDirToPlayer);

	switch (eDir)
	{
	case Engine::DIR_LOOK:
		return vDirToPlayer;
		break;

	case Engine::DIR_RIGHT:
		D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vDirToPlayer);

		return vRight;
		break;

	case Engine::DIR_UP:
		return _vec3(0.f, 1.f, 0.f);
		break;
	}

	return _vec3();
}

void CDynamicCamera::Movement(const _float & fTimeDelta)
{
	m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));

	// 타겟 At 위치 보정
	m_pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
	vPlayerPos.y += m_fInterpolY;
	m_vAt = vPlayerPos;

	// 플레이어 따라다니기
	_vec3 vDir = m_vEye - m_vAt;
	D3DXVec3Normalize(&vDir, &vDir);

	_vec3 vPos = vDir * m_fDistanceFromTarget;
	m_vEye = m_vAt + vPos;
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	if (Get_DIKeyState(DIK_Q) & 0x80)
	{
		if (true == m_bClick)
			return;

		m_bClick = true;

		if (true == m_bFix)
			m_bFix = false;

		else
			m_bFix = true;

	}
	else
		m_bClick = false;

	if (false == m_bFix)
		return;
}

void CDynamicCamera::Mouse_Move(void)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	_long	dwMouse = 0;

	if (m_pPlayerTrans)
	{
		_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
		vPlayerPos.y += m_fInterpolY;

		_vec3 vDirToCam = m_vEye - vPlayerPos;
		D3DXVec3Normalize(&vDirToCam, &vDirToCam);

		if (dwMouse = Get_DIMouseMove(DIMS_Y))
		{
			_vec3 vRight = {};
			_vec3 vCamLook = Get_CamLook();
			D3DXVec3Normalize(&vCamLook, &vCamLook);

			D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vCamLook);
			D3DXVec3Normalize(&vRight, &vRight);

			_matrix matRotate;
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationAxis(&matRotate, &vRight, D3DXToRadian(dwMouse * 0.1f));
			D3DXVec3TransformNormal(&vDirToCam, &vDirToCam, &matRotate);
		}

		if (dwMouse = Get_DIMouseMove(DIMS_X))
		{
			_vec3 vPlayerUp = *m_pPlayerTrans->Get_Info(INFO_UP);
			D3DXVec3Normalize(&vPlayerUp, &vPlayerUp);

			_matrix matRotate;
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationAxis(&matRotate, &vPlayerUp, D3DXToRadian(dwMouse * 0.1f));
			D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);
		}

		m_vEye = m_vAt + vDirToCam * m_fDistanceFromTarget;
	}
}

void CDynamicCamera::Mouse_Fix(void)
{
	POINT		pt = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CDynamicCamera*		pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDynamicCamera::Free(void)
{
	CCamera::Free();
}

