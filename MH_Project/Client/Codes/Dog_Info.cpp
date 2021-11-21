#include "stdafx.h"
#include "Dog_Info.h"
#include "Dog.h"

#include "Export_Function.h"

CDog_Info::CDog_Info(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSpawn_Info(pGraphicDev)
{
}

CDog_Info::CDog_Info(const CDog_Info& rhs)
	: CSpawn_Info(rhs)
{
}

CDog_Info::~CDog_Info(void)
{
}


HRESULT CDog_Info::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CSpawn_Info::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

	m_pTransformCom->Update_Component(0.f);

	return S_OK;
}

HRESULT CDog_Info::LateReady_Object()
{
	FAILED_CHECK_RETURN(CSpawn_Info::LateReady_Object(), E_FAIL);

	m_vSpawnPos = *m_pTransformCom->Get_Info(INFO_POS);
	m_vRotateInfo = *m_pTransformCom->Get_RotateInfo();

	m_pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	return S_OK;
}

_int CDog_Info::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CSpawn_Info::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	Contact(fTimeDelta);

	//Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

_int CDog_Info::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CSpawn_Info::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CDog_Info::Render_Object(void)
{
}

CDog_Info* CDog_Info::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDog_Info*	pInstance = new CDog_Info(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CDog_Info::Free(void)
{
	CGameObject::Free();
}

HRESULT CDog_Info::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

void CDog_Info::Contact(const _float& fTimeDelta)
{
	if (D3DXVec3Length(&(*m_pTransformCom->Get_Info(INFO_POS) - *m_pPlayerTrans->Get_Info(INFO_POS))) <= m_fContactPlayerDis)
	{
		m_bContact = true;
	}

	if (m_bContact)
	{
		// Soldier 풀링 스폰 코드 
		for (_uint i = 0; i < DOG_COUNT; ++i)
		{
			wstring wstrName = L"Dog_";
			wstrName += to_wstring(i);

			CDog*	pDog = static_cast<CDog*>(Engine::Get_GameObject(L"Enemies", wstrName));
			CTransform*	pTransform = static_cast<CTransform*>(pDog->Get_Component(L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				pDog->Set_Enable(m_vSpawnPos, m_vRotateInfo);

				break;
			}
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

