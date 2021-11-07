#pragma once
#include "afxwin.h"


// CUITool 대화 상자입니다.

class CUITool : public CDialogEx
{
	DECLARE_DYNAMIC(CUITool)

public:
	CUITool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUITool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnSelchangeListUI();

	afx_msg void OnBnClickedButtonUIAdd();
	afx_msg void OnBnClickedButtonDeleteUI();

	afx_msg void OnBnClickedButtonApplyPos();
	afx_msg void OnBnClickedButtonApplyScale();

	afx_msg void OnBnClickedButtonSaveUI();
	afx_msg void OnBnClickedButtonLoadUI();

public:
	CListBox m_ListBoxUI;
	
	CString m_cstrUIName;

	float m_fPosX;
	float m_fPosY;
	float m_fScaleX;
	float m_fScaleY;
};
