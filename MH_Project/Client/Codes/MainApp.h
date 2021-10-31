#ifndef MainApp_h__
#define MainApp_h__

#include "Define.h"
#include "Base.h"
#include "Engine_Define.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END
class CMainApp : public CBase
{
private:		// 생성자, 소멸자
	explicit CMainApp(void);
	virtual ~CMainApp(void);

	// public 함수, 변수
public:
	HRESULT			Ready_MainApp(void);
	_int			Update_MainApp(const _float& fTimeDelta);
	_int			LateUpdate_MainApp(const _float& fTimeDelta);
	void			Render_MainApp(void);
	// protected 함수, 변수
	// private 함수, 변수

private:
	HRESULT				SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT				Ready_Scene(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement);

private:
	CGraphicDev*			m_pDeviceClass = nullptr;
	CManagement*			m_pManagementClass = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;

public:
	static CMainApp*		Create(void);		// 생성 함수
	virtual	void			Free(void);			// 소멸 함수
};

#endif // MainApp_h__

