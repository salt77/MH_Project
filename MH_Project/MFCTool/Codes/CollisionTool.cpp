// ../Codes/CollisionTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/CollisionTool.h"
#include "afxdialogex.h"
#include "MFC_Player.h"
#include "MFC_Ahglan.h"


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
	, m_fBoxMinX(0)
	, m_fBoxMinY(0)
	, m_fBoxMinZ(0)
	, m_fBoxMaxX(0)
	, m_fBoxMaxY(0)
	, m_fBoxMaxZ(0)
	, m_fBoxCreateTime(0)
	, m_fBoxDeleteTime(0)
	, m_fAniTime(0)
	, m_fAniEndTime(0)
{
	//m_vecParsing.reserve(16);
}

CCollisionTool::~CCollisionTool()
{
	//if (m_pParsing)
	//	Safe_Delete(m_pParsing);
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
	DDX_Control(pDX, IDC_COMBO_COLTAG, m_ComboBox);
	DDX_Text(pDX, IDC_EDIT_MIN_X, m_fBoxMinX);
	DDX_Text(pDX, IDC_EDIT_MIN_Y, m_fBoxMinY);
	DDX_Text(pDX, IDC_EDIT_MIN_Z, m_fBoxMinZ);
	DDX_Text(pDX, IDC_EDIT_MAX_X, m_fBoxMaxX);
	DDX_Text(pDX, IDC_EDIT_MAX_Y, m_fBoxMaxY);
	DDX_Text(pDX, IDC_EDIT_MAX_Z, m_fBoxMaxZ);
	DDX_Text(pDX, IDC_EDIT_CREATE_TIME, m_fBoxCreateTime);
	DDX_Text(pDX, IDC_EDIT_DELETE_TIME, m_fBoxDeleteTime);
	DDX_Control(pDX, IDC_LIST2, m_ListBoxCol);
	DDX_Text(pDX, IDC_EDIT3, m_fAniTime);
	DDX_Text(pDX, IDC_EDIT4, m_fAniEndTime);
	DDX_Control(pDX, IDC_EDIT3, m_EditFrameTime);
	DDX_Control(pDX, IDC_EDIT4, m_EditEndTime);
}


BEGIN_MESSAGE_MAP(CCollisionTool, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOBJ, &CCollisionTool::OnTreeObjSelChanged)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCollisionTool::OnBnClickedObjectAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCollisionTool::OnBnClickedDeleteObject)
	ON_BN_CLICKED(IDC_Btn_AddCollider, &CCollisionTool::OnBnClickedAddCollider)
	ON_LBN_SELCHANGE(IDC_LIST_Collider, &CCollisionTool::OnListSelchangeCollider)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEBONE, &CCollisionTool::OnTreeBoneSelchanged)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CCollisionTool::OnBtnClickedSave)
	ON_EN_CHANGE(IDC_EDIT1, &CCollisionTool::OnEditChangeAni)
	ON_BN_CLICKED(IDC_BUTTON_Y, &CCollisionTool::OnBnClickedButtonY)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, &CCollisionTool::OnBnClickedButtonX)
	ON_BN_CLICKED(IDC_BUTTON_Z, &CCollisionTool::OnBnClickedButtonZ)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CCollisionTool::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_CREATETIME_OKI, &CCollisionTool::OnBnClickedCreateTimeOK)
	ON_BN_CLICKED(IDC_BUTTON_DELETETIME_OK, &CCollisionTool::OnBnClickedDeleteTimeOK)
	ON_LBN_SELCHANGE(IDC_LIST2, &CCollisionTool::OnListSelChangeBoxCol)
	ON_BN_CLICKED(IDC_BUTTON_Delete_Col, &CCollisionTool::OnBnClickedDeleteCol)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCollisionTool 메시지 처리기입니다.


BOOL CCollisionTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(1, 10, NULL);

	m_hRoot = m_TreeObj.InsertItem(TEXT("Object"), 0, 0, TVI_ROOT, TVI_LAST);
	m_hPlayer = m_TreeObj.InsertItem(TEXT("Player"), 0, 0, m_hRoot, TVI_LAST);
	m_hAhglan = m_TreeObj.InsertItem(TEXT("Ahglan"), 0, 0, m_hRoot, TVI_LAST);

	m_ComboBox.AddString(L"Sphere_Damaged");
	m_ComboBox.AddString(L"Sphere_Hit");
	m_ComboBox.AddString(L"Box_Damaged");
	m_ComboBox.AddString(L"Box_Hit");

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
	else if (m_TreeObj.GetSelectedItem() == m_hAhglan)
		pToolView->Add_Object(OBJECTADD_MFC_AHGLAN);

	// Bone 추가
	_uint iIndex = 0;

	//m_hBoneRoot1 = m_TreeBone.InsertItem((CA2W)pName, 0, 0, TVI_ROOT, TVI_LAST);

	if (m_TreeObj.GetSelectedItem() == m_hPlayer)
	{
		list<D3DXMESHCONTAINER_DERIVED*>		listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_PLAYER);
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();

		for (; iter != listTemp.end(); ++iter)
		{
			char pName[16] = "Player_Bone";
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
	else if (m_TreeObj.GetSelectedItem() == m_hAhglan)
	{
		list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_AHGLAN);
		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();

		for (; iter != listTemp.end(); ++iter)
		{
			char pName[16] = "Ahglan_Bone";
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

	COLLIDERTYPE eSel = (COLLIDERTYPE)m_ComboBox.GetCurSel();
	
	HTREEITEM hObjItem = m_TreeObj.GetSelectedItem();
	CString cstrObjName = m_TreeObj.GetItemText(hObjItem);

	OBJECTADD_MFC eObjType;
	if (L"Player" == cstrObjName)
		eObjType = OBJECTADD_MFC_PLAYER;
	else if (L"Ahglan" == cstrObjName)
		eObjType = OBJECTADD_MFC_AHGLAN;

	switch (eSel)
	{
	case Engine::COLTYPE_BOX_DAMAGED:
		pToolView->Add_Collider(m_fBoxMinX, m_fBoxMinY, m_fBoxMinZ, m_fBoxMaxX, m_fBoxMaxY, m_fBoxMaxZ, (wstring)m_cstrColName, eSel, eObjType);
		m_ListBoxCol.AddString(m_cstrColName);
		break;

	case Engine::COLTYPE_BOX_HIT:
		pToolView->Add_Collider(m_fBoxMinX, m_fBoxMinY, m_fBoxMinZ, m_fBoxMaxX, m_fBoxMaxY, m_fBoxMaxZ, (wstring)m_cstrColName, eSel, eObjType);
		m_ListBoxCol.AddString(m_cstrColName);
		break;

	case Engine::COLTYPE_SPHERE_DAMAGED:
		pToolView->Add_Collider(m_fColScale, (wstring)m_cstrColName, eSel, eObjType);
		m_ListBoxCollider.AddString(m_cstrColName);
		break;

	case Engine::COLTYPE_SPHERE_HIT:
		pToolView->Add_Collider(m_fColScale, (wstring)m_cstrColName, eSel, eObjType);
		m_ListBoxCollider.AddString(m_cstrColName);
		break;

	default:
		MessageBoxW(L"Collider Type Not Selected", L"Error", MB_OK);
		break;
	}

	UpdateData(FALSE);
}


void CCollisionTool::OnBnClickedDeleteCol()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	HTREEITEM hItemObj = m_TreeObj.GetSelectedItem();
	CString cstrObjName = m_TreeObj.GetItemText(hItemObj);

	COLLIDERTYPE eSel = (COLLIDERTYPE)m_ComboBox.GetCurSel();
	_uint iColSel = -1;
	CString cstrColName = L"";

	switch (eSel)
	{
	case Engine::COLTYPE_BOX_DAMAGED:
		iColSel = m_ListBoxCol.GetCurSel();
		m_ListBoxCol.GetText(iColSel, cstrColName);
		m_ListBoxCol.DeleteString(iColSel);

		break;

	case Engine::COLTYPE_BOX_HIT:
		iColSel = m_ListBoxCol.GetCurSel();
		m_ListBoxCol.GetText(iColSel, cstrColName);
		m_ListBoxCol.DeleteString(iColSel);

		break;

	case Engine::COLTYPE_SPHERE_DAMAGED:
		iColSel = m_ListBoxCollider.GetCurSel();
		m_ListBoxCollider.GetText(iColSel, cstrColName);
		m_ListBoxCollider.DeleteString(iColSel);

		break;

	case Engine::COLTYPE_SPHERE_HIT:
		iColSel = m_ListBoxCollider.GetCurSel();
		m_ListBoxCollider.GetText(iColSel, cstrColName);
		m_ListBoxCollider.DeleteString(iColSel);

		break;
	}

	if (L"Player" == cstrObjName)
	{
		pToolView->Delete_Collider((wstring)cstrColName, eSel, OBJECTADD_MFC_PLAYER);
	}

	else if (L"Ahglan" == cstrObjName)
	{
		pToolView->Delete_Collider((wstring)cstrColName, eSel, OBJECTADD_MFC_AHGLAN);
	}

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

	pToolView->Set_ChangeColType((wstring)cstrName, COL_TRUE);

	for (_int i = 0; i < m_ListBoxCollider.GetCount(); ++i)
	{
		if (i != m_iListCulSelCol)
		{
			m_ListBoxCollider.GetText(i, cstrName);

			pToolView->Set_ChangeColType((wstring)cstrName, COL_FALSE);
		}
	}

	UpdateData(FALSE);
}



void CCollisionTool::OnListSelChangeBoxCol()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	m_iListCulSelBoxCollider = m_ListBoxCol.GetCurSel();

	CString cstrName = L"";
	m_ListBoxCol.GetText(m_iListCulSelBoxCollider, cstrName);

	pToolView->Set_ChangeColType((wstring)cstrName, COL_TRUE, COLTYPE_BOX_HIT);

	for (_int i = 0; i < m_ListBoxCol.GetCount(); ++i)
	{
		if (i != m_iListCulSelBoxCollider)
		{
			m_ListBoxCol.GetText(i, cstrName);

			pToolView->Set_ChangeColType((wstring)cstrName, COL_FALSE, COLTYPE_BOX_HIT);
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
		CString cstrListSphereCol, cstrListBoxCol;
		if (m_ListBoxCollider.GetCount() != 0)
			m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListSphereCol);
		if (m_ListBoxCol.GetCount() != 0)
			m_ListBoxCol.GetText(m_iListCulSelBoxCollider, cstrListBoxCol);

		// Bone 찾기
		CString cstrItemName = m_TreeBone.GetItemText(hItem);

		HTREEITEM hObjItem = m_TreeObj.GetSelectedItem();
		CString cstrObjName = m_TreeObj.GetItemText(hObjItem);

		list<D3DXMESHCONTAINER_DERIVED*>	listTemp;
		if (L"Player" == cstrObjName)
		{
			listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_PLAYER);
		}
		else if (L"Ahglan" == cstrObjName)
		{
			listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_AHGLAN);
		}
		else
		{
			MessageBoxW(L"Not Selected Obj", L"Error", MB_OK);

			return;
		}

		list<D3DXMESHCONTAINER_DERIVED*>::iterator	iter = listTemp.begin();

		for (; iter != listTemp.end(); )
		{
			for (_uint i = 0; i < (*iter)->dwNumBones; ++i)
			{
				if ((*iter)->pSkinInfo->GetBoneName(i) == cstrItemName)
				{
					COLLIDERTYPE eColType = (COLLIDERTYPE)m_ComboBox.GetCurSel();

					switch (eColType)
					{
					case Engine::COLTYPE_BOX_DAMAGED:
						if (L"Player" == cstrObjName)
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", (wstring)cstrListBoxCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListBoxCol, eColType, OBJECTADD_MFC_PLAYER);
						}
						else if (L"Ahglan" == cstrObjName)
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", (wstring)cstrListBoxCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListBoxCol, eColType, OBJECTADD_MFC_AHGLAN);
						}

						break;

					case Engine::COLTYPE_BOX_HIT:
						if (L"Player" == cstrObjName)
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", (wstring)cstrListBoxCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListBoxCol, eColType, OBJECTADD_MFC_PLAYER);
						}
						else if (L"Ahglan" == cstrObjName)
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", (wstring)cstrListBoxCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListBoxCol, eColType, OBJECTADD_MFC_AHGLAN);
						}

						break;

					case Engine::COLTYPE_SPHERE_DAMAGED:
						if (L"Player" == cstrObjName)
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", (wstring)cstrListSphereCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListSphereCol, eColType, OBJECTADD_MFC_PLAYER);
						}
						else if (L"Ahglan" == cstrObjName)
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", (wstring)cstrListSphereCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListSphereCol, eColType, OBJECTADD_MFC_AHGLAN);
						}

						break;

					case Engine::COLTYPE_SPHERE_HIT:
						if (L"Player" == cstrObjName)
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", (wstring)cstrListSphereCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListSphereCol, eColType, OBJECTADD_MFC_PLAYER);
						}
						else if (L"Ahglan" == cstrObjName)
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", (wstring)cstrListSphereCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
							pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListSphereCol, eColType, OBJECTADD_MFC_AHGLAN);
						}

						break;
					}

					break;
				}
			}

			++iter;
		}
	}

	UpdateData(FALSE);
}


void CCollisionTool::OnEditChangeAni()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	HTREEITEM hItem = m_TreeObj.GetSelectedItem();

	if (!m_TreeObj.ItemHasChildren(hItem))
	{
		CString cstrObjName = m_TreeObj.GetItemText(hItem);

		if (L"Player" == cstrObjName)
			pToolView->Set_ObjectAniIndex(m_AniIndex, OBJECTADD_MFC_PLAYER);
		else if (L"Ahglan" == cstrObjName)
			pToolView->Set_ObjectAniIndex(m_AniIndex, OBJECTADD_MFC_AHGLAN);
	}

	UpdateData(FALSE);
}


void CCollisionTool::OnBnClickedButtonX()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolX(m_fColX, (wstring)cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnBnClickedButtonY()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolY(m_fColY, (wstring)cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnBnClickedButtonZ()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	// Collider 찾기
	CString cstrListBoxCol;
	m_ListBoxCollider.GetText(m_iListCulSelCol, cstrListBoxCol);

	pToolView->Set_ColliderMatrixInterpolZ(m_fColZ, (wstring)cstrListBoxCol);

	UpdateData(FALSE);
}


void CCollisionTool::OnBtnClickedSave()
{
	CFileDialog Dlg(FALSE,// FALSE가 다른이름으로 저장. 
		L"dat",
		L"*.dat",
		OFN_OVERWRITEPROMPT,
		L"Data File(*.dat) | *.dat||",
		this, 0, 0);
	TCHAR szFilePath[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szFilePath);
	//PathRemoveFileSpec(szFilePath);

	//lstrcat(szFilePath, L"\\Data\\Terrain\\");

	/*if (!PathIsDirectory(szFilePath))
	{
	PathRemoveFileSpec(szFilePath);
	PathRemoveFileSpec(szFilePath);
	PathRemoveFileSpec(szFilePath);
	PathRemoveFileSpec(szFilePath);
	lstrcat(szFilePath, L"\\Data\\Terrain\\");
	}*/
	Dlg.m_ofn.lpstrInitialDir = szFilePath;

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		HTREEITEM hItem = m_TreeObj.GetSelectedItem();
		CString cstrItemName = m_TreeObj.GetItemText(hItem);

		map<const wstring, CCollider*>	mapTemp;
		map<const wstring, CCollider*>::iterator	iter;
		map<const wstring, CBoxCollider*>	mapBoxTemp;
		map<const wstring, CBoxCollider*>::iterator		iterBox;

		if (L"Player" == cstrItemName)
		{
			CMFC_Player* pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

			mapTemp = pPlayer->Get_ColliderMap();
			iter = mapTemp.begin();

			mapBoxTemp = pPlayer->Get_BoxColliderMap();
			iterBox = mapBoxTemp.begin();
		}

		else if (L"Ahglan" == cstrItemName)
		{
			CMFC_Ahglan* pAhglan = dynamic_cast<CMFC_Ahglan*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Ahglan"));

			mapTemp = pAhglan->Get_ColliderMap();
			iter = mapTemp.begin();

			mapBoxTemp = pAhglan->Get_BoxColliderMap();
			iterBox = mapBoxTemp.begin();
		}
		
		else
		{
			MessageBoxW(L"Error", L"TreeObj 제대로 선택", MB_OK);
		}

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		_uint iSize = mapTemp.size();
		_uint iBoxSize = mapBoxTemp.size();

		DWORD dwbyte = 0;
		DWORD dwStringSize = 0;
		DWORD dwStringSize2 = 0;

		// Size 저장용
		WriteFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
		WriteFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

		//m_pParsing = new PS_Collider;
		//PS_Collider tParsing;
		_float fRadius;
		CString cstrColName, cstrBoneName;

		for (; iter != mapTemp.end(); ++iter)
		{
			//tParsing.wstrColName = iter->first;
			//tParsing.fRadius = iter->second->Get_Radius();
			//tParsing.wstrBoneName = iter->second->Get_BoneName();
			fRadius = iter->second->Get_Radius();
			cstrColName = iter->first.c_str();
			cstrBoneName = iter->second->Get_BoneName().c_str();
			
			dwStringSize = (cstrColName.GetLength() + 1) * sizeof(TCHAR);//tParsing.wstrColName.size();
			dwStringSize2 = (cstrBoneName.GetLength() + 1) * sizeof(TCHAR);//tParsing.wstrBoneName.size();

			WriteFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, cstrColName.GetString(), dwStringSize, &dwbyte, nullptr);
			WriteFile(hFile, cstrBoneName.GetString(), dwStringSize2, &dwbyte, nullptr);
			WriteFile(hFile, &fRadius, sizeof(_float), &dwbyte, nullptr);
			WriteFile(hFile, &iter->second->Get_ColldierType(), sizeof(COLLIDERTYPE), &dwbyte, nullptr);
		}

		for (; iterBox != mapBoxTemp.end(); ++iterBox)
		{
			cstrColName = iterBox->first.c_str();
			cstrBoneName = iterBox->second->Get_BoneName().c_str();

			dwStringSize = (cstrColName.GetLength() + 1) * sizeof(TCHAR);
			dwStringSize2 = (cstrBoneName.GetLength() + 1) * sizeof(TCHAR);

			WriteFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, cstrColName.GetString(), dwStringSize, &dwbyte, nullptr);
			WriteFile(hFile, cstrBoneName.GetString(), dwStringSize2, &dwbyte, nullptr);
			WriteFile(hFile, &iterBox->second->Get_Min(), sizeof(_vec3), &dwbyte, nullptr);
			WriteFile(hFile, &iterBox->second->Get_Max(), sizeof(_vec3), &dwbyte, nullptr);
			WriteFile(hFile, &iterBox->second->Get_ColldierType(), sizeof(COLLIDERTYPE), &dwbyte, nullptr);
		}

		//TILEINFO m_tTielInfo;
		//m_tTielInfo = m_pTerrain->Tile_Save();
		//m_tTielInfo.bMap_Collider = m_Radio_Collider.GetCheck() ? true : false;

		//WriteFile(hFile, &m_tTielInfo.m_iFVF, sizeof(_uint), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.m_iVertexCountX, sizeof(_uint), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.m_iVertexCountZ, sizeof(_uint), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.m_iVertexSize, sizeof(_uint), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.m_iIndexSize, sizeof(_uint), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.m_IndexFormat, sizeof(D3DFORMAT), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.ivecSize[0], sizeof(int), &dwbyte, nullptr);
		//WriteFile(hFile, &m_tTielInfo.ivecSize[1], sizeof(int), &dwbyte, nullptr);
		//for (int i = 0; i < m_tTielInfo.ivecSize[0]; i++)
		//	WriteFile(hFile, &m_tTielInfo.pVertices[i], sizeof(VTX_MFCTEXTURE), &dwbyte, nullptr);
		//for (int i = 0; i < m_tTielInfo.ivecSize[1]; i++)
		//	WriteFile(hFile, &m_tTielInfo.pIndices[i], sizeof(INDEX16), &dwbyte, nullptr);

		CloseHandle(hFile);
	}
	else
	{
		pMain = nullptr;
		pView = nullptr;
	}

	return;
}


void CCollisionTool::OnBnClickedButtonLoad()
{
	CFileDialog Dlg(TRUE,// FALSE가 다른이름으로 저장. 
		L"dat",
		L"*.dat",
		OFN_OVERWRITEPROMPT,
		0, 0, 0, 0);
	TCHAR szFilePath[MAX_PATH]{};

	//GetCurrentDirectory(MAX_PATH, szFilePath);
	//PathRemoveFileSpec(szFilePath);

	//lstrcat(szFilePath, L"\\Data\\Terrain\\");
	//if (!PathIsDirectory(szFilePath))
	//{
	//	PathRemoveFileSpec(szFilePath);
	//	PathRemoveFileSpec(szFilePath);
	//	PathRemoveFileSpec(szFilePath);
	//	PathRemoveFileSpec(szFilePath);
	//	lstrcat(szFilePath, L"\\Data\\Terrain\\");
	//}
	//Dlg.m_ofn.lpstrInitialDir = szFilePath;

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	//m_pParsing = new PS_Collider;
	//PS_Collider		tParsing;

	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwbyte = 0;
		DWORD dwStringSize = 0;
		DWORD dwStringSize2 = 0;
		_float fRadius = 0.f;
		TCHAR*	pNameBuff = nullptr;
		TCHAR*	pNameBuff2 = nullptr;
		CString cstrColName, cstrBoneName;
		wstring wstrBoneName;
		COLLIDERTYPE	eColliderType;
		_vec3	vMin = {};
		_vec3	vMax = {};

		_uint iSize = 0;
		_uint iBoxSize = 0;

		ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);
		ReadFile(hFile, &iBoxSize, sizeof(_uint), &dwbyte, nullptr);

		for (_uint i = 0; i < iSize; ++i)
		{
			if (0 == dwbyte)
				break;

			ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
			ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

			pNameBuff = new TCHAR[dwStringSize];
			pNameBuff2 = new TCHAR[dwStringSize2];

			ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
			ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
			ReadFile(hFile, &fRadius, sizeof(_float), &dwbyte, nullptr);
			ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

			cstrColName = pNameBuff;
			cstrBoneName = pNameBuff2;

			// Bone 찾기
			HTREEITEM hItem = m_TreeObj.GetSelectedItem();
			CString hItemName = m_TreeObj.GetItemText(hItem);

			if (L"Player" == hItemName)
			{
				CMFC_Player* pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

				pPlayer->Add_Collider(fRadius, pNameBuff, eColliderType);
				m_ListBoxCollider.AddString(pNameBuff);

				list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_PLAYER);
				list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

				for (; iterList != listTemp.end(); ++iterList)
				{
					for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
					{
						_tchar	pTemp[64] = L"";
						MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

						if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!StrCmpCW(pTemp, pNameBuff2))
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", cstrColName.operator LPCWSTR(), ID_STATIC))->Set_BoneName(cstrBoneName.operator LPCWSTR());
							pToolView->Set_ColliderMatrix((*iterList)->ppCombinedTransformMatrix[i], cstrColName.operator LPCWSTR(), eColliderType);

							break;
						}
					}
				}
			}
			
			else if (L"Ahglan" == hItemName)
			{
				CMFC_Ahglan* pAhglan = dynamic_cast<CMFC_Ahglan*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Ahglan"));

				pAhglan->Add_Collider(fRadius, pNameBuff, eColliderType);
				m_ListBoxCollider.AddString(pNameBuff);

				list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_AHGLAN);
				list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

				for (; iterList != listTemp.end(); ++iterList)
				{
					for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
					{
						_tchar	pTemp[64] = L"";
						MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

						if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!StrCmpCW(pTemp, pNameBuff2))
						{
							dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", cstrColName.operator LPCWSTR(), ID_STATIC))->Set_BoneName(cstrBoneName.operator LPCWSTR());
							pToolView->Set_ColliderMatrix((*iterList)->ppCombinedTransformMatrix[i], cstrColName.operator LPCWSTR(), eColliderType, OBJECTADD_MFC_AHGLAN);

							break;
						}
					}
				}
			}

			else
			{
				MessageBoxW(L"Error", L"TreeObj 제대로 선택", MB_OK);
			}

			Safe_Delete(pNameBuff);
			Safe_Delete(pNameBuff2);
		}

		for (_uint i = 0; i < iBoxSize; ++i)
		{
			if (0 == dwbyte)
				break;

			ReadFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
			ReadFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);

			pNameBuff = new TCHAR[dwStringSize];
			pNameBuff2 = new TCHAR[dwStringSize2];

			ReadFile(hFile, pNameBuff, dwStringSize, &dwbyte, nullptr);
			ReadFile(hFile, pNameBuff2, dwStringSize2, &dwbyte, nullptr);
			ReadFile(hFile, &vMin, sizeof(_vec3), &dwbyte, nullptr);
			ReadFile(hFile, &vMax, sizeof(_vec3), &dwbyte, nullptr);
			ReadFile(hFile, &eColliderType, sizeof(COLLIDERTYPE), &dwbyte, nullptr);

			cstrColName = pNameBuff;
			cstrBoneName = pNameBuff2;

			// Bone 찾기
			HTREEITEM hItem = m_TreeObj.GetSelectedItem();
			CString hItemName = m_TreeObj.GetItemText(hItem);

			if (L"Player" == hItemName)
			{
				CMFC_Player* pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

				pPlayer->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff, eColliderType);
				m_ListBoxCol.AddString(pNameBuff);

				// Bone 찾기
				list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_PLAYER);
				list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

				for (; iterList != listTemp.end(); ++iterList)
				{
					for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
					{
						_tchar	pTemp[64] = L"";
						MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

						if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!StrCmpCW(pTemp, pNameBuff2))
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", cstrColName.operator LPCWSTR(), ID_STATIC))->Set_BoneName(cstrBoneName.operator LPCWSTR());
							//::g_wstrBoneName = m_pParsing->wstrBoneName;
							pToolView->Set_ColliderMatrix((*iterList)->ppCombinedTransformMatrix[i], cstrColName.operator LPCWSTR(), eColliderType);

							break;
						}
					}
				}
			}

			else if (L"Ahglan" == hItemName)
			{
				CMFC_Ahglan* pAhglan = dynamic_cast<CMFC_Ahglan*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Ahglan"));

				pAhglan->Add_Collider(vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z, pNameBuff, eColliderType);
				m_ListBoxCol.AddString(pNameBuff);

				// Bone 찾기
				list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList(OBJECTADD_MFC_PLAYER);
				list<D3DXMESHCONTAINER_DERIVED*>::iterator	iterList = listTemp.begin();

				for (; iterList != listTemp.end(); ++iterList)
				{
					for (_uint i = 0; i < (*iterList)->dwNumBones; ++i)
					{
						_tchar	pTemp[64] = L"";
						MultiByteToWideChar(CP_ACP, 0, (*iterList)->pSkinInfo->GetBoneName(i), strlen((*iterList)->pSkinInfo->GetBoneName(i)), pTemp, 64);

						if (/*(*iterList)->pSkinInfo->GetBoneName(i)*/!StrCmpCW(pTemp, pNameBuff2))
						{
							dynamic_cast<CBoxCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", cstrColName.operator LPCWSTR(), ID_STATIC))->Set_BoneName(cstrBoneName.operator LPCWSTR());
							//::g_wstrBoneName = m_pParsing->wstrBoneName;
							pToolView->Set_ColliderMatrix((*iterList)->ppCombinedTransformMatrix[i], cstrColName.operator LPCWSTR(), eColliderType);

							break;
						}
					}
				}
			}

			else
			{
				MessageBoxW(L"Error", L"TreeObj 제대로 선택", MB_OK);
			}

			Safe_Delete(pNameBuff);
			Safe_Delete(pNameBuff2);
		}

		UpdateData(FALSE);

		CloseHandle(hFile);
	}

	pToolView->Invalidate(FALSE);

	return;
}


void CCollisionTool::OnBnClickedCreateTimeOK()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCollisionTool::OnBnClickedDeleteTimeOK()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCollisionTool::OnTimer(UINT_PTR nIDEvent)
{
	HTREEITEM		hItem;
	CString			cstrObjName;
	CDynamicMesh*	pMeshCom = nullptr;
	CGameObject*	pGameObj = nullptr;
	WCHAR			pBuff;

	_uint iComboSel = m_ComboBox.GetCurSel();

	switch (nIDEvent)
	{
	case 1:
		hItem = m_TreeObj.GetSelectedItem();

		if (!m_TreeObj.ItemHasChildren(hItem))
		{
			cstrObjName = m_TreeObj.GetItemText(hItem);

			if (L"Player" == cstrObjName)
			{
				pMeshCom = static_cast<CDynamicMesh*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Mesh", ID_STATIC));

				if (pMeshCom && 
					iComboSel != 0 && 
					iComboSel != 1)
				{
					UpdateData(TRUE);

					m_fAniTime = pMeshCom->Get_AniFrameTime();
					m_fAniEndTime = (_float)pMeshCom->Get_AniFrameEndTime() * 0.5f;

					pBuff = (WCHAR)m_fAniTime;
					m_EditFrameTime.SetWindowText(&pBuff);
					pBuff = (WCHAR)m_fAniEndTime;
					m_EditEndTime.SetWindowText(&pBuff);

					UpdateData(FALSE);
				}
			}
			else if (L"Ahglan" == cstrObjName)
			{
				pMeshCom = static_cast<CDynamicMesh*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Mesh", ID_STATIC));

				if (pMeshCom &&
					iComboSel != 0 &&
					iComboSel != 1)
				{
					UpdateData(TRUE);

					m_fAniTime = pMeshCom->Get_AniFrameTime();
					m_fAniEndTime = (_float)pMeshCom->Get_AniFrameEndTime() * 0.5f;

					pBuff = (WCHAR)m_fAniTime;
					m_EditFrameTime.SetWindowText(&pBuff);
					pBuff = (WCHAR)m_fAniEndTime;
					m_EditEndTime.SetWindowText(&pBuff);

					UpdateData(FALSE);
				}
			}
		}
		break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
