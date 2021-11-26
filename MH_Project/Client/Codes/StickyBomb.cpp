#include "stdafx.h"
#include "StickyBomb.h"
#include "DynamicCamera.h"
#include "Ahglan.h"

#include "Export_Function.h"

CStickyBomb::CStickyBomb(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fSpeed(15.f)
	, m_fFallSpeed(5.f)
	, m_fTime(0.f)
{
}

CStickyBomb::CStickyBomb(const CStickyBomb& rhs)
	: CGameObject(rhs)
	, m_fSpeed(15.f)
	, m_fFallSpeed(5.f)
	, m_fTime(0.f)
{
}

CStickyBomb::~CStickyBomb(void)
{
}


HRESULT CStickyBomb::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(SCALE_PLAYER + 0.01f, SCALE_PLAYER + 0.01f, SCALE_PLAYER + 0.01f);

	return S_OK;
}

HRESULT CStickyBomb::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	CTransform*		pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	CDynamicCamera*	pCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	if (pCamera && pPlayerTrans)
	{
		_vec3	vPlayerPos = *pPlayerTrans->Get_Info(INFO_POS);
		vPlayerPos.y += 0.5f;
		m_vDir = *pPlayerTrans->Get_Info(INFO_LOOK);
		D3DXVec3Normalize(&m_vDir, &m_vDir);

		_vec3	vPos = vPlayerPos + m_vDir * 0.5f;
		m_pTransformCom->Set_Pos(&vPos);

		m_vStartPos = vPos;
		m_vMovePos = vPos;

		_vec3	vCamDir = pCamera->Get_CamDirVector(DIR_LOOK);
		_vec3	vCamPos = pCamera->Get_EyePos();
		_vec3	vAtPos = pCamera->Get_AtPos();

		m_vFlightDir = vAtPos - vCamPos;
		D3DXVec3Normalize(&m_vFlightDir, &m_vFlightDir);

		m_fCamAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vCamDir, &m_vFlightDir)));

		if (vCamPos.y < vAtPos.y)
		{
			m_fCamAngle *= 0.35f;
			m_fFallSpeed += m_fCamAngle;
		}
		else
		{
			m_fCamAngle *= 0.2f;
			m_fFallSpeed -= m_fCamAngle;
		}
	}

	Ready_Collider();

	return S_OK;
}

_int CStickyBomb::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	Movement(fTimeDelta);
	Collision();
	Bomb();

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CStickyBomb::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->LateUpdate_Collider(m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	return iExit;
}

void CStickyBomb::Render_Object(void)
{
	//if (!m_mapBoxColliderCom.empty())
	//{
	//	map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

	//	for (; iter != m_mapBoxColliderCom.end(); ++iter)
	//	{
	//		if (iter->second->Get_CanCollision())
	//		{
	//			iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	//		}
	//	}
	//}

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


CStickyBomb* CStickyBomb::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStickyBomb*	pInstance = new CStickyBomb(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStickyBomb::Free(void)
{
	CGameObject::Free();
}

HRESULT CStickyBomb::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CStaticMesh*>(Clone_Prototype(L"Proto_Mesh_StickyBomb"));
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
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CStickyBomb::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}

HRESULT CStickyBomb::Ready_Collider()
{
	//D3DXComputeBoundingBox(m_pTransformCom->Get_Info(INFO_POS), 8, sizeof(_vec3), &m_vMin, &m_vMax);

	//Add_Collider(m_vMin.x, m_vMin.y, m_vMin.z, m_vMax.x, m_vMax.y, m_vMax.z, L"Collider_StickyBomb", COLTYPE_BOX_HIT);
	FAILED_CHECK_RETURN(Add_Collider(-5.f, -5.f, -5.f, 5.f, 5.f, 5.f, L"Collider_StickyBomb", m_pTransformCom->Get_WorldMatrix(), COLTYPE_BOX_HIT), E_FAIL);

	return S_OK;
}

void CStickyBomb::Movement(const _float & fTimeDelta)
{
	// p = p - (vo * t - 0.5f * GRAVITY * t^2)
	// p = p0 + (vo * t) + (0.5 * a * t ^ 2)
	// v = v0 + (a * t)
	if (!m_bSticky)
	{
		_float	fY = m_pTransformCom->Get_Info(INFO_POS)->y;

		if (0.1f >= fY)
		{
			m_bDead = true;
			SoundMgr(L"effect_bomb_explode.wav", CSoundMgr::PLAYER_EFFECT);

			return;
		}

		fY = m_vStartPos.y - (m_fFallSpeed * m_fTime + (0.5f * GRAVITY * m_fTime * m_fTime));
		m_fTime -= 0.006f;

		m_pTransformCom->Move_Pos(&m_vFlightDir, m_fSpeed, fTimeDelta);
		_vec3 vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
		m_pTransformCom->Set_Pos(vMyPos.x, fY, vMyPos.z);
	}
}

void CStickyBomb::Bomb()
{
	if (m_bSticky)
	{
		m_bDead = true;
		SoundMgr(L"effect_bomb_explode.wav", CSoundMgr::PLAYER_EFFECT);

		CAhglan*	pAhglan = static_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));

		if (pAhglan)
		{
			switch (m_eMode)
			{
			case CStickyBomb::CHEST:
				pAhglan->BombAttacked(*m_pTransformCom->Get_Info(INFO_POS), L"golem_ahglan_Chest.tga");
				break;

			case CStickyBomb::HEAD:
				pAhglan->BombAttacked(*m_pTransformCom->Get_Info(INFO_POS), L"golem_ahglan_Head.tga");
				break;

			case CStickyBomb::RHAND:
				pAhglan->BombAttacked(*m_pTransformCom->Get_Info(INFO_POS), L"golem_ahglan_RHand.tga");
				break;

			default:
				pAhglan->BombAttacked(*m_pTransformCom->Get_Info(INFO_POS), L"");
				break;
			}
		}
	}
}

void CStickyBomb::Collision()
{
	CAhglan*	pAhglan = static_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));

	if (pAhglan)
	{
		map<const wstring, CBoxCollider*>::iterator		iter_BombCol	 = m_mapBoxColliderCom.begin();

		map<const wstring, CBoxCollider*>				mapBossBoxCol	 = pAhglan->Get_MapBoxCollider();
		map<const wstring, CBoxCollider*>::iterator		iter_BossDamaged = mapBossBoxCol.begin();

		for (; iter_BossDamaged != mapBossBoxCol.end(); ++iter_BossDamaged)
		{
			if (m_pCalculatorCom->Collision_OBB(&iter_BossDamaged->second->Get_Min(), &iter_BossDamaged->second->Get_Max(), iter_BossDamaged->second->Get_ColliderWorld(),
												&iter_BombCol->second->Get_Min(), &iter_BombCol->second->Get_Max(), m_pTransformCom->Get_WorldMatrix()))
			{
				m_bSticky = true;

				if (L"Chest" == iter_BossDamaged->first)
				{
					m_eMode = CHEST;
				}
				else if (L"RHand" == iter_BossDamaged->first)
				{
					m_eMode = RHAND;
				}
				else if (L"Head" == iter_BossDamaged->first)
				{
					m_eMode = HEAD;
				}
			}
		}
	}
}

