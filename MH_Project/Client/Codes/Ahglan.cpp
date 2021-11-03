#include "stdafx.h"
#include "Ahglan.h"
#include "Player.h"
#include "DynamicCamera.h"

#include "Export_Utility.h"

USING(Engine)

CAhglan::CAhglan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CAhglan::~CAhglan()
{
}


HRESULT CAhglan::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(0.f, 0.f, -7.f);
	m_pTransformCom->Set_Scale(SCALE_AHGLAN, SCALE_AHGLAN, SCALE_AHGLAN);
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180));

	return S_OK;
}

HRESULT CAhglan::LateReady_Object()
{
	CGameObject::LateReady_Object();

	m_pMainCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	list<D3DXMESHCONTAINER_DERIVED*>listTemp = m_pMeshCom->Get_MeshContainerList();
	list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();
	for (_uint i = 0; i < (*iter)->dwNumBones; ++i)
	{
		_tchar	pTemp[64] = L"";
		MultiByteToWideChar(CP_ACP, 0, (*iter)->pSkinInfo->GetBoneName(i), strlen((*iter)->pSkinInfo->GetBoneName(i)), pTemp, 64);

		if (!wcscmp(L"ValveBiped_Bip01_R_Foot", pTemp))
		{
			memcpy(&m_vRFootPos, &((*iter)->ppCombinedTransformMatrix[i]->_41), sizeof(_vec3));
		}
		else if (!wcscmp(L"ValveBiped_Bip01_L_Foot", pTemp))
		{
			memcpy(&m_vLFootPos, &((*iter)->ppCombinedTransformMatrix[i]->_41), sizeof(_vec3));
		}
	}

	return S_OK;
}

_int CAhglan::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	m_fTimeDelta = fTimeDelta;

	Movement();
	Animation_Control();
	Collision_Control();
	MoveOn_Skill();
	RotationOn_Skill();

	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CAhglan::Render_Object(void)
{
	if (!m_mapColliderCom.empty())
	{
		map<const wstring, CCollider*>::iterator	iter = m_mapColliderCom.begin();

		for (; iter != m_mapColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}
	if (!m_mapBoxColliderCom.empty())
	{
		map<const wstring, CBoxCollider*>::iterator		iter = m_mapBoxColliderCom.begin();

		for (; iter != m_mapBoxColliderCom.end(); ++iter)
		{
			if (iter->second->Get_CanCollision())
			{
				iter->second->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
			}
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
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

	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CAhglan::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Engine::Clone_Prototype(L"Proto_Mesh_Ahglan"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CAhglan::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 10.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrl.Diffuse);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrl.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrl.Ambient);

	pEffect->SetFloat("g_fPower", tMtrl.Power);

	const D3DLIGHT9*	pLightInfo = Get_Light();
	NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	return S_OK;
}

void CAhglan::Contact()
{
	if (BS_ENTRY <= m_eBossAction &&
		m_fDistance <= DIS_LONG)
	{
		CDynamicCamera* pCamera = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));
		if (pCamera)
			pCamera->Set_CameraMode(CDynamicCamera::MODE_AHGLAN_START);

		m_iAniIndex = ENTRY_CONTACT;

		Animation_Control();
	}
}

void CAhglan::Movement()
{
	m_pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	if (m_pPlayer)
		m_pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

	m_vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
	if (m_pPlayerTrans)
		m_vPlayerPos = *m_pPlayerTrans->Get_Info(INFO_POS);
	m_fDistance = D3DXVec3Length(&(m_vMyPos - m_vPlayerPos));

	_vec3	vPlayerDir = m_vPlayerPos - m_vMyPos;
	m_vDir = -(*m_pTransformCom->Get_Info(INFO_LOOK));
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

	m_fAngle = D3DXToDegree(acos(D3DXVec3Dot(&m_vDir, &vPlayerDir)));

	// 아글란 메쉬의 축이 반대로 뒤틀려있다. 그래서 계산도 반대로 함. Look == Back, Right == Left;
	if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) > 0.f)
		m_bTargetIsRight = false;
	else if (D3DXVec3Dot(&_vec3(0.f, 1.f, 0.f), D3DXVec3Cross(&_vec3(), &vPlayerDir, &m_vDir)) < 0.f)
		m_bTargetIsRight = true;


	if (m_bCanAction && m_pPlayerTrans)
	{
		if (BS_DAMAGED <= m_eBossAction)
		{

		}
		else if (BS_ATK <= m_eBossAction)
		{
			if (DIS_SHORT > m_fDistance)
			{
				if (m_dwWindmillCoolDown + m_dwWindmillDelay < GetTickCount())
				{
					m_iAniIndex = ATK_WINDMILL;
				}
				else
				{
					if (30.f >= m_fRand)
					{
						m_iAniIndex = ATK_TWOHANDS;
					}
					else if (50.f >= m_fRand)
					{
						m_iAniIndex = ATK_ONEHAND;
					}
					else if (60.f >= m_fRand)
					{
						m_iAniIndex = TAUNT;
					}
					else if (90.f >= m_fRand)
					{
						m_iAniIndex = ATK_STAMP;
					}
					else
					{
						if (m_bTargetIsRight)
						{
							m_iAniIndex = ATK_TURNRIGHT;
						}
						else
						{
							m_iAniIndex = ATK_TURNLEFT;
						}
					}
				}
			}
		}
		else if (BS_IDLE <= m_eBossAction)
		{
			if (DIS_SHORTEST >= m_fDistance &&
				20.f >= m_fRand)
			{
				if (m_bTargetIsRight)
				{
					m_iAniIndex = ATK_TURNRIGHT;
				}
				else
				{
					m_iAniIndex = ATK_TURNLEFT;
				}
			}
			else if (60.f <= m_fAngle)
			{
				if (m_bTargetIsRight)
				{
					m_iAniIndex = TURN_RIGHT;
				}
				else
				{
					m_iAniIndex = TURN_LEFT;
				}
			}
			else if (DIS_SHORT < m_fDistance)
			{
				if (m_dwRollingAtkCoolDown + m_dwRollingAtkDelay < GetTickCount())
				{
					m_iAniIndex = ATK_ROLLING_ONETIME_BEGIN;
				}
				else
				{
					m_iAniIndex = WALK;
				}
			}
		}
	}
}

void CAhglan::MoveOn_Skill()
{
	if (m_bSkillMove)
	{
		if (m_fSkillMoveStartTime <= m_fAniTime &&
			m_fSkillMoveEndTime >= m_fAniTime)
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, m_fSkillMoveSpeed, m_fTimeDelta));
		}
		else if (m_fSkillMoveEndTime < m_fAniTime)
		{
			m_bSkillMove = false;
		}
	}
}

void CAhglan::RotationOn_Skill()
{
	if (m_bSkillRotation)
	{
		if (m_fSkillRotStartTime <= m_fAniTime &&
			m_fSkillRotEndTime >= m_fAniTime)
		{
			if (10.f <= m_fAngle)
			{
				if (m_bTargetIsRight)
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(m_fSkillRotSpeed * m_fTimeDelta));
				}
				else
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-m_fSkillRotSpeed * m_fTimeDelta));
				}
			}
		}
		else if (m_fSkillRotEndTime < m_fAniTime)
		{
			m_bSkillRotation = false;
		}
	}
}

void CAhglan::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();
	m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

	// 상태 변경 시 한번만 실행
	m_eCurState = (STATE)m_iAniIndex;
	if (m_eCurState != m_ePreState)
	{
		m_fAniTime = 0.f;

		switch (m_eCurState)
		{
		case WALK:
			m_eBossAction = BS_IDLE;

			m_pMeshCom->Set_TrackSpeed(2.15f);
			break;

		case TURN_RIGHT:
			m_eBossAction = BS_IDLE;
			break;

		case TURN_LEFT:
			m_eBossAction = BS_IDLE;
			break;

		case TAUNT:
			m_eBossAction = BS_ATK;
			break;

		case SPAWN:
			break;

		case LOW_HEALTH:
			m_eBossAction = BS_IDLE;
			break;

		case IDLE:
			m_eBossAction = BS_IDLE;
			break;

		case ATK_WINDMILL:
			m_eBossAction = BS_ATK;

			m_bCanAction = false;

			m_dwWindmillCoolDown = GetTickCount();
			m_dwCoolDownInterpol += 5000;
			m_dwWindmillDelay = m_dwCoolDownInterpol + rand() % 20000;

			m_fAniEndDelay = 1.05f;
			break;

		case ATK_TWOHANDS_COMBO:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE(0.f, 3.5f, (_float)m_lfAniEnd * 0.45f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.8f);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case ATK_TWOHANDS:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd * 0.8f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.8f);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case ATK_TURNRIGHT:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.15f + m_fRandSpeed);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case ATK_TURNLEFT:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.15f + m_fRandSpeed);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case ATK_STAMP:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.05f, 4.f, (_float)m_lfAniEnd);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.9f);

			if (30.f >= m_fRand)
				m_fAniEndDelay = 0.95f;
			else
				m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case ATK_ROLLING_TWICE:
			break;

		case ATK_ROLLING_ONETIME_END:
			m_pMeshCom->Set_TrackSpeed(2.1f);

			m_fAniEndDelay = 1.125f;
			m_bCanAction = false;
			break;

		case ATK_ROLLING_ONETIME_BEGIN:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(1.8f);
			m_dwRollingStartTime = GetTickCount();
			m_dwRollingAtkCoolDown = GetTickCount();
			m_dwRollingAtkDelay = m_dwCoolDownInterpol + rand() % 10000;

			m_fAniEndDelay = 0.95f;
			m_bCanAction = false;
			break;

		case ATK_ONEHAND:
			m_eBossAction = BS_ATK;

			m_pMeshCom->Set_TrackSpeed(2.f + m_fRandSpeed);
			BS_SKILL_MOVE((_float)m_lfAniEnd * 0.25f, 3.f, (_float)m_lfAniEnd * 0.5f);
			BS_SKILL_ROTATION(0.f, 180.f, (_float)m_lfAniEnd * 0.5f);

			m_fAniEndDelay = 1.05f;
			m_bCanAction = false;
			break;

		case DAMAGE_FROM_FRONT:
			m_eBossAction = BS_DAMAGED;

			m_bCanAction = false;
			break;

		case DAMAGE_FROM_BACK:
			m_eBossAction = BS_DAMAGED;

			m_bCanAction = false;
			break;

		case ENTRY_IDLE:
			m_eBossAction = BS_ENTRY;
			m_lfAniEnd = 1.f;
			break;

		case ENTRY_CONTACT:
			m_pMeshCom->Set_TrackSpeed(1.85f);
			m_fAniEndDelay = 1.08f;
			break;
		}

		m_ePreState = m_eCurState;
	}

	// 상태 변경 시 매 프레임 실행
	switch (m_eCurState)
	{
	case WALK:
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, m_fSpeed, m_fTimeDelta));

		if (5.f <= m_fAngle)
		{
			if (m_bTargetIsRight)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
			}
			else
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
			}
		}

		if (DIS_SHORT > m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_eBossAction = BS_ATK;
		}
		break;

	case TURN_RIGHT:
		if (5.f <= m_fAngle)
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * m_fTimeDelta));
		break;

	case TURN_LEFT:
		if (5.f <= m_fAngle)
			m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * -m_fTimeDelta));
		break;

	case TAUNT:
		break;

	case SPAWN:
		break;

	case LOW_HEALTH:
		break;

	case IDLE:
		if (DIS_SHORT > m_fDistance)
		{
			// 강제로 다음 행동이 ATK을 수행하게 한다. 
			m_eBossAction = BS_ATK;
		}
		break;

	case ATK_WINDMILL:
		if (m_fAniTime <= m_lfAniEnd * 0.3f)
		{
			m_pMainCamera->Set_CameraShake(true, CAMSHAKE_POWER * 0.5f, 4000, 3.f);
		}
		break;

	case ATK_TWOHANDS_COMBO:
		break;

	case ATK_TWOHANDS:
		break;

	case ATK_TURNRIGHT:
		break;

	case ATK_TURNLEFT:
		break;

	case ATK_STAMP:
		break;

	case ATK_ROLLING_TWICE:
		break;

	case ATK_ROLLING_ONETIME_END:
		break;

	case ATK_ROLLING_ONETIME_BEGIN:
		if (m_dwRollingStartTime + 1800 > GetTickCount())
		{
			if (10.f <= m_fAngle)
			{
				if (m_bTargetIsRight)
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f * m_fTimeDelta));
				}
				else
				{
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-270.f * m_fTimeDelta));
				}
			}
		}
		if (m_dwRollingStartTime + 1800 < GetTickCount() &&
			m_dwRollingStartTime + 2700 > GetTickCount())
		{
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->MoveOn_NaviMesh(&m_vMyPos, &m_vDir, 22.5f, m_fTimeDelta));
		}
		break;

	case ATK_ONEHAND:
		break;

	case DAMAGE_FROM_FRONT:
		break;

	case DAMAGE_FROM_BACK:
		break;

	case ENTRY_IDLE:
		Contact();
		break;

	case ENTRY_CONTACT:
		break;

	default:
		break;
	}

	if (m_fAniTime >= m_lfAniEnd * m_fAniEndDelay + m_fRandSpeed)
	{
		if (m_iAniIndex == (_uint)ATK_ROLLING_ONETIME_BEGIN)
		{
			m_iAniIndex = (_uint)ATK_ROLLING_ONETIME_END;
		}
		else if (m_iAniIndex == (_uint)ATK_STAMP &&
			0.95f == m_fAniEndDelay)
		{
			if (m_bTargetIsRight)
			{
				m_iAniIndex = ATK_TURNRIGHT;
			}
			else
			{
				m_iAniIndex = ATK_TURNLEFT;
			}
		}
		else if (m_iAniIndex == (_uint)TAUNT)
		{
			m_iAniIndex = ATK_TWOHANDS_COMBO;

			m_fRand = Engine::Random(0.f, 100.f);
		}
		else if (m_iAniIndex != (_uint)ENTRY_IDLE &&
				 m_iAniIndex != (_uint)ENTRY_CONTACT &&
				 m_iAniIndex != (_uint)WALK)
		{
			if (m_iAniIndex == (_uint)ATK_TURNLEFT)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f));
			}
			else if (m_iAniIndex == (_uint)ATK_TURNRIGHT)
			{
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f));
			}

			m_fRand = Engine::Random(0.f, 100.f);

			m_iAniIndex = (_uint)IDLE;
			m_eBossAction = BS_IDLE;

			m_pMeshCom->Set_TrackSpeed(2.f);
			m_fRandSpeed = (rand() % 300) * 0.001f;

			m_bCanAction = true;
		}
		else if (m_iAniIndex == (_uint)ENTRY_CONTACT)
		{
			m_iAniIndex = (_uint)SPAWN;
		}
	}
}

void CAhglan::Collision_Control()
{
	_float	fAniTime = m_pMeshCom->Get_AniFrameTime();

	map<const wstring, CBoxCollider*>::iterator	iter_Damaged = m_mapBoxColliderCom.begin();
	map<const wstring, CCollider*>::iterator	iter_Hit = m_mapColliderCom.begin();

	// DamagedBox
	if (!m_mapBoxColliderCom.empty())
	{
		for (; iter_Damaged != m_mapBoxColliderCom.end(); ++iter_Damaged)
		{
			if (ATK_WINDMILL == m_iAniIndex)
			{
				iter_Damaged->second->Set_CanCollision(false);
			}
			else
			{
				iter_Damaged->second->Set_CanCollision(true);
			}
		}
	}

	// HitBox
	switch (m_eCurState)
	{
	case CAhglan::ATK_WINDMILL:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.13f, m_lfAniEnd * 0.7f);
		}
		break;

	case CAhglan::ATK_TWOHANDS_COMBO:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_BodyAtk" == iter_Hit->first ||
				L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.3f, m_lfAniEnd * 0.4f);
			}
		}
		break;

	case CAhglan::ATK_TWOHANDS:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_BodyAtk" == iter_Hit->first ||
				L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.5f);
			}
		}
		break;

	case CAhglan::ATK_TURNRIGHT:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.35f, m_lfAniEnd * 0.55f);
			}
		}
		break;

	case CAhglan::ATK_TURNLEFT:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LHand" == iter_Hit->first ||
				L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.4f, m_lfAniEnd * 0.65f);
			}
		}
		break;

	case CAhglan::ATK_STAMP:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_LFoot" == iter_Hit->first)
			{
				if (m_lfAniEnd * 0.1f <= fAniTime &&
					m_lfAniEnd * 0.23f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else if (m_lfAniEnd * 0.5f <= fAniTime &&
					m_lfAniEnd * 0.65f >= fAniTime)
				{
					iter_Hit->second->Set_CanCollision(true);
				}
				else
				{
					iter_Hit->second->Set_CanCollision(false);
				}
			}
			else if (L"Hit_RFoot" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.35f, m_lfAniEnd * 0.43f);
			}
		}
		break;

	case CAhglan::ATK_ROLLING_TWICE:
		break;

	case CAhglan::ATK_ROLLING_ONETIME_END:
		break;

	case CAhglan::ATK_ROLLING_ONETIME_BEGIN:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.55f, m_lfAniEnd * 0.95f);
		}
		break;

	case CAhglan::ATK_ONEHAND:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			if (L"Hit_RHand" == iter_Hit->first)
			{
				HITBOX_CONTROLL_SPHERE(m_lfAniEnd * 0.3f, m_lfAniEnd * 0.42f);
			}
		}
		break;

	default:
		for (; iter_Hit != m_mapColliderCom.end(); ++iter_Hit)
		{
			iter_Hit->second->Set_CanCollision(false);
		}
		break;
	}
}

HRESULT CAhglan::Add_NaviMesh()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pNaviMeshCom = dynamic_cast<CNaviMesh*>(Engine::Clone_Prototype(L"Proto_NaviMesh"));
	NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}


CAhglan * CAhglan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAhglan*	pInstance = new CAhglan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CAhglan::Free(void)
{
	CGameObject::Free();
}
