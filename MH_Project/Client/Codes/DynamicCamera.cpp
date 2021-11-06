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
	Camera_Shake();
	At_Update(fTimeDelta);
	Mode_Change();

	if (true == m_bFix)
	{
		Mouse_Fix();
	}
	Mouse_Move();

	//m_matWorld = *m_pTransformCom->Get_WorldMatrix();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return iExit;
}

_int CDynamicCamera::LateUpdate_Object(const _float & fTimeDelta)
{
	_int	iExit = CCamera::LateUpdate_Object(fTimeDelta);

	return iExit;
}

_vec3 CDynamicCamera::Get_CamDirVector(DIR eDir)
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

void CDynamicCamera::Set_CameraShake(_bool bShakeType, _float fPower, _ulong dwEndTime, _float fWaveInterpol)
{
	if (!bShakeType)
	{
		m_bShake = true;
		m_bLongShake = false;
		m_fShakePower = fPower;
		m_dwShakeTime = GetTickCount();
		m_dwShakeDelay = dwEndTime;
		m_fShakeWaveX = 0.f;
		m_fShakeWaveY = 0.f;
		m_fFXProgress = 0.f;
		m_fFYProgress = 0.f;
		m_fLongWaveInterpol = fWaveInterpol;
		m_vShakeInterpol = _vec3(0.f, 0.f, 0.f);
	}
	else
	{
		m_bShake = false;
		m_bLongShake = true;
		m_fShakePower = fPower;
		m_dwShakeTime = GetTickCount();
		m_dwShakeDelay = dwEndTime;
		m_fShakeWaveX = 0.f;
		m_fShakeWaveY = 0.f;
		m_fFXProgress = 0.f;
		m_fFYProgress = 0.f;
		m_fLongWaveInterpol = fWaveInterpol;
		m_vShakeInterpol = _vec3(0.f, 0.f, 0.f);
	}
}

void CDynamicCamera::Camera_Shake()
{
	if (m_bShake &&
		m_dwShakeTime + m_dwShakeDelay >= GetTickCount())
	{
		m_fFXProgress += WaveFxProgressive/* + Engine::Random(-0.001f, 0.002f)*/;
		m_fShakeWaveX += m_fFXProgress;
		m_fFYProgress += WaveFyProgressive/* + Engine::Random(-0.001f, 0.002f)*/;
		m_fShakeWaveY += m_fFYProgress;
		m_vShakeInterpol.x = sin(m_fShakeWaveX * 10.f) * powf(0.5f, m_fShakeWaveX) * m_fShakePower;
		m_vShakeInterpol.y = sin(m_fShakeWaveY * 5.f) * powf(0.5f, m_fShakeWaveY) * m_fShakePower;
		m_vShakeInterpol.z = sin(m_fShakeWaveX * 10.f) * powf(0.5f, m_fShakeWaveX) * m_fShakePower;
	}
	else if (m_bLongShake &&
		m_dwShakeTime + m_dwShakeDelay >= GetTickCount())
	{
		m_fFXProgress += WaveFxProgressive/* + Engine::Random(-0.001f, 0.002f)*/;
		m_fShakeWaveX += m_fFXProgress;
		m_fFYProgress += WaveFyProgressive/* + Engine::Random(-0.001f, 0.002f)*/;
		m_fShakeWaveY += m_fFYProgress;
		m_vShakeInterpol.x = sin(m_fShakeWaveX * m_fLongWaveInterpol) * powf(0.9f, m_fShakeWaveX * 0.02f) * m_fShakePower;
		m_vShakeInterpol.y = sin(m_fShakeWaveY * m_fLongWaveInterpol * 0.5f) * powf(0.9f, m_fShakeWaveY * 0.02f) * m_fShakePower;
		m_vShakeInterpol.z = sin(m_fShakeWaveX * m_fLongWaveInterpol) * powf(0.9f, m_fShakeWaveX * 0.02f) * m_fShakePower;
	}
	else if (m_bShake &&
		m_dwShakeTime + m_dwShakeDelay < GetTickCount())
	{
		m_bShake = false;
		m_bLongShake = false;
		m_fShakePower = 0.025f;
		m_fShakeWaveX = 0.f;
		m_fShakeWaveY = 0.f;
		m_fFXProgress = 0.f;
		m_fFYProgress = 0.f;
		m_fLongWaveInterpol = 0.5f;
		m_vShakeInterpol = _vec3(0.f, 0.f, 0.f);
	}
}

void CDynamicCamera::Mode_Change()
{
	if (m_eCurMode != m_ePreMode)
	{
		CTransform* pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		switch (m_eCurMode)
		{
		case CDynamicCamera::MODE_NORMAL:
			if (MODE_AHGLAN_COMPLETE == m_ePreMode &&
				SCENE_ID::SCENE_STAGE == Engine::Get_SceneID())
			{
				if (pBossTrans)
				{
					_vec3	vDir = *m_pPlayerTrans->Get_Info(INFO_POS) - *pBossTrans->Get_Info(INFO_POS);
					D3DXVec3Normalize(&vDir, &vDir);

					m_vEye = vDir * 100.f;
				}
			}
			else if (MODE_SECONDARY == m_ePreMode)
			{
				_vec3	vPos = *m_pPlayerTrans->Get_Info(INFO_POS);
				vPos.y = m_fInterpolY;
				_vec3	vDir = -*m_pPlayerTrans->Get_Info(INFO_LOOK);

				m_vEye = vPos + vDir * m_fDistanceFromTarget;
			}
			break;

		case CDynamicCamera::MODE_SECONDARY:
			break;

		case CDynamicCamera::MODE_AHGLAN_START:
			if (pBossTrans)
			{
				m_dwStartTime = GetTickCount();
				m_vEye = *D3DXVec3Normalize(&(-*pBossTrans->Get_Info(INFO_RIGHT)), &(-*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget * 500.f);
			}
			break;

		case CDynamicCamera::MODE_AHGLAN_RISE:
			Set_CameraShake(true, CAMSHAKE_POWER * 3.f, 3000);
			m_dwRiseTime = GetTickCount();
			break;

		case CDynamicCamera::MODE_AHGLAN_STAND:
			if (pBossTrans)
			{
				Set_CameraShake(true, CAMSHAKE_POWER * 3.f, 650, 1.75f);
				m_dwStandTime = GetTickCount();
				m_vEye = *D3DXVec3Normalize(&(+*pBossTrans->Get_Info(INFO_RIGHT)), &(+*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget);
			}
			break;

		case CDynamicCamera::MODE_AHGLAN_COMPLETE:
			if (pBossTrans)
			{
				m_vEye = *D3DXVec3Normalize(&(-*pBossTrans->Get_Info(INFO_RIGHT)), &(-*pBossTrans->Get_Info(INFO_RIGHT))) * (m_fDistanceFromTarget);
				m_dwCompleteTime = GetTickCount();
			}
			break;
		}

		m_ePreMode = m_eCurMode;
	}

	switch (m_eCurMode)
	{
	case CDynamicCamera::MODE_AHGLAN_START:
		if (m_dwStartTime + 2950 < GetTickCount())
			m_eCurMode = MODE_AHGLAN_RISE;
		if (!m_bSoundGolemEntry &&
			m_dwStartTime + 2350 < GetTickCount())
		{
			m_bSoundGolemEntry = true;
			SoundMgr(L"golem_cs_entry.wav", CSoundMgr::MONSTER);
		}
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

void CDynamicCamera::At_Update(const _float& fTimeDelta)
{
	if (MODE_NORMAL == m_eCurMode)
	{
		m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));

		// 타겟 At 위치 보정
		m_pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
		_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
		vPlayerPos.y += m_fInterpolY;

		m_vAt = vPlayerPos + m_vShakeInterpol - m_vPreShakeInterpol;
		m_vPreShakeInterpol = m_vShakeInterpol;
	}
	else if (MODE_SECONDARY == m_eCurMode)
	{
		m_pPlayerTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
		_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
		vPlayerPos.y += m_fInterpolY - 0.35f;
		_vec3 vPlayerRight = *m_pPlayerTrans->Get_Info(INFO_LOOK);
		D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);


		m_vAt = vPlayerPos + (vPlayerRight * 1.5f);
		// 무조건 정면을 바라보는 At
		//m_vAt = vPlayerPos + (vPlayerRight * 150.f);
	}
	else if (MODE_AHGLAN_START == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			m_vAt = *pBossTrans->Get_Info(INFO_POS) + m_vShakeInterpol - m_vPreShakeInterpol;
			m_vPreShakeInterpol = m_vShakeInterpol;
		}
	}
	else if (MODE_AHGLAN_RISE == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (pBossTrans)
		{
			_vec3	vAtTarget = *pBossTrans->Get_Info(INFO_POS);
			vAtTarget.y += m_fRiseUp;

			m_vAt = vAtTarget + m_vShakeInterpol - m_vPreShakeInterpol;
			m_vPreShakeInterpol = m_vShakeInterpol;

			m_fRiseUp += 0.0075f;
		}
	}
	else if (MODE_AHGLAN_STAND == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		_vec3	vAtTarget = *pBossTrans->Get_Info(INFO_POS);
		vAtTarget.y += m_fRiseUp;

		m_vAt = vAtTarget + m_vShakeInterpol - m_vPreShakeInterpol;
		m_vPreShakeInterpol = m_vShakeInterpol;
	}
	else if (MODE_AHGLAN_COMPLETE == m_eCurMode)
	{
		CTransform*	pBossTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		_vec3	vAtTarget = *pBossTrans->Get_Info(INFO_POS);
		vAtTarget.y += m_fRiseUp;

		m_vAt = vAtTarget + m_vShakeInterpol - m_vPreShakeInterpol;
		m_vPreShakeInterpol = m_vShakeInterpol;
	}
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	if (Engine::Key_Down('Q'))
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
	//_matrix		matCamWorld;
	//D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

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

			m_vEye = (m_vAt + vDirToCam * m_fDistanceFromTarget);// + (m_vShakeInterpol - m_vPreShakeInterpol);
			//m_vPreShakeInterpol = m_vShakeInterpol;
		}
	}
	else if (MODE_SECONDARY == m_eCurMode)
	{
		_long	dwMouse = 0;

		if (m_pPlayerTrans)
		{
			_vec3 vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
			vPlayerPos.y += m_fInterpolY - 0.35f;
			_vec3 vPlayerRight = *m_pPlayerTrans->Get_Info(INFO_RIGHT);
			_vec3 vPlayerBack = -(*m_pPlayerTrans->Get_Info(INFO_LOOK));

			_vec3	vDir = vPlayerRight + (vPlayerBack * 2.75f);
			D3DXVec3Normalize(&vDir, &vDir);

			// y축 고정 시점
			// m_vEye = vPlayerPos + vDir * 1.15f;
			_vec3	TempPos = vPlayerPos + vDir * 1.15f;
			m_vEye.x = TempPos.x;
			m_vEye.z = TempPos.z;

			if (dwMouse = Get_DIMouseMove(DIMS_Y))
			{
				m_vEye.y += dwMouse * 0.01f;

				if (m_vEye.y >= 1.0f)
				{
					m_vEye.y = 1.0f;
				}
				else if (m_vEye.y <= 0.2f)
				{
					m_vEye.y = 0.2f;
				}
			}
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

				Set_CameraShake(true, CAMSHAKE_POWER * 6.f, 800, 3.5f);
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

