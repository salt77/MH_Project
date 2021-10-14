#ifndef Calculator_h__
#define Calculator_h__

#include "Component.h"

BEGIN(Engine)
class CTransform;
class CTerrainTex;

class ENGINE_DLL CCalculator : public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9  pGraphicDev);
	explicit CCalculator(const CCalculator& rhs);
	virtual ~CCalculator(void);

public:
	HRESULT			Ready_Calculator(void);
	_float			Compute_HeightOnTerrain(const _vec3* pPos, 
											const _vec3* pTerrainVtx, 
											const _ulong& dwCntX,
											const _ulong& dwCntZ,
											const _ulong& dwVtxItv = 1);

	_vec3			Picking_OnTerrain(HWND hWnd, const CTerrainTex* pTerrainBufferCom, const CTransform* pTerrainTransCom);

public:
	static CCalculator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*		Clone(void);
private:
	virtual void	Free(void);
};

END
#endif // Calculator_h__
