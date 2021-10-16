// ../Codes/CollisionTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/CollisionTool.h"
#include "afxdialogex.h"


// CCollisionTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCollisionTool, CDialogEx)

CCollisionTool::CCollisionTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG5, pParent)
	, m_fColScale(1.f)
	, m_AniIndex(0)
	, m_cstrColName(_T(""))
	, m_fColX(0)
	, m_fColY(0)
	, m_fColZ(0)
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
	DDX_Text(pDX, IDC_EDIT1, m_AniIndex);
	DDX_Control(pDX, IDC_LIST_Collider, m_ListBoxCollider);
	DDX_Text(pDX, IDC_EDIT2, m_cstrColName);
	DDX_Text(pDX, IDC_EDIT3_ColX, m_fColX);
	DDX_Text(pDX, IDC_EDIT4_ColY, m_fColY);
	DDX_Text(pDX, IDC_EDIT5_ColZ, m_fColZ);
}


BEGIN_MESSAGE_MAP(CCollisionTool, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOBJ, &CCollisionTool::OnTreeObjSelChanged)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCollisionTool::OnBnClickedObjectAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCollisionTool::OnBnClickedDeleteObject)
	ON_BN_CLICKED(IDC_Btn_AddCollider, &CCollisionTool::OnBnClickedAddCollider)
	ON_LBN_SELCHANGE(IDC_LIST_Collider, &CCollisionTool::OnListSelchangeCollider)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEBONE, &CCollisionTool::OnTreeBoneSelchanged)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CCollisionTool::OnBtnClickedSave)
	ON_EN_CHANGE(IDC_EDIT3_ColX, &CCollisionTool::OnEditChangeColX)
	ON_EN_CHANGE(IDC_EDIT4_ColY, &CCollisionTool::OnEditChangeColY)
	ON_EN_CHANGE(IDC_EDIT5_ColZ, &CCollisionTool::OnEditChangeColZ)
	ON_EN_CHANGE(IDC_EDIT1, &CCollisionTool::OnEditChangeAni)
END_MESSAGE_MAP()


// CCollisionTool 메시지 처리기입니다.


BOOL CCollisionTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hRoot = m_TreeObj.InsertItem(TEXT("Object"), 0, 0, TVI_ROOT, TVI_LAST);
	m_hPlayer = m_TreeObj.InsertItem(TEXT("Player"), 0, 0, m_hRoot, TVI_LAST);
	m_hAhglan = m_TreeObj.InsertItem(TEXT("Ahglan"), 0, 0, m_hRoot, TVI_LAST);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCollisionTool::OnTreeObjSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	// 안 씀
}


void CCollisionTool::OnBnClickedObjectAdd()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (m_TreeObj.GetSelectedItem() == m_hPlayer)
		pToolView->Add_Object(OBJECTADD_MFC_PLAYER);

	// Bone 추가
	_uint iIndex = 0;

	//m_hBoneRoot1 = m_TreeBone.InsertItem((CA2W)pName, 0, 0, TVI_ROOT, TVI_LAST);

	list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList();
	list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();

	for (; iter != listTemp.end(); ++iter)
	{
		char pName[16] = "Bone";
		string pInt = to_string(iIndex);
		const char* pTemp = pInt.c_str();
		strcat_s(pName, sizeof(pName), pTemp);

		HTREEITEM hTemp = m_TreeBone.InsertItem((CA2W)pName);

		for (_uint i = 0; i < (*iter)->dwNumBones; ++i)
		{
			const char* pBoneName = (*iter)->pSkinInfo->GetBoneName(i);

			m_TreeBone.InsertItem((CA2W)pBoneName, 0, 0, hTemp, TVI_LAST);
		}

		++iIndex;
	}
}

void CCollisionTool::OnBnClickedDeleteObject()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (m_TreeObj.GetSelectedItem() == m_hPlayer)
		pToolView->Delete_Object(OBJECTADD_MFC_PLAYER);
}

void CCollisionTool::OnBnClickedAddCollider()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	pToolView->Add_Collider(m_fColScale, m_cstrColName);
	m_ListBoxCollider.AddString(m_cstrColName);

	UpdateData(FALSE);
}


void CCollisionTool::OnListSelchangeCollider()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	m_iListCulSelCol = m_ListBoxCollider.GetCurSel();

	CString cstrName = L"";
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrName);

	pToolView->Set_ChangeColType(cstrName, COL_TRUE);

	for (_int i = 0; i < m_ListBoxCollider.GetCount(); ++i)
	{
		if (i != m_iListCulSelCol)
		{
			m_ListBoxCollider.GetText(i, cstrName);

			pToolView->Set_ChangeColType(cstrName, COL_FALSE);
		}
	}

	UpdateData(FALSE);
}


void CCollisionTool::OnTreeBoneSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	if (!m_TreeBone.ItemHasChildren(hItem))
	{
		// Collider 찾기
		CString cstrListBoxCol;
		m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

		// Bone 찾기
		CString cstrItemName = m_TreeBone.GetItemText(hItem);

		list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList();
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();

		for (; iter != listTemp.end(); )
		{
			for (_uint i = 0; i < (*iter)->dwNumBones; ++i)
			{
				if ((*iter)->pSkinInfo->GetBoneName(i) == cstrItemName)
				{
					pToolView->Set_ColliderMatrix(&(*iter)->pRenderingMatrix[i], cstrListBoxCol);
					break;
				}
			}

			++iter;
		}
	}

	UpdateData(FALSE);
}


void CCollisionTool::OnBtnClickedSave()
{

}


void CCollisionTool::OnEditChangeColX()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolX(m_fColX, cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnEditChangeColY()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolY(m_fColY, cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnEditChangeColZ()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolZ(m_fColZ, cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnEditChangeAni()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	pToolView->Set_ObjectAniIndex(m_AniIndex);

	UpdateData(FALSE);
}
