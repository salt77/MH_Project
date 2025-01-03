#ifndef Stage_h__
#define Stage_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"

#include "Player.h"
#include "Ahglan.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "StageMesh.h"
#include "HitBox_Pos.h"
#include "FadeInOut.h"

class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene() override;
	virtual HRESULT LateReady_Scene() override;
	virtual _int    Update_Scene(const _float& fTimeDelta) override;
	virtual	_int	LateUpdate_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene() override;

private:
	HRESULT				Ready_Layer_Environment(const wstring pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const wstring pLayerTag);
	HRESULT				Ready_Layer_UI(const wstring pLayerTag);
	HRESULT				Ready_Layer_Effect(const wstring pLayerTag);
	HRESULT				Ready_Layer_Balista(const wstring pLayerTag);
	HRESULT				Ready_Prototype();
	HRESULT				Ready_LightInfo();

	HRESULT				Load_PlayerCol();
	HRESULT				Load_AhglanCol();
	HRESULT				Load_Navimesh();

private:
	_ulong					m_dwRenderCnt = 0;
	_tchar					m_szFPS[256] = L"FPS : 0";
	_float					m_fTime = 0.f;

	vector<_matrix>			m_vecSavePoint;

public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free() override;

};


#endif // Stage_h__
