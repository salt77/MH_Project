#include "AniCtrl.h"

USING(Engine)

CAniCtrl::CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl)
	: m_pAniCtrl(pAniCtrl)
	, m_iCurrentTrack(0)
	, m_iNewTrack(1)
	, m_fAccTime(0.f)
	, m_iOldAniIndex(999)
	, m_dPeriod(0.0)
{
	m_pAniCtrl->AddRef();
}

CAniCtrl::CAniCtrl(const CAniCtrl & rhs)
	: m_iCurrentTrack(rhs.m_iCurrentTrack)
	, m_iNewTrack(rhs.m_iNewTrack)
	, m_fAccTime(rhs.m_fAccTime)
	, m_iOldAniIndex(rhs.m_iOldAniIndex)
	, m_dPeriod(0.0)
{
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),// 복제 시 원본객체가 가진 최대 애니메이션의 개수
		rhs.m_pAniCtrl->GetMaxNumAnimationSets(),	// 복제 시 구동 가능한 애니메이션 셋의 최대 개수(대개 첫 번째 인자값과 동일)
		rhs.m_pAniCtrl->GetMaxNumTracks(),	// 제공되어지는 최대 트랙의 개수(진짜 많이 사용해야 두 개)
		rhs.m_pAniCtrl->GetMaxNumEvents(),	// 애니메이션이 지닌 내부적인 이벤트 정보(우리는 사용하지 않음)
		&m_pAniCtrl);
}

CAniCtrl::~CAniCtrl()
{
}


HRESULT CAniCtrl::Ready_AniCtrl(void)
{
	return S_OK;
}

void CAniCtrl::Set_AnimationIndex(const _uint & iIndex)
{
	if (m_iOldAniIndex == iIndex)
		return;

	m_iNewTrack = (m_iCurrentTrack == 0 ? 1 : 0);

	LPD3DXANIMATIONSET		pAS = nullptr;

	m_pAniCtrl->GetAnimationSet(iIndex, &pAS);
	// m_pAniCtrl->GetAnimationSetByName(, &pAS);

	// 트랙위에 애니메이션 셋(모션)을 설정하는 함수
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAS);

	// 현재 애니메이션 셋의 재생 완료 시간을 반환하는 함수
	m_dPeriod = pAS->GetPeriod();

	// 트랙 재생 시 발생할 수 있는 각종 이벤트 정보를 해제하는 함수(트랙끼리 모션 보간을 할 때 실패의 가능성이 있어서 이벤트 정보를 강제로 해제시킴)
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// 현재 지정한 트랙을 활성화 또는 비활성화를 결정하는데 시점을 지정할 수 있는 함수
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	// 지정된 트랙의 재생속도를 지정하는 함수
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// 지정된 트랙의 재생 가중치를 지정하는 함수
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.05f, m_fAccTime, 0.2, D3DXTRANSITION_LINEAR);

	// 트랙의 활성화 유무를 결정하는 함수
	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, m_fAniSpeed, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.95f, m_fAccTime, 0.2, D3DXTRANSITION_LINEAR);

	// advancedtime 함수 호출시 증가하던 시간 값을 초기화
	m_pAniCtrl->ResetTime();
	m_fAccTime = 0.f;

	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_iOldAniIndex = iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniCtrl::Play_Animation(const _float & fTimeDelta)
{
	//AdvanceTime 함수 호출 시, 내부적으로 알 수 없는 시간이 증가
	// 애니메이션 교체 시, 내부적으로 증가하는 시간을 reset해줘야 문제가 생기지 않는다

	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);	// 2인자 : 애니메이션 동작 시 사운드나 이펙트에 대한 처리를 담당하는 객체 주소
	m_fAccTime += fTimeDelta;
}

_bool CAniCtrl::Is_AnimationsetFinish(void)
{
	D3DXTRACK_DESC		TrackInfo;
	ZeroMemory(&TrackInfo, sizeof(D3DXTRACK_DESC));

	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);

	if (TrackInfo.Position >= m_dPeriod - 0.1)
		return true;

	return false;
}

CAniCtrl * CAniCtrl::Create(LPD3DXANIMATIONCONTROLLER pAniCtrl)
{
	CAniCtrl*	pInstance = new CAniCtrl(pAniCtrl);

	if (FAILED(pInstance->Ready_AniCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

CAniCtrl * CAniCtrl::Create(const CAniCtrl & rhs)
{
	CAniCtrl*	pInstance = new CAniCtrl(rhs);

	if (FAILED(pInstance->Ready_AniCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

void CAniCtrl::Free(void)
{
}
