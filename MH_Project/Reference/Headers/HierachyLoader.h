#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class CHierachyLoader : public ID3DXAllocateHierarchy
{
private:
	explicit CHierachyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pPath);
	virtual ~CHierachyLoader(void);

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name,
									CONST D3DXMESHDATA *pMeshData,
									CONST D3DXMATERIAL *pMaterials,
									CONST D3DXEFFECTINSTANCE *pEffectInstances,
									DWORD NumMaterials,
									CONST DWORD *pAdjacency,
									LPD3DXSKININFO pSkinInfo,
									LPD3DXMESHCONTAINER *ppNewMeshContainer);

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

private:
	void		Allocate_Name(char** ppName, const char* pFrameName);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	const wstring				m_pPath;

public:
	static CHierachyLoader*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pPath);

public:
	_ulong	Release(void);
};

END