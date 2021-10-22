#ifndef Frustum_h__
#define Frustum_h__

#include "Base.h"
#include "Engine_Define.h"
#include "QuadTree.h"

BEGIN(Engine)

class ENGINE_DLL CFrustum : public	CBase
{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFrustum(void);

public:
	LPDIRECT3DDEVICE9		Get_GraphicDev(void) { return m_pGraphicDev; }

public:
	HRESULT					Ready_Frustum(void);
	_bool					Isin_Frustum(const _vec3* pPos);
	_bool					Isin_Frustum(const _vec3* pPos, const _float& fRadius);
	_bool					Isin_Frustum_ForObject(const _vec3* pWorldPos, const _float& fRadius);
	void					Isin_Frustum_ForTerrain(const _vec3* pVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, INDEX32* pIndex, _ulong* pTriCnt, CQuadTree* pQuadTree = nullptr);


private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_vec3					m_vPoint[8];
	D3DXPLANE				m_Plane[6];

public:
	static CFrustum*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};

END
#endif // Frustum_h__
