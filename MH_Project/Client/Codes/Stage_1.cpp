#include "stdafx.h"
#include "Stage_1.h"
#include "DamageFont.h"
#include "CollisionMgr.h"

#include "Dog.h"
#include "Soldier.h"
#include "Knight.h"
#include "Cloyan.h"
#include "Dog_Info.h"
#include "Soldier_Info.h"
#include "Knight_Info.h"
#include "Cloyan_Info.h"

#include "Trail_Sword.h"
#include "Trail_Smash.h"
#include "SlashPoint.h"
#include "CriticalEfx.h"
#include "RadialBlur.h"
#include "Efx_Distortion.h"

#include "Trail_Cloyan.h"

#include "Trap.h"
#include "Balista.h"
#include "Wall_Collision.h"
#include "Wall_Symbol.h"
#include "Box.h"
#include "LastRoom_Trigger.h"
#include "Stage_Title_UI.h"
#include "Ahglan_FontName.h"
#include "Ahglan_StageUI.h"
#include "Ahglan_Stage_Back_UI.h"

#include "DataMgr.h"
#include "Export_Function.h"
#include "Logo.h"

CStage_1::CStage_1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
	m_vecSavePoint.reserve(400);
}

CStage_1::~CStage_1(void)
{
}


HRESULT CStage_1::Ready_Scene()
{
	FAILED_CHECK_RETURN(CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Effect(L"Effect"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Balista(L"Balista"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Box(L"Box"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Symbol(L"Symbol"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Trigger(L"Trigger"), E_FAIL);

	return S_OK;
}

HRESULT CStage_1::LateReady_Scene()
{
	m_eSceneID = SCENE_STAGE_1;

	m_pLayer = CLayer::Create();
	m_pSpawnLayer = CLayer::Create();
	m_pEnemyLayer = CLayer::Create();

	FAILED_CHECK_RETURN(Load_NaviMesh(), E_FAIL);
	FAILED_CHECK_RETURN(Load_PlayerInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Load_PlayerCol(), E_FAIL);
	FAILED_CHECK_RETURN(Load_DogInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Load_SoldierInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Load_KnightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Load_CloyanInfo(), E_FAIL);

	m_mapLayer.emplace(L"GameLogic_Spawn", m_pSpawnLayer);
	m_mapLayer.emplace(L"Enemies", m_pEnemyLayer);

	CCollisionMgr::GetInstance()->Ready_CollisionMgr();

	return S_OK;
}

Engine::_int CStage_1::Update_Scene(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;

	_int iExit = CScene::Update_Scene(fTimeDelta);

	Load_NextStage();

	CCollisionMgr::GetInstance()->Update_CollisionMgr(fTimeDelta);

	return iExit;
}

_int CStage_1::LateUpdate_Scene(const _float & fTimeDelta)
{
	_int iExit = CScene::LateUpdate_Scene(fTimeDelta);

	return iExit;
}

void CStage_1::Render_Scene()
{
	// DEBUG 용
	//m_dwRenderCnt++;

	//if (m_fTime >= 1.f)
	//{
	//	wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
	//	m_dwRenderCnt = 0;
	//	m_fTime = 0.f;
	//}

	//Render_Font(L"Font_DFP", m_szFPS, &_vec2(850.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CStage_1::Ready_Layer_Environment(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// DynamicCamera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
				  &_vec3(0.f, 1.5f, 13.f),
				  &_vec3(0.f, 0.f, 1.f),
				  &_vec3(0.f, 1.f, 0.f),
				  D3DXToRadian(60.f), (_float)WINCX / WINCY,
				  0.1f, 300.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_GameLogic(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*			pGameObject = nullptr;

	// Terrain 추가
	pGameObject = CTerrain::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	// StageMesh
	pGameObject = CStageMesh_1::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StageMesh", pGameObject), E_FAIL);

	// Player
	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

	// Dummy
	pGameObject = CHitBox_Pos::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HitBox_Pos", pGameObject), E_FAIL);

	// Trail_Sword
	pGameObject = CTrail_Sword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Sword_Trail", pGameObject), E_FAIL);

	pGameObject = CTrail_Sword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Sword_Trail2", pGameObject), E_FAIL);

	// Trail_Smash
	pGameObject = CTrail_Smash::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Smash_Trail", pGameObject), E_FAIL);

	pGameObject = CTrail_Smash::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Smash_Trail2", pGameObject), E_FAIL);

	// Trail_Cloyan
	pGameObject = CTrail_Cloyan::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Cloyan_Trail", pGameObject), E_FAIL);

	// Trap
	pGameObject = CTrap::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Trap", pGameObject), E_FAIL);

	// 오브젝트 탐색을 용이하게 하기 위해서 레이어를 분류해서 넣어둠. 
	//// Balista
	//for (_uint i = 0; i < BALISTA_COUNT; ++i)
	//{
	//	wstring	wstrName = L"Balista_";
	//	wstrName += to_wstring(i);

	//	pGameObject = CBalista::Create(m_pGraphicDev);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);
	//}

	pGameObject = CWall_Collision::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall_Collision", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_UI(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	// FadeInOut
	pGameObject = CFadeInOut::Create(m_pGraphicDev, 0.f, 0.f, WINCX * 2.f, WINCY * 2.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FadeInOut_UI", pGameObject), E_FAIL);

	// Quest Target에 붙는 UI
	pGameObject = CAhglan_Stage_Back_UI::Create(m_pGraphicDev, 85.f, 50.f, 300.f, 200.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"0.Cloyan_Quest_Back_UI", pGameObject), E_FAIL);

	pGameObject = CAhglan_StageUI::Create(m_pGraphicDev, 85.f, 65.f, 200.f, 100.f, BOSS_CLOYAN);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"1.Cloyan_QuestTarget", pGameObject), E_FAIL);

	pGameObject = CAhglan_FontName::Create(m_pGraphicDev, 45.f, 30.f, 100.f, 50.f, BOSS_CLOYAN);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"2.Cloyan_FontName", pGameObject), E_FAIL);

	// Stage_Title 
	pGameObject = CStage_Title_UI::Create(m_pGraphicDev, 200.f, WINCY - 250.f, 300.f, 150.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Stage_Title_UI", pGameObject), E_FAIL);

	// Damage Font UI (풀링 사용 => 미리 생성 (종류별로 30개씩 생성 -> 최대 4자릿수 데미지))
	for (_uint i = 0; i < DAMAGEFONT_COUNT; ++i)
	{
		wstring wstrNormalFont = L"DamageFont_Normal_UI_";

		wstrNormalFont += to_wstring(i);

		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_NORMAL);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrNormalFont, pGameObject), E_FAIL);
	}
	for (_uint i = 0; i < DAMAGEFONT_SKILL_COUNT; ++i)
	{
		wstring wstrSkillFont = L"DamageFont_Skill_UI_";

		wstrSkillFont += to_wstring(i);

		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_SKILL);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrSkillFont, pGameObject), E_FAIL);
	}
	for (_uint i = 0; i < DAMAGEFONT_MONSTER_COUNT; ++i)
	{
		wstring wstrMonsterFont = L"DamageFont_Monster_UI_";

		wstrMonsterFont += to_wstring(i);

		pGameObject = CDamageFont::Create(m_pGraphicDev, DAMAGEFONT_MONSTER);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrMonsterFont, pGameObject), E_FAIL);
	}

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_Effect(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*		pGameObject = nullptr;

	// SlashPoint
	for (_uint i = 0; i < SLASHPOINT_COUNT; ++i)
	{
		wstring	wstrName = L"Efx_SlashPoint_";
		wstrName += to_wstring(i);

		pGameObject = CSlashPoint::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);
	}

	pGameObject = CCriticalEfx::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Efx_Critical", pGameObject), E_FAIL);

	pGameObject = CRadialBlur::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Efx_RadiulBlur", pGameObject), E_FAIL);

	pGameObject = CEfx_Distortion::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Efx_Distortion", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_Balista(const wstring pLayerTag)
{
	CLayer*	pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*	pGameObject = nullptr;

	// Balista
	for (_uint i = 0; i < BALISTA_COUNT; ++i)
	{
		wstring	wstrName = L"Balista_";
		wstrName += to_wstring(i);

		pGameObject = CBalista::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);
	}

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_Box(const wstring pLayerTag)
{
	CLayer*	pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*	pGameObject = nullptr;

	pGameObject = CBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Box", pGameObject), E_FAIL);

	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&BOX_POS);

	pGameObject = CBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Box_2", pGameObject), E_FAIL);

	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&BOX_2_POS);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_Symbol(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*	pGameObject = nullptr;

	pGameObject = CWall_Symbol::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall_Symbol", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Layer_Trigger(const wstring pLayerTag)
{
	CLayer*		pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	CGameObject*	pGameObject = nullptr;

	pGameObject = CLastRoom_Trigger::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Trigger_LastRoom", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_Prototype(void)
{
	return S_OK;
}

HRESULT CStage_1::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Direction = _vec3(0.5f, -1.f, 0.5f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CStage_1::Load_PlayerInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Stage_1_Player.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;

	_uint iObjType = -1;
	_uint iObjCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iObjType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);

	ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

	CTransform* pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));

	pPlayerTrans->Set_Pos(&vPos);
	//pPlayerTrans->Set_Scale(vScale.x, vScale.y, vScale.z);
	pPlayerTrans->RotationFromOriginAngle(ROT_X, vRotate.x);
	pPlayerTrans->RotationFromOriginAngle(ROT_Y, vRotate.y);
	pPlayerTrans->RotationFromOriginAngle(ROT_Z, vRotate.z);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_DogInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Dog_Obj.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	NULL_CHECK_RETURN(m_pSpawnLayer, E_FAIL);

	DWORD dwbyte = 0;

	_uint iObjType = -1;
	_uint iObjCount = -1;
	_uint iTargetCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iObjType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iTargetCount, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iTargetCount; ++i)
	{
		if (0 == dwbyte)
			return E_FAIL;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

		//wstring wstrName = L"Dog_";
		wstring wstrName = L"Dog_Info_";
		wstrName += to_wstring(i);

		CGameObject*		pGameObject = nullptr;

		pGameObject = CDog_Info::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pSpawnLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&vPos);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_X, vRotate.x);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Y, vRotate.y);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Z, vRotate.z);
	}

	// Pooling할 객체 생성 
	NULL_CHECK_RETURN(m_pEnemyLayer, E_FAIL);

	for (_uint i = 0; i < DOG_COUNT; ++i)
	{
		wstring wstrName = L"Dog_";
		wstrName += to_wstring(i);

		CGameObject*		pGameObject = nullptr;

		pGameObject = CDog::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pEnemyLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&POOLING_POS);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_SoldierInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Soldier_Obj.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	NULL_CHECK_RETURN(m_pSpawnLayer, E_FAIL);

	DWORD dwbyte = 0;

	_uint iObjType = -1;
	_uint iObjCount = -1;
	_uint iTargetCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iObjType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iTargetCount, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iTargetCount; ++i)
	{
		if (0 == dwbyte)
			return E_FAIL;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

		//wstring wstrName = L"Soldier_";
		wstring wstrName = L"Soldier_Info_";
		wstrName += to_wstring(i);

		CGameObject*		pGameObject = nullptr;

		//pGameObject = CSoldier::Create(m_pGraphicDev);
		//NULL_CHECK_RETURN(pGameObject, E_FAIL);
		//FAILED_CHECK_RETURN(pLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);
		pGameObject = CSoldier_Info::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pSpawnLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&vPos);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_X, vRotate.x);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Y, vRotate.y);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Z, vRotate.z);
	}

	// Pooling할 객체 생성 
	NULL_CHECK_RETURN(m_pEnemyLayer, E_FAIL);

	for (_uint i = 0; i < SOLDIER_COUNT; ++i)
	{
		wstring wstrName = L"Soldier_";
		wstrName += to_wstring(i);

		CGameObject*		pGameObject = nullptr;

		pGameObject = CSoldier::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pEnemyLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&POOLING_POS);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_KnightInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Knight_Obj.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	NULL_CHECK_RETURN(m_pSpawnLayer, E_FAIL);

	DWORD dwbyte = 0;

	_uint iObjType = -1;
	_uint iObjCount = -1;
	_uint iTargetCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iObjType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iTargetCount, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iTargetCount; ++i)
	{
		if (0 == dwbyte)
			return E_FAIL;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

		//wstring wstrName = L"Knight_";
		wstring wstrName = L"Knight_Info_";
		wstrName += to_wstring(i);

		CGameObject*	pGameObject = nullptr;

		pGameObject = CKnight_Info::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pSpawnLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&vPos);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_X, vRotate.x);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Y, vRotate.y);
		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Z, vRotate.z);
	}

	// Pooling할 객체 생성 
	NULL_CHECK_RETURN(m_pEnemyLayer, E_FAIL);

	for (_uint i = 0; i < KNIGHT_COUNT; ++i)
	{
		wstring wstrName = L"Knight_";
		wstrName += to_wstring(i);

		CGameObject*		pGameObject = nullptr;

		pGameObject = CKnight::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(m_pEnemyLayer->Add_GameObject(wstrName, pGameObject), E_FAIL);

		dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&POOLING_POS);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_CloyanInfo()
{
	HANDLE hFile = CreateFile(L"../../Data/Cloyan_Obj.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	NULL_CHECK_RETURN(m_pSpawnLayer, E_FAIL);

	DWORD dwbyte = 0;

	_uint iObjType = -1;
	_uint iObjCount = -1;
	_uint iTargetCount = -1;

	_vec3	vPos, vScale, vRotate;

	ReadFile(hFile, &iObjType, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);
	//ReadFile(hFile, &iTargetCount, sizeof(_uint), &dwbyte, nullptr);

	if (0 == dwbyte)
		return E_FAIL;

	ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
	ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

	CGameObject*	pGameObject = nullptr;

	pGameObject = CCloyan_Info::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pSpawnLayer->Add_GameObject(L"Cloyan_Info", pGameObject), E_FAIL);

	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&vPos);
	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_X, vRotate.x);
	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Y, vRotate.y);
	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->RotationFromOriginAngle(ROT_Z, vRotate.z);

	// Pooling할 객체 생성 
	NULL_CHECK_RETURN(m_pEnemyLayer, E_FAIL);

	pGameObject = nullptr;
	pGameObject = CCloyan::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_pEnemyLayer->Add_GameObject(L"Cloyan", pGameObject), E_FAIL);

	dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform", ID_DYNAMIC))->Set_Pos(&POOLING_POS);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage_1::Load_PlayerCol()
{
	HANDLE hFile = CreateFile(L"../../Data/Lethita_HitBox.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;
	DWORD dwStringSize = 0;
	DWORD dwStringSize2 = 0;
	_float fRadius = 0.f;
	TCHAR*	pNameBuff = nullptr;
	TCHAR*	pNameBuff2 = nullptr;
	wstring wstrColName, wstrBoneName;
	COLLIDERTYPE	eColliderType;
	_vec3	vMin = {};
	_vec3	vMax = {};

	_uint iSize = 0;
	_uint iBoxSize = 0;

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
		ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

		pNameBuff = new TCHAR[dwStringSize];
		pNameBuff2 = new TCHAR[dwStringSize2];

		ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
		ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
		ReadFile(hFile, &fRadius, sizeof(_float), &dwbyte, nullptr);
		ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

		wstrColName = pNameBuff;
		wstrBoneName = pNameBuff2;

		// Bone 찾기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
		CDynamicMesh* pMeshCom = dynamic_cast<CDynamicMesh*>(pPlayer->Get_Component(L"Com_Mesh", ID_STATIC));

		pPlayer->Add_Collider(fRadius, pNameBuff, dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CCollider*>(Engine::Get_Component(L"GameLogic", L"Player", wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CCollider*>(pPlayer->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);

	}

	CloseHandle(hFile);

	hFile = CreateFile(L"../../Data/Lethita_WeaponHitBox.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
	ReadFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iBoxSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
		ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

		pNameBuff = new TCHAR[dwStringSize];
		pNameBuff2 = new TCHAR[dwStringSize2];

		ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
		ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
		ReadFile(hFile, &vMin, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vMax, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

		wstrColName = pNameBuff;
		wstrBoneName = pNameBuff2;

		// Bone 찾기
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
		CDynamicMesh* pMeshCom = dynamic_cast<CDynamicMesh*>(pPlayer->Get_Component(L"Com_Mesh", ID_STATIC));

		pPlayer->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff,
			dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		// Bone 찾기
		list<D3DXMESHCONTAINER_DERIVED*>			listTemp = pMeshCom->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

		for (; iterList != listTemp.end(); ++iterList)
		{
			for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
			{
				_tchar	pTemp[64] = L"";
				MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

				if (!wcscmp(pTemp, pNameBuff2))
				{
					dynamic_cast<CBoxCollider*>(Engine::Get_Component(L"GameLogic", L"Player", wstrColName, ID_STATIC))->Set_BoneName(wstrBoneName);
					dynamic_cast<CBoxCollider*>(pPlayer->Get_Component((wstring)wstrColName, ID_STATIC))->Set_Matrix((*iterList)->ppCombinedTransformMatrix[i]);

					break;
				}
			}
		}

		Safe_Delete(pNameBuff);
		Safe_Delete(pNameBuff2);
	}

	CloseHandle(hFile);

	// 검에 붙어있지 않은 히트박스 임의 생성
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	CTransform*	pHitBox_Pos = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"HitBox_Pos", L"Com_Transform", ID_DYNAMIC));

	pPlayer->Add_Collider(-30.f, -60.f, -30.f, 30.f, 60.f, 30.f, L"Other_Attack", pHitBox_Pos->Get_WorldMatrix(), COLTYPE_BOX_OTHER);

	return S_OK;
}

HRESULT CStage_1::Load_NaviMesh()
{
	HANDLE hFile = CreateFile(L"../../Data/Stage_1_Navi20.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwbyte = 0;
	_uint iSize = 0;
	_matrix matPoint;
	ZeroMemory(matPoint, sizeof(_matrix));

	ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		if (0 == dwbyte)
			break;

		ReadFile(hFile, &matPoint, sizeof(_matrix), &dwbyte, nullptr);

		m_vecSavePoint.push_back(matPoint);
	}

	CloseHandle(hFile);

	FAILED_CHECK_RETURN(Engine::Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev, m_vecSavePoint.size(), m_vecSavePoint)), E_FAIL);

	CPlayer*	pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	if (pPlayer)
		pPlayer->Add_NaviMesh();

	return S_OK;
}

HRESULT CStage_1::Load_NextStage()
{
	if (m_bReadyToLoadStage)
	{
		CFadeInOut*		pFadeInOut = static_cast<CFadeInOut*>(Engine::Get_GameObject(L"UI", L"FadeInOut_UI"));

		if (pFadeInOut)
		{
			m_fFadeOutRatio += FADEOUTSPEED * 5.f;
			pFadeInOut->Set_ValueRatio(m_fFadeOutRatio);
		}

		if (1.f <= m_fFadeOutRatio)
		{
			CPlayer*	pPlayer = static_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
			CDataMgr::GetInstance()->Set_PlayerData(pPlayer->Get_TagPlayerInfo());

			// 메인 보스 스테이지로 로드하는 코드 작성하기 
			Engine::Clear_Prototype_ForNextStage();
			CScene*		pScene = CLogo::Create(Engine::Get_CurSceneDevice(), 0);
			NULL_CHECK(pScene);

			CManagement*	pManagement = nullptr;
			Engine::Get_Management(&pManagement);
			FAILED_CHECK_RETURN(pManagement->Set_Scene(pScene), );
		}
	}

	return S_OK;
}

HRESULT CStage_1::Delete_ForNextStage()
{
	map<const wstring, CLayer*>::iterator	iter = m_mapLayer.begin();

	for (; iter != m_mapLayer.end(); ++iter)
	{
		Safe_Release(iter->second);
		m_mapLayer.erase(iter->first);
	}

	return S_OK;
}

CStage_1* CStage_1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage_1*	pInstance = new CStage_1(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Safe_Release(pInstance);

	return pInstance;
}

void CStage_1::Free()
{
	Safe_Release(m_pLayer);
	Safe_Release(m_pSpawnLayer);
	Safe_Release(m_pEnemyLayer);

	CCollisionMgr::DestroyInstance();

	CScene::Free();
}
