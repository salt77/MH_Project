
// MFCToolView.cpp : CMFCToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MFCTool.h"
#endif

#include "MainFrm.h"
#include "MFCToolDoc.h"
#include "MFCToolView.h"
#include "MFC_Camera.h"
#include "MFC_Terrain.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCToolView

IMPLEMENT_DYNCREATE(CMFCToolView, CScrollView)

BEGIN_MESSAGE_MAP(CMFCToolView, CScrollView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMFCToolView ����/�Ҹ�

CMFCToolView::CMFCToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMFCToolView::~CMFCToolView()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);
}

//BOOL CMFCToolView::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
//	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
//
//	return CView::PreCreateWindow(cs);
//}

// CMFCToolView �׸���

HRESULT CMFCToolView::Add_Prototype()
{
	Ready_GraphicDev(g_hWnd, MODE_WIN, MAPTOOL_WINCX, MAPTOOL_WINCY, &m_pDeviceClass);
	m_pDeviceClass->AddRef();

	m_pGraphicDev = m_pDeviceClass->GetDevice();
	m_pGraphicDev->AddRef();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	Ready_Prototype(L"Proto_Transform", CTransform::Create(m_pGraphicDev));
	Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 32, 32));
	Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL, 2));

	return S_OK;
}

HRESULT CMFCToolView::Ready_DefaultSettings()
{
	Ready_Font(m_pGraphicDev, L"Font_Default", L"����", 15, 15, FW_NORMAL);
	//Ready_InputDev(g_hInst, g_hWnd);

	m_pLayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);

	CGameObject* pObj = nullptr;

	pObj = CMFC_Terrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain", pObj);

	AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain = dynamic_cast<CMFC_Terrain*>(Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));


	pObj = CMFC_Camera::Create(m_pGraphicDev,
		&_vec3(0.f, 3.f, -5.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f), (_float)MAPTOOL_WINCX / MAPTOOL_WINCY,
		0.1f, 1000.f);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Camera", pObj);

	AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pCamera = dynamic_cast<CMFC_Camera*>(Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));

	Ready_Timer(L"Timer_Immediate");

	return S_OK;
}

HRESULT CMFCToolView::Add_NewTerrain(_uint iRowX, _uint iColZ, _uint iInterval)
{
	Delete_Prototype(L"Proto_Buffer_TerrainTex");
	m_pLayer->Delete_Layer(L"MFC_Terrain");

	// ������Ÿ�� ����
	Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, iRowX, iColZ, iInterval));

	// Layer ����
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);

	CGameObject* pObj = nullptr;

	pObj = CMFC_Terrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain", pObj);

	AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain = dynamic_cast<CMFC_Terrain*>(Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));

	return S_OK;
}

void CMFCToolView::OnDraw(CDC* /*pDC*/)
{
	CMFCToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

	CMFC_Terrain* pTerrain = pTerrain = static_cast<CMFC_Terrain*>(Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));
	if (m_pTerrain)
	{
		switch (m_iRenderTerrain)
		{
		case 0:
			pTerrain->Set_RenderType(RENDERTYPE_MFC_SOLID);
			m_pTerrain->Render_Object();
			break;
		case 2:
			break;
		default:
			pTerrain->Set_RenderType(RENDERTYPE_MFC_WIREFRAME);
			m_pTerrain->Render_Object();
			break;
		}
	}

	Render_End();

	Invalidate(FALSE);
}


// CMFCToolView �μ�

//BOOL CMFCToolView::OnPreparePrinting(CPrintInfo* pInfo)
//{
//	// �⺻���� �غ�
//	return DoPreparePrinting(pInfo);
//}
//
//void CMFCToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
//{
//	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
//}
//
//void CMFCToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
//{
//	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
//}
//

// CMFCToolView ����

#ifdef _DEBUG
//void CMFCToolView::AssertValid() const
//{
//	CView::AssertValid();
//}
//
//void CMFCToolView::Dump(CDumpContext& dc) const
//{
//	CView::Dump(dc);
//}

CMFCToolDoc* CMFCToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCToolDoc)));
	return (CMFCToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCToolView �޽��� ó����


void CMFCToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, CSize(600, 600));
	g_hWnd = m_hWnd;
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	RECT rcMain = {};
	pMain->GetWindowRect(&rcMain);

	RECT rcView{};
	GetClientRect(&rcView);

	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;

	pMain->SetWindowPos(nullptr, 0, 0, MAPTOOL_WINCX + iGapX, MAPTOOL_WINCY + iGapY, SWP_NOMOVE);

	Add_Prototype();
	Ready_DefaultSettings();
}


BOOL CMFCToolView::PreTranslateMessage(MSG* pMsg)
{
	Set_TimeDelta(L"Timer_Immediate");
	_float fDeltaTime = Get_TimeDelta(L"Timer_Immediate");

	m_pCamera->Update_Object(fDeltaTime);
	if (m_pTerrain)
		m_pTerrain->Update_Object(fDeltaTime);

	Invalidate(FALSE);

	return CScrollView::PreTranslateMessage(pMsg);
}
