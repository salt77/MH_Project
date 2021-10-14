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
}


BEGIN_MESSAGE_MAP(CNavmeshTool, CDialogEx)
END_MESSAGE_MAP()


// CNavmeshTool 메시지 처리기입니다.
