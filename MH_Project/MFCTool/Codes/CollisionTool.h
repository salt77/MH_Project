#pragma once
#include "afxcmn.h"


// CCollisionTool ��ȭ �����Դϴ�.

class CCollisionTool : public CDialogEx
{
	DECLARE_DYNAMIC(CCollisionTool)

public:
	CCollisionTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCollisionTool();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
