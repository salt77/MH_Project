// ../Codes/Terrain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/TerrainTool.h"
#include "afxdialogex.h"


// CTerrainTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTerrainTool, CDialogEx)

CTerrainTool::CTerrainTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_bRenderSoild(FALSE)
	, m_bRenderWireFrame(false)
	, m_bRenderNone(false)
	, m_iRowX(0)
	, m_iColZ(0)
	, m_iInterval(0)
	, m_iDetail(0)
{
}

CTerrainTool::~CTerrainTool()
{
}

void CTerrainTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_bRenderSoild);
	DDX_Text(pDX, IDC_EDIT1, m_iRowX);
	DDX_Text(pDX, IDC_EDIT2, m_iColZ);
	DDX_Text(pDX, IDC_EDIT3, m_iInterval);
	DDX_Text(pDX, IDC_EDIT4, m_iDetail);
}


BEGIN_MESSAGE_MAP(CTerrainTool, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTerrainTool::OnBnClickedApply)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &CTerrainTool::RadioCtrl)
	ON_BN_CLICKED(IDC_BUTTON2, &CTerrainTool::OnBnClickedApplyRenderType)
END_MESSAGE_MAP()


// CTerrainTool 메시지 처리기입니다.


void CTerrainTool::OnBnClickedApply()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	pToolView->Add_NewTerrain(m_iRowX, m_iColZ, m_iInterval);

	UpdateData(FALSE);
	Invalidate(FALSE);
}

void CTerrainTool::RadioCtrl(UINT ID)
{
	switch (ID)
	{
	case IDC_RADIO1:
		m_iRenderType = 0;
		break;
	case IDC_RADIO2:
		m_iRenderType = 1;
		break;
	case IDC_RADIO3:
		m_iRenderType = 2;
		break;
	}

	Invalidate(FALSE);
}


void CTerrainTool::OnBnClickedApplyRenderType()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	pToolView->Set_RenderTerrain(m_iRenderType);
}
