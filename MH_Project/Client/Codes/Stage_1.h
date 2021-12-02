#ifndef Stage_1_h__
#define Stage_1_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"

#include "Player.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "StageMesh_1.h"
#include "HitBox_Pos.h"
#include "FadeInOut.h"

class CStage_1 : public CScene
{
private:
	explicit CStage_1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_1();

public:
	virtual HRESULT Ready_Scene() override;
	virtual HRESULT LateReady_Scene() override;
	virtual _int    Update_Scene(const _float& fTimeDelta) override;
	virtual	_int	LateUpdate_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene() override;

public:
	void				Set_ReadyLoadStage() { m_bReadyToLoadStage = true; }

private:
	HRESULT				Ready_Layer_Environment(const wstring pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const wstring pLayerTag);
	HRESULT				Ready_Layer_UI(const wstring pLayerTag);
	HRESULT				Ready_Layer_Effect(const wstring pLayerTag);
	HRESULT				Ready_Layer_Balista(const wstring pLayerTag);
	HRESULT				Ready_Layer_Box(const wstring pLayerTag);
	HRESULT				Ready_Layer_Symbol(const wstring pLayerTag);
	HRESULT				Ready_Layer_Trigger(const wstring pLayerTag);


	HRESULT				Ready_Prototype();
	HRESULT				Ready_LightInfo();

	HRESULT				Load_PlayerInfo();
	HRESULT				Load_DogInfo();
	HRESULT				Load_SoldierInfo();
	HRESULT				Load_KnightInfo();
	HRESULT				Load_CloyanInfo();
	HRESULT				Load_PlayerCol();
	HRESULT				Load_NaviMesh();

public:
	HRESULT				Load_NextStage();
	HRESULT				Delete_ForNextStage();

private:
	_bool					m_bReadyToLoadStage = false;

	_ulong					m_dwRenderCnt = 0;
	_tchar					m_szFPS[256] = L"FPS : 0";
	_float					m_fTime = 0.f;

	_float					m_fFadeOutRatio = 0.f;

	vector<_matrix>			m_vecSavePoint;

	CLayer*					m_pLayer = nullptr;
	CLayer*					m_pSpawnLayer = nullptr;
	CLayer*					m_pEnemyLayer = nullptr;

public:
	static CStage_1*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free() override;
};


#endif // Stage_1_h__
