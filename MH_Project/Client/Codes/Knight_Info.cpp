#include "stdafx.h"
#include "Knight_Info.h"
#include "Knight.h"

#include "Export_Function.h"

CKnight_Info::CKnight_Info(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSpawn_Info(pGraphicDev)
{
}

CKnight_Info::CKnight_Info(const CKnight_Info& rhs)
	: CSpawn_Info(rhs)
{
}

CKnight_Info::~CKnight_Info(void)
{
}


HRESULT CKnight_Info::Ready_Object()
{
	FAILED_CHECK_RETURN(CSpawn_Info::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

	m_pTransformCom->Update_Component(0.f);

	m_fAreaLimit = DIS_MID;
	m_fContactPlayerDis = DIS_SHORT + 1.f;

	return S_OK;
}

HRESULT CKnight_Info::LateReady_Object()
{
	FAILED_CHECK_RETURN(CSpawn_Info::LateReady_Object(), E_FAIL);

	m_vSpawnPos = *m_pTransformCom->Get_Info(INFO_POS);
	m_vRotateInfo = *m_pTransformCom->Get_RotateInfo();

	m_pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	return S_OK;
}

_int CKnight_Info::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CSpawn_Info::Update_Object(fTimeDelta);

	if (m_bDead)
		return iExit;

	Contact(fTimeDelta);

	//Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

_int CKnight_Info::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CSpawn_Info::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CKnight_Info::Render_Object(void)
{
}

CKnight_Info* CKnight_Info::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKnight_Info*	pInstance = new CKnight_Info(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CKnight_Info::Free(void)
{
	CGameObject::Free();
}

HRESULT CKnight_Info::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

void CKnight_Info::Contact(const _float& fTimeDelta)
{
	if (D3DXVec3Length(&(*m_pTransformCom->Get_Info(INFO_POS) - *m_pPlayerTrans->Get_Info(INFO_POS))) <= m_fContactPlayerDis)
	{
		m_bContact = true;
	}

	if (m_bContact)
	{
		// Soldier 풀링 스폰 코드 
		_uint	iIndex = 0;

		for (_uint i = 0; i < KNIGHT_COUNT; ++i)
		{
			wstring wstrName = L"Knight_";
			wstrName += to_wstring(iIndex);

			CKnight*	pKnight = static_cast<CKnight*>(Engine::Get_GameObject(L"Enemies", wstrName));
			CTransform*	pTransform = static_cast<CTransform*>(pKnight->Get_Component(L"Com_Transform", ID_DYNAMIC));

			if (POOLING_POS == *pTransform->Get_Info(INFO_POS))
			{
				pKnight->Set_Enable(m_vSpawnPos, m_vRotateInfo);

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

