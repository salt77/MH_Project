#include "stdafx.h"
#include "MFC_Camera.h"


CMFC_Camera::CMFC_Camera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{
}

CMFC_Camera::CMFC_Camera(const CMFC_Camera & rhs)
	: CCamera(rhs)
{
}

CMFC_Camera::~CMFC_Camera()
{
}


HRESULT CMFC_Camera::Ready_Object(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CMFC_Camera::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CCamera::Update_Object(fTimeDelta);

	if (!m_bActionCam)
		Key_Input(fTimeDelta);

	if (!m_bActionCam)
		m_pTransformCom->Set_Pos(&m_vEye);
	else
		m_pTransformCom->Set_Pos(&m_vActionEye);

	return iExit;
}

HRESULT CMFC_Camera::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

void CMFC_Camera::Key_Input(const _float & fTimeDelta)
{
	_matrix		matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	if (GetAsyncKeyState('T') & 0x8000)
	{
		m_bCamMove = true;
	}
	else if (GetAsyncKeyState('G') & 0x8000)
	{
		m_bCamMove = false;
	}

	if (m_bCamMove)
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			_vec3	vLook;
			memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

			_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

			m_vEye += vLength;
			m_vAt += vLength;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			_vec3	vLook;
			memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

			_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

			m_vEye -= vLength;
			m_vAt -= vLength;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			_vec3	vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));

			_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

			m_vEye += vLength;
			m_vAt += vLength;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			_vec3	vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));

			_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

			m_vEye -= vLength;
			m_vAt -= vLength;
		}

		if (GetAsyncKeyState('Q') & 0x8000)
		{
			_vec3	vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vAt -= *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;
		}
		if (GetAsyncKeyState('E') & 0x8000)
		{
			_vec3	vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vAt += *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;
		}

		if (GetAsyncKeyState('R') & 0x8000)
		{
			_vec3	vUp;
			memcpy(&vUp, &matWorld.m[1][0], sizeof(_vec3));
			D3DXVec3Normalize(&vUp, &vUp);

			m_vAt += *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
		}
		if (GetAsyncKeyState('F') & 0x8000)
		{
			_vec3	vUp;
			memcpy(&vUp, &matWorld.m[1][0], sizeof(_vec3));
			D3DXVec3Normalize(&vUp, &vUp);

			m_vAt -= *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
		}

		if (GetAsyncKeyState('Z') & 0x8000)
		{
			_vec3	vUp;
			memcpy(&vUp, &matWorld.m[1][0], sizeof(_vec3));
			D3DXVec3Normalize(&vUp, &vUp);

			m_vEye += *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
			m_vAt += *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
		}
		if (GetAsyncKeyState('X') & 0x8000)
		{
			_vec3	vUp;
			memcpy(&vUp, &matWorld.m[1][0], sizeof(_vec3));
			D3DXVec3Normalize(&vUp, &vUp);

			m_vEye -= *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
			m_vAt -= *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;
		}
	}
}

CMFC_Camera * CMFC_Camera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CMFC_Camera* pInstance = new CMFC_Camera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMFC_Camera::Free(void)
{
	CCamera::Free();
}
