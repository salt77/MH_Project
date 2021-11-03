
// MFCToolView.h : CMFCToolView 클래스의 인터페이스
//

#pragma once

class CMFCToolDoc;
class CMFC_Camera;
class CMFC_Terrain;
class CMFC_Player;
class CMFC_CamEye;
class CMFC_CamAt;
class CMFC_Ahglan;
class CMFC_Stage;

BEGIN(Engine)

class CLayer;

END

#include "NavmeshTool.h"

class CMFCToolView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CMFCToolView();
	DECLARE_DYNCREATE(CMFCToolView)

// 특성입니다.
public:
	void AssertValid() const;
	CMFCToolDoc* GetDocument() const;

// 작업입니다.
public:
	list<D3DXMESHCONTAINER_DERIVED*>	Get_MeshContainerList(OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);

	// Object Tool 관련 Get 함수들
	const _bool&						Get_ObjMouseMode() { return m_bObjOnMouse; }
	const OBJECTADD_MFC&				Get_ObjMode() { return m_eObjMode; }

public:
	void	Set_RenderTerrain(_uint _iRenderType) { m_iRenderTerrain = _iRenderType; }
	void	Set_ChangeColType(wstring cstrName, COLTYPE eColType, COLLIDERTYPE eColliderType = COLTYPE_SPHERE_DAMAGED, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ObjectAniIndex(_uint iIndex, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ColliderMatrix(_matrix* matInfo, wstring cstrColName, COLLIDERTYPE eColType = COLTYPE_SPHERE_DAMAGED, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ColliderMatrixInterpolX(_float fX, wstring cstrColName);
	void	Set_ColliderMatrixInterpolY(_float fY, wstring cstrColName);
	void	Set_ColliderMatrixInterpolZ(_float fZ, wstring cstrColName);
	void	Set_NavMeshToolPointer(CNavmeshTool* pNavmeshToolPointer) { m_pNavMeshTool = pNavmeshToolPointer; }
	void	Set_ObjectToolMode(_bool bObjOnMouse, OBJECTADD_MFC eObjMode) { m_bObjOnMouse = bObjOnMouse;  m_eObjMode = eObjMode; }

public:
	HRESULT	Add_Prototype();
	HRESULT Ready_DefaultSettings();
	HRESULT	Ready_LightInfo();

	HRESULT Add_NewTerrain(_uint iRowX, _uint iColZ, _uint iInterval);
	HRESULT	Add_NewNaviMesh();
	HRESULT Add_NewNaviMesh(vector<_matrix>	vecSavePoint);
	HRESULT	DeleteAll_NaviMesh();

	HRESULT	Add_Object(OBJECTADD_MFC _eObjectType, wstring ObjTag = L"");
	HRESULT	Delete_Object(OBJECTADD_MFC _eObjectType, wstring ObjTag = L"");

	HRESULT Add_Collider(_float fRadius, wstring cstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	HRESULT	Add_Collider(_float vMinX, _float vMinY, _float vMinZ,
						_float vMaxX, _float vMaxY, _float vMaxZ,
						wstring wstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	HRESULT	Delete_Collider(wstring wstrName, COLLIDERTYPE eColliderType, OBJECTADD_MFC eObjType);
	HRESULT Apply_Collider(_float fColScale, _uint iAniIndex);

private:
	CGraphicDev*		m_pDeviceClass = nullptr;
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	CManagement*		m_pManagementClass = nullptr;
	CMFC_Camera*		m_pCamera = nullptr;
	CMFC_Terrain*		m_pTerrain = nullptr;
	CMFC_Terrain*		m_pTerrain2 = nullptr;
	CMFC_Terrain*		m_pTerrain3 = nullptr;
	CMFC_Player*		m_pPlayer = nullptr;
	CMFC_CamEye*		m_pCamEye = nullptr;
	CMFC_CamAt*			m_pCamAt = nullptr;
	CMFC_Ahglan*		m_pAhglan = nullptr;
	CMFC_Stage*			m_pStage = nullptr;
	CLayer*				m_pLayer = nullptr;

	// Object Tool 관련 변수들
	_bool				m_bObjOnMouse = false;
	OBJECTADD_MFC		m_eObjMode = OBJECTADD_MFC_END;
	////////////////

	// NavMesh Tool 관련 변수들
	CNavmeshTool*		m_pNavMeshTool = nullptr;

	CTerrainTex*		m_pTerrainTex = nullptr;
	CTransform*			m_pTerrainTrans = nullptr;
	CCalculator*		m_pCalculatorCom = nullptr;

	_uint				m_iClickCount = 0;
	_matrix				m_matPoint;
	vector<_matrix>		m_vecPoint;
	////////////////

	_uint		m_iRenderTerrain = 0;

	_float		m_fDeltaTime = 0.f;

	// FPS
	_ulong		m_dwRenderCnt = 0;
	_tchar		m_szFPS[256];
	_float		m_fTime = 0.f;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);

// 구현입니다.
public:
	virtual ~CMFCToolView();

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // MFCToolView.cpp의 디버그 버전
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

