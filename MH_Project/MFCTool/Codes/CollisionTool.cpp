// ../Codes/CollisionTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "../Codes/CollisionTool.h"
#include "afxdialogex.h"
#include "MFC_Player.h"


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

	pToolView->Add_Collider(m_fColScale, (wstring)m_cstrColName);
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
					dynamic_cast<CCollider*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", (wstring)cstrListBoxCol, ID_STATIC))->Set_BoneName(wstring(cstrItemName));
					//::g_wstrBoneName = cstrItemName;
					pToolView->Set_ColliderMatrix((*iter)->ppCombinedTransformMatrix[i], (wstring)cstrListBoxCol);

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

	pToolView->Set_ObjectAniIndex(m_AniIndex);

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

		CMFC_Player* pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		map<const wstring, CCollider*>	mapTemp = pPlayer->Get_ColliderMap();
		map<const wstring, CCollider*>::iterator	iter = mapTemp.begin();

		_uint iSize = mapTemp.size();

		DWORD dwbyte = 0;
		DWORD dwStringSize = 0;
		DWORD dwStringSize2 = 0;

		// Size 저장용
		WriteFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

		//m_pParsing = new PS_Collider;
		PS_Collider tParsing;
		CString cstrColName, cstrBoneName;

		for (; iter != mapTemp.end(); ++iter)
		{
			tParsing.wstrColName = iter->first;
			tParsing.fRadius = iter->second->Get_Radius();
			tParsing.wstrBoneName = iter->second->Get_BoneName();
			cstrColName = iter->first.c_str();
			cstrBoneName = iter->second->Get_BoneName().c_str();
			
			dwStringSize = (cstrColName.GetLength() + 1) * sizeof(TCHAR);//tParsing.wstrColName.size();
			dwStringSize2 = (cstrBoneName.GetLength() + 1) * sizeof(TCHAR);//tParsing.wstrBoneName.size();

			WriteFile(hFile, &dwStringSize, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, &dwStringSize2, sizeof(DWORD), &dwbyte, nullptr);
			WriteFile(hFile, cstrColName.GetString(), dwStringSize, &dwbyte, nullptr);
			WriteFile(hFile, cstrBoneName.GetString(), dwStringSize2, &dwbyte, nullptr);
			WriteFile(hFile, &tParsing.fRadius, sizeof(_float), &dwbyte, nullptr);
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

		pPlayer = nullptr;
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
	CMFC_Player* pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

	UpdateData(TRUE);

	//m_pParsing = new PS_Collider;
	PS_Collider		tParsing;

	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwbyte = 0;
		DWORD dwStringSize = 0;
		DWORD dwStringSize2 = 0;
		TCHAR*	pNameBuff = nullptr;
		TCHAR*	pNameBuff2 = nullptr;
		CString cstrColName, cstrBoneName;
		wstring wstrBoneName;

		_uint iSize = 0;

		ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

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
			ReadFile(hFile, &tParsing.fRadius, sizeof(_float), &dwbyte, nullptr);

			cstrColName = pNameBuff;
			cstrBoneName = pNameBuff2;

			pPlayer->Add_Collider(tParsing.fRadius, pNameBuff);
			m_ListBoxCollider.AddString(pNameBuff);

			// Bone 찾기
			list<D3DXMESHCONTAINER_DERIVED*>	listTemp = pToolView->Get_MeshContainerList();
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
						//::g_wstrBoneName = m_pParsing->wstrBoneName;
						pToolView->Set_ColliderMatrix((*iterList)->ppCombinedTransformMatrix[i], cstrColName.operator LPCWSTR());

						break;
					}
				}
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
