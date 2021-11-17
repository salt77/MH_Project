#include "NaviMesh.h"

USING(Engine)

Engine::CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_dwIndex(0)
{
	m_vecCell.reserve(64);
}

Engine::CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CComponent(rhs.m_pGraphicDev)
	, m_dwIndex(rhs.m_dwIndex)
	, m_vecCell(rhs.m_vecCell)
{
	for (auto& iter : m_vecCell)
		iter->AddRef();
}

Engine::CNaviMesh::~CNaviMesh(void)
{

}

HRESULT Engine::CNaviMesh::Ready_NaviMesh(_uint iCellCount, vector<_matrix> vecPoint)
{
	CCell*	pCell = nullptr;
	_ulong	dwIndex = 0;

	_vec3 vFirstPoint, vSecondPoint, vThirdPoint;

	for (_uint index = 0; index < iCellCount; ++index)
	{
		memcpy(&vFirstPoint, &vecPoint[index]._11, sizeof(_vec3));
		memcpy(&vSecondPoint, &vecPoint[index]._21, sizeof(_vec3));
		memcpy(&vThirdPoint, &vecPoint[index]._31, sizeof(_vec3));

		//_vec3	vTemp = _vec3(0.f, 0.f, 0.f);

		//_vec2	_1 = _vec2(vecPoint[index]._21, vecPoint[index]._23);
		//_vec2	_2 = _vec2(vecPoint[index]._11, vecPoint[index]._13);
		//_vec2	_3 = _vec2(vecPoint[index]._31, vecPoint[index]._33);

		//_float fComputeCCW = _1.x * _2.y + _2.x * _3.y + _3.x * _1.y - (_2.x * _1.y + _3.x * _2.y + _1.x * _3.y);

		//if (0.f > fComputeCCW)
		//{
		//	vTemp = vFirstPoint;
		//	vFirstPoint = vThirdPoint;
		//	vThirdPoint = vTemp;
		//}

		pCell = CCell::Create(m_pGraphicDev, (_ulong)index, &vFirstPoint, &vSecondPoint, &vThirdPoint);
		NULL_CHECK_RETURN(pCell, E_FAIL);
		m_vecCell.push_back(pCell);
	}

	FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

HRESULT CNaviMesh::Ready_NaviMesh()
{
	CCell*		pCell = nullptr;

	// 0
	pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(), &_vec3(0.f, 0.f, 2.f), &_vec3(2.f, 0.f, 0.f), &_vec3(0.f, 0.f, 0.f));
	NULL_CHECK_RETURN(pCell, E_FAIL);
	m_vecCell.push_back(pCell);

	// 1
	pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(), &_vec3(0.f, 0.f, 2.f), &_vec3(2.f, 0.f, 2.f), &_vec3(2.f, 0.f, 0.f));
	NULL_CHECK_RETURN(pCell, E_FAIL);
	m_vecCell.push_back(pCell);

	// 2
	pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(), &_vec3(0.f, 0.f, 4.f), &_vec3(2.f, 0.f, 2.f), &_vec3(0.f, 0.f, 2.f));
	NULL_CHECK_RETURN(pCell, E_FAIL);
	m_vecCell.push_back(pCell);

	// 3
	pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(), &_vec3(2.f, 0.f, 2.f), &_vec3(4.f, 0.f, 0.f), &_vec3(2.f, 0.f, 0.f));
	NULL_CHECK_RETURN(pCell, E_FAIL);
	m_vecCell.push_back(pCell);

	FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMesh(void)
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();
}

_vec3 CNaviMesh::MoveOn_NaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir,
								const _float & fSpeed, const _float & fTimeDelta)
{
	if (!m_vecCell.empty())
	{
		_vec3		vEndPos = *pTargetPos + (*pTargetDir * fSpeed * fTimeDelta);

		if (CCell::COMPARE_MOVE == m_vecCell[m_dwIndex]->Compare_Position(&vEndPos, &m_dwIndex))
			return vEndPos;

		else if (CCell::COMPARE_STOP == m_vecCell[m_dwIndex]->Compare_Position(&vEndPos, &m_dwIndex))
			return *pTargetPos;
	}

	return *pTargetPos;
}

void CNaviMesh::DeleteAllCell()
{
	_uint iSize = m_vecCell.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		Safe_Release(m_vecCell[i]);
	}

	m_vecCell.clear();
}

HRESULT Engine::CNaviMesh::Link_Cell(void)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if (i == j)
				continue;

			// 1. 얻어온 이웃 셀의 메모리 정보가 null인 경우
			// 2. 얻어온 이웃 셀의 두 점이 같은 두 점을 공유하는 경우
			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
						m_vecCell[i]->Get_Point(CCell::POINT_B),
						m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
						m_vecCell[i]->Get_Point(CCell::POINT_C),
						m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
						m_vecCell[i]->Get_Point(CCell::POINT_A),
						m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}
		}
	}

	return S_OK;
}


Engine::CNaviMesh* Engine::CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iCellCount, vector<_matrix> vecPoint)
{
	CNaviMesh*	pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMesh(iCellCount, vecPoint)))
		Safe_Release(pInstance);

	return pInstance;
}

CNaviMesh * CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviMesh*	pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMesh()))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void Engine::CNaviMesh::Free(void)
{
	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();

	CComponent::Free();
}

