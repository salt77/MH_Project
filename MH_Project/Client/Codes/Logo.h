#ifndef Logo_h__
#define Logo_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"
#include "Loading.h"

#include "Player.h"
#include "LoadingBar_BackEffect.h"
#include "LoadingBar_Progress.h"
#include "FadeInOut.h"


class CLogo : public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual HRESULT Ready_Scene(_uint iStageNum);
	virtual HRESULT	LateReady_Scene() override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene() override;

private:
	HRESULT				Ready_Layer_Environment(const wstring pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const wstring pLayerTag) { return S_OK; }
	HRESULT				Ready_Layer_UI(const wstring pLayerTag);
	HRESULT				Ready_Prototype();

private:
	void				Update_ProgressUI(const _float& fTimeDelta);

private:
	CLoading*			m_pLoading = nullptr;

	_uint				m_iStageNum = 0;

	_float				m_fProgressBar = 0.f;
	_float				m_fFadeOut = 0.f;

public:
	static CLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iStageNum = 0);

private:
	virtual void Free() override;

};


#endif // Logo_h__
