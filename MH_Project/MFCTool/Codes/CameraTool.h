#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CCameraTool 대화 상자입니다.
class CMainFrame;
class CMFCToolView;
class CMFC_Camera;

class CCameraTool : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraTool)

public:
	CCameraTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnListEyeSelChange();
	afx_msg void OnListAtSelChange();

	afx_msg void OnBnClickedEyeAdd();
	afx_msg void OnBnClickedDeleteEye();

	afx_msg void OnBnClickedAddAt();
	afx_msg void OnBnClickedDeleteAt();

	afx_msg void OnDeltaposSpinSectionSpeed(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnComboSelChangeCamType();

	afx_msg void OnBnClickedSaveCam();
	afx_msg void OnBnClickedLoadCam();

public:
	_uint m_iEyeTagIndex = 0;
	_uint m_iAtTagIndex = 0;

	float m_fEyeX;
	float m_fEyeY;
	float m_fEyeZ;
	float m_fAtX;
	float m_fAtY;
	float m_fAtZ;
	float m_fSectionSpeed;

	CListBox m_ListEye;
	CListBox m_ListAt;

	CSpinButtonCtrl m_spSpeed;

	CMFC_Camera*	m_pCamera = nullptr;
	CComboBox		m_ComboCamType;
};
