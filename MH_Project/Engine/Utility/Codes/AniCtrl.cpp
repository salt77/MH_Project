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
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),// ���� �� ������ü�� ���� �ִ� �ִϸ��̼��� ����
		rhs.m_pAniCtrl->GetMaxNumAnimationSets(),	// ���� �� ���� ������ �ִϸ��̼� ���� �ִ� ����(�밳 ù ��° ���ڰ��� ����)
		rhs.m_pAniCtrl->GetMaxNumTracks(),	// �����Ǿ����� �ִ� Ʈ���� ����(��¥ ���� ����ؾ� �� ��)
		rhs.m_pAniCtrl->GetMaxNumEvents(),	// �ִϸ��̼��� ���� �������� �̺�Ʈ ����(�츮�� ������� ����)
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

	// Ʈ������ �ִϸ��̼� ��(���)�� �����ϴ� �Լ�
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAS);

	// ���� �ִϸ��̼� ���� ��� �Ϸ� �ð��� ��ȯ�ϴ� �Լ�
	m_dPeriod = pAS->GetPeriod();

	// Ʈ�� ��� �� �߻��� �� �ִ� ���� �̺�Ʈ ������ �����ϴ� �Լ�(Ʈ������ ��� ������ �� �� ������ ���ɼ��� �־ �̺�Ʈ ������ ������ ������Ŵ)
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// ���� ������ Ʈ���� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�� �����ϴµ� ������ ������ �� �ִ� �Լ�
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	// ������ Ʈ���� ����ӵ��� �����ϴ� �Լ�
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// ������ Ʈ���� ��� ����ġ�� �����ϴ� �Լ�
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.05f, m_fAccTime, 0.2, D3DXTRANSITION_LINEAR);

	// Ʈ���� Ȱ��ȭ ������ �����ϴ� �Լ�
	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, m_fAniSpeed, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.95f, m_fAccTime, 0.2, D3DXTRANSITION_LINEAR);

	// advancedtime �Լ� ȣ��� �����ϴ� �ð� ���� �ʱ�ȭ
	m_pAniCtrl->ResetTime();
	m_fAccTime = 0.f;

	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_iOldAniIndex = iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniCtrl::Play_Animation(const _float & fTimeDelta)
{
	//AdvanceTime �Լ� ȣ�� ��, ���������� �� �� ���� �ð��� ����
	// �ִϸ��̼� ��ü ��, ���������� �����ϴ� �ð��� reset����� ������ ������ �ʴ´�

	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);	// 2���� : �ִϸ��̼� ���� �� ���峪 ����Ʈ�� ���� ó���� ����ϴ� ��ü �ּ�
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
