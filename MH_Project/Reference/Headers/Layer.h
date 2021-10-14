#ifndef Layer_h__
#define Layer_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer(void);
	virtual ~CLayer(void);

public:
	CGameObject*	Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	CComponent*		Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);
	
public:
	HRESULT		Add_GameObject(const _tchar* pObjTag, CGameObject* pInstance);

	HRESULT		Ready_Layer(void);
	_int		Update_Layer(const _float& fTimeDelta);

public:
	HRESULT		Delete_Layer(const _tchar* pObjTag);

private:
	map<const _tchar*, CGameObject*>		m_mapObject;

	//map<const _tchar*, vector<CGameObject*>>		m_mapObject;

public:
	static CLayer*		Create(void);
	virtual void		Free(void);

};


END
#endif // Layer_h__
