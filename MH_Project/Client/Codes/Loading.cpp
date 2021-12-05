#include "stdafx.h"
#include "Loading.h"

#include "Export_Function.h"
#include "DynamicMesh.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(_tchar) * 256);
}

CLoading::~CLoading(void)
{
}


unsigned int CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	_uint		iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_STAGE_1:
		iFlag = pLoading->Loading_ForStage_1();
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	//_endthreadex(0);

	return iFlag;
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoading = eLoading;

	return 0;
}

Engine::_uint CLoading::Loading_ForStage()
{
	lstrcpy(m_szLoading, L"Loading Buffer..");

	Loading_ForBufferAll();

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 64, 48)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex2", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 1)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex3", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 2)), E_FAIL);


	lstrcpy(m_szLoading, L"Loading Texture..");

	Loading_ForTextureAll();

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel_%d.tga", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain2", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel2_%d.tga", TEX_NORMAL, 2)), E_FAIL);

	// Ahglan 등장 이후 소개 폰트
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Ahglan_Bottom_FontUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/golem_ahglan_font_bottom.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Ahglan_Bottom_FontUI2", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/ortel_font_bottom.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Ahglan_FontUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/golem_ahglan_font.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Ahglan_QuestUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/quest_golem_ahglan_target.tga", TEX_NORMAL)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Etc..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);

	//FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev)), E_FAIL);
	// FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Collider", CCollider::Create(m_pGraphicDev, nullptr, 0, 0)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Stage..");

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Stage", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Stage/", L"Stage.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Player..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Player", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Lethita/", L"Lethita.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Object..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Ahglan", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Ahglan/", L"Ahglan.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_StickyBomb", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Prop/Fragment/", L"Fragment.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Balista", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Prop/Balista/", L"Balista.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Complete!");

	m_bFinish = true;

	return 0;
}

_uint CLoading::Loading_ForStage_1()
{
	lstrcpy(m_szLoading, L"Loading Buffer..");

	Loading_ForBufferAll();
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 128, 164)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Texture..");

	Loading_ForTextureAll();

	//FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel_%d.tga", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel3_%d.tga", TEX_NORMAL, 2)), E_FAIL);

	// Boss 등장 이후 소개 폰트
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Cloyan_FontUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/Cloyan_Font_Name.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Cloyan_Bottom_FontUI2", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/Ortel_Font_Name_2.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Cloyan_QuestUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/quest_mankind_cloyan_target.tga", TEX_NORMAL)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Etc..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Stage..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Stage_1", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Stage_1/", L"Stage_1.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Player..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Player", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Lethita/", L"Lethita.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Object..");
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Cloyan", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Mankind/Cloyan/", L"Cloyan.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Dog", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Mankind/Dog/", L"Dog.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Soldier", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Mankind/Soldier/", L"Soldier.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Knight", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Mankind/Knight/", L"Knight.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Trap", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Trap/", L"Trap.X")), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Balista", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Prop/Balista/", L"Balista.X")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Mesh_Box", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Prop/Box/", L"Box.X")), E_FAIL);

	lstrcpy(m_szLoading, L"Loading Complete!");

	m_bFinish = true;

	return 0;
}

_uint CLoading::Loading_ForBufferAll()
{
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_Trail", CTrail::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_SmashTrail", CSmashTrail::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_BalistaTrail", CBalistaTrail::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Buffer_CubeTex", CCubeTex::Create(m_pGraphicDev)), E_FAIL);
	return 0;
}

_uint CLoading::Loading_ForTextureAll()
{
	// SkyBox 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_SkyBox", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/SkyBox0.dds", TEX_CUBE)), E_FAIL);

	// Trail 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_SwordTrail", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/heroes_longblade_trail02.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_CloyanTrail", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/trail_cloyan.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_BalistaTrail", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/heroes_longblade_trail02.tga", TEX_NORMAL)), E_FAIL);

	// Player UI
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Hpbar_BackUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Hpbar_Back.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Hpbar_LerpUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Hpbar_Lerp.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Hpbar_ValueUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Hpbar_Value.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Steminabar_BackUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Steminabar_Back.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Steminabar_ValueUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Steminabar_Value.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Spbar_BackUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Spbar_Back.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_Spbar_ValueUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Player_Spbar_Value.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Player_SlotUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Slot.png", TEX_NORMAL)), E_FAIL);

	// Boss Hpbar(모든 보스 공용)
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Hpbar_BackUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossHpBar_Base.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Hpbar_GreenUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossHpBar_Green.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Hpbar_YellowUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossHpBar_Yellow.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Hpbar_RedUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossHpBar_Red.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Hpbar_FontUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossLife%d.png", TEX_NORMAL, 11)), E_FAIL);

	// Boss 등장 이후 소개 백그라운드(공용)
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_NamingSceneUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/BossNamingScene_02.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Boss_Quest_Back_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Quest_Back_UI.png", TEX_NORMAL)), E_FAIL);
	
	// Damage Font UI
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Damage_Font_NormalUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Font_Damage/Font_Damage_Normal_%d.png", TEX_NORMAL, 10)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Damage_Font_SkillUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Font_Damage/Font_Damage_Skill_%d.png", TEX_NORMAL, 10)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Damage_Font_MonsterUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Font_Damage/Font_Damage_Monster_%d.png", TEX_NORMAL, 10)), E_FAIL);

	// Effect
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_SlashSpin", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/efx_sword_slash_spin.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_SlashPoint", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/efx_sword_slashpoint.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Critical", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/critical_roll.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Ground_Hit", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/ground_hit.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Symbol", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/ghostwall_symbol.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Efx_Bomb", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/bomb.png", TEX_NORMAL)), E_FAIL);

	// Shader Effect 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Dissolve", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/dissolve.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Distortion", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Effect/distortion.png", TEX_NORMAL)), E_FAIL);
	
	// Announce 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Announce_BalistaAttack", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Balista_Announced.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Announce_BalistaReady", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Announce_Balista_Ready.png", TEX_NORMAL)), E_FAIL);

	// Buff UI && QuickSlot Skill UI
	//FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_CriticalUI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/skill_lethita_glidingfury.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Sp_Fever_Icon", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_lethita_fever.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Fury_No7_Icon", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_lethita_gliding_loop.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Sp_Reinforce", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_lethita_reinforce_fury.tga", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Buff_Reinforce", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_hp_mastery.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Buff_Stemina_Recovery", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_breath.tga", TEX_NORMAL)), E_FAIL);

	// QuickSlot Item 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Potion_Hp", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/recovery_potion.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Potion_Stemina", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/stamina_potion_lv1.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Potion_Sp", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/stim_potion_lvl3.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Item_StickBomb", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/sticky_bomb.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Skill_Balista", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/skill_battle_will.tga", TEX_NORMAL)), E_FAIL);
	
	// Buff Tooltip
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Tooltip_Stemina", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Tooltip_Stemina.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Tooltip_Reinforce", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player/Tooltip_Reinforce.png", TEX_NORMAL)), E_FAIL);

	// Stage_Title
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_Stage_Title_UI", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Boss/Stage_Title_%d.png", TEX_NORMAL, 2)), E_FAIL);

	// End_Scene 
	FAILED_CHECK_RETURN(Ready_Prototype(L"Proto_Texture_End_Scene", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Ending/END_background.png", TEX_NORMAL)), E_FAIL);
	
	return 0;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	Safe_Release(m_pGraphicDev);

	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
}

