#include "stdafx.h"
#include "DynamicCamera.h"
#include "Player.h"
#include "Ahglan.h"

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
	At_Update();
	Mode_Change();

	if (true == m_bFix)
	{
		Mouse_Fix();
		Mouse_Move();
	}

	//Camera_Shake();

	//m_matWorld = *m_pTransformCom->Get_WorldMatrix();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return iExit;
}

_int CDynamicCamera::LateUpdate_Object(const _float & fTimeDelta)
{
	_int	iExit = CCamera::LateUpdate_Object(fTimeDelta);

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

void CDynamicCamera::Camera_Shake()
{
	_vec3	vRandomTarget = _vec3(Engine::Random(-2.f, 2.f), Engine::Random(-2.f, 2.f), Engine::Random(-2.f, 2.f));
	_vec3	vTargetMove = m_vEye + vRandomTarget;
	_vec3	vTargetDir = vTargetMove - m_vEye;

	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	m_vEye += vTargetDir;
}

void CDynamicCamera::Mode_Change()
{
	if (m_eCurMode != m_ePreMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		switch (m_eCurMode)
		{
		case CDynamicCamera::MODE_NORMAL:
			break;

		case CDynamicCamera::MODE_AHGLAN_START:
			m_dwStartTime = GetTickCount();
			m_vEye = *D3DXVec3Normalize(&(-*pBossTrans->Get_Info(INFO_RIGHT)), &(-*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget * 500.f);
			break;

		case CDynamicCamera::MODE_AHGLAN_RISE:
			m_dwRiseTime = GetTickCount();
			break;

		case CDynamicCamera::MODE_AHGLAN_STAND:
			m_dwStandTime = GetTickCount();
			m_vEye = *D3DXVec3Normalize(&(+*pBossTrans->Get_Info(INFO_RIGHT)), &(+*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget);
			break;

		case CDynamicCamera::MODE_AHGLAN_COMPLETE:
			m_vEye = *D3DXVec3Normalize(&(-*pBossTrans->Get_Info(INFO_RIGHT)), &(-*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget);
			m_dwCompleteTime = GetTickCount();
			break;
		}

		m_ePreMode = m_eCurMode;
	}

	switch (m_eCurMode)
	{
	case CDynamicCamera::MODE_AHGLAN_START:
		if (m_dwStartTime + 2950 < GetTickCount())
			m_eCurMode = MODE_AHGLAN_RISE;
		break;

	case CDynamicCamera::MODE_AHGLAN_RISE:
		if (m_dwRiseTime + 2150 < GetTickCount())
			m_eCurMode = MODE_AHGLAN_STAND;
		break;

	case CDynamicCamera::MODE_AHGLAN_STAND:
		if (m_dwStandTime + 1200 < GetTickCount())
			m_eCurMode = MODE_AHGLAN_COMPLETE;
		break;

	case CDynamicCamera::MODE_AHGLAN_COMPLETE:
		if (m_dwCompleteTime + 6100 < GetTickCount())
			m_eCurMode = MODE_NORMAL;
		break;
	}
}

void CDynamicCamera::At_Update()
{
	if (MODE_NORMAL == m_eCurMode)
	{
		m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));

		// 타겟 At 위치 보정
		m_pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
		_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
		vPlayerPos.y += m_fInterpolY;
		m_vAt = vPlayerPos;

		// 플레이어 이동 전 m_vEye 위치 저장
		m_vFollowDir = m_vEye - m_vAt;
		D3DXVec3Normalize(&m_vFollowDir, &m_vFollowDir);
	}
	else if (MODE_AHGLAN_START == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			m_vAt = *pBossTrans->Get_Info(INFO_POS);
		}
	}
	else if (MODE_AHGLAN_RISE == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3	vAtTarget = *pBossTrans->Get_Info(INFO_POS);
			vAtTarget.y += m_fRiseUp;

			m_vAt = vAtTarget;

			m_fRiseUp += 0.0075f;
		}
	}
	else if (MODE_AHGLAN_STAND == m_eCurMode)
	{

	}
	else if (MODE_AHGLAN_COMPLETE == m_eCurMode)
	{

	}
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

	if (MODE_NORMAL == m_eCurMode)
	{
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
	else if (MODE_AHGLAN_START == m_eCurMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3 vUp = _vec3(0.f, 1.f, 0.f);
			_vec3 vBossPos = *pBossTrans->Get_Info(INFO_POS);
			vBossPos.y += m_fInterpolY;

			m_vEye.y = m_fInterpolY + 1.f;
			_vec3 vDirToCam = m_vEye - vBossPos;
			D3DXVec3Normalize(&vDirToCam, &vDirToCam);

			_matrix matRotate;
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(-0.025f));
			D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

			m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 4.5f);
		}
	}
	else if (MODE_AHGLAN_RISE == m_eCurMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3 vUp = _vec3(0.f, 1.f, 0.f);
			_vec3 vBossPos = *pBossTrans->Get_Info(INFO_POS);
			vBossPos.y += m_fInterpolY;

			m_vEye.y = m_fInterpolY + 1.f;
			_vec3 vDirToCam = m_vEye - vBossPos;
			D3DXVec3Normalize(&vDirToCam, &vDirToCam);

			_matrix matRotate;
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(-0.0075f));
			D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

			m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 1.f);
			m_vEye.y = m_fInterpolY + 0.25f;
		}
	}
	else if (MODE_AHGLAN_STAND == m_eCurMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3 vUp = _vec3(0.f, 1.f, 0.f);
			_vec3 vBossPos = *pBossTrans->Get_Info(INFO_POS);
			vBossPos.y += m_fInterpolY;

			m_vEye.y = m_fInterpolY + 1.f;
			_vec3 vDirToCam = m_vEye - vBossPos;
			D3DXVec3Normalize(&vDirToCam, &vDirToCam);

			_matrix matRotate;
			D3DXMatrixIdentity(&matRotate);
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(0.0075f));
			D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

			m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 5.f);
			m_vEye.y = m_fInterpolY + 1.f;
		}
	}
	else if (MODE_AHGLAN_COMPLETE == m_eCurMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3 vUp = _vec3(0.f, 1.f, 0.f);
			_vec3 vBossPos = *pBossTrans->Get_Info(INFO_POS);
			vBossPos.y += m_fInterpolY;

			if (m_dwCompleteTime + 3700 >= GetTickCount())
			{
				m_vEye.y = m_fInterpolY + 0.5f;
				_vec3 vDirToCam = m_vEye - m_vAt;
				D3DXVec3Normalize(&vDirToCam, &vDirToCam);

				_matrix matRotate;
				D3DXMatrixIdentity(&matRotate);
				D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(0.0075f));
				D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

				m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 2.f + m_fFarAway);
				m_fFarAway += 0.0015f;
			}
			else
			{
				m_vEye.y = m_fInterpolY;
				_vec3 vDirToCam = m_vEye - m_vAt;
				D3DXVec3Normalize(&vDirToCam, &vDirToCam);

				_matrix matRotate;
				D3DXMatrixIdentity(&matRotate);
				D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(0.0075f));
				D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

				m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 2.f + m_fFarAway);

				if (m_fFarAway <= 4.f)
				{
					m_fFarAway += 0.12f;
				}
			}
		}
	}
}

void CDynamicCamera::Mouse_Fix(void)
{
	POINT	pt = { WINCX >> 1, WINCY >> 1 };

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

