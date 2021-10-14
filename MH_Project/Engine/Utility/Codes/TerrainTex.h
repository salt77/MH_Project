#ifndef TerrainTex_h__
#define TerrainTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
private:
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex(void);

public:
	const _vec3*		Get_VtxPos(void) const { return m_pPos; }
	// 높이맵의 픽셀의 개수를 정점의 개수와 일치시켜놓았기 때문에 정점의 개수라고 봐도 무방(수업 코드 기준)
	_ulong				Get_VtxCntX(void) const { return m_iH.biWidth; }
	_ulong				Get_VtxCntZ(void) const { return m_iH.biHeight; }

	void				Copy_Indices(INDEX32* pIndex, const _ulong& dwTriCnt); 

public:
	HRESULT Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv) ;
	virtual void Render_Buffer(void) override;

private:
	HANDLE					m_hFile;
	BITMAPFILEHEADER		m_fH;
	BITMAPINFOHEADER		m_iH;

	_vec3*					m_pPos = nullptr;

public:
	static CTerrainTex*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
								const _ulong& dwCntX, 
								const _ulong& dwCntZ, 
								const _ulong& dwVtxItv = 1);

	virtual void Free(void) override;
	virtual CComponent* Clone(void) override;	
};

END
#endif // TerrainTex_h__
