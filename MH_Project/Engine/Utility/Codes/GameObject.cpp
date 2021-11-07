#include "GameObject.h"

USING(Engine)

Engine::CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();

	ZeroMemory(&m_tInfo, sizeof(INFO));
}

Engine::CGameObject::CGameObject(const CGameObject& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CGameObject::~CGameObject(void)
{
}


void CGameObject::Set_Damage(_uint iDamage)
{
	if (iDamage <= m_tInfo.iHp)
	{
		m_tInfo.iHp -= iDamage;

		if (m_bBoss)
		{
			if (iDamage >= m_iLineHp)
			{
				m_iLineHp = m_iMaxLineHp + (m_iLineHp - iDamage);
			}
			else
			{
				m_iLineHp -= iDamage;
			}
		}
	}
	else
	{
		m_tInfo.iHp = 0;
	}
}

HRESULT Engine::CGameObject::Ready_Object()
{
	return S_OK;
}

HRESULT CGameObject::LateReady_Object()
{
	return S_OK;
}

Engine::_int Engine::CGameObject::Update_Object(const _float& fTimeDelta)
{
	if (!m_bLateReady)
	{
		m_bLateReady = true;

		LateReady_Object();
	}

	_int iResult = NO_EVENT;

	if (m_bDead)
		return OBJ_DEAD;

	for (auto& iter : m_mapComponent[ID_DYNAMIC])
	{
		iResult = iter.second->Update_Component(fTimeDelta);

		if (iResult & 0x80000000)
			return -1;
	}

	return iResult;
}

_int CGameObject::LateUpdate_Object(const _float & fTimeDelta)
{
	return _int();
}

void CGameObject::Compute_ViewZ(const _vec3 * pPos)
{
	_matrix		matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

HRESULT CGameObject::Add_Collider(_float fRadius, wstring wstrName, const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CCollider::Create(m_pGraphicDev, fRadius, pColliderMatrix, eColliderType);
	m_mapColliderCom.emplace(wstrName, dynamic_cast<CCollider*>(pComponent));
	if (m_mapColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

HRESULT CGameObject::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, 
								  const _matrix* pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CComponent*		pComponent = CBoxCollider::Create(m_pGraphicDev, vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, pColliderMatrix, eColliderType);
	m_mapBoxColliderCom.emplace(wstrName, dynamic_cast<CBoxCollider*>(pComponent));
	if (m_mapBoxColliderCom.empty())
		return E_FAIL;

	m_mapComponent[ID_STATIC].emplace(wstrName, pComponent);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const wstring pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent[eID].end())
		return nullptr;

	return iter->second;
}

Engine::CComponent* Engine::CGameObject::Get_Component(const wstring pComponentTag, COMPONENTID eID)
{
	CComponent*		pComponent = Find_Component(pComponentTag, eID);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}


void Engine::CGameObject::Free(void)
{
	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}

