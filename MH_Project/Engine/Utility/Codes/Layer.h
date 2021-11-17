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
	CGameObject*	Get_GameObject(const wstring pLayerTag, const wstring pObjTag);
	CComponent*		Get_Component(const wstring pObjTag, const wstring pComponentTag, COMPONENTID eID);
	const	map<const wstring, CGameObject*>&	Get_MapObject() { return m_mapObject; }
	
public:
	HRESULT		Add_GameObject(const wstring pObjTag, CGameObject* pInstance);

	HRESULT		Ready_Layer(void);
	_int		Update_Layer(const _float& fTimeDelta);
	_int		LateUpdate_Layer(const _float& fTimeDelta);
	_uint		Render_Layer(const _float& fTimeDelta);

public:
	HRESULT		Delete_Layer(const wstring pObjTag);

private:
	map<const wstring, CGameObject*>		m_mapObject;

public:
	static CLayer*		Create(void);
	virtual void		Free(void);
};

END

#endif // Layer_h__