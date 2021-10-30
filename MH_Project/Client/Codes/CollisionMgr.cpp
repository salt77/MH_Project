#include "stdafx.h"
#include "CollisionMgr.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}


void CCollisionMgr::Collision_Box(const wstring & _HitLayerTag, const wstring & _DamagedLayerTag)
{

}

void CCollisionMgr::Collision_Sphere(const wstring & _HitLayerTag, const wstring & _DamagedLayerTag)
{

}


void CCollisionMgr::Free(void)
{
}
