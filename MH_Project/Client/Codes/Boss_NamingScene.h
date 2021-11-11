#pragma once

#include "UI.h"

#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

class CBoss_NamingScene final : public CUI
{
private:
	explicit CBoss_NamingScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss_NamingScene(void);

public:
	virtual HRESULT Ready_Object(_float fX, _float fY, _float fSizeX, _float fSizeY, NAMINGSCENE_ID eID, BOSS_ID eBossId);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

protected:
	HRESULT				Add_Component(void);
	virtual HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	
private:
	void				FadeInOut(const _float& fTimeDelta);

private:
	_bool				m_bOneTimeCheck = false;

	_uint				m_iFadeCount = 0;

	_ulong				m_dwBossNameInStart = GetTickCount();
	_ulong				m_dwBossNameInDelay = 2500;

	_ulong				m_dwBackOutTime = GetTickCount();
	_ulong				m_dwBackOutDelay = 2500;
	_ulong				m_dwBossNameOutTime = GetTickCount();
	_ulong				m_dwBossNameOutDelay = 2000;

	NAMINGSCENE_ID		m_eNamingId = NAMING_END;
	BOSS_ID				m_eBossId = BOSS_END;

public:
	static CBoss_NamingScene*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY, NAMINGSCENE_ID eID, BOSS_ID eBossId);
	virtual void		Free(void);
};
