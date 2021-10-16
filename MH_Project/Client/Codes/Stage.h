#ifndef Stage_h__
#define Stage_h__

#include "Define.h"
#include "Scene.h"

#include "BackGround.h"

#include "Player.h"
//#include "Monster.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
//#include "Stone.h"

class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;

private:
	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT				Ready_Prototype(void);
	HRESULT				Ready_LightInfo(void);

public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // Stage_h__
