#pragma once

#include "afxcmn.h"
#include "afxwin.h"


// CObjectTool 대화 상자입니다.

USING(Engine)

class CObjectTool : public CDialogEx
{
	DECLARE_DYNAMIC(CObjectTool)

public:
	CObjectTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedObjAdd();
	afx_msg void OnBnClickedObjDelete();
	afx_msg void OnEditChangePosX();
	afx_msg void OnEditChangePosY();
	afx_msg void OnEditChangePosZ();
	afx_msg void OnEditChangeRotX();
	afx_msg void OnEditChangeRotY();
	afx_msg void OnEditChangeRotZ();
	afx_msg void OnEditChangeSclX();
	afx_msg void OnEditChangeSclY();
	afx_msg void OnEditChangeSclZ();
	afx_msg void OnBnClickedObjSave();
	afx_msg void OnBnClickedObjLoad();

public:
	HTREEITEM	m_hMainObj, m_hSubObj;
	HTREEITEM	m_hPlayer, m_hAhglan;

	CTreeCtrl m_TreeObjectTool;
	CListBox m_ListObjAdd;
	CComboBox m_ComboObjScene;

	CTerrainTex*	m_pTerrainTex = nullptr;
	CTransform*		m_pTerrainTrans = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;

	CString m_cstrObjName;

	float m_fObjPosX;
	float m_fObjPosY;
	float m_fObjPosZ;

	float m_fObjRotX;
	float m_fObjRotY;
	float m_fObjRotZ;

	float m_fObjSclX;
	float m_fObjSclY;
	float m_fObjSclZ;
};
