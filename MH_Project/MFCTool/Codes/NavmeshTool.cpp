// ../Codes/CNavmeshTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/NavmeshTool.h"
#include "afxdialogex.h"

#include "MFC_Player.h"
#include "MainFrm.h"
#include "MFCToolView.h"


// CNavmeshTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNavmeshTool, CDialogEx)

CNavmeshTool::CNavmeshTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	m_vecSavePoint.reserve(64);
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
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	pToolView->DeleteAll_NaviMesh();
	m_vecSavePoint.clear();
}


void CNavmeshTool::OnBnClickedSave()
{
	CMFC_Player*	pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));
	NULL_CHECK_RETURN(pPlayer, );

	_uint		iSize;
	_vec3		vPoint[3];
	_matrix		matPoint;
	ZeroMemory(vPoint, sizeof(_vec3) * 3);
	ZeroMemory(matPoint, sizeof(_matrix));
	
	if (pPlayer)
	{
		CFileDialog Dlg(FALSE,// FALSE가 다른이름으로 저장. 
			L"dat",
			L"*.dat",
			OFN_OVERWRITEPROMPT,
			L"Data File(*.dat) | *.dat||",
			this, 0, 0);
		TCHAR szFilePath[MAX_PATH] = L"";

		GetCurrentDirectory(MAX_PATH, szFilePath);
		Dlg.m_ofn.lpstrInitialDir = szFilePath;

		if (IDOK == Dlg.DoModal())
		{
			m_vecSavePoint.clear();
			vector<CCell*>	vecCell = pPlayer->Get_CellVector();

			if (!vecCell.empty())
			{
				iSize = vecCell.size();

				for (_uint i = 0; i < vecCell.size(); ++i)
				{
					for (_uint j = 0; j < CCell::POINT_END; ++j)
					{
						vPoint[j] = *vecCell[i]->Get_Point((CCell::POINT) j);
					}

					memcpy(&matPoint._11, &vPoint[0], sizeof(_vec3));
					memcpy(&matPoint._21, &vPoint[1], sizeof(_vec3));
					memcpy(&matPoint._31, &vPoint[2], sizeof(_vec3));

					m_vecSavePoint.push_back(matPoint);
				}
			}

			CString strFilePath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (INVALID_HANDLE_VALUE == hFile)
				return;

			DWORD dwbyte = 0;

			// Size 저장용
			WriteFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

			for (_uint i = 0; i < m_vecSavePoint.size(); ++i)
			{
				WriteFile(hFile, &m_vecSavePoint[i], sizeof(_matrix), &dwbyte, nullptr);
			}

			CloseHandle(hFile);
		}
	}

	return;
}


void CNavmeshTool::OnBnClickedLoad()
{
	CFileDialog Dlg(TRUE,// FALSE가 다른이름으로 저장. 
		L"dat",
		L"*.dat",
		OFN_OVERWRITEPROMPT,
		0, 0, 0, 0);

	TCHAR szFilePath[MAX_PATH]{};

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwbyte = 0;
		_uint iSize = 0;
		_matrix matPoint;
		ZeroMemory(matPoint, sizeof(_matrix));

		ReadFile(hFile, &iSize, sizeof(_uint), &dwbyte, nullptr);

		for (_uint i = 0; i < iSize; ++i)
		{
			if (0 == dwbyte)
				break;

			ReadFile(hFile, &matPoint, sizeof(_matrix), &dwbyte, nullptr);

			m_vecSavePoint.push_back(matPoint);
		}

		CloseHandle(hFile);
	}

	pToolView->Add_NewNaviMesh(m_vecSavePoint);
	pToolView->Invalidate(FALSE);

	return;
}


BOOL CNavmeshTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	//m_pTerrain = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));
	m_pTerrainTex = dynamic_cast<CTerrainTex*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Buffer", ID_STATIC));
	m_pTerrainTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Transform", ID_DYNAMIC));
	m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Calculator", ID_STATIC));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
