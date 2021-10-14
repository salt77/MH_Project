#ifndef Monster_h__
#define Monster_h__

#include "GameObject.h"

BEGIN(Engine)

class CTriCol;
class CTransform;

END

class CMonster : public CGameObject
{
private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT			Add_Component(void);

private:
	CTriCol*		m_pBufferCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

public:
	static CMonster*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};

#endif // Monster_h__
