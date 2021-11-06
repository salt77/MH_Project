
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
#include "MFC_Stage.h"
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
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()

	ON_WM_LBUTTONDOWN()
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

	Engine::Create_Management(m_pGraphicDev, &m_pManagementClass);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Components
	Engine::Ready_Prototype(L"Proto_Transform", CTransform::Create(m_pGraphicDev));

	Engine::Ready_Prototype(L"Proto_Buffer_TerrainTex", CTerrainTex::Create(m_pGraphicDev, 48, 48));
	Engine::Ready_Prototype(L"Proto_Buffer_TerrainTex2", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 1));
	Engine::Ready_Prototype(L"Proto_Buffer_TerrainTex3", CTerrainTex::Create(m_pGraphicDev, 96, 48, 1, 2));

	Engine::Ready_Prototype(L"Proto_Texture_Terrain", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel_%d.png", TEX_NORMAL));
	Engine::Ready_Prototype(L"Proto_Texture_Terrain2", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Ortel2_%d.png", TEX_NORMAL));

	Engine::Ready_Prototype(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev));
	//Engine::Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev));

	// Meshes
	Engine::Ready_Prototype(L"Proto_StaticMesh_Stage", CStaticMesh::Create(m_pGraphicDev, L"../Bin/Resource/Mesh/StaticMesh/Stage/", L"Stage.X"));

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

	// Terrain_Grass 추가
	pObj = CMFC_Terrain::Create(m_pGraphicDev, 1);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain2", pObj);
	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain2 = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain2"));

	pObj = CMFC_Terrain::Create(m_pGraphicDev, 2);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain3", pObj);
	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain3 = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain3"));

	// Terrain 추가
	pObj = CMFC_Terrain::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Terrain", pObj);
	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pTerrain = dynamic_cast<CMFC_Terrain*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Terrain"));

	// Stage 추가
	pObj = CMFC_Stage::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pObj, E_FAIL);
	m_pLayer->Add_GameObject(L"MFC_Stage", pObj);
	Engine::AddGameObjectInManager(L"GameLogic", m_pLayer);

	m_pStage = dynamic_cast<CMFC_Stage*>(Engine::Get_MFCGameObject(L"GameLogic", L"MFC_Stage"));

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

	// NavMesh 관련한 것들
	m_pTerrainTex = dynamic_cast<CTerrainTex*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Buffer", ID_STATIC));
	m_pTerrainTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Transform", ID_DYNAMIC));
	m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Terrain", L"Com_Calculator", ID_STATIC));

	Ready_Timer(L"Timer_Immediate");

	SetTimer(1, 10, NULL);

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

HRESULT CMFCToolView::Add_NewNaviMesh()
{
	Engine::Delete_Prototype(L"Proto_NaviMesh");

	Engine::Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev, m_vecPoint.size(), m_vecPoint));
	m_pPlayer->Add_NaviMesh(m_vecPoint.size(), m_vecPoint);

	return S_OK;
}

HRESULT CMFCToolView::Add_NewNaviMesh(vector<_matrix> vecSavePoint)
{
	Engine::Delete_Prototype(L"Proto_NaviMesh");

	Engine::Ready_Prototype(L"Proto_NaviMesh", CNaviMesh::Create(m_pGraphicDev, vecSavePoint.size(), vecSavePoint));
	m_pPlayer->Add_NaviMesh(vecSavePoint.size(), vecSavePoint);

	m_vecPoint = vecSavePoint;

	return S_OK;
}

HRESULT CMFCToolView::DeleteAll_NaviMesh()
{
	if (m_pPlayer)
		m_pPlayer->DeleteAll_NaviMesh();

	m_iClickCount = 0;
	m_vecPoint.clear();

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

	CTransform*	pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	CTransform*	pAhglanTrans = dynamic_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"Ahglan", L"Com_Transform", ID_DYNAMIC));
	switch (eObjType)
	{
	case OBJECTADD_MFC_PLAYER:
		if (m_pPlayer)
			m_pPlayer->Add_Collider(fRadius, cstrName, pPlayerTrans->Get_WorldMatrix(), eColliderType);

		break;

	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
			m_pAhglan->Add_Collider(fRadius, cstrName, pAhglanTrans->Get_WorldMatrix(), eColliderType);

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
			m_pPlayer->Add_Collider(vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, wstrName,
									dynamic_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

		break;
	case OBJECTADD_MFC_AHGLAN:
		if (m_pAhglan)
			m_pAhglan->Add_Collider(vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, wstrName, 
									dynamic_cast<CTransform*>(m_pAhglan->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_WorldMatrix(), eColliderType);

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

	//m_fTime += m_fDeltaTime;
	//++m_dwRenderCnt;

	//if (m_fTime >= 1.f)
	//{
	//	wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
	//	m_dwRenderCnt = 0;
	//	m_fTime = 0.f;
	//}

	//Render_Font(L"Font_Jinji", m_szFPS, &_vec2(600.f, 600.f), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

	/*Engine::Render_GameObject(m_pGraphicDev);*/

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	/*Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));*/

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
	/*m_pLayer->Update_Layer(m_fDeltaTime);
	m_pLayer->Render_Layer(m_fDeltaTime);*/

	//Render_End();//

	//Invalidate(FALSE);
}


// CMFCToolView 진단

#ifdef _DEBUG
void CMFCToolView::AssertValid() const
{
	CView::AssertValid();
}

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
	/*Set_TimeDelta(L"Timer_Immediate");
	m_fDeltaTime = Get_TimeDelta(L"Timer_Immediate");

	m_pCamera->Update_Object(m_fDeltaTime);*/
	//if (m_pTerrain)
	//	m_pTerrain->Update_Object(fDeltaTime);

	//Invalidate(FALSE);

	return CScrollView::PreTranslateMessage(pMsg);
}


void CMFCToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	NULL_CHECK_RETURN(m_pTerrainTex, );
	NULL_CHECK_RETURN(m_pTerrainTrans, );
	NULL_CHECK_RETURN(m_pCalculatorCom, );

	vector<CCell*>	vecCell = m_pPlayer->Get_CellVector();
	//vector<CCell*>::iterator	iter = vecCell.begin();

	_vec3 vPoint = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pTerrainTex, m_pTerrainTrans);

	//for (; iter != vecCell.end(); ++iter)
	//{
	//	for (_uint i = 0; i < CCell::POINT_END; ++i)
	//	{
	//		if (D3DXVec3Length(&(vPoint - iter->Get_Point((CCell::POINT) i)) <= 3.f))
	//		{
	//			vPoint = iter->Get_Point((CCell::POINT) i);
	//		}
	//	}
	//}

	for (_uint i = 0; i < vecCell.size(); ++i)
	{
		for (_uint j = 0; j < CCell::POINT_END; ++j)
		{
			if (D3DXVec3Length(&(vPoint - *(vecCell[i]->Get_Point((CCell::POINT) j)))) <= 1.f)
			{
				vPoint = *(vecCell[i]->Get_Point((CCell::POINT) j));
			}
		}
	}

	if (vPoint == _vec3(0.f, 0.f, 0.f))
	{
		return;
	}
	else
	{
		switch (m_iClickCount)
		{
		case 0:
			m_matPoint._11 = vPoint.x;
			m_matPoint._12 = vPoint.y;
			m_matPoint._13 = vPoint.z;

			++m_iClickCount;

			break;

		case 1:
			m_matPoint._21 = vPoint.x;
			m_matPoint._22 = vPoint.y;
			m_matPoint._23 = vPoint.z;

			++m_iClickCount;

			break;

		case 2:
			m_matPoint._31 = vPoint.x;
			m_matPoint._32 = vPoint.y;
			m_matPoint._33 = vPoint.z;

			m_iClickCount = 0;

			m_vecPoint.push_back(m_matPoint);

			Add_NewNaviMesh();
			m_pNavMeshTool->AddNaviMeshString(m_vecPoint.size());

			D3DXMatrixIdentity(&m_matPoint);

			break;
		}
	}

	CScrollView::OnRButtonDown(nFlags, point);
}


void CMFCToolView::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		Set_TimeDelta(L"Timer_Immediate");
		m_fDeltaTime = Get_TimeDelta(L"Timer_Immediate");

		m_pCamera->Update_Object(m_fDeltaTime);
		m_pLayer->Update_Layer(m_fDeltaTime);

		Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));

		Engine::Render_GameObject(m_pGraphicDev);
		m_pTerrain->Render_Object();
		//m_pLayer->Render_Layer(m_fDeltaTime);

		Render_End();

		// Object Tool 관련 기능
		if (m_bObjOnMouse)
		{
			_vec3 vPoint = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pTerrainTex, m_pTerrainTrans);

			CTransform*	pPlayerTrans = nullptr;
			CTransform*	pAhglanTrans = nullptr;

			switch (m_eObjMode)
			{
			case OBJECTADD_MFC_PLAYER:
				pPlayerTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Player", L"Com_Transform", ID_DYNAMIC));
				pPlayerTrans->Set_Pos(&vPoint);

				break;

			case OBJECTADD_MFC_AHGLAN:
				pAhglanTrans = static_cast<CTransform*>(Engine::Get_MFCComponent(L"GameLogic", L"MFC_Ahglan", L"Com_Transform", ID_DYNAMIC));
				pAhglanTrans->Set_Pos(&vPoint);

				break;

			default:
				break;
			}
		}

		break;

	default:
		break;
	}

	CScrollView::OnTimer(nIDEvent);
}


void CMFCToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bObjOnMouse)
	{
		m_bObjOnMouse = false;
		m_eObjMode = OBJECTADD_MFC_END;
	}

	CScrollView::OnLButtonDown(nFlags, point);
}
