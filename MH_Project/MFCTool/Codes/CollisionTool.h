#pragma once
#include "afxcmn.h"


// CCollisionTool 대화 상자입니다.

class CCollisionTool : public CDialogEx
{
	DECLARE_DYNAMIC(CCollisionTool)

public:
	CCollisionTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCollisionTool();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnTreeObjSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedObjectAdd();
	afx_msg void OnBnClickedDeleteObject();
	afx_msg void OnBnClickedApplyCollider();
	afx_msg void OnBnClickedAddCollider();

public:
	CTreeCtrl m_TreeObj;
	CTreeCtrl m_TreeBone;

	HTREEITEM	m_hRoot, m_hPlayer, m_hAhglan;

	float m_fColScale;
};
