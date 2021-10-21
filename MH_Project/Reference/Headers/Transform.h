#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform(void);

public:
	void				Set_Scale(const _float& fX, const _float& fY, const _float& fZ);
	void				Set_Pos(const _float& fX, const _float& fY, const _float& fZ);
	void				Set_Pos(const _vec3* pPos);
	void				Set_WorldMatrix(const _matrix* pWorld);
	//void				Set_Dir(INFO eDirInfo, _vec3 vTargetDir);

public:
	const _matrix*		Get_WorldMatrix(void) const { return &m_matWorld; }
	void				Get_WorldMatrix(_matrix* pMatrix) const;
	void				Get_INFO(INFO eType, _vec3* pInfo);
	const _vec3*		Get_Info(INFO eType);

public:
	void				Move_Pos(const _vec3* pDir, const _float& fSpeed, const _float& fTimeDelta);
	void				Rotation(ROTATION eType, const _float& fAngle);
	void				Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	const _matrix*		Compute_LookAtTarget(const _vec3* pTargetPos);

public:
	HRESULT				Ready_Transform(void);
	virtual _int		Update_Component(const _float& fTimeDelta);

public:
	_vec3			m_vInfo[INFO_END];
	_vec3			m_vScale;
	_vec3			m_vAngle;
	_matrix			m_matWorld;

public:
	static CTransform*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual	CComponent*		Clone(void);

private:
	virtual void	Free(void);
};


END
#endif // Transform_h__
