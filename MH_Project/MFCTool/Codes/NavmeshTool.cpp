// ../Codes/CNavmeshTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/NavmeshTool.h"
#include "afxdialogex.h"


// CNavmeshTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNavmeshTool, CDialogEx)

CNavmeshTool::CNavmeshTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
}

CNavmeshTool::~CNavmeshTool()
{
}

void CNavmeshTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeNavmesh);
}


BEGIN_MESSAGE_MAP(CNavmeshTool, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CNavmeshTool::OnTreeSelChangeNavmesh)
	ON_BN_CLICKED(IDC_BUTTON_DeleteNav, &CNavmeshTool::OnBnClickedDeleteNav)
	ON_BN_CLICKED(IDC_BUTTON_DeleteAllNav, &CNavmeshTool::OnBnClickedDeleteAllNav)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CNavmeshTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CNavmeshTool::OnBnClickedLoad)
END_MESSAGE_MAP()


// CNavmeshTool 메시지 처리기입니다.


void CNavmeshTool::OnTreeSelChangeNavmesh(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);



	*pResult = 0;
}


void CNavmeshTool::OnBnClickedDeleteNav()
{

}


void CNavmeshTool::OnBnClickedDeleteAllNav()
{
	
}


void CNavmeshTool::OnBnClickedSave()
{

}


void CNavmeshTool::OnBnClickedLoad()
{

}
