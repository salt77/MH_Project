// ../Codes/ObjectTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/ObjectTool.h"
#include "afxdialogex.h"


// CObjectTool ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CObjectTool, CDialogEx)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CObjectTool::~CObjectTool()
{
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialogEx)
END_MESSAGE_MAP()


// CObjectTool �޽��� ó�����Դϴ�.
