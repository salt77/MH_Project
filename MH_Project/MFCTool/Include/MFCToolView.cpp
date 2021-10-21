
// MFCToolView.cpp : CMFCToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCTool.h"
#endif

#include "MainFrm.h"
#include "MFCToolDoc.h"
#include "MFCToolView.h"
#include "MFC_Camera.h"
#include "MFC_Terrain.h"
#include "MFC_Player.h"
#include "MFC_Ahglan.h"
#include "MFC_CamEye.h"
#include "MFC_CamAt.h"
#include "MFC_CamInterpol.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCToolView

USING(Engine)

IMPLEMENT_DYNCREATE(CMFCToolView, CScrollView)

BEGIN_MESSAGE_MAP(CMFCToolView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMFCToolView 생성/소멸

CMFCToolView::CMFCToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMFCToolView::~CMFCToolView()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);
}

//BOOL CMFCToolView::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: CREATESTRUCT cs를 수정하여 여기에서
//	//  Window 클래스 또는 스타일을 수정합니다.
//
//	return CView::PreCreateWindow(cs);
//}

// CMFCToolView 그리기

list<D3DXMESHCONTAINER_DERIVED*> CMFCToolView::Get_MeshContainerList(OBJECTADD_MFC eObjType)
{
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
		{
			return dynamic_cast<CDynamicMesh*>(m_pPlayer->Get_Component(L"Com_Mesh", ID_STATIC))->Get_MeshContainerList();
		}

		break;
	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
		{
			return dynamic_cast<CDynamicMesh*>(m_pAhglan->Get_Component(L"Com_Mesh", ID_STATIC))->Get_MeshContainerList();
		}

		break;
	}
	
	return list<D3DXMESHCONTAINER_DERIVED*>();
}

void CMFCToolView::Set_ChangeColType(wstring cstrName, COLTYPE eColType, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType)
{
	Engine::Set_RenderColType(cstrName, eColType, eColliderType);
}

void CMFCToolView::Set_ObjectAniIndex(_uint iIndex, OBJECTADD_MFC eObjType)
{
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		m_pPlayer->Set_AniIndex(iIndex);
		break;
	case OBJECTADD_MFC_AHGLAN:
		m_pAhglan->Set_AniIndex(iIndex);
		break;
	}
}

void CMFCToolView::Set_ColliderMatrix(_matrix* matInfo, wstring cstrColName, COLLIDERTYPE eColType, OBJECTADD_MFC eObjType)
{
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
		{
			switch (eColType)
			{
			case Engine::COLTYPE_BOX_DAMAGED:
				dynamic_cast<CBoxCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_BOX_HIT:
				dynamic_cast<CBoxCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_SPHERE_DAMAGED:
				dynamic_cast<CCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_SPHERE_HIT:
				dynamic_cast<CCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			}
		}

		break;

	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
		{
			switch (eColType)
			{
			case Engine::COLTYPE_BOX_DAMAGED:
				dynamic_cast<CBoxCollider*>(m_pAhglan->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_BOX_HIT:
				dynamic_cast<CBoxCollider*>(m_pAhglan->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_SPHERE_DAMAGED:
				dynamic_cast<CCollider*>(m_pAhglan->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			case Engine::COLTYPE_SPHERE_HIT:
				dynamic_cast<CCollider*>(m_pAhglan->Get_Component((wstring)cstrColName, ID_STATIC))->Set_Matrix(matInfo);
				break;
			}
		}

		break;
	}
}

void CMFCToolView::Set_ColliderMatrixInterpolX(_float fX, wstring cstrColName)
{
	if (m_pPlayer)
	{
		dynamic_cast<CCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_MatrixInterpolX(fX);
	}
}

void CMFCToolView::Set_ColliderMatrixInterpolY(_float fY, wstring cstrColName)
{
	if (m_pPlayer)
	{
		dynamic_cast<CCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_MatrixInterpolY(fY);
	}
}

void CMFCToolView::Set_ColliderMatrixInterpolZ(_float fZ, wstring cstrColName)
{
	if (m_pPlayer)
	{
		dynamic_cast<CCollider*>(m_pPlayer->Get_Component((wstring)cstrColName, ID_STATIC))->Set_MatrixInterpolZ(fZ);
	}
}

HRESULT CMFCToolView::Add_Prototype()
{
	Ready_GraphicDev(g_hWnd, MODE_WIN, MAPTOOL_WINCX, MAPTOOL_WINCY, &m_pDeviceClass);
	m_pDeviceClass->AddRef();

	m_pGraphicDev = m_pDeviceClass->GetDevice();
	m_pGraphicDev->AddRef();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	Engine::Ready_Prototype(L"Proto_Transform", CTransform::Create(m_pGraphicDev));
	Engine::Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 32, 32));
	Engine::Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL, 2));
	Engine::Ready_Prototype(L"Proto_DynamicMesh_Player", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Lethita/", L"Lethita.X"));
	Engine::Ready_Prototype(L"Proto_DynamicMesh_Ahglan", CDynamicMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/DynamicMesh/Ahglan/", L"Ahglan.X"));
	
	//Engine::Ready_Prototype(L"Proto_StaticMesh_Player", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Lethita/", L"Lethita.X"));

	return S_OK;
}

HRESULT CMFCToolView::Ready_DefaultSettings()
{
	Ready_Font(m_pGraphicDev, L"Font_Default", L"바탕", 15, 15, FW_NORMAL);
	Ready_Font(m_pGraphicDev, L"Font_Jinji", L"궁서", 30, 30, FW_HEAVY);
	//Ready_InputDev(g_hInst, g_hWnd);	// Font 추가

	Ready_LightInfo();

	m_pLayer = CLayer::Create();
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);

	CGameObject* pObj = nullptr;

	// Terrain 추가
	pObj = CMFC_Terrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain", pObj);

	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));

	// Camera 추가 
	pObj = CMFC_Camera::Create(m_pGraphicDev,
		&_vec3(0.f, 3.f, -5.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f), (_float)MAPTOOL_WINCX / MAPTOOL_WINCY,
		0.1f, 1000.f);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Camera", pObj);

	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pCamera = dynamic_cast<CMFC_Camera*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Camera"));

	//// Player 추가
	//pObj = CMFC_Player::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pObj, E_FAIL);
	//m_pLayer->Add_GameObject(L"MFC_Player", pObj);

	//AddGameObjectInManager(L"GameLogic", m_pLayer);

	//m_pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));


	Ready_Timer(L"Timer_Immediate");

	return S_OK;
}

HRESULT CMFCToolView::Ready_LightInfo()
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0);

	return S_OK;
}

HRESULT CMFCToolView::Add_NewTerrain(_uint iRowX, _uint iColZ, _uint iInterval)
{
	Engine::Delete_Prototype(L"Proto_Buffer_TerrainTex");
	m_pLayer->Delete_Layer(L"MFC_Terrain");

	// 프로토타입 생성
	Engine::Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, iRowX, iColZ, iInterval));

	// Layer 삽입
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);

	CGameObject* pObj = nullptr;

	pObj = CMFC_Terrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain", pObj);

	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));

	return S_OK;
}

HRESULT CMFCToolView::Add_Object(OBJECTADD_MFC _eObjectType, wstring ObjTag)
{
	CGameObject* pObj = nullptr;

	switch (_eObjectType)
	{
	case OBJECTADD_MFC_PLAYER:
		pObj = CMFC_Player::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Add_GameObject(L"MFC_Player", pObj);
		Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

		m_pPlayer = dynamic_cast<CMFC_Player*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Player"));

		break;

	case OBJECTADD_MFC_AHGLAN:
		pObj = CMFC_Ahglan::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Add_GameObject(L"MFC_Ahglan", pObj);
		Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

		m_pAhglan = dynamic_cast<CMFC_Ahglan*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Ahglan"));

		break;

	case OBJECTADD_MFC_CAMEYE:
		pObj = CMFC_CamEye::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Add_GameObject(ObjTag, pObj);
		Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

		m_pCamEye = dynamic_cast<CMFC_CamEye*>(Engine::Get_MFCGameObject(L"GameLogic", ObjTag));

		break;

	case OBJECTADD_MFC_CAMAT:
		pObj = CMFC_CamAt::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Add_GameObject(ObjTag, pObj);
		Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

		m_pCamAt = dynamic_cast<CMFC_CamAt*>(Engine::Get_MFCGameObject(L"GameLogic", ObjTag));

		break;

	case OBJECTADD_MFC_CAMINTERPOL:
		CMFC_CamInterpol* pCamInterpol = dynamic_cast<CMFC_CamInterpol*>(Engine::Get_MFCGameObject(L"GameLogic", ObjTag));

		if (!pCamInterpol)
		{
			pObj = CMFC_CamInterpol::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pObj, E_FAIL);
			NULL_CHECK_RETURN(m_pLayer, E_FAIL);
			m_pLayer->Add_GameObject(ObjTag, pObj);
			Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);
		}

		break;
	}

	return S_OK;
}

HRESULT CMFCToolView::Delete_Object(OBJECTADD_MFC _eObjectType, wstring ObjTag)
{
	switch (_eObjectType)
	{
	case OBJECTADD_MFC_PLAYER:
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Delete_Layer(L"MFC_Player");
		m_pPlayer = nullptr;
		//Engine::DeleteGameObjectInManager(L"Player");
		break;

	case OBJECTADD_MFC_AHGLAN:
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Delete_Layer(L"MFC_Ahglan");
		m_pPlayer = nullptr;
		break;

	case OBJECTADD_MFC_CAMEYE:
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Delete_Layer(ObjTag);
		break;

	case OBJECTADD_MFC_CAMAT:
		NULL_CHECK_RETURN(m_pLayer, E_FAIL);
		m_pLayer->Delete_Layer(ObjTag);
		break;
	}

	return S_OK;
}

HRESULT CMFCToolView::Add_Collider(_float fRadius, wstring cstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType)
{
	// Collider는 프로토타입 생성 안 함
	//Engine::Ready_Prototype(L"Proto_Collider", CCollider::Create(m_pGraphicDev, fRadius));
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
			m_pPlayer->Add_Collider(fRadius, cstrName, eColliderType);

		break;

	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
			m_pAhglan->Add_Collider(fRadius, cstrName, eColliderType);

		break;
	}

	return S_OK;
}

HRESULT CMFCToolView::Add_Collider(_float vMinX, _float vMinY, _float vMinZ, _float vMaxX, _float vMaxY, _float vMaxZ, wstring wstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType)
{
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
			m_pPlayer->Add_Collider(vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, wstrName, eColliderType);

		break;
	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
			m_pAhglan->Add_Collider(vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, wstrName, eColliderType);

		break;
	}

	return S_OK;
}

HRESULT CMFCToolView::Delete_Collider(wstring wstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType)
{
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
		{
			m_pPlayer->Delete_Collider(wstrName, eColliderType);
		}

		break;
	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
		{
			m_pAhglan->Delete_Collider(wstrName, eColliderType);
		}

		break;
	}

	return S_OK;
}

HRESULT CMFCToolView::Apply_Collider(_float fColScale, _uint iAniIndex)
{
	return S_OK;
}

void CMFCToolView::OnDraw(CDC* /*pDC*/)
{
	CMFCToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_fTime += m_fDeltaTime;
	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_dwRenderCnt = 0;
		m_fTime = 0.f;
	}

	Render_Font(L"Font_Jinji", m_szFPS, &_vec2(600.f, 600.f), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

	Engine::Render_GameObject(m_pGraphicDev);
	if (m_pTerrain)
		m_pTerrain->Update_Object(m_fDeltaTime);
	if (m_pPlayer)
		m_pPlayer->Update_Object(m_fDeltaTime);
	if (m_pAhglan)
		m_pAhglan->Update_Object(m_fDeltaTime);

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

	//CMFC_Terrain* pTerrain = static_cast<CMFC_Terrain*>(Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));
	//if (m_pTerrain)
	//{
	//	switch (m_iRenderTerrain)
	//	{
	//	case 0:
	//		m_pTerrain->Render_Object();
	//		break;
	//	case 2:
	//		break;
	//	default:
	//		m_pTerrain->Render_Object();
	//		break;
	//	}
	//}

	// Terrain은 선택적으로 보이게 해야하는데 어차피 안쓸거라 놔둔다. 
	m_pLayer->Update_Layer(m_fDeltaTime);
	m_pLayer->Render_Layer(m_fDeltaTime);

	//if (m_pPlayer)
	//	m_pPlayer->Render_Object();

	//if (m_pCamEye)
	//	m_pCamEye->Render_Object();

	//if (m_pCamAt)
	//	m_pCamAt->Render_Object();

	Render_End();

	Invalidate(FALSE);
}


// CMFCToolView 인쇄

//BOOL CMFCToolView::OnPreparePrinting(CPrintInfo* pInfo)
//{
//	// 기본적인 준비
//	return DoPreparePrinting(pInfo);
//}
//
//void CMFCToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
//{
//	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
//}
//
//void CMFCToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
//{
//	// TODO: 인쇄 후 정리 작업을 추가합니다.
//}
//

// CMFCToolView 진단

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

CMFCToolDoc* CMFCToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCToolDoc)));
	return (CMFCToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCToolView 메시지 처리기


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
	m_fDeltaTime = Get_TimeDelta(L"Timer_Immediate");

	m_pCamera->Update_Object(m_fDeltaTime);
	//if (m_pTerrain)
	//	m_pTerrain->Update_Object(fDeltaTime);

	Invalidate(FALSE);

	return CScrollView::PreTranslateMessage(pMsg);
}
