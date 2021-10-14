#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh : public CComponent
{
private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh();

public:
	HRESULT		Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void		Render_Meshes();

private:
	LPD3DXMESH				m_pMesh;		// 노말정보를 삽입한 메쉬 정보를 담고 있는 객체
	LPD3DXMESH				m_pOriMesh;		// 최초 로드 시점의 메쉬 정보를 담고 있는 객체

	LPD3DXBUFFER			m_pAdjacency;	// 메쉬가 지닌 정점을 근거로 이뤄진 폴리곤 정보를 통해 법선을 생성하기 위한 값
	LPD3DXBUFFER			m_pSubset;		// 메쉬를 구성하는 파츠격에 해당하는 정보
											// 서브셋의 개수 == 재질의 개수 == 텍스처의 개수
	D3DXMATERIAL*			m_pMtrl;		// 텍스처의 이름과 재질의 색상 정보를 보관하기 위한 구조체
	_ulong					m_dwSubsetCnt;	// 서브셋의 개수
	LPDIRECT3DTEXTURE9*		m_ppTexture;	// 여러장의 메쉬 텍스처를 동적 배열의 형태로 보관하기 위한 포인터

	_vec3*					m_pVtxPos;		// 메쉬가 지닌 정점의 위치값들을 동적 할당(배열)한 메모리 공간에 보관하기 위한 포인터
	_ulong					m_dwStride;		// 정점의 크기를 저장하기 위한 변수
	_ulong					m_dwVtxCnt;		// 메쉬가 지닌 정점의 개수를 저장


public:
	static	CStaticMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent * Clone(void) override;

private:
	virtual void	Free();
};

END