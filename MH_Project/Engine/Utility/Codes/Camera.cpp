#include "Camera.h"

USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_vAt(rhs.m_vAt)
	, m_vEye(rhs.m_vEye)
	, m_vUp(rhs.m_vUp)
	, m_fFov(rhs.m_fFov)
	, m_fAspect(rhs.m_fAspect)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
	, m_matView(rhs.m_matView)
	, m_matProj(rhs.m_matProj)
{
}

Engine::CCamera::~CCamera(void)
{
}


HRESULT Engine::CCamera::Ready_Object(void)
{
	CGameObject::Render_Object();

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

Engine::_int Engine::CCamera::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void Engine::CCamera::Free(void)
{
	CGameObject::Free();
}


