#include "stdafx.h"
#include "CollisionMgr.h"

#include "Player.h"
#include "Ahglan.h"
#include "StickyBomb.h"
#include "DamageFont.h"
#include "DynamicCamera.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}


HRESULT CCollisionMgr::Ready_CollisionMgr()
{
	m_eSceneID = Engine::Get_SceneID();

	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		m_pAhglan = dynamic_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));
		NULL_CHECK_RETURN(m_pAhglan, E_FAIL);

		//m_pStickyBomb = dynamic_cast<CStickyBomb*>(Engine::Get_GameObject(L"Gam"));
		break;
	}

	return S_OK;
}

_uint CCollisionMgr::Update_CollisionMgr()
{
	Update_MultipleCollision();

	Collision_PlayerAttack();
	Collision_MonsterAttack();

	return 0;
}

void CCollisionMgr::Collision_PlayerAttack()
{
	map<const wstring, CBoxCollider*>				mapPlayerHit = m_pPlayer->Get_MapBoxCollider();
	map<const wstring, CBoxCollider*>::iterator		iter_PlayerHit = mapPlayerHit.begin();

	_bool	bPlayerAtkEnd = false;
	_int	iInterpolDamage = (_int)Engine::Random(-300.f, 300.f);

	CTransform*		pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

	if (SCENE_STAGE == m_eSceneID)
	{
		map<const wstring, CBoxCollider*>				mapBossBoxCol = m_pAhglan->Get_MapBoxCollider();
		map<const wstring, CBoxCollider*>::iterator		iter_BossDamaged = mapBossBoxCol.begin();

		CTransform*		pAhglanTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));

		if (!pAhglanTrans)
			return;

		// Player 공격
		if (DIS_MID >= D3DXVec3Length(&(*pPlayerTrans->Get_Info(INFO_POS) - *pAhglanTrans->Get_Info(INFO_POS))))
		{
			if (m_pPlayer->Get_CanHit())
			{
				if (m_pAhglan)
				{
					for (; iter_PlayerHit != mapPlayerHit.end(); ++iter_PlayerHit)
					{
						iter_BossDamaged = mapBossBoxCol.begin();

						for (; iter_BossDamaged != mapBossBoxCol.end(); ++iter_BossDamaged)
						{
							if (iter_PlayerHit->second->Get_CanCollision() &&
								iter_BossDamaged->second->Get_CanCollision())
							{
								if (Collision_OBB(&iter_PlayerHit->second->Get_Min(), &iter_PlayerHit->second->Get_Max(), iter_PlayerHit->second->Get_ColliderWorld(),
												  &iter_BossDamaged->second->Get_Min(), &iter_BossDamaged->second->Get_Max(), iter_BossDamaged->second->Get_ColliderWorld()))
								{
									bPlayerAtkEnd = true;

									switch (m_pPlayer->Get_CurState())
									{
									case Engine::STATE_SP_FEVER:
										m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);

										m_pPlayer->Add_Buff(BUFF_CRITICAL, 1500);
										SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);

										Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_FURY:
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
										m_pPlayer->Add_Buff(BUFF_CRITICAL, 1500);
										SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);

										Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_FURY2:
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);

										Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_DOUBLE_CRECSENT:
										SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
										m_pPlayer->Set_StopMotion(true, 100);
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_SMASH4:
										m_pPlayer->Set_StopMotion(true, 100);
										SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_SMASH3:
										m_pPlayer->Set_StopMotion(true, 100);
										SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_SMASH2_B:
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_SMASH2BPOWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH2BPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_SMASH2:
										m_pPlayer->Set_StopMotion(true, 100);
										//SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASH2POWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH2POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_SMASH1:
										m_pPlayer->Set_StopMotion(true, 75);
										SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
										m_pPlayer->Set_SpPoint(true);

										m_pAhglan->Set_Damage(PLAYER_SMASH1POWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_SMASH1POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
										break;

									case Engine::STATE_ATK1:
										m_pPlayer->Set_StopMotion(true, 40);
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
										Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
										break;

									case Engine::STATE_ATK2:
										m_pPlayer->Set_StopMotion(true, 40);
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
										Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
										break;

									case Engine::STATE_ATK3:
										m_pPlayer->Set_StopMotion(true, 40);
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
										Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
										break;

									case Engine::STATE_ATK4:
										m_pPlayer->Set_StopMotion(true, 40);
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
										Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
										break;

									case Engine::STATE_DASHATK:
										SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
										m_pPlayer->Set_SpPoint(false);

										m_pAhglan->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
										m_pPlayer->Compute_Critical();
										Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
										break;
									}

									m_pPlayer->Set_CanHit(false);

									break;
								}
							}
						}

						if (bPlayerAtkEnd)
							break;
					}
				}
			}
		}
	}

	else if (SCENE_STAGE_1 == m_eSceneID)
	{
		// Player 공격
		for (; iter_PlayerHit != mapPlayerHit.end(); ++iter_PlayerHit)
		{
			CLayer*	pLayer = Engine::Get_Layer(L"Enemies");
			if (!pLayer)
				return;

			map<const wstring, CGameObject*> mapObject = Engine::Get_MapObject(L"Enemies");
			map<const wstring, CGameObject*>::iterator	iter_Enemy = mapObject.begin();

			for (; iter_Enemy != mapObject.end(); ++iter_Enemy)
			{
				_bool	bAlreadyHit = false;

				map<const wstring, CBoxCollider*>	mapEnemyBoxCol = iter_Enemy->second->Get_MapBoxCollider();
				map<const wstring, CBoxCollider*>::iterator		iter_EnemyBoxCol = mapEnemyBoxCol.begin();

				for (; iter_EnemyBoxCol != mapEnemyBoxCol.end(); ++iter_EnemyBoxCol)
				{
					list<CBoxCollider*>::iterator	iter_AlreadyDamaged = m_listEnemyDamagedCol.begin();

					for (; iter_AlreadyDamaged != m_listEnemyDamagedCol.end(); ++iter_AlreadyDamaged)
					{
						if (iter_EnemyBoxCol->second == *iter_AlreadyDamaged)
						{
							bAlreadyHit = true;

							break;
						}
					}

					if (bAlreadyHit)
						break;

					if (iter_PlayerHit->second->Get_CanCollision() &&
						iter_EnemyBoxCol->second->Get_CanCollision())
					{
						if (Collision_OBB(&iter_PlayerHit->second->Get_Min(), &iter_PlayerHit->second->Get_Max(), iter_PlayerHit->second->Get_ColliderWorld(),
										  &iter_EnemyBoxCol->second->Get_Min(), &iter_EnemyBoxCol->second->Get_Max(), iter_EnemyBoxCol->second->Get_ColliderWorld()))
						{
							bPlayerAtkEnd = true;

							m_listEnemyDamagedCol.emplace_back(iter_EnemyBoxCol->second);

							switch (m_pPlayer->Get_CurState())
							{
							case Engine::STATE_SP_FEVER:
								iter_Enemy->second->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
								m_pPlayer->Add_Buff(BUFF_CRITICAL, 1500);
								SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);

								Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_FURY:
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
								m_pPlayer->Add_Buff(BUFF_CRITICAL, 1500);
								SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);

								Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_FURY2:
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);

								Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_DOUBLE_CRECSENT:
								SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
								m_pPlayer->Set_StopMotion(true, 100);
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_SMASH4:
								m_pPlayer->Set_StopMotion(true, 100);
								SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_SMASH3:
								m_pPlayer->Set_StopMotion(true, 100);
								SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_SMASH2_B:
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH2BPOWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH2BPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_SMASH2:
								m_pPlayer->Set_StopMotion(true, 100);
								//SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH2POWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH2POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_SMASH1:
								m_pPlayer->Set_StopMotion(true, 75);
								SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
								m_pPlayer->Set_SpPoint(true);

								iter_Enemy->second->Set_Damage(PLAYER_SMASH1POWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_SMASH1POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
								break;

							case Engine::STATE_ATK1:
								m_pPlayer->Set_StopMotion(true, 40);
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
								Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
								break;

							case Engine::STATE_ATK2:
								m_pPlayer->Set_StopMotion(true, 40);
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
								Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
								break;

							case Engine::STATE_ATK3:
								m_pPlayer->Set_StopMotion(true, 40);
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
								Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
								break;

							case Engine::STATE_ATK4:
								m_pPlayer->Set_StopMotion(true, 40);
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
								Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
								break;

							case Engine::STATE_DASHATK:
								SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
								m_pPlayer->Set_SpPoint(false);

								iter_Enemy->second->Set_Damage(PLAYER_ATKPOWER + iInterpolDamage);
								m_pPlayer->Compute_Critical();
								Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
								break;
							}

							break;
						}
					}
				}


				if (bPlayerAtkEnd)
					break;
			}
		}
	}
}

void CCollisionMgr::Collision_MonsterAttack()
{
	map<const wstring, CCollider*>					mapPlayerDamaged = m_pPlayer->Get_MapCollider();
	map<const wstring, CCollider*>::iterator		iter_PlayerDamaged = mapPlayerDamaged.begin();

	CTransform*		pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

	_bool	bEnemyAtkEnd = false;
	_int	iInterpolDamage = (_int)Engine::Random(-300.f, 300.f);

	if (SCENE_STAGE == m_eSceneID)
	{
		CTransform*		pAhglanTrans = static_cast<CTransform*>(m_pAhglan->Get_Component(L"Com_Transform", ID_DYNAMIC));

		if (!pAhglanTrans)
			return;

		map<const wstring, CCollider*>					mapBossSphereCol = m_pAhglan->Get_MapCollider();
		map<const wstring, CCollider*>::iterator		iter_BossHit = mapBossSphereCol.begin();

		if (DIS_MID >= D3DXVec3Length(&(*pPlayerTrans->Get_Info(INFO_POS) - *pAhglanTrans->Get_Info(INFO_POS))))
		{
			if (m_pAhglan)
			{
				mapBossSphereCol = m_pAhglan->Get_MapCollider();

				for (_uint i = 0; i < mapPlayerDamaged.size(); ++i)
				{
					iter_BossHit = mapBossSphereCol.begin();

					for (_uint j = 0; j < mapBossSphereCol.size(); ++j)
					{
						if (iter_PlayerDamaged->second->Get_CanCollision() &&
							iter_BossHit->second->Get_CanCollision())
						{
							if (Collision_Sphere(iter_PlayerDamaged->second->Get_Center(), iter_PlayerDamaged->second->Get_Radius() * SCALE_PLAYER,
												 iter_BossHit->second->Get_Center(), iter_BossHit->second->Get_Radius() * SCALE_AHGLAN))
							{
								bEnemyAtkEnd = true;

								_vec3 vLookDir = *pPlayerTrans->Get_Info(INFO_RIGHT);
								_vec3 vToBossDir = iter_BossHit->second->Get_Center() - *pPlayerTrans->Get_Info(INFO_POS);
								D3DXVec3Normalize(&vLookDir, &vLookDir);
								D3DXVec3Normalize(&vToBossDir, &vToBossDir);

								if (D3DXVec3Dot(&vToBossDir, &vLookDir) > 0.f)
								{
									m_pPlayer->Set_Damage(AHGLAN_ATKPOWER + iInterpolDamage, iter_PlayerDamaged->second->Get_ColliderWorld(), true);
								}
								else
								{
									m_pPlayer->Set_Damage(AHGLAN_ATKPOWER + iInterpolDamage, iter_PlayerDamaged->second->Get_ColliderWorld(), false);
								}

								break;
							}
						}

						++iter_BossHit;
					}

					++iter_PlayerDamaged;

					if (bEnemyAtkEnd)
						break;
				}
			}
		}
	}

	else if (SCENE_STAGE_1 == m_eSceneID)
	{
		CLayer*	pLayer = Engine::Get_Layer(L"Enemies");
		if (!pLayer)
			return;

		map<const wstring, CGameObject*> mapObject = Engine::Get_MapObject(L"Enemies");
		map<const wstring, CGameObject*>::iterator	iter_Enemy = mapObject.begin();

		for (; iter_Enemy != mapObject.end(); ++iter_Enemy)
		{
			if (POOLING_POS != *static_cast<CTransform*>(iter_Enemy->second->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_Info(INFO_POS))
			{
				map<const wstring, CCollider*>	mapEnemyCol = iter_Enemy->second->Get_MapCollider();
				map<const wstring, CCollider*>::iterator	iter_EnemyCol = mapEnemyCol.begin();

				for (; iter_EnemyCol != mapEnemyCol.end(); ++iter_EnemyCol)
				{
					iter_PlayerDamaged = mapPlayerDamaged.begin();

					for (; iter_PlayerDamaged != mapPlayerDamaged.end(); ++iter_PlayerDamaged)
					{
						if (iter_PlayerDamaged->second->Get_CanCollision() &&
							iter_EnemyCol->second->Get_CanCollision())
						{
							if (Collision_Sphere(iter_PlayerDamaged->second->Get_Center(), iter_PlayerDamaged->second->Get_Radius() * SCALE_PLAYER,
								iter_EnemyCol->second->Get_Center(), iter_EnemyCol->second->Get_Radius() * SCALE_MANKIND))
							{
								bEnemyAtkEnd = true;

								_vec3 vLookDir = *pPlayerTrans->Get_Info(INFO_RIGHT);
								_vec3 vToEnemyDir = iter_EnemyCol->second->Get_Center() - *pPlayerTrans->Get_Info(INFO_POS);
								D3DXVec3Normalize(&vLookDir, &vLookDir);
								D3DXVec3Normalize(&vToEnemyDir, &vToEnemyDir);

								if (D3DXVec3Dot(&vToEnemyDir, &vLookDir) > 0.f)
								{
									m_pPlayer->Set_Damage(MANKIND_ATKPOWER + iInterpolDamage, iter_PlayerDamaged->second->Get_ColliderWorld(), true);
								}
								else
								{
									m_pPlayer->Set_Damage(MANKIND_ATKPOWER + iInterpolDamage, iter_PlayerDamaged->second->Get_ColliderWorld(), false);
								}

								break;
							}
						}
					}

					if (bEnemyAtkEnd)
						break;
				}
			}
		}
	}
}

void CCollisionMgr::Update_MultipleCollision()
{
	if (m_pPlayer)
	{
		m_ePlayerCurState = m_pPlayer->Get_CurState();

		if (m_ePlayerPreState != m_ePlayerCurState)
		{
			m_listEnemyDamagedCol.clear();

			m_ePlayerPreState = m_ePlayerCurState;
		}
	}
}

_bool CCollisionMgr::Collision_Sphere(const _vec3 & vDstCenter, const _float & fDstRadius, const _vec3 & vSrcCenter, const _float & fSrcRadius)
{
	_float fRadius = fDstRadius + fSrcRadius;
	_float fCenterDis = D3DXVec3Length(&(vDstCenter - vSrcCenter));

	if ((fDstRadius + fSrcRadius) < D3DXVec3Length(&(vDstCenter - vSrcCenter)))
		return false;

	return true;
}

_bool CCollisionMgr::Collision_AABB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	_vec3	vDestMin, vDestMax, vSourMin, vSourMax;
	_float	fDest, fSour;

	D3DXVec3TransformCoord(&vDestMin, pDestMin, pDestWorld);
	D3DXVec3TransformCoord(&vDestMax, pDestMax, pDestWorld);

	D3DXVec3TransformCoord(&vSourMin, pSourMin, pSourWorld);
	D3DXVec3TransformCoord(&vSourMax, pSourMax, pSourWorld);

	// x축에서 바라봤을 때

	fDest = max(vDestMin.x, vSourMin.x);
	fSour = min(vDestMax.x, vSourMax.x);

	if (fSour < fDest)
		return false;

	// y축에서 바라봤을 때

	fDest = max(vDestMin.y, vSourMin.y);
	fSour = min(vDestMax.y, vSourMax.y);

	if (fSour < fDest)
		return false;

	// z축에서 바라봤을 때

	fDest = max(vDestMin.z, vSourMin.z);
	fSour = min(vDestMax.z, vSourMax.z);

	if (fSour < fDest)
		return false;

	return true;
}

_bool CCollisionMgr::Collision_OBB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	OBB		tObb[2];
	ZeroMemory(tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], pDestMin, pDestMax);
	Set_Point(&tObb[1], pSourMin, pSourMax);

	for (_ulong i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], pSourWorld);
	}

	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, pSourWorld);

	for (_ulong i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);

	_float		fDistance[3];	// 0번 ; 첫번째 박스에서 각 벡터를 임의의 축으로 투영시킨 길이의 합
								// 1번 ; 두번째 박스에서 각 벡터를 임의의 축으로 투영시킨 길이의 합
								// 2번 ; 박스와 박스의 중점 간의 거리를 임의의 축으로 투영시킨 길이의 합		
	for (_ulong i = 0; i < 2; ++i)
	{
		for (_ulong j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[0].vCenter - tObb[1].vCenter), &tObb[i].vAxis[j]));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;
		}
	}

	return true;
}

void CCollisionMgr::Set_Point(OBB * pObb, const _vec3 * pMin, const _vec3 * pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	// 뒷면
	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f;
}

void CCollisionMgr::Set_Axis(OBB * pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_ulong i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

void CCollisionMgr::Pooling_DamageFont(_uint iDamage, const _matrix * pMatrix, DAMAGEFONT_ID eID)
{
	_uint	iDigitThousands = _uint((iDamage % 10000) * 0.001f);
	_uint	iDigitHundreds = _uint((iDamage % 1000) * 0.01f);
	_uint	iDigitTen = _uint((iDamage % 100) * 0.1f);
	_uint	iDigitOne = _uint(iDamage % 10);

	CDamageFont*	pFont0 = nullptr;
	CDamageFont*	pFont1 = nullptr;
	CDamageFont*	pFont2 = nullptr;
	CDamageFont*	pFont3 = nullptr;

	CTransform*		pTransform = nullptr;

	CDynamicCamera*		pMainCam = static_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Environment", L"DynamicCamera"));

	_vec3	vCamRight = pMainCam->Get_CamDirVector(DIR_RIGHT);

	_vec3	vPos;
	vPos.x = pMatrix->_41 + Engine::Random(-0.5f, 0.5f);
	if (0.5f > pMatrix->_42)
	{
		vPos.y = 0.5f;
	}
	else
	{
		vPos.y = pMatrix->_42;
	}
	vPos.z = pMatrix->_43 + Engine::Random(-0.5f, 0.5f);

	wstring		wstrNormalFont = L"DamageFont_Normal_UI_";
	wstring		wstrSkillFont = L"DamageFont_Skill_UI_";
	wstring		wstrMonsterFont = L"DamageFont_Monster_UI_";

	switch (eID)
	{
	case Engine::DAMAGEFONT_NORMAL:
		for (_uint i = 0; i < DAMAGEFONT_COUNT; ++i)
		{
			wstrNormalFont = L"DamageFont_Normal_UI_";
			wstrNormalFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrNormalFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;

	case Engine::DAMAGEFONT_SKILL:
		for (_uint i = 0; i < DAMAGEFONT_SKILL_COUNT; ++i)
		{
			wstrSkillFont = L"DamageFont_Skill_UI_";
			wstrSkillFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrSkillFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;

	case Engine::DAMAGEFONT_MONSTER:
		for (_uint i = 0; i < DAMAGEFONT_MONSTER_COUNT; ++i)
		{
			wstrMonsterFont = L"DamageFont_Monster_UI_";
			wstrMonsterFont += to_wstring(i);

			pTransform = static_cast<CTransform*>(Engine::Get_Component(L"UI", wstrMonsterFont, L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					break;
				}

				continue;
			}
		}
		break;
	}

	if (1000 <= iDamage)
	{
		if (pFont0 && pFont1 && pFont2 && pFont3)
		{
			pFont0->Set_EnableDamageFont(vPos, iDigitThousands, 1000);
			pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (100 <= iDamage)
	{
		if (pFont1 && pFont2 && pFont3)
		{
			pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (10 <= iDamage)
	{
		if (pFont2 && pFont3)
		{
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (1 <= iDamage)
	{
		if (pFont3)
		{
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
}


void CCollisionMgr::Free(void)
{
}
