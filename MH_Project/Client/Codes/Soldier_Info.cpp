#include "stdafx.h"
#include "Soldier_Info.h"
#include "Soldier.h"

#include "Export_Function.h"

CSoldier_Info::CSoldier_Info(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSpawn_Info(pGraphicDev)
{
}

CSoldier_Info::CSoldier_Info(const CSoldier_Info& rhs)
	: CSpawn_Info(rhs)
{
}

CSoldier_Info::~CSoldier_Info(void)
{
}


HRESULT CSoldier_Info::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CSpawn_Info::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

	m_pTransformCom->Update_Component(0.f);

	return S_OK;
}

HRESULT CSoldier_Info::LateReady_Object()
{
	FAILED_CHECK_RETURN(CSpawn_Info::LateReady_Object(), E_FAIL);

	m_vSpawnPos = *m_pTransformCom->Get_Info(INFO_POS);
	m_vRotateInfo = *m_pTransformCom->Get_RotateInfo();

	m_pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	return S_OK;
}

_int CSoldier_Info::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CSpawn_Info::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	Contact(fTimeDelta);

	//Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

_int CSoldier_Info::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CSpawn_Info::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CSoldier_Info::Render_Object(void)
{
}

CSoldier_Info* CSoldier_Info::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSoldier_Info*	pInstance = new CSoldier_Info(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CSoldier_Info::Free(void)
{
	CGameObject::Free();
}

HRESULT CSoldier_Info::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

void CSoldier_Info::Contact(const _float& fTimeDelta)
{
	if (D3DXVec3Length(&(*m_pTransformCom->Get_Info(INFO_POS) - *m_pPlayerTrans->Get_Info(INFO_POS))) <= m_fContactPlayerDis)
	{
		m_bContact = true;
	}

	if (m_bContact)
	{
		// Soldier 풀링 스폰 코드 
		_uint	iIndex = 0;

		for (_uint i = 0; i < SOLDIER_COUNT; ++i)
		{
			wstring wstrName = L"Soldier_";
			wstrName += to_wstring(iIndex);

			CSoldier*	pSoldier = static_cast<CSoldier*>(Engine::Get_GameObject(L"Enemies", wstrName));
			CTransform*	pTransform = static_cast<CTransform*>(pSoldier->Get_Component(L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				pSoldier->Set_Enable(m_vSpawnPos, m_vRotateInfo);

				break;
			}

			++iIndex;
		}
		//

		CLayer* pLayer = Engine::Get_Layer(L"GameLogic_Spawn");
		if (!pLayer)
			return;

		map<const wstring, CGameObject*>	mapObject = Engine::Get_MapObject(L"GameLogic_Spawn");
		map<const wstring, CGameObject*>::iterator	iter = mapObject.begin();

		for (; iter != mapObject.end(); ++iter)
		{
			if (D3DXVec3Length(&(*static_cast<CSpawn_Info*>(iter->second)->Get_Position() - *m_pTransformCom->Get_Info(INFO_POS))) <= m_fAreaLimit)
			{
				static_cast<CSpawn_Info*>(iter->second)->Set_Contact();
			}
		}

		m_bDead = true;
	}
}

