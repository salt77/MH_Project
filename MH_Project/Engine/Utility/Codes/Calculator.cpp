#include "Calculator.h"

#include "Transform.h"
#include "TerrainTex.h"

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
	

	// 오른쪽 위 삼각형
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
													const CTransform* pTerrainTransCom)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	
	_vec3		vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);
	
	// 윈도우 좌표를 투영 좌표로 변환
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 좌표에서 뷰스페이스로 변환
	_matrix		matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰스페이스에서 월드 스페이스로 변환
	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드스페이스에서 로컬 스페이스로 변환

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


	for (_ulong i = 0; i < dwVtXCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtXCntX - 1; ++j)
		{
			_ulong dwIndex = i * dwVtXCntX + j;

			// 오른쪽 위 삼각형

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
							0.f,
							pTerrainVtx[dwVtxIdx[1]].z + fV * (pTerrainVtx[dwVtxIdx[2]].z - pTerrainVtx[dwVtxIdx[1]].z));
			}

			// 왼쪽 아래 삼각형

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
							0.f,
							pTerrainVtx[dwVtxIdx[2]].z + fV * (pTerrainVtx[dwVtxIdx[0]].z - pTerrainVtx[dwVtxIdx[2]].z));
			}

		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

