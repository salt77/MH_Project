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

	if (!m_bActionCam)
	{
		D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	}
	else
	{
		if (_vec3(0.f, 0.f, 0.f) != m_vStalkTarget)
		{
			_vec3 vDir = m_vStalkTarget - m_vActionEye;
			D3DXVec3Normalize(&vDir, &vDir);

			m_vActionEye += vDir * m_fSpeed;

			D3DXMatrixLookAtLH(&m_matView, &m_vActionEye, &m_vActionAt, &m_vUp);

			if (D3DXVec3Length(&(m_vStalkTarget - m_vEye)) < 0.5f)
			{
				m_vStalkTarget = _vec3(0.f, 0.f, 0.f);
				//m_bActionCam = false;
			}
		}

		else
		{
			m_bActionCam = false;
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

_vec3 & CCamera::Get_CamLook()
{
	_vec3 vEye = m_vEye;
	vEye.y = m_vAt.y;
	_vec3 vDir = m_vAt - vEye;

	return *D3DXVec3Normalize(&vDir, &vDir);
}

void Engine::CCamera::Free(void)
{
	CGameObject::Free();
}


