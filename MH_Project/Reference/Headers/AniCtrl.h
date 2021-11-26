#pragma once

#include "Engine_Define.h"
#include "Base.h"

#include "HierachyLoader.h"

BEGIN(Engine)

class ENGINE_DLL CAniCtrl : public CBase
{
private:
	explicit CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	explicit CAniCtrl(const CAniCtrl& rhs);
	virtual ~CAniCtrl(void);

public:
	LPD3DXANIMATIONCONTROLLER		Get_AnimationCtrl(void) { return m_pAniCtrl; }
	const	_float&					Get_AniFrameTime() { return m_fAccTime; }
	const	_double					Get_AniFrameEndTime() { return m_dPeriod / (m_fAniSpeed * 1.1f); }
	const	_float&					Get_AniSpeed() { return m_fAniSpeed; }

public:
	void			Set_TrackSpeed(_float fSpeed) { m_fAniSpeed = fSpeed; }
	void			Set_ResetAniTime() { m_pAniCtrl->ResetTime(); m_fAccTime = 0.f; }

public:
	HRESULT			Ready_AniCtrl(void);
	void			Set_AnimationIndex(const _uint& iIndex);
	void			Play_Animation(const _float& fTimeDelta);
	_bool			Is_AnimationSetFinish(const _float& fEndTime);

private:
	LPD3DXANIMATIONCONTROLLER				m_pAniCtrl;

	_uint									m_iCurrentTrack;
	_uint									m_iNewTrack;
	_float									m_fAccTime;			// 애니메이션 재생 시간을 저장하기 위한 변수
	_uint									m_iOldAniIndex;		// 기존에 재생되던 애니메이션 셋 인덱스를 저장하여 예외처리를 하기 위해 사용하는 변수
	_double									m_dPeriod;			// 애니메이션 셋 하나당 가지는 실제 총 재생시간을 저장하기 위한 변수

	// 추가적인 변수들
	_float									m_fAniSpeed = 2.f;

public:
	static CAniCtrl*		Create(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	static CAniCtrl*		Create(const CAniCtrl& rhs);
	virtual void			Free(void);
};

END
