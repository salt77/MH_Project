#pragma once
#include "afxcmn.h"
#include "afxwin.h"


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

	afx_msg void OnBnClickedAddCollider();

	afx_msg void OnListSelchangeCollider();

	afx_msg void OnTreeBoneSelchanged(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnEditChangeAni();

	afx_msg void OnBnClickedButtonX();
	afx_msg void OnBnClickedButtonY();
	afx_msg void OnBnClickedButtonZ();

	afx_msg void OnBtnClickedSave();
	afx_msg void OnBnClickedButtonLoad();

public:
	CTreeCtrl m_TreeObj;
	CTreeCtrl m_TreeBone;

	HTREEITEM	m_hRoot, m_hPlayer, m_hAhglan;
	//HTREEITEM	m_hBoneRoot1, m_hBoneRoot2, m_hBoneRoot3, m_hBoneRoot4, m_hBoneRoot5, m_hBoneRoot6, m_hBoneRoot7;

	CListBox m_ListBoxCollider;

	CString m_cstrColName;
	float m_fColScale;
	UINT m_AniIndex;

	_int	m_iListCulSelCol;

	float m_fColX;
	float m_fColY;
	float m_fColZ;

	//vector<PS_Collider>	m_vecParsing;
	//PS_Collider*		m_pParsing = nullptr;
};
