#include "stdafx.h"
#include "Balista.h"

#include "DynamicCamera.h"


CBalista::CBalista(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBalista::CBalista(const CBalista & rhs)
	: CGameObject(rhs)
{
}

CBalista::~CBalista()
{
}


HRESULT CBalista::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(&POOLING_POS);
	m_pTransformCom->Set_Scale(SCALE_BALISTA * 1.5f, SCALE_BALISTA * 1.5f, SCALE_BALISTA * 1.25f);

	m_vOriginScale = *m_pTransformCom->Get_ScaleInfo();

	D3DXMatrixIdentity(&m_matStick);

	return S_OK;
}

HRESULT CBalista::LateReady_Object()
{
	m_eCurSceneID = Engine::Get_SceneID();

	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component_ForTrail(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Collider(), E_FAIL);

	switch (m_eCurSceneID)
	{
	case SCENE_STAGE:
		m_fSpeed = 122.5f;
		m_fFallingSpeed = 300.f;
		m_fOriginFallingSpeed = m_fFallingSpeed;
		m_fSpeedDown = 100.f;
		m_dwSurviveDelay = 6000;

		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(70.f));
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(-65.f));

		m_pMainCam = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));
		NULL_CHECK_RETURN(m_pMainCam, E_FAIL);
		break;

	case SCENE_STAGE_1:
		m_fSpeed = 75.f;
		m_fFallingSpeed = 0.f;
		m_fOriginFallingSpeed = m_fFallingSpeed;
		m_fSpeedDown = 0.f;
		m_dwSurviveDelay = 500;

		m_pTransformCom->Set_Scale(SCALE_BALISTA, SCALE_BALISTA, SCALE_BALISTA);
		break;
	}

	m_pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	return S_OK;
}

_int CBalista::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	m_fTimeDelta = fTimeDelta;

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		Movement(fTimeDelta);

		Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

_int CBalista::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (SCENE_STAGE_1 == m_eCurSceneID)
	{
		if (!m_mapColliderCom.empty())
		{
			m_mapColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		m_mapBoxColliderCom.begin()->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
	}

	return iExit;
}

void CBalista::Render_Object()
{
	//map<const wstring, CCollider*>::iterator	iter_Sphere = m_mapColliderCom.begin();
	//map<const wstring, CBoxCollider*>::iterator	iter_Box = m_mapBoxColliderCom.begin();

	//for (; iter_Sphere != m_mapColliderCom.end(); ++iter_Sphere)
	//{
	//	iter_Sphere->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	//}
	//for (; iter_Box != m_mapBoxColliderCom.end(); ++iter_Box)
	//{
	//	iter_Box->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	//}

	if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS))
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		pEffect->AddRef();

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		_uint iMaxPass = 0;

		pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
												// 2인자 : 시작하는 방식을 묻는 FLAG
		pEffect->BeginPass(0);

		m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}

	if (SCENE_STAGE == m_eCurSceneID)
	{
		_vec3	vMin = m_mapBoxColliderCom.begin()->second->Get_Min();
		_vec3	vMax = m_mapBoxColliderCom.begin()->second->Get_Max();
		vMin.x = -1.f;
		vMin.y = -1.f;
		//vMin.z = -1.f;
		vMax.x = 1.f;
		vMax.y = 1.f;
		vMax.z = 1.f;
		m_pTrailCom->Set_InfoForTrail(m_fTimeDelta, vMin, vMax, m_mapBoxColliderCom.begin()->second->Get_ColliderWorld());

		if (POOLING_POS != *m_pTransformCom->Get_Info(INFO_POS) && 
			!m_bEnemyHit && 
			0.f <= m_fFallingSpeed)
		{
			LPD3DXEFFECT	 pEffect = m_pTrailShaderCom->Get_EffectHandle();
			pEffect->AddRef();

			FAILED_CHECK_RETURN(SetUp_ConstantTable_ForTrail(pEffect), );

			_uint iMaxPass = 0;

			pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
													// 2인자 : 시작하는 방식을 묻는 FLAG
			pEffect->BeginPass(0);

			m_pTrailCom->Render_Buffer();

			pEffect->EndPass();
			pEffect->End();

			Safe_Release(pEffect);
		}
	}
}

HRESULT CBalista::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_Balista"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Normal"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CBalista::Add_Component_ForTrail()
{
	if (SCENE_STAGE == m_eCurSceneID)
	{
		///////// *** For. Trail Component *** /////////
		CComponent*		pComponent = nullptr;

		// Buffer
		pComponent = m_pTrailCom = dynamic_cast<CBalistaTrail*>(Clone_Prototype(L"Proto_Buffer_BalistaTrail"));
		NULL_CHECK_RETURN(m_pTrailCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Buffer", pComponent);

		// Texture
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Prototype(L"Proto_Texture_BalistaTrail"));
		NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Texture", pComponent);

		// Shader
		pComponent = m_pTrailShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Trail"));
		NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_TrailShader", pComponent);
	}

	return S_OK;
}

HRESULT CBalista::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlphaValue", m_fAlphaValue);

	return S_OK;
}

HRESULT CBalista::SetUp_ConstantTable_ForTrail(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlphaValue);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}

HRESULT CBalista::Ready_Collider()
{
	switch (m_eCurSceneID)
	{
	case SCENE_STAGE:
		FAILED_CHECK_RETURN(Add_Collider(-60.f, -60.f, -120.f, 60.f, 60.f, 60.f, L"Collider_Box", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_HIT), E_FAIL);
		break;

	case SCENE_STAGE_1:
		FAILED_CHECK_RETURN(Add_Collider(12.5f, L"Collider_Sphere", m_pTransformCom->Get_WorldMatrix(), COLTYPE_SPHERE_HIT), E_FAIL);
		FAILED_CHECK_RETURN(Add_Collider(-5.f, -5.f, -5.f, 5.f, 5.f, 5.f, L"Collider_Box", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_HIT), E_FAIL);
		break;
	}

	return S_OK;
}

void CBalista::Movement(const _float& fTimeDelta)
{
	if (SCENE_STAGE_1 == Engine::Get_SceneID())
	{
		if (!m_bCollision)
		{
			m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed, fTimeDelta);
		}
		else
		{
			if (m_dwSurviveTime + m_dwSurviveDelay < GetTickCount())
			{
				m_bCollision = false;
				m_bEnemyHit = true;
	
				D3DXMatrixIdentity(&m_matStick);
				m_pTransformCom->Set_Pos(&POOLING_POS);
			}
			else if (0.f != m_matStick._43)
			{
				_vec3	vPos = *m_pTransformCom->Get_Info(INFO_POS);
				vPos.z = m_matStick._43;

				m_pTransformCom->Set_Pos(&vPos);
			}
		}
	}
	else if (SCENE_STAGE == Engine::Get_SceneID())
	{
		if (35.f >= m_pTransformCom->Get_Info(INFO_POS)->y && 
			0.f > m_fFallingSpeed)
		{
			if (!m_bFlySoundCheck)
			{
				m_bFlySoundCheck = true;

				SoundMgrLowerVol(L"effect_ballista_flying.wav", CSoundMgr::BATTLE2, 1.5f / D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))));
			}
		}

		if (0.5f >= m_pTransformCom->Get_Info(INFO_POS)->y)
		{
			m_bCollision = true;
			m_bEnemyHit = true;

			_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
			vPos.y = 0.f;
			m_pTransformCom->Set_Pos(&vPos);

			if (!m_bSoundCheck)
			{
				_uint	iRand = rand() % 3;
				m_bSoundCheck = true;
				
				m_pMainCam->Set_CameraShake(false, CAMSHAKE_POWER * 2.5f);
				switch (iRand)
				{
				case 0:
					if (DIS_SHORTEST <= D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))))
					{
						SoundMgrLowerVol(L"effect_ballista_landing.wav", CSoundMgr::BATTLE, 0.5f / D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))));
					}
					else
					{
						SoundMgrLowerVol(L"effect_ballista_landing.wav", CSoundMgr::BATTLE, 0.15f);
					}
					break;
					
				case 1:
					if (DIS_SHORTEST <= D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))))
					{
						SoundMgrLowerVol(L"effect_ballista_landing2.wav", CSoundMgr::BATTLE, 0.5f / D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))));
					}
					else
					{
						SoundMgrLowerVol(L"effect_ballista_landing2.wav", CSoundMgr::BATTLE, 0.15f);
					}
					break;

				case 2:
					if (DIS_SHORTEST <= D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))))
					{
						SoundMgrLowerVol(L"effect_ballista_landing3.wav", CSoundMgr::BATTLE, 0.5f / D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))));
					}
					else
					{
						SoundMgrLowerVol(L"effect_ballista_landing3.wav", CSoundMgr::BATTLE, 0.15f);
					}
					break;
				}
				/*if (DIS_SHORTEST <= D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))))
				{
					SoundMgrLowerVol(L"Hit_Flesh_Stab.wav", CSoundMgr::BATTLE, 0.75f / D3DXVec3Length(&(*m_pPlayerTrans->Get_Info(INFO_POS) - *m_pTransformCom->Get_Info(INFO_POS))));
				}
				else
				{
					SoundMgrLowerVol(L"Hit_Flesh_Stab.wav", CSoundMgr::BATTLE, 0.35f);
				}*/
			}
		}

		if (!m_bCollision)
		{
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&m_vDir, &m_vDir), m_fSpeed, fTimeDelta);
			m_pTransformCom->Move_Pos(&_vec3(0.f, 1.f, 0.f), m_fFallingSpeed, fTimeDelta);

			m_fFallingSpeed -= m_fSpeedDown * fTimeDelta;
		}
		else
		{
			if (m_dwSurviveTime + m_dwSurviveDelay < GetTickCount())
			{
				m_fAlphaValue -= m_fAlphaInterpol * fTimeDelta;

				if (0.f > m_fAlphaValue)
				{
					m_bCollision = false;
					m_bEnemyHit = false;
					m_bSoundCheck = false;
					m_bFlySoundCheck = false;

					m_fAlphaValue = 1.f;
					m_fFallingSpeed = m_fOriginFallingSpeed;
					m_pTransformCom->Set_Pos(&POOLING_POS);
				}
			}
		}
	}
}

void CBalista::Set_EnableBalista(_vec3 vPos, _vec3 vDir)
{
	m_bCollision = false;
	m_bEnemyHit = false;
	m_bSoundCheck = false;
	m_bFlySoundCheck = false;
	m_fFallingSpeed = m_fOriginFallingSpeed;
	m_fAlphaValue = 1.f;
	m_dwSurviveDelay = 3000;

	m_pTransformCom->Set_Pos(&vPos);
	m_vDir = *D3DXVec3Normalize(&vDir, &vDir);

	D3DXMatrixIdentity(&m_matStick);
}

void CBalista::Set_ReturnPoolingPos()
{
	m_pTransformCom->Set_Pos(&POOLING_POS);
}


CBalista * CBalista::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBalista*	pInstance = new CBalista(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBalista::Free()
{
	CGameObject::Free();
}
