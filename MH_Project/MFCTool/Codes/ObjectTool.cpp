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

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangePosZ()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotX()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotY()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeRotZ()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclX()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclY()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnEditChangeSclZ()
{
	UpdateData(TRUE);

	HTREEITEM	hItem = m_TreeObjectTool.GetSelectedItem();
	CString		cstrItemName = m_TreeObjectTool.GetItemText(hItem);

	if (L"Player" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
	}
	else if (L"Ahglan" == cstrItemName)
	{
		CTransform*	pObjTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
	}

	UpdateData(FALSE);
}


void CObjectTool::OnBnClickedObjSave()
{
}


void CObjectTool::OnBnClickedObjLoad()
{
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
