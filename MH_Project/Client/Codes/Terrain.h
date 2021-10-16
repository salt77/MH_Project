#ifndef Terrain_h__
#define Terrain_h__

#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CTerrain : public CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT				Add_Component(void);
	HRESULT				SetUp_Material(void);

private:
	CTerrainTex*		m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};

#endif // Terrain_h__
