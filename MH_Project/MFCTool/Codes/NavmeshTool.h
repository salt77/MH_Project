#pragma once
#include "afxcmn.h"


// CNavmeshTool ��ȭ �����Դϴ�.

class CNavmeshTool : public CDialogEx
{
	DECLARE_DYNAMIC(CNavmeshTool)

public:
	CNavmeshTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNavmeshTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTreeSelChangeNavmesh(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDeleteNav();
	afx_msg void OnBnClickedDeleteAllNav();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();

public:
	CTreeCtrl m_TreeNavmesh;
};
