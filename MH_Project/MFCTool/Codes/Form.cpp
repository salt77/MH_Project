// ../Codes/Form.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/Form.h"

#include "TerrainTool.h"
#include "ObjectTool.h"
#include "NavmeshTool.h"
#include "CameraTool.h"
#include "CollisionTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CForm

IMPLEMENT_DYNCREATE(CForm, CFormView)

CForm::CForm()
	: CFormView(IDD_FORM)
{
}

CForm::~CForm()
{
	delete m_pTerrainTool;
	m_pTerrainTool = nullptr;

	delete m_pObjectTool;
	m_pObjectTool = nullptr;

	delete m_pNavmeshTool;
	m_pNavmeshTool = nullptr;

	delete m_pCameraTool;
	m_pCameraTool = nullptr;

	delete m_pCollisionTool;
	m_pCollisionTool = nullptr;
}

void CForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

BEGIN_MESSAGE_MAP(CForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CForm::OnTabControl)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CForm 진단입니다.

#ifdef _DEBUG
void CForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CForm 메시지 처리기입니다.



void CForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	//SetTimer(1, 30, NULL);

	m_Tab.InsertItem(0, _T("Terrain"));
	m_Tab.InsertItem(1, _T("Object"));
	m_Tab.InsertItem(2, _T("Navmesh"));
	m_Tab.InsertItem(3, _T("Camera"));
	m_Tab.InsertItem(4, _T("Collision"));

	m_Tab.SetCurSel(0);

	CRect rect;
	m_Tab.GetWindowRect(&rect);

	m_pTerrainTool = new CTerrainTool;
	m_pTerrainTool->Create(IDD_DIALOG1, &m_Tab);
	m_pTerrainTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	m_pTerrainTool->ShowWindow(SW_SHOW);

	m_pObjectTool = new CObjectTool;
	m_pObjectTool->Create(IDD_DIALOG2, &m_Tab);
	m_pObjectTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	//m_pObjectTool->ShowWindow(SW_SHOW);

	m_pNavmeshTool = new CNavmeshTool;
	m_pNavmeshTool->Create(IDD_DIALOG3, &m_Tab);
	m_pNavmeshTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	//m_pNavmeshTool->ShowWindow(SW_SHOW);

	m_pCameraTool = new CCameraTool;
	m_pCameraTool->Create(IDD_DIALOG4, &m_Tab);
	m_pCameraTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	//m_pCameraTool->ShowWindow(SW_SHOW);

	m_pCollisionTool = new CCollisionTool;
	m_pCollisionTool->Create(IDD_DIALOG5, &m_Tab);
	m_pCollisionTool->MoveWindow(0, 25, rect.Width(), rect.Height());
	//m_pCameraTool->ShowWindow(SW_SHOW);
}


void CForm::OnTabControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSel = m_Tab.GetCurSel();

	switch (iSel)
	{
	case 0:
		m_pTerrainTool->ShowWindow(SW_SHOW);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_SHOW);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_SHOW);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 3:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_SHOW);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 4:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}


//void CForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	switch (nChar)
//	{
//	case VK_ESCAPE:
//		DestroyWindow();
//		break;
//	}
//
//	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
//}


void CForm::OnDraw(CDC* /*pDC*/)
{
	int iSel = m_Tab.GetCurSel();

	switch (iSel)
	{
	case 0:
		m_pTerrainTool->ShowWindow(SW_SHOW);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_SHOW);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_SHOW);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 3:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_SHOW);
		m_pCollisionTool->ShowWindow(SW_HIDE);
		break;
	case 4:
		m_pTerrainTool->ShowWindow(SW_HIDE);
		m_pObjectTool->ShowWindow(SW_HIDE);
		m_pNavmeshTool->ShowWindow(SW_HIDE);
		m_pCameraTool->ShowWindow(SW_HIDE);
		m_pCollisionTool->ShowWindow(SW_SHOW);
		break;
	}

	//Invalidate(FALSE);
}
