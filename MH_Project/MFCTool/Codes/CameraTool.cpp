// ../Codes/CameraTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/CameraTool.h"
#include "afxdialogex.h"


// CCameraTool ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCameraTool, CDialogEx)

CCameraTool::CCameraTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

CCameraTool::~CCameraTool()
{
}

void CCameraTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCameraTool, CDialogEx)
END_MESSAGE_MAP()


// CCameraTool �޽��� ó�����Դϴ�.
