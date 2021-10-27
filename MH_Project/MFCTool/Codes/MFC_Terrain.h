#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;

END

class CMFC_Terrain final : public CGameObject
{
public:
	explicit CMFC_Terrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMFC_Terrain(const CMFC_Terrain& rhs);
	virtual ~CMFC_Terrain();

public:
	virtual HRESULT Ready_Object(_uint iGrass = 0);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;

private:
	HRESULT		Add_Component();
	HRESULT		Setup_Material();

public:
	void		Set_RenderType(RENDERTYPE_MFC _eRenderType) { m_eRenderType = _eRenderType; }

private:
	_uint			m_iGrass = 0;

	CTerrainTex*	m_pBufferCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;

private:
	RENDERTYPE_MFC	m_eRenderType = RENDERTYPE_MFC_END;

public:
	static	CMFC_Terrain*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iGrass = 0);
	virtual void			Free();
};

