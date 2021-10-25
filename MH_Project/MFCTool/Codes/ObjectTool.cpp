// ../Codes/ObjectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/ObjectTool.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "MFCToolView.h"


// CObjectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialogEx)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_cstrObjName(_T(""))
	, m_fObjPosX(0)
	, m_fObjPosY(0)
	, m_fObjPosZ(0)
	, m_fObjRotX(0)
	, m_fObjRotY(0)
	, m_fObjRotZ(0)
	, m_fObjSclX(0)
	, m_fObjSclY(0)
	, m_fObjSclZ(0)
{

}

CObjectTool::~CObjectTool()
{
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeObjectTool);
	DDX_Control(pDX, IDC_LIST1, m_ListObjAdd);
	DDX_Text(pDX, IDC_EDIT_OBJNAME, m_cstrObjName);
	DDX_Text(pDX, IDC_EDIT_OBJ_POSX, m_fObjPosX);
	DDX_Text(pDX, IDC_EDIT3_OBJ_POSY, m_fObjPosY);
	DDX_Text(pDX, IDC_EDIT_OBJ_POSZ, m_fObjPosZ);
	DDX_Text(pDX, IDC_EDIT_OBJ_ROTX, m_fObjRotX);
	DDX_Text(pDX, IDC_EDIT_OBJ_ROTY, m_fObjRotY);
	DDX_Text(pDX, IDC_EDIT_OBJ_ROTZ, m_fObjRotZ);
	DDX_Text(pDX, IDC_EDIT_OBJ_SCLX, m_fObjSclX);
	DDX_Text(pDX, IDC_EDIT_OBJ_SCLY, m_fObjSclY);
	DDX_Text(pDX, IDC_EDIT_OBJ_SCLZ, m_fObjSclZ);
	DDX_Control(pDX, IDC_COMBO1, m_ComboObjScene);
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CObjectTool::OnBnClickedObjAdd)
	ON_BN_CLICKED(IDC_BUTTON2, &CObjectTool::OnBnClickedObjDelete)
	ON_EN_CHANGE(IDC_EDIT_OBJ_POSX, &CObjectTool::OnEditChangePosX)
	ON_EN_CHANGE(IDC_EDIT3_OBJ_POSY, &CObjectTool::OnEditChangePosY)
	ON_EN_CHANGE(IDC_EDIT_OBJ_POSZ, &CObjectTool::OnEditChangePosZ)
	ON_EN_CHANGE(IDC_EDIT_OBJ_ROTX, &CObjectTool::OnEditChangeRotX)
	ON_EN_CHANGE(IDC_EDIT_OBJ_ROTY, &CObjectTool::OnEditChangeRotY)
	ON_EN_CHANGE(IDC_EDIT_OBJ_ROTZ, &CObjectTool::OnEditChangeRotZ)
	ON_EN_CHANGE(IDC_EDIT_OBJ_SCLX, &CObjectTool::OnEditChangeSclX)
	ON_EN_CHANGE(IDC_EDIT_OBJ_SCLY, &CObjectTool::OnEditChangeSclY)
	ON_EN_CHANGE(IDC_EDIT_OBJ_SCLZ, &CObjectTool::OnEditChangeSclZ)
	ON_BN_CLICKED(IDC_BUTTON3, &CObjectTool::OnBnClickedObjSave)
	ON_BN_CLICKED(IDC_BUTTON4, &CObjectTool::OnBnClickedObjLoad)

	ON_WM_TIMER()
END_MESSAGE_MAP()


// CObjectTool 메시지 처리기입니다.



void CObjectTool::OnBnClickedObjAdd()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	UpdateData(TRUE);

	if (m_TreeObjectTool.GetSelectedItem() == m_hPlayer)
	{
		pToolView->Add_Object(OBJECTADD_MFC_PLAYER);
		pToolView->Set_ObjectToolMode(TRUE, OBJECTADD_MFC_PLAYER);

		m_ListObjAdd.AddString(m_cstrObjName);
	}
	else if (m_TreeObjectTool.GetSelectedItem() == m_hAhglan)
	{
		pToolView->Add_Object(OBJECTADD_MFC_AHGLAN);
		pToolView->Set_ObjectToolMode(TRUE, OBJECTADD_MFC_AHGLAN);

		m_ListObjAdd.AddString(m_cstrObjName);
	}

	UpdateData(FALSE);
}


void CObjectTool::OnBnClickedObjDelete()
{

}


void CObjectTool::OnEditChangePosX()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);
	_vec3		vPos = {};

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.x = m_fObjPosX;
		pObjTrans->Set_Pos(&vPos);
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.x = m_fObjPosX;
		pObjTrans->Set_Pos(&vPos);
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangePosY()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);
	_vec3		vPos = {};

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.y = m_fObjPosY;
		pObjTrans->Set_Pos(&vPos);
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.y = m_fObjPosY;
		pObjTrans->Set_Pos(&vPos);
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangePosZ()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);
	_vec3		vPos = {};

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.z = m_fObjPosZ;
		pObjTrans->Set_Pos(&vPos);
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

		vPos = *pObjTrans->Get_Info(INFO_POS);
		vPos.z = m_fObjPosZ;
		pObjTrans->Set_Pos(&vPos);
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotX()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode())
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotY()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode())
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotZ()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode()) 
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->RotationFromOriginAngle(ROT_X, D3DXToRadian(m_fObjRotX));
			pObjTrans->RotationFromOriginAngle(ROT_Y, D3DXToRadian(m_fObjRotY));
			pObjTrans->RotationFromOriginAngle(ROT_Z, D3DXToRadian(m_fObjRotZ));
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclX()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode())
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleX(m_fObjSclX);
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleX(m_fObjSclX);
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclY()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode())
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleY(m_fObjSclY);
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleY(m_fObjSclY);
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclZ()
{
	UpdateData(TRUE);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	if (pView->Get_ObjMouseMode())
	{
		if (OBJECTADD_MFC_PLAYER == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleZ(m_fObjSclZ);
		}
		else if (OBJECTADD_MFC_AHGLAN == pView->Get_ObjMode())
		{
			CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));

			pObjTrans->Set_ScaleZ(m_fObjSclZ);
		}
	}

	UpdateData(FALSE);
}


void CObjectTool::OnBnClickedObjSave()
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

	/*
	1. SceneType
	2. 객체의 갯수
	3. 객체별 정보
	 => Name, Position, Rotation, Scale
	*/
	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		DWORD	dwbyte = 0;

		CTransform* pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
		// SceneType : 0. Ahglan	1. Normal	2. MidBoss
		_uint iComboSel = m_ComboObjScene.GetCurSel();
		_uint iListCount = m_ListObjAdd.GetCount() - 1;

		WriteFile(hFile, &iComboSel, sizeof(_uint), &dwbyte, nullptr);
		WriteFile(hFile, &iListCount, sizeof(_uint), &dwbyte, nullptr);
		WriteFile(hFile, pPlayerTrans->Get_Info(INFO_POS), sizeof(_vec3), &dwbyte, nullptr);
		WriteFile(hFile, pPlayerTrans->Get_ScaleInfo(), sizeof(_vec3), &dwbyte, nullptr);
		WriteFile(hFile, pPlayerTrans->Get_RotateInfo(), sizeof(_vec3), &dwbyte, nullptr);

		for (_uint i = 0; i < iListCount; ++i)
		{
			CTransform* pTrans = nullptr;

			switch (iComboSel)
			{
			case 0:
				pTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
				break;

			case 1:
				break;

			case 2:
				break;
			}

			WriteFile(hFile, pTrans->Get_Info(INFO_POS), sizeof(_vec3), &dwbyte, nullptr);
			WriteFile(hFile, pTrans->Get_ScaleInfo(), sizeof(_vec3), &dwbyte, nullptr);
			WriteFile(hFile, pTrans->Get_RotateInfo(), sizeof(_vec3), &dwbyte, nullptr);
		}

		CloseHandle(hFile);
	}
}


void CObjectTool::OnBnClickedObjLoad()
{
	CFileDialog Dlg(TRUE,// FALSE가 다른이름으로 저장. 
		L"dat",
		L"*.dat",
		OFN_OVERWRITEPROMPT,
		0, 0, 0, 0);
	TCHAR szFilePath[MAX_PATH]{};

	UpdateData(TRUE);

	if (IDOK == Dlg.DoModal())
	{
		CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwbyte = 0;

		_uint iSceneType = -1;
		_uint iObjCount = -1;

		_vec3	vPos, vScale, vRotate;

		ReadFile(hFile, &iSceneType, sizeof(_uint), &dwbyte, nullptr);
		ReadFile(hFile, &iObjCount, sizeof(_uint), &dwbyte, nullptr);

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
		ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

		pToolView->Add_Object(OBJECTADD_MFC_PLAYER, L"MFC_Player");
		CTransform* pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
		pPlayerTrans->Set_Pos(&vPos);
		pPlayerTrans->Set_Scale(vScale.x, vScale.y, vScale.z);
		pPlayerTrans->RotationFromOriginAngle(ROT_X, vRotate.x);
		pPlayerTrans->RotationFromOriginAngle(ROT_Y, vRotate.y);
		pPlayerTrans->RotationFromOriginAngle(ROT_Z, vRotate.z);

		for (_uint i = 0; i < iObjCount; ++i)
		{
			if (0 == dwbyte)
				return;

			ReadFile(hFile, &vPos, sizeof(_vec3), &dwbyte, nullptr);
			ReadFile(hFile, &vScale, sizeof(_vec3), &dwbyte, nullptr);
			ReadFile(hFile, &vRotate, sizeof(_vec3), &dwbyte, nullptr);

			CTransform* pTrans = nullptr;

			switch (iSceneType)
			{
			case 0:
				pToolView->Add_Object(OBJECTADD_MFC_AHGLAN, L"MFC_Ahglan");
				pTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
				if (pTrans)
				{
					pTrans->Set_Pos(&vPos);
					pTrans->Set_Scale(vScale.x, vScale.y, vScale.z);
					pTrans->RotationFromOriginAngle(ROT_X, vRotate.x);
					pTrans->RotationFromOriginAngle(ROT_Y, vRotate.y);
					pTrans->RotationFromOriginAngle(ROT_Z, vRotate.z);
				}
				break;

			case 1:
				break;

			case 2:
				break;
			}
		}

		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}


BOOL CObjectTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(1, 10, NULL);

	m_pTerrainTex = dynamic_cast<CTerrainTex*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Buffer", ID_STATIC));
	m_pTerrainTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Transform", ID_DYNAMIC));
	m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Calculator", ID_STATIC));

	m_hMainObj = m_TreeObjectTool.InsertItem(TEXT("Main"), 0, 0, TVI_ROOT, TVI_LAST);
	m_hSubObj = m_TreeObjectTool.InsertItem(TEXT("Sub"), 0, 0, TVI_ROOT, TVI_LAST);

	m_hPlayer = m_TreeObjectTool.InsertItem(TEXT("Player"), 0, 0, m_hMainObj, TVI_LAST);
	m_hAhglan = m_TreeObjectTool.InsertItem(TEXT("Ahglan"), 0, 0, m_hMainObj, TVI_LAST);

	m_ComboObjScene.AddString(L"1_Ahglan");
	m_ComboObjScene.AddString(L"2_Normal");
	m_ComboObjScene.AddString(L"3_MidBoss");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CObjectTool::OnTimer(UINT_PTR nIDEvent)
{
	//_uint iSel = -1;
	//CString cstrObjName = L"";
	//CString cstrObjType = L"";

	//switch (nIDEvent)
	//{
	//case 1:
	//	iSel = m_ListObjAdd.GetCurSel();
	//	m_ListObjAdd.GetText(iSel, cstrObjName);
	//	cstrObjType = cstrObjName.Left();

	//	break;

	//default:
	//	break;
	//}

	CDialogEx::OnTimer(nIDEvent);
}
