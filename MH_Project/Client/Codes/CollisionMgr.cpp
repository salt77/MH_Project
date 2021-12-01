#include "stdafx.h"
#include "CollisionMgr.h"

#include "Logo.h"
#include "Stage.h"
#include "Stage_1.h"

#include "Player.h"
#include "Ahglan.h"
#include "StickyBomb.h"
#include "DamageFont.h"
#include "SlashPoint.h"
#include "CriticalEfx.h"
#include "DynamicCamera.h"
#include "Wall_Collision.h"
#include "Balista.h"
#include "Box.h"
#include "LastRoom_Trigger.h"
#include "Wall_Symbol.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}


HRESULT CCollisionMgr::Ready_CollisionMgr()
{
	m_eSceneID = Engine::Get_SceneID();

	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	m_pPlayerTrans = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	switch (m_eSceneID)
	{
	case Engine::SCENE_STAGE:
		m_pAhglan = dynamic_cast<CAhglan*>(Engine::Get_GameObject(L"GameLogic", L"Ahglan"));
		NULL_CHECK_RETURN(m_pAhglan, E_FAIL);
		break;

	case Engine::SCENE_STAGE_1:
		m_pWall = dynamic_cast<CWall_Collision*>(Engine::Get_GameObject(L"GameLogic", L"Wall_Collision"));
		NULL_CHECK_RETURN(m_pWall, E_FAIL);
		break;
	}

	return S_OK;
}

_uint CCollisionMgr::Update_CollisionMgr(const _float& fTimeDelta)
{
	if (m_bCanCollision)
	{
		Update_MultipleCollision();

		Collision_PlayerAttack();
		Collision_MonsterAttack();
		Collision_Balista_Stage();
		Collision_Balista_Stage_1();
		Collision_Trigger();
		Collision_Box_Player_Stage_1(fTimeDelta);
	}

	CollisionMgr_FrameStart();

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
		if (!m_bMainBossEnd)
		{
			if (m_pAhglan->Get_DissoveOn())
			{
				m_bMainBossEnd = true;
			}

			if (m_pAhglan)
			{
				map<const wstring, CBoxCollider*>				mapBossBoxCol = m_pAhglan->Get_MapBoxCollider();
				map<const wstring, CBoxCollider*>::iterator		iter_BossDamaged = mapBossBoxCol.begin();

				CTransform*		pAhglanTrans = static_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));
				CCriticalEfx*	pEfx = nullptr;

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

								_bool	bAlreadyHit = false;

								for (; iter_BossDamaged != mapBossBoxCol.end(); ++iter_BossDamaged)
								{
									if (iter_PlayerHit->second->Get_CanCollision() &&
										iter_BossDamaged->second->Get_CanCollision())
									{
										if (Engine::STATE_SMASH4_B == m_pPlayer->Get_CurState())
										{
											list<CBoxCollider*>::iterator	iter_AlreadyDamaged = m_listEnemyDamagedCol.begin();

											for (; iter_AlreadyDamaged != m_listEnemyDamagedCol.end(); ++iter_AlreadyDamaged)
											{
												if (iter_BossDamaged->second == *iter_AlreadyDamaged)
												{
													bAlreadyHit = true;
													break;
												}
											}
											if (!bAlreadyHit)
											{
												if (DIS_SMASH4B >= D3DXVec3Length(&(*pPlayerTrans->Get_Info(INFO_POS) - *(pAhglanTrans->Get_Info(INFO_POS)))))
												{
													m_pPlayer->Set_SpPoint(true);
													m_pPlayer->Set_StopMotion(true, 100);

													m_pAhglan->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
													m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
													Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);

													bPlayerAtkEnd = true;
													m_listEnemyDamagedCol.emplace_back(iter_BossDamaged->second);
													break;
												}
											}
										}

										if (Collision_OBB(&iter_PlayerHit->second->Get_Min(), &iter_PlayerHit->second->Get_Max(), iter_PlayerHit->second->Get_ColliderWorld(),
											&iter_BossDamaged->second->Get_Min(), &iter_BossDamaged->second->Get_Max(), iter_BossDamaged->second->Get_ColliderWorld()))
										{
											bPlayerAtkEnd = true;

											switch (m_pPlayer->Get_CurState())
											{
											case Engine::STATE_SP_FEVER:
												m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);

												m_pPlayer->Add_Buff(BUFF_CRITICAL, 1500);
												pEfx = static_cast<CCriticalEfx*>(Engine::Get_GameObject(L"Effect", L"Efx_Critical"));
												if (pEfx)
												{
													_vec3	vPos;
													memcpy(&vPos, &iter_BossDamaged->second->Get_ColliderWorld()->_41, sizeof(_vec3));
													pEfx->Set_EnableCriticalEfx(vPos);
												}
												SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);

												Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_FURY_NO1:
												m_pPlayer->Set_SpPoint(false);

												m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
												Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_FURY_NO2:
												m_pPlayer->Set_SpPoint(false);

												m_pAhglan->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
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
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_SMASH4:
												m_pPlayer->Set_StopMotion(true, 100);
												SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
												m_pPlayer->Set_SpPoint(true);

												m_pAhglan->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_SMASH3:
												m_pPlayer->Set_StopMotion(true, 100);
												SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
												m_pPlayer->Set_SpPoint(true);

												m_pAhglan->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_SMASH2_B:
												SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
												m_pPlayer->Set_SpPoint(false);

												m_pAhglan->Set_Damage(PLAYER_SMASH2BPOWER + iInterpolDamage);
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_SMASH2BPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_SMASH2:
												m_pPlayer->Set_StopMotion(true, 100);
												//SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
												m_pPlayer->Set_SpPoint(true);

												m_pAhglan->Set_Damage(PLAYER_SMASH2POWER + iInterpolDamage);
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_SMASH2POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
												break;

											case Engine::STATE_SMASH1:
												m_pPlayer->Set_StopMotion(true, 75);
												SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
												m_pPlayer->Set_SpPoint(true);

												m_pAhglan->Set_Damage(PLAYER_SMASH1POWER + iInterpolDamage);
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
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
												m_pPlayer->Compute_Critical(iter_BossDamaged->second->Get_ColliderWorld());
												Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
												break;
											}

											if (PL_ATK == m_pPlayer->Get_CurAction())
											{
												Pooling_SlashPoint(iter_PlayerHit->second->Get_ColliderWorld());
											}
											else if (PL_SMASH == m_pPlayer->Get_CurAction() ||
												PL_SKILL == m_pPlayer->Get_CurAction())
											{
												Pooling_SlashPoint(iter_PlayerHit->second->Get_ColliderWorld(), true);
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
		}
	}

	else if (SCENE_STAGE_1 == m_eSceneID)
	{
		CCriticalEfx*	pEfx = nullptr;

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
				if (0 < iter_Enemy->second->Get_TagInfo().iHp)
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
							if (Engine::STATE_SMASH4_B == m_pPlayer->Get_CurState())
							{
								CTransform*	pEnemyTrans = static_cast<CTransform*>(iter_Enemy->second->Get_Component(L"Com_Transform", ID_DYNAMIC));

								if (DIS_SMASH4B >= D3DXVec3Length(&(*pPlayerTrans->Get_Info(INFO_POS) - *(pEnemyTrans->Get_Info(INFO_POS)))))
								{
									m_pPlayer->Set_SpPoint(true);
									m_pPlayer->Set_StopMotion(true, 100);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);

									bPlayerAtkEnd = true;
									m_listEnemyDamagedCol.emplace_back(iter_EnemyBoxCol->second);
									break;
								}
							}

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
									pEfx = static_cast<CCriticalEfx*>(Engine::Get_GameObject(L"Effect", L"Efx_Critical"));
									if (pEfx)
									{
										_vec3	vPos;
										memcpy(&vPos, &iter_EnemyBoxCol->second->Get_ColliderWorld()->_41, sizeof(_vec3));
										pEfx->Set_EnableCriticalEfx(vPos);
									}

									SoundMgrLowerVol(L"hit_common_critical.wav", CSoundMgr::BATTLE, 0.3f);
									Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_FURY_NO1:
									m_pPlayer->Set_SpPoint(false);

									iter_Enemy->second->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
									Pooling_DamageFont(PLAYER_SMASHPOWER2 + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_FURY_NO2:
									m_pPlayer->Set_SpPoint(false);

									iter_Enemy->second->Set_Damage(PLAYER_SMASHPOWER2 + iInterpolDamage);
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
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_SMASH4:
									m_pPlayer->Set_StopMotion(true, 100);
									SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
									m_pPlayer->Set_SpPoint(true);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH4POWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH4POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_SMASH3:
									m_pPlayer->Set_StopMotion(true, 100);
									SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
									m_pPlayer->Set_SpPoint(true);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH3POWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH3POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_SMASH2_B:
									SoundMgrLowerVol(L"Hit_Flesh_Slash.wav", CSoundMgr::PLAYER_EFFECT, 0.05f);
									m_pPlayer->Set_SpPoint(false);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH2BPOWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH2BPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_SMASH2:
									m_pPlayer->Set_StopMotion(true, 100);
									//SoundMgrLowerVol(L"Hit_HardFlesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
									m_pPlayer->Set_SpPoint(true);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH2POWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_SMASH2POWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);
									break;

								case Engine::STATE_SMASH1:
									m_pPlayer->Set_StopMotion(true, 75);
									SoundMgrLowerVol(L"Hit_Flesh_StrongSlash.wav", CSoundMgr::PLAYER_EFFECT, 0.07f);
									m_pPlayer->Set_SpPoint(true);

									iter_Enemy->second->Set_Damage(PLAYER_SMASH1POWER + iInterpolDamage);
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
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
									m_pPlayer->Compute_Critical(iter_EnemyBoxCol->second->Get_ColliderWorld());
									Pooling_DamageFont(PLAYER_ATKPOWER + iInterpolDamage, iter_PlayerHit->second->Get_ColliderWorld(), DAMAGEFONT_NORMAL);
									break;
								}

								if (PL_ATK == m_pPlayer->Get_CurAction())
								{
									Pooling_SlashPoint(iter_PlayerHit->second->Get_ColliderWorld());
								}
								else if (PL_SMASH == m_pPlayer->Get_CurAction() ||
										 PL_SKILL == m_pPlayer->Get_CurAction())
								{
									Pooling_SlashPoint(iter_PlayerHit->second->Get_ColliderWorld(), true);
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
		if (!m_bMainBossEnd)
		{
			if (m_pAhglan->Get_DissoveOn())
			{
				m_bMainBossEnd = true;
			}

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

void CCollisionMgr::Collision_Balista_Stage()
{
	if (SCENE_STAGE == m_eSceneID)
	{
		if (!m_bMainBossEnd)
		{
			if (m_pAhglan->Get_DissoveOn())
			{
				m_bMainBossEnd = true;
			}

			map<const wstring, CGameObject*>	mapObject = Engine::Get_MapObject(L"Balista");
			map<const wstring, CGameObject*>::iterator		iter_Balista = mapObject.begin();

			for (; iter_Balista != mapObject.end(); ++iter_Balista)
			{
				CBalista*	pBalista = static_cast<CBalista*>(iter_Balista->second);
				CTransform*	pBaliTrans = static_cast<CTransform*>(pBalista->Get_Component(L"Com_Transform", ID_DYNAMIC));

				if (!pBalista->Get_AlreadyEnemyHit() &&
					POOLING_POS != *pBaliTrans->Get_Info(INFO_POS))
				{
					map<const wstring, CBoxCollider*>	iter_BalistaCol = iter_Balista->second->Get_MapBoxCollider();

					map<const wstring, CBoxCollider*>	mapAhglanCol = m_pAhglan->Get_MapBoxCollider();
					map<const wstring, CBoxCollider*>::iterator		iter_AhglanCol = mapAhglanCol.begin();

					for (; iter_AhglanCol != mapAhglanCol.end(); ++iter_AhglanCol)
					{
						if (Collision_OBB(&iter_BalistaCol.begin()->second->Get_Min(), &iter_BalistaCol.begin()->second->Get_Max(), iter_BalistaCol.begin()->second->Get_ColliderWorld(),
							&iter_AhglanCol->second->Get_Min(), &iter_AhglanCol->second->Get_Max(), iter_AhglanCol->second->Get_ColliderWorld()))
						{
							pBalista->Set_EnemyHit();

							m_pAhglan->Set_Damage(STAGE_BALISTA_POWER);
							Pooling_DamageFont(STAGE_BALISTA_POWER, iter_AhglanCol->second->Get_ColliderWorld(), DAMAGEFONT_SKILL);

							break;
						}
					}
				}
			}
		}
	}
}

void CCollisionMgr::Collision_Balista_Stage_1()
{
	if (SCENE_STAGE_1 == m_eSceneID)
	{
		CTransform*		pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC));

		map<const wstring, CBoxCollider*>	mapWallCollider = m_pWall->Get_MapBoxCollider();
		map<const wstring, CBoxCollider*>::iterator		iter_WallBox = mapWallCollider.begin();

		map<const wstring, CGameObject*> mapObject = Engine::Get_MapObject(L"Balista");
		map<const wstring, CGameObject*>::iterator	iter_Balista = mapObject.begin();

		map<const wstring, CGameObject*> mapBoxObject = Engine::Get_MapObject(L"Box");
		map<const wstring, CGameObject*>::iterator	iter_Box = mapBoxObject.begin();

		// Wall - Balista 충돌 
		for (; iter_Balista != mapObject.end(); ++iter_Balista)
		{
			CBalista*	pBalista = static_cast<CBalista*>(iter_Balista->second);
			iter_Box = mapBoxObject.begin();

			if (!pBalista->Get_AlreadyCollision())
			{
				map<const wstring, CBoxCollider*>	mapBalistaCol = pBalista->Get_MapBoxCollider();

				if (Collision_AABB(&iter_WallBox->second->Get_Min(), &iter_WallBox->second->Get_Max(), iter_WallBox->second->Get_ColliderWorld(),
								   &mapBalistaCol.begin()->second->Get_Min(), &mapBalistaCol.begin()->second->Get_Max(), mapBalistaCol.begin()->second->Get_ColliderWorld()))
				{
					pBalista->Set_CollisionWall();
					pBalista->Set_EnemyHit();

					if (pPlayerTrans)
					{
						_vec3	vPlayerPos = *pPlayerTrans->Get_Info(INFO_POS);
						_vec3	vWallPos = *static_cast<CTransform*>(m_pWall->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_Info(INFO_POS);

						if (DIS_LONG >= D3DXVec3Length(&(vPlayerPos - vWallPos)))
						{
							SoundMgrLowerVol(L"Hit_Flesh_Stab.wav", CSoundMgr::BATTLE, 0.1f / D3DXVec3Length(&(vPlayerPos - vWallPos)));
						}
					}
				}

				// Box - Balista 충돌 
				for (; iter_Box != mapBoxObject.end(); ++iter_Box)
				{
					CBox*	pBox = static_cast<CBox*>(iter_Box->second);

					map<const wstring, CBoxCollider*>	mapBoxCol = pBox->Get_MapBoxCollider();

					if (Collision_AABB(&mapBalistaCol.begin()->second->Get_Min(), &mapBalistaCol.begin()->second->Get_Max(), mapBalistaCol.begin()->second->Get_ColliderWorld(),
									   &mapBoxCol.begin()->second->Get_Min(), &mapBoxCol.begin()->second->Get_Max(), mapBoxCol.begin()->second->Get_ColliderWorld()))
					{
						pBalista->Set_CollisionWall();
						pBalista->Set_EnemyHit();
						pBalista->Set_StickBox(*mapBoxCol.begin()->second->Get_ColliderWorld());
						pBalista->Set_ReturnPoolingPos();

						if (pPlayerTrans)
						{
							_vec3	vPlayerPos = *pPlayerTrans->Get_Info(INFO_POS);
							_vec3	vBoxPos = *static_cast<CTransform*>(pBox->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_Info(INFO_POS);

							if (DIS_LONG >= D3DXVec3Length(&(vPlayerPos - vBoxPos)))
							{
								SoundMgrLowerVol(L"Hit_Flesh_Stab.wav", CSoundMgr::BATTLE, 0.1f / D3DXVec3Length(&(vPlayerPos - vBoxPos)));
							}
						}
					}
				}
			}
		}

		// Player - Balista 충돌 
		if (pPlayerTrans)
		{
			_bool	bPlayerAttacked = false;

			_vec3	vPlayerPos = *pPlayerTrans->Get_Info(INFO_POS);

			map<const wstring, CCollider*>					mapPlayerDamaged = m_pPlayer->Get_MapCollider();
			map<const wstring, CCollider*>::iterator		iter_PlayerDamaged = mapPlayerDamaged.begin();

			for (; iter_PlayerDamaged != mapPlayerDamaged.end(); ++iter_PlayerDamaged)
			{
				iter_Balista = mapObject.begin();

				for (; iter_Balista != mapObject.end(); ++iter_Balista)
				{
					CBalista*	pBalista = static_cast<CBalista*>(iter_Balista->second);

					if (pBalista)
					{
						if (!pBalista->Get_AlreadyEnemyHit())
						{
							if (DIS_MID >= D3DXVec3Length(&(vPlayerPos - *static_cast<CTransform*>(pBalista->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_Info(INFO_POS))))
							{
								map<const wstring, CCollider*>	mapBalistaCol = pBalista->Get_MapCollider();

								if (Collision_Sphere(iter_PlayerDamaged->second->Get_Center(), iter_PlayerDamaged->second->Get_Radius() * SCALE_PLAYER,
									mapBalistaCol.begin()->second->Get_Center(), mapBalistaCol.begin()->second->Get_Radius() * SCALE_BALISTA))
								{
									bPlayerAttacked = true;
									pBalista->Set_EnemyHit();
									m_pPlayer->Set_Damage(AHGLAN_ATKPOWER, iter_PlayerDamaged->second->Get_ColliderWorld());

									break;
								}
							}
						}
					}
				}

				if (bPlayerAttacked)
					break;
			}
		}
	}
}

void CCollisionMgr::Collision_Trigger()
{
	if (SCENE_STAGE_1 == m_eSceneID)
	{
		if (m_pPlayer)
		{
			// LastRoom Trigger Collision 
			map<const wstring, CGameObject*>	mapTriggerObj = Engine::Get_MapObject(L"Trigger");
			map<const wstring, CGameObject*>	mapPortalObj = Engine::Get_MapObject(L"Symbol");

			CWall_Symbol*	pSymbol = nullptr;

			if (0 < mapPortalObj.size())
			{
				pSymbol = static_cast<CWall_Symbol*>(mapPortalObj.begin()->second);
			}

			if (0 < mapTriggerObj.size())
			{
				map<const wstring, CCollider*>		mapTriggerCol = mapTriggerObj.begin()->second->Get_MapCollider();

				map<const wstring, CCollider*>	mapPlayerCol = m_pPlayer->Get_MapCollider();
				map<const wstring, CCollider*>::iterator	iter_PlayerCol = mapPlayerCol.begin();

				for (; iter_PlayerCol != mapPlayerCol.end(); ++iter_PlayerCol)
				{
					if (Collision_Sphere(mapTriggerCol.begin()->second->Get_Center(), mapTriggerCol.begin()->second->Get_Radius(),
										 iter_PlayerCol->second->Get_Center(), iter_PlayerCol->second->Get_Radius() * SCALE_PLAYER))
					{
						mapTriggerObj.begin()->second->Set_Dead();
						pSymbol->Set_ReadyOpenPortal();

						break;
					}
				}
			}

			// NextStage Portal Open Trigger 
			if (0 < mapPortalObj.size())
			{
				if (pSymbol->Get_ReadyToOpenPortal())
				{
					_bool	bAllClear = true;

					map<const wstring, CGameObject*>	mapRemainEnemy = Engine::Get_MapObject(L"Enemies");
					map<const wstring, CGameObject*>::iterator		iter_Enemy = mapRemainEnemy.begin();

					for (; iter_Enemy != mapRemainEnemy.end(); ++iter_Enemy)
					{
						if (0 < iter_Enemy->second->Get_TagInfo().iHp)
						{
							bAllClear = false;

							break;
						}
					}

					if (bAllClear)
						pSymbol->Set_NextStagePortal();
				}
			}

			// NextStage Portal Collision 
			if (0 < mapPortalObj.size())
			{
				if (pSymbol->Get_CanNextStageOpen())
				{
					map<const wstring, CBoxCollider*>	mapPortalCol = pSymbol->Get_MapBoxCollider();

					map<const wstring, CBoxCollider*>	mapPlayerCol = m_pPlayer->Get_MapBoxCollider();
					map<const wstring, CBoxCollider*>::iterator		iter_PlayerCol = mapPlayerCol.begin();

					if (!m_bNextStageLoad)
					{
						for (; iter_PlayerCol != mapPlayerCol.end(); ++iter_PlayerCol)
						{
							if (Collision_OBB(&mapPortalCol.begin()->second->Get_Min(), &mapPortalCol.begin()->second->Get_Max(), mapPortalCol.begin()->second->Get_ColliderWorld(),
											  &iter_PlayerCol->second->Get_Min(), &iter_PlayerCol->second->Get_Max(), iter_PlayerCol->second->Get_ColliderWorld()))
							{
								m_bNextStageLoad = true;

								static_cast<CStage_1*>(Engine::Get_CurrentScenePointer())->Set_ReadyLoadStage();
							}
						}
					}
				}
			}
		}
	}
}

void CCollisionMgr::Collision_Box_Player_Stage_1(const _float& fTimeDelta)
{
	if (SCENE_STAGE_1 == m_eSceneID)
	{
		if (m_pPlayer)
		{
			map<const wstring, CGameObject*>	mapBoxObj = Engine::Get_MapObject(L"Box");
			map<const wstring, CGameObject*>::iterator		iter_Box = mapBoxObj.begin();

			for (; iter_Box != mapBoxObj.end(); ++iter_Box)
			{
				CBox*	pBox = static_cast<CBox*>(iter_Box->second);

				map<const wstring, CCollider*>	mapPlayerCol = m_pPlayer->Get_MapCollider();
				map<const wstring, CCollider*>::iterator	iter_PlayerCol = mapPlayerCol.begin();

				map<const wstring, CCollider*>	mapBoxCol = pBox->Get_MapCollider();
				map<const wstring, CCollider*>::iterator	iter_BoxObjCol = mapBoxCol.begin();

				for (; iter_BoxObjCol != mapBoxCol.end(); ++iter_BoxObjCol)
				{
					for (; iter_PlayerCol != mapPlayerCol.end(); ++iter_PlayerCol)
					{
						if (L"Body" == iter_PlayerCol->first)
						{
							if (Collision_Sphere(iter_PlayerCol->second->Get_Center(), iter_PlayerCol->second->Get_Radius() * SCALE_PLAYER,
								iter_BoxObjCol->second->Get_Center(), iter_BoxObjCol->second->Get_Radius() * SCALE_NORMAL * 1.6f))
							{
								m_pPlayer->Set_PushState(true);
								pBox->MoveOn_NaviMesh(_vec3(0.f, 0.f, 1.f), m_pPlayer->Get_PlayerSpeed(), fTimeDelta);
							}
							else
							{
								m_pPlayer->Set_PushState(false);
							}
						}
					}
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

void CCollisionMgr::CollisionMgr_FrameStart()
{
	if (!m_bCollisionFrame)
	{
		m_bCollisionFrame = true;
		m_bCanCollision = true;
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
	_uint	iDigitTenThousands = _uint((iDamage % 100000) * 0.0001f);
	_uint	iDigitThousands = _uint((iDamage % 10000) * 0.001f);
	_uint	iDigitHundreds = _uint((iDamage % 1000) * 0.01f);
	_uint	iDigitTen = _uint((iDamage % 100) * 0.1f);
	_uint	iDigitOne = _uint(iDamage % 10);

	CDamageFont*	pFontFirst = nullptr;
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
				if (!pFontFirst)
				{
					pFontFirst = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
				}
				else if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrNormalFont));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
					break;
				}

				pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));

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
				if (!pFontFirst)
				{
					pFontFirst = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
				}
				else if (!pFont0)
				{
					pFont0 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrSkillFont));
				}
				else if (pFont0 && pFont1 && pFont2 && pFont3)
				{
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
					break;
				}

				pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));

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
				}
				else if (!pFont1)
				{
					pFont1 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
				}
				else if (!pFont2)
				{
					pFont2 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
				}
				else if (!pFont3)
				{
					pFont3 = dynamic_cast<CDamageFont*>(Engine::Get_GameObject(L"UI", wstrMonsterFont));
				}
				else if (pFontFirst && pFont0 && pFont1 && pFont2 && pFont3)
				{
					pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));
					break;
				}

				pTransform->Set_Pos(&(POOLING_POS + POOLING_POS));

				continue;
			}
		}
		break;
	}

	if (10000 <= iDamage)
	{
		if (pFontFirst && pFont0 && pFont1 && pFont2 && pFont3)
		{
			pFontFirst->Set_EnableDamageFont(vPos, iDigitTenThousands, 10000);
			pFont0->Set_EnableDamageFont(vPos, iDigitThousands, 1000);
			pFont1->Set_EnableDamageFont(vPos, iDigitHundreds, 100);
			pFont2->Set_EnableDamageFont(vPos, iDigitTen, 10);
			pFont3->Set_EnableDamageFont(vPos, iDigitOne, 1);
		}
	}
	else if (1000 <= iDamage)
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

void CCollisionMgr::Pooling_SlashPoint(const _matrix * pMatrix, _bool bIsSmash)
{
	_vec3	vPos;
	memcpy(&vPos, &pMatrix->_41, sizeof(_vec3));

	for (_uint i = 0; i < SLASHPOINT_COUNT; ++i)
	{
		wstring	wstrName = L"Efx_SlashPoint_";
		wstrName += to_wstring(i);

		CTransform*	pTransform = static_cast<CTransform*>(Engine::Get_Component(L"Effect", wstrName, L"Com_Transform", ID_DYNAMIC));

		if (pTransform)
		{
			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				static_cast<CSlashPoint*>(Engine::Get_GameObject(L"Effect", wstrName))->Set_EnableSlashPoint(vPos, bIsSmash);

				break;
			}
		}
	}
}


void CCollisionMgr::Free()
{
}
