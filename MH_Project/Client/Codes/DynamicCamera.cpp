#include "stdafx.h"
#include "DynamicCamera.h"
#include "Player.h"
#include "Ahglan.h"
#include "Stage.h"
#include "Stage_1.h"

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
	m_vVirtualEye = m_vEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_Object(), E_FAIL);

	CComponent* pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);


	return S_OK;
}

HRESULT CDynamicCamera::LateReady_Object()
{
	CCamera::LateReady_Object();

	m_pFadeInOut = dynamic_cast<CFadeInOut*>(Engine::Get_GameObject(L"UI", L"FadeInOut_UI"));

	switch (Engine::Get_SceneID())	
	{
	case SCENE_STAGE:
		m_pStage = dynamic_cast<CStageMesh*>(Engine::Get_GameObject(L"GameLogic", L"StageMesh"));
		m_pStageMesh = dynamic_cast<CStageMesh*>(Engine::Get_GameObject(L"GameLogic", L"StageMesh"))->Get_MeshInfo();
		break;

	case SCENE_STAGE_1:
		m_pStage_1 = dynamic_cast<CStageMesh_1*>(Engine::Get_GameObject(L"GameLogic", L"StageMesh"));
		m_pStageMesh = dynamic_cast<CStageMesh_1*>(Engine::Get_GameObject(L"GameLogic", L"StageMesh"))->Get_MeshInfo();
		break;
	}

	return S_OK;
}

Engine::_int CDynamicCamera::Update_Object(const _float& fTimeDelta)
{
	_int	iExit = CCamera::Update_Object(fTimeDelta);

	Key_Input(fTimeDelta);
	Camera_Shake(fTimeDelta);
	At_Update(fTimeDelta);
	Mode_Change(fTimeDelta);

	if (true == m_bFix)
	{
		Mouse_Fix();
	}
	Highlight_SkillShot();
	Mouse_Move();
	CutScene_Eye(fTimeDelta);

	//m_matWorld = *m_pTransformCom->Get_WorldMatrix();
	m_pTransformCom->Set_Pos(&m_vEye);
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

void CDynamicCamera::Sync_PlayerPos(_vec3 vDir)
{
	_float	fDistance = D3DXVec3Length(&(m_vPrePlayerPos - *m_pPlayerTrans->Get_Info(INFO_POS)));

	m_vEye += vDir * fDistance;
	m_vVirtualEye += vDir * fDistance;
}

void CDynamicCamera::Highlight_SkillShot()
{
	if (m_dwHighlightStart + m_dwHighlightDelay >= GetTickCount())
	{
		if (m_fHighlightDistance < m_fDistanceFromTarget)
		{
			m_fDistanceFromTarget -= (m_fDistanceFromTarget - m_fHighlightDistance) * 0.1f;
		}
	}
	else
	{
		if (!m_bCollisionCam)
		{
			if (m_fOriginDistanceFromTarget > m_fDistanceFromTarget)
			{
				m_fDistanceFromTarget += (m_fOriginDistanceFromTarget - m_fDistanceFromTarget) * 0.1f;
			}
			else
			{
				m_fDistanceFromTarget = m_fOriginDistanceFromTarget;
			}
		}
	}
}

void CDynamicCamera::Camera_Shake(const _float& fTimeDelta)
{
	if (m_bShake &&
		m_dwShakeTime + m_dwShakeDelay >= GetTickCount())
	{
		m_fFXProgress += WaveFxProgressive * fTimeDelta;
		m_fShakeWaveX += m_fFXProgress;
		m_fFYProgress += WaveFyProgressive * fTimeDelta;
		m_fShakeWaveY += m_fFYProgress;
		m_vShakeInterpol.x = sin(m_fShakeWaveX * 10.f) * powf(0.5f, m_fShakeWaveX) * m_fShakePower;
		m_vShakeInterpol.y = sin(m_fShakeWaveY * 5.f) * powf(0.5f, m_fShakeWaveY) * m_fShakePower;
		m_vShakeInterpol.z = sin(m_fShakeWaveX * 10.f) * powf(0.5f, m_fShakeWaveX) * m_fShakePower;
	}
	else if (m_bLongShake &&
			 m_dwShakeTime + m_dwShakeDelay >= GetTickCount())
	{
		m_fFXProgress += WaveFxProgressive * fTimeDelta;
		m_fShakeWaveX += m_fFXProgress;
		m_fFYProgress += WaveFyProgressive * fTimeDelta;
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

void CDynamicCamera::Mode_Change(const _float& fTimeDelta)
{
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

					At_Update(fTimeDelta);
					m_vEye = vDir * DIS_LONG;

					m_pPlayerTrans->Rotation(ROT_Y, D3DXToRadian(45));
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
			Set_CameraShake(true, CAMSHAKE_POWER * 4.f, 2900, 0.25f);
			m_dwRiseTime = GetTickCount();
			break;

		case CDynamicCamera::MODE_AHGLAN_STAND:
			if (pBossTrans)
			{
				Set_CameraShake(true, CAMSHAKE_POWER * 1.f, 650, 2.f);
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

			m_fRiseUp += 1.05f * fTimeDelta;
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
			m_vVirtualEye = m_vAt + vDirToCam * m_fOriginDistanceFromTarget;
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

	Collision_StageMesh();
}

void CDynamicCamera::Mouse_Fix(void)
{
	POINT	pt = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CDynamicCamera::CutScene_Eye(const _float& fTimeDelta)
{
	if (MODE_AHGLAN_START == m_eCurMode)
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
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(-2.5f * fTimeDelta));
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
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(-0.95f * fTimeDelta));
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
			D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(0.75f * fTimeDelta));
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
				D3DXMatrixRotationAxis(&matRotate, &vUp, D3DXToRadian(0.75f * fTimeDelta));
				D3DXVec3TransformCoord(&vDirToCam, &vDirToCam, &matRotate);

				m_vEye = m_vAt + vDirToCam * (m_fDistanceFromTarget + 2.f + m_fFarAway);
				m_fFarAway += 0.25f * fTimeDelta;

				Set_CameraShake(true, CAMSHAKE_POWER * 7.f, 1000, 4.f);
			}
			else
			{
				if (m_dwCompleteTime + 4700 < GetTickCount())
				{
					m_pFadeInOut->Set_AhglanFadeOut();
				}

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
					m_fFarAway += 20.f * fTimeDelta;
				}
			}
		}
	}
}

void CDynamicCamera::Collision_StageMesh()
{
	if (MODE_NORMAL == m_eCurMode)
	{
		BOOL	bHit = false;
		_float	fDistance = 0.f;

		_float	fCamToPlayerDistance = D3DXVec3Length(&(m_vVirtualEye - m_vAt));

		_vec3		vAtPos = m_vAt;
		_vec3		vEyePos = m_vEye;
		_vec3		vVirtualEyePos = m_vVirtualEye;
		_vec3		vCamDir = vAtPos - vEyePos;

		_matrix		matWorld, matView, matProj;

		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		switch (Engine::Get_SceneID())
		{
		case SCENE_STAGE:
			matWorld = *m_pStage->Get_WorldMatrix();
			break;

		case SCENE_STAGE_1:
			matWorld = *m_pStage_1->Get_WorldMatrix();
			break;
		}

		D3DXMatrixInverse(&matView, nullptr, &matView);
		D3DXMatrixInverse(&matWorld, nullptr, &matWorld);

		D3DXVec3TransformCoord(&vAtPos, &vAtPos, &matWorld);
		D3DXVec3TransformCoord(&vEyePos, &vEyePos, &matWorld);
		D3DXVec3TransformCoord(&vVirtualEyePos, &vVirtualEyePos, &matWorld);
		D3DXVec3TransformNormal(&vCamDir, &vCamDir, &matWorld);
		D3DXVec3Normalize(&vCamDir, &vCamDir);

		D3DXIntersect(m_pStageMesh->Get_MeshInfo(), &vVirtualEyePos, &vCamDir, &bHit, nullptr, nullptr, nullptr, &fDistance, nullptr, nullptr);

		fDistance *= 0.02f;

		if (bHit &&
			fDistance < fCamToPlayerDistance)
		{
			m_bCollisionCam = true;
			m_fDistanceFromTarget = m_fOriginDistanceFromTarget - fDistance;
		}
		else
		{
			m_bCollisionCam = false;
			m_fDistanceFromTarget = m_fOriginDistanceFromTarget;
		}
	}
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

