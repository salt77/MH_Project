#include "stdafx.h"
#include "Buff_Player.h"
#include "Player_SlotUI.h"

#include "Export_Function.h"

CBuff_Player::CBuff_Player(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CBuff_Player::~CBuff_Player()
{
}


HRESULT CBuff_Player::Ready_Object()
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);

	return S_OK;
}

HRESULT CBuff_Player::LateReady_Object()
{
	FAILED_CHECK_RETURN(CUI::LateReady_Object(), E_FAIL);

	return S_OK;
}

_int CBuff_Player::Update_Object(const _float& fTimeDelta)
{
	_int iExit = CUI::Update_Object(fTimeDelta);

	return iExit;
}

_int CBuff_Player::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CUI::LateUpdate_Object(fTimeDelta);

	return _int();
}

void CBuff_Player::Render_Object()
{
}

void CBuff_Player::Blink_Icon(const _float& fTimeDelta)
{
	if (m_bBuffOn)
	{
		if (0 == m_iBlinkOneCycle)
		{
			if (0.f < m_fValueRatio)
			{
				m_fValueRatio -= m_fAlphaInterpol * fTimeDelta;
			}
			else
			{
				++m_iBlinkOneCycle;
			}
		}
		else
		{
			if (0.85f > m_fValueRatio)
			{
				m_fValueRatio += m_fAlphaInterpol * fTimeDelta;
			}
			else
			{
				m_iBlinkOneCycle = 0;
			}
		}
	}
}

void CBuff_Player::Free()
{
	CUI::Free();
}

