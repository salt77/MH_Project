#include "Calculator.h"

#include "Transform.h"
#include "TerrainTex.h"
#include "StaticMesh.h"

USING(Engine)

Engine::CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{

}

Engine::CCalculator::CCalculator(const CCalculator& rhs)
	: CComponent(rhs)
{

}

Engine::CCalculator::~CCalculator(void)
{

}

HRESULT Engine::CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

Engine::_float Engine::CCalculator::Compute_HeightOnTerrain(const _vec3* pPos,
	const _vec3* pTerrainVtx,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv)
{
	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fRatioX = (pPos->x - pTerrainVtx[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fRatioZ = (pTerrainVtx[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	_float	fHeight[4] = {
			pTerrainVtx[dwIndex + dwCntX].y,
			pTerrainVtx[dwIndex + dwCntX + 1].y,
			pTerrainVtx[dwIndex + 1].y,
			pTerrainVtx[dwIndex].y
	};


	// ������ �� �ﰢ��
	if (fRatioX > fRatioZ)
	{
		return fHeight[0] + fRatioX * (fHeight[1] - fHeight[0]) + fRatioZ * (fHeight[2] - fHeight[1]);
	}
	else
	{
		return fHeight[0] + fRatioX * (fHeight[2] - fHeight[3]) + fRatioZ * (fHeight[3] - fHeight[0]);
	}
}

Engine::CCalculator* Engine::CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*		pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCalculator::Free(void)
{
	CComponent::Free();
}

Engine::CComponent* Engine::CCalculator::Clone(void)
{
	return new CCalculator(*this);
}

Engine::_vec3 Engine::CCalculator::Picking_OnTerrain(HWND hWnd,
													 const CTerrainTex* pTerrainBufferCom,
													 const CTransform*  pTerrainTransCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3		vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	// ������ ��ǥ�� ���� ��ǥ�� ��ȯ
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f;

	// ���� ��ǥ���� �佺���̽��� ��ȯ
	_matrix		matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// �佺���̽����� ���� �����̽��� ��ȯ
	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���彺���̽����� ���� �����̽��� ��ȯ

	_matrix		matWorld;
	pTerrainTransCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_ulong		dwVtXCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong		dwVtXCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const	_vec3*		pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_float	fU, fV, fDist;

	_ulong	dwVtxIdx[3];

	_vec3	vTerrainPos;
	memcpy(&vTerrainPos, &matWorld._41, sizeof(_vec3));

	for (_ulong i = 0; i < dwVtXCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtXCntX - 1; ++j)
		{
			_ulong dwIndex = i * dwVtXCntX + j;

			// ������ �� �ﰢ��

			dwVtxIdx[0] = dwIndex + dwVtXCntX;
			dwVtxIdx[1] = dwIndex + dwVtXCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[2]],
				&vRayPos,
				&vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[1]].x + fU * (pTerrainVtx[dwVtxIdx[0]].x - pTerrainVtx[dwVtxIdx[1]].x),
							 -vTerrainPos.y,
							 (pTerrainVtx[dwVtxIdx[1]].z + fV * (pTerrainVtx[dwVtxIdx[2]].z - pTerrainVtx[dwVtxIdx[1]].z) - 50.f));
			}

			// ���� �Ʒ� �ﰢ��

			dwVtxIdx[0] = dwIndex + dwVtXCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[2]],
				&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[0]],
				&vRayPos,
				&vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[2]].x + fU * (pTerrainVtx[dwVtxIdx[1]].x - pTerrainVtx[dwVtxIdx[2]].x),
							 -vTerrainPos.y,
							 (pTerrainVtx[dwVtxIdx[2]].z + fV * (pTerrainVtx[dwVtxIdx[0]].z - pTerrainVtx[dwVtxIdx[2]].z) - 50.f));
			}

		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CCalculator::Picking_OnMesh(HWND hWnd, CStaticMesh * pMeshBufferCom, const CTransform * pMeshTransCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3		vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	// ������ ��ǥ�� ���� ��ǥ�� ��ȯ
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f;

	// ���� ��ǥ���� �佺���̽��� ��ȯ
	_matrix		matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// �佺���̽����� ���� �����̽��� ��ȯ
	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���彺���̽����� ���� �����̽��� ��ȯ

	_matrix		matWorld;
	pMeshTransCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	//_ulong		dwVtXCntX = pTerrainBufferCom->Get_VtxCntX();
	//_ulong		dwVtXCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const _vec3*	pTerrainVtx = pMeshBufferCom->Get_VtxPos();

	_float	fU, fV, fDist;

	_ulong	dwVtxIdx[3];


	for (_ulong i = 0; i < pMeshBufferCom->Get_VtxCnt() - 2; ++i)
	{
		if (D3DXIntersectTri(&pTerrainVtx[i],
			&pTerrainVtx[i + 1],
			&pTerrainVtx[i + 2],
			&vRayPos,
			&vRayDir,
			&fU, &fV, &fDist))
		{
			return _vec3(pTerrainVtx[dwVtxIdx[1]].x + fU * (pTerrainVtx[dwVtxIdx[0]].x - pTerrainVtx[dwVtxIdx[1]].x),
						 0.f,
						 pTerrainVtx[dwVtxIdx[1]].z + fV * (pTerrainVtx[dwVtxIdx[2]].z - pTerrainVtx[dwVtxIdx[1]].z));
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_bool CCalculator::Collision_Sphere(const _vec3& vDstCenter, const _float& fDstRadius, const _vec3& vSrcCenter, const _float& fSrcRadius)
{
	_float fRadius = fDstRadius + fSrcRadius;
	_float fCenterDis = D3DXVec3Length(&(vDstCenter - vSrcCenter));
	if ((fDstRadius + fSrcRadius) < D3DXVec3Length(&(vDstCenter - vSrcCenter)))
		return false;

	return true;
}

_bool CCalculator::Collision_AABB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	_vec3	vDestMin, vDestMax, vSourMin, vSourMax;
	_float	fDest, fSour;

	D3DXVec3TransformCoord(&vDestMin, pDestMin, pDestWorld);
	D3DXVec3TransformCoord(&vDestMax, pDestMax, pDestWorld);

	D3DXVec3TransformCoord(&vSourMin, pSourMin, pSourWorld);
	D3DXVec3TransformCoord(&vSourMax, pSourMax, pSourWorld);

	// x�࿡�� �ٶ���� ��

	fDest = max(vDestMin.x, vSourMin.x);
	fSour = min(vDestMax.x, vSourMax.x);

	if (fSour < fDest)
		return false;

	// y�࿡�� �ٶ���� ��

	fDest = max(vDestMin.y, vSourMin.y);
	fSour = min(vDestMax.y, vSourMax.y);

	if (fSour < fDest)
		return false;

	// z�࿡�� �ٶ���� ��

	fDest = max(vDestMin.z, vSourMin.z);
	fSour = min(vDestMax.z, vSourMax.z);

	if (fSour < fDest)
		return false;

	return true;
}

_bool CCalculator::Collision_OBB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
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

	_float		fDistance[3];	// 0�� ; ù��° �ڽ����� �� ���͸� ������ ������ ������Ų ������ ��
								// 1�� ; �ι�° �ڽ����� �� ���͸� ������ ������ ������Ų ������ ��
								// 2�� ; �ڽ��� �ڽ��� ���� ���� �Ÿ��� ������ ������ ������Ų ������ ��		
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

void CCalculator::Set_Point(OBB * pObb, const _vec3 * pMin, const _vec3 * pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	// �޸�
	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f;
}

void CCalculator::Set_Axis(OBB * pObb)
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

