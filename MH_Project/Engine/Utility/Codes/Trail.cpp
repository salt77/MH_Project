#include "Trail.h"

USING(Engine)

CTrail::CTrail(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTrail::CTrail(const CTrail & rhs)
	: CVIBuffer(rhs)
{
}

CTrail::~CTrail()
{
}


HRESULT CTrail::Ready_Buffer()
{
	m_dwTriCnt = 15;
	m_dwVtxCnt = m_dwTriCnt * 2;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_IdxFmt = D3DFMT_INDEX16;
	m_dwIdxSize = sizeof(INDEX16);

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_pVB->Lock(0, 0, (void**)&m_pVertex, 0);

	m_pVertex = new VTXTEX[20];
	ZeroMemory(m_pVertex, sizeof(VTXTEX) * 20);

	Management_Point();

	list<_vec3>::iterator	iter = m_listPoint.begin();

	for (_uint i = 0; iter != m_listPoint.end(); )
	{
		m_pVertex[i].vPosition = *iter;

		if (i % 2)
		{
			m_pVertex[i].vTexUV = _vec2(1 / (20 - 1.f), 0.f);
		}
		else
		{
			m_pVertex[i].vTexUV = _vec2(i - 1 / (20 - 1.f), 1.f);
		}

		++i;
		++iter;
	}

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_uint i = 0; i < m_dwTriCnt; i += 2)
	{
		pIndex[i]._0 = i + 3;
		pIndex[i]._1 = i + 1;
		pIndex[i]._2 = i;

		pIndex[i + 1]._0 = i + 2;
		pIndex[i + 1]._1 = i + 3;
		pIndex[i + 1]._2 = i;
	}

	m_pIB->Unlock();

	return S_OK;
}

void CTrail::Render_Buffer()
{
	Management_Point();

	CVIBuffer::Render_Buffer();
}

//void CTrail::SetVertexTrail(const _vec3 * pPos, const _ulong & dwCnt)
//{
//	m_pVertex = new VTXTEX[dwCnt];
//	ZeroMemory(m_pVertex, sizeof(VTXTEX) * dwCnt);
//
//	for (_uint i = 0; i < dwCnt; ++i)
//	{
//		m_pVertex[i].vPosition = pPos[i];
//
//		if (i % 2)
//		{
//			m_pVertex[i].vTexUV = _vec2(1 / (dwCnt - 1.f), 0.f);
//		}
//		else
//		{
//			m_pVertex[i].vTexUV = _vec2(i - 1 / (dwCnt - 1.f), 1.f);
//		}
//	}
//
//	m_dwTriCnt = dwCnt - 2;
//}

void CTrail::Management_Point()
{
	while (m_dwVtxCnt < m_listPoint.size())
	{
		m_listPoint.pop_front();
	}

	m_fInTime += m_fDeltaTime;

	if (0.001f <= m_fInTime)
	{
		m_dwTriCnt = m_dwVtxCnt >> 1;

		_vec3	vPoint[2];
		vPoint[0] = _vec3(0.f, 0.f, 0.f);
		vPoint[1] = _vec3(0.f, 0.f, 0.f);

		//////////////
		VTXTEX*		pVertex = nullptr;
		m_pVB->Lock(0, 0, (void**)&pVertex, 0);

		list<_vec3>::iterator	iter = m_listPoint.begin();

		for (_uint i = 0; iter != m_listPoint.end(); )
		{
			pVertex[i].vPosition = *iter;

			if (i % 2)
			{
				pVertex[i].vTexUV = _vec2(i / _float(m_dwVtxCnt), 1.f);
			}
			else
			{
				pVertex[i].vTexUV = _vec2(i / _float(m_dwVtxCnt * 0.5f), 0.f);
			}

			++i;
			++iter;
		}

		m_pVB->Unlock();

		INDEX16*	pIndex = nullptr;
		m_pIB->Lock(0, 0, (void**)&pIndex, 0);

		for (_uint i = 0; i < m_dwTriCnt; i += 2)
		{
			pIndex[i]._0 = i + 3;
			pIndex[i]._1 = i + 1;
			pIndex[i]._2 = i;

			pIndex[i + 1]._0 = i + 2;
			pIndex[i + 1]._1 = i + 3;
			pIndex[i + 1]._2 = i;
		}

		m_pIB->Unlock();

		//////////////

		D3DXVec3TransformCoord(&vPoint[0], &m_vWeaponUpper, m_pMatWeapon);
		D3DXVec3TransformCoord(&vPoint[1], &m_vWeaponLower, m_pMatWeapon);

		for (_uint i = 0; i < 2; ++i)
		{
			m_listPoint.emplace_back(vPoint[i]);
		}

		m_fInTime = 0.f;
	}
}


CComponent * CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrail*		pInstance = new CTrail(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CTrail::Clone()
{
	return new CTrail(*this);
}

void CTrail::Free()
{
	Safe_Delete(m_pVertex);

	CVIBuffer::Free();
}
