// ../Codes/CollisionTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/CollisionTool.h"
#include "afxdialogex.h"


// CCollisionTool ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCollisionTool, CDialogEx)

CCollisionTool::CCollisionTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG5, pParent)
	, m_fColScale(1.f)
{

}

CCollisionTool::~CCollisionTool()
{
}

void CCollisionTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREEOBJ, m_TreeObj);
	DDX_Control(pDX, IDC_TREEBONE, m_TreeBone);
	DDX_Text(pDX, IDC_EDIT_ColScale, m_fColScale);
}


BEGIN_MESSAGE_MAP(CCollisionTool, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOBJ, &CCollisionTool::OnTreeObjSelChanged)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCollisionTool::OnBnClickedObjectAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCollisionTool::OnBnClickedDeleteObject)
	ON_BN_CLICKED(IDC_Btn_ApplyCollider, &CCollisionTool::OnBnClickedApplyCollider)
	ON_BN_CLICKED(IDC_Btn_AddCollider, &CCollisionTool::OnBnClickedAddCollider)
END_MESSAGE_MAP()


// CCollisionTool �޽��� ó�����Դϴ�.


BOOL CCollisionTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hRoot = m_TreeObj.InsertItem(TEXT("Object"), 0, 0, TVI_ROOT, TVI_LAST);
	m_hPlayer = m_TreeObj.InsertItem(TEXT("Player"), 0, 0, m_hRoot, TVI_LAST);
	m_hAhglan = m_TreeObj.InsertItem(TEXT("Ahglan"), 0, 0, m_hRoot, TVI_LAST);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CCollisionTool::OnTreeObjSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	// �� ��
}


void CCollisionTool::OnBnClickedObjectAdd()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (m_TreeObj.GetSelectedItem() == m_hPlayer)
		pToolView->Add_Object(OBJECTADD_MFC_PLAYER);
}

void CCollisionTool::OnBnClickedDeleteObject()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (m_TreeObj.GetSelectedItem() == m_hPlayer)
		pToolView->Delete_Object(OBJECTADD_MFC_PLAYER);
}

void CCollisionTool::OnBnClickedApplyCollider()
{
}

void CCollisionTool::OnBnClickedAddCollider()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	pToolView->Add_Collider(m_fColScale);

	UpdateData(FALSE);
}
