#pragma once
#include "afxcmn.h"



// CForm �� ���Դϴ�.
class CTerrainTool;
class CObjectTool;
class CNavmeshTool;
class CCameraTool;
class CCollisionTool;

class CForm : public CFormView
{
	DECLARE_DYNCREATE(CForm)

protected:
	CForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTerrainTool*	m_pTerrainTool = nullptr;
	CObjectTool*	m_pObjectTool = nullptr;
	CNavmeshTool*	m_pNavmeshTool = nullptr;
	CCameraTool*	m_pCameraTool = nullptr;
	CCollisionTool*	m_pCollisionTool = nullptr;

	// TabControl
	CTabCtrl m_Tab;

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTabControl(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDraw(CDC* /*pDC*/);
};


