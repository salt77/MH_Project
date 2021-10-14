#ifndef Logo_h__
#define Logo_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"
#include "Loading.h"

#include "Player.h"
#include "Monster.h"

class CLogo : public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;

private:
	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }
	HRESULT				Ready_Prototype(void);

private:
	CLoading*			m_pLoading = nullptr;

public:
	static CLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // Logo_h__
