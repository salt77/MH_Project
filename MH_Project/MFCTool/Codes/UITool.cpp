// ../Codes/Terrain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/UITool.h"
#include "afxdialogex.h"
#include "MFC_Terrain.h"


// CUITool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUITool, CDialogEx)

CUITool::CUITool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
	, m_cstrUIName(_T(""))
{
}

CUITool::~CUITool()
{
}

void CUITool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxUI);
	DDX_Text(pDX, IDC_EDIT_POS_X_UI, m_fPosX);
	DDX_Text(pDX, IDC_EDIT_POS_Y_UI, m_fPosY);
	DDX_Text(pDX, IDC_EDIT_SCALE_X_UI, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y_UI, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT_NAME_UI, m_cstrUIName);
}


BEGIN_MESSAGE_MAP(CUITool, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUITool::OnLbnSelchangeListUI)
	ON_BN_CLICKED(IDC_BUTTON_UI_ADD, &CUITool::OnBnClickedButtonUIAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_UI, &CUITool::OnBnClickedButtonDeleteUI)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_POS, &CUITool::OnBnClickedButtonApplyPos)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_SCALE, &CUITool::OnBnClickedButtonApplyScale)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_UI, &CUITool::OnBnClickedButtonSaveUI)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_UI, &CUITool::OnBnClickedButtonLoadUI)
END_MESSAGE_MAP()



// CUITool 메시지 처리기입니다.

void CUITool::OnLbnSelchangeListUI()
{
	
}


void CUITool::OnBnClickedButtonUIAdd()
{
	
}


void CUITool::OnBnClickedButtonDeleteUI()
{
	
}


void CUITool::OnBnClickedButtonApplyPos()
{
	
}


void CUITool::OnBnClickedButtonApplyScale()
{
	
}


void CUITool::OnBnClickedButtonSaveUI()
{
	
}


void CUITool::OnBnClickedButtonLoadUI()
{
	
}
