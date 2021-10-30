#pragma once

#include "Base.h"

#include "Engine_Define.h"
#include "Engine_Macro.h"

USING(Engine)

class CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr);

private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	void	Collision_Box(const wstring & _HitLayerTag, const wstring & _DamagedLayerTag);
	void	Collision_Sphere(const wstring & _HitLayerTag, const wstring & _DamagedLayerTag);

public:
	virtual void Free(void) override;
};

