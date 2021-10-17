// ../Codes/CameraTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "../Codes/CameraTool.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "MFCToolView.h"
#include "MFC_Camera.h"
#include "MFC_CamEye.h"
#include "MFC_CamAt.h"


// CCameraTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraTool, CDialogEx)

CCameraTool::CCameraTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG4, pParent)
	, m_fEyeX(0)
	, m_fEyeY(0)
	, m_fEyeZ(0)
	, m_fAtX(0)
	, m_fAtY(0)
	, m_fAtZ(0)
	, m_fSectionSpeed(0)
{

}

CCameraTool::~CCameraTool()
{
}

void CCameraTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EYE, m_ListEye);
	DDX_Control(pDX, IDC_LIST_AT, m_ListAt);
	DDX_Text(pDX, IDC_EDIT_EYE_X, m_fEyeX);
	DDX_Text(pDX, IDC_EDIT_EYE_Y, m_fEyeY);
	DDX_Text(pDX, IDC_EDIT_EYE_Z, m_fEyeZ);
	DDX_Text(pDX, IDC_EDIT_AT_X, m_fAtX);
	DDX_Text(pDX, IDC_EDIT_AT_Y, m_fAtY);
	DDX_Text(pDX, IDC_EDIT_AT_Z, m_fAtZ);
	DDX_Text(pDX, IDC_EDIT_SECTION_SPEED, m_fSectionSpeed);
	DDX_Control(pDX, IDC_SPIN_SECTION_SPEED, m_spSpeed);
}


BEGIN_MESSAGE_MAP(CCameraTool, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_EYE, &CCameraTool::OnListEyeSelChange)
	ON_LBN_SELCHANGE(IDC_LIST_AT, &CCameraTool::OnListAtSelChange)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EYE, &CCameraTool::OnBnClickedEyeAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_EYE, &CCameraTool::OnBnClickedDeleteEye)
	ON_BN_CLICKED(IDC_BUTTON_ADD_AT, &CCameraTool::OnBnClickedAddAt)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_AT, &CCameraTool::OnBnClickedDeleteAt)
	ON_BN_CLICKED(IDC_BUTTON_CAMSAVE, &CCameraTool::OnBnClickedSaveCam)
	ON_BN_CLICKED(IDC_BUTTON_CAMLOAD, &CCameraTool::OnBnClickedLoadCam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SECTION_SPEED, &CCameraTool::OnDeltaposSpinSectionSpeed)

END_MESSAGE_MAP()


// CCameraTool 메시지 처리기입니다.
BOOL CCameraTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pCamera = dynamic_cast<CMFC_Camera*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));

	if (!m_pCamera)
		return FALSE;

	return TRUE;
}


void CCameraTool::OnListEyeSelChange()
{
	_uint iSel = m_ListEye.GetCurSel();

	wstring wstrEyeName = L"MFC_CamEye" + std::to_wstring(iSel);

	dynamic_cast<CMFC_CamEye*>(Engine::Get_MFCGameObject(L"GameLogic", wstrEyeName))->Set_Selected(COL_TRUE);

	CString cstrEyeName = L"";
	for (_int i = 0; i < m_ListEye.GetCount(); ++i)
	{
		if (i != iSel)
		{
			m_ListEye.GetText(i, cstrEyeName);

			dynamic_cast<CMFC_CamEye*>(Engine::Get_MFCGameObject(L"GameLogic", (wstring)cstrEyeName))->Set_Selected(COL_FALSE);
		}
	}
}


void CCameraTool::OnListAtSelChange()
{
	_uint iSel = m_ListAt.GetCurSel();

	wstring wstrAtName = L"MFC_CamAt" + std::to_wstring(iSel);

	dynamic_cast<CMFC_CamAt*>(Engine::Get_MFCGameObject(L"GameLogic", wstrAtName))->Set_Selected(COL_TRUE);

	CString cstrAtName = L"";
	for (_int i = 0; i < m_ListAt.GetCount(); ++i)
	{
		if (i != iSel)
		{
			m_ListAt.GetText(i, cstrAtName);

			dynamic_cast<CMFC_CamAt*>(Engine::Get_MFCGameObject(L"GameLogic", (wstring)cstrAtName))->Set_Selected(COL_FALSE);
		}
	}
}


void CCameraTool::OnBnClickedEyeAdd()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	wstring	wstrName = L"MFC_CamEye" + std::to_wstring(m_iEyeTagIndex);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pName, strlen(pName), pTchar, 32);

	pToolView->Add_Object(OBJECTADD_MFC_CAMEYE, wstrName);
	m_ListEye.AddString(wstrName.c_str());

	++m_iEyeTagIndex;
}


void CCameraTool::OnBnClickedDeleteEye()
{

}


void CCameraTool::OnBnClickedAddAt()
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMFCToolView* pToolView = dynamic_cast<CMFCToolView*>(pMain->m_tMainSplitter.GetPane(0, 1));

	wstring	wstrName = L"MFC_CamAt" + std::to_wstring(m_iAtTagIndex);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pName, strlen(pName), pTchar, 32);

	pToolView->Add_Object(OBJECTADD_MFC_CAMAT, wstrName);
	m_ListAt.AddString(wstrName.c_str());

	++m_iAtTagIndex;
}


void CCameraTool::OnBnClickedDeleteAt()
{
	
}


void CCameraTool::OnBnClickedSaveCam()
{

}


void CCameraTool::OnBnClickedLoadCam()
{

}


void CCameraTool::OnDeltaposSpinSectionSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


}
