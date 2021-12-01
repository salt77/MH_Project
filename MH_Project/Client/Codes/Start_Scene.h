#ifndef Start_h__
#define Start_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"
#include "Logo.h"
#include "FadeInOut.h"


class CStart_Scene : public CScene
{
private:
	explicit CStart_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStart_Scene();

public:
	virtual HRESULT Ready_Scene() override;
	virtual HRESULT	LateReady_Scene() override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene() override;

private:
	HRESULT				Ready_Layer_Environment(const wstring pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const wstring pLayerTag) { return S_OK; }
	HRESULT				Ready_Layer_UI(const wstring pLayerTag);
	HRESULT				Ready_Prototype();

private:
	void				Ready_to_Logo(const _float& fTimeDelta);

private:
	_bool				m_bReadyFadeOut = false;

	_float				m_fFadeOut = 0.f;

public:
	static CStart_Scene*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void			Free() override;

};


#endif // Start_h__
