#ifndef Stage_1_h__
#define Stage_1_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"

#include "Player.h"
#include "Ahglan.h"
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
	virtual ~CStage_1(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual HRESULT LateReady_Scene() override;
	virtual _int    Update_Scene(const _float& fTimeDelta) override;
	virtual	_int	LateUpdate_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene(void) override;

private:
	HRESULT				Ready_Layer_Environment(const wstring pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const wstring pLayerTag);
	HRESULT				Ready_Layer_UI(const wstring pLayerTag);
	HRESULT				Ready_Prototype(void);
	HRESULT				Ready_LightInfo(void);

	HRESULT				Load_ObjectInfo();
	HRESULT				Load_PlayerCol();
	HRESULT				Load_Navimesh();

private:
	_ulong					m_dwRenderCnt = 0;
	_tchar					m_szFPS[256] = L"0";
	_float					m_fTime = 0.f;

	vector<_matrix>			m_vecSavePoint;

public:
	static CStage_1*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void) override;

};


#endif // Stage_1_h__