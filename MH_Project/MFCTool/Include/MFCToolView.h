
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

BEGIN(Engine)

class CLayer;

END

class CMFCToolView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CMFCToolView();
	DECLARE_DYNCREATE(CMFCToolView)

// 특성입니다.
public:
	CMFCToolDoc* GetDocument() const;

// 작업입니다.
public:
	list<D3DXMESHCONTAINER_DERIVED*>	Get_MeshContainerList(OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);

public:
	void	Set_RenderTerrain(_uint _iRenderType) { m_iRenderTerrain = _iRenderType; }
	void	Set_ChangeColType(wstring cstrName, COLTYPE eColType, COLLIDERTYPE eColliderType = COLTYPE_SPHERE_DAMAGED, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ObjectAniIndex(_uint iIndex, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ColliderMatrix(_matrix* matInfo, wstring cstrColName, COLLIDERTYPE eColType = COLTYPE_SPHERE_DAMAGED, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ColliderMatrixInterpolX(_float fX, wstring cstrColName);
	void	Set_ColliderMatrixInterpolY(_float fY, wstring cstrColName);
	void	Set_ColliderMatrixInterpolZ(_float fZ, wstring cstrColName);

public:
	HRESULT	Add_Prototype();
	HRESULT Ready_DefaultSettings();
	HRESULT	Ready_LightInfo();

	HRESULT Add_NewTerrain(_uint iRowX, _uint iColZ, _uint iInterval);

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
	CMFC_Camera*		m_pCamera = nullptr;
	CMFC_Terrain*		m_pTerrain = nullptr;
	CMFC_Player*		m_pPlayer = nullptr;
	CMFC_CamEye*		m_pCamEye = nullptr;
	CMFC_CamAt*			m_pCamAt = nullptr;
	CMFC_Ahglan*		m_pAhglan = nullptr;
	CLayer*				m_pLayer = nullptr;

	_uint		m_iRenderTerrain = 0;

	_float		m_fDeltaTime = 0.f;

	// FPS
	_ulong		m_dwRenderCnt = 0;
	_tchar		m_szFPS[256];
	_float		m_fTime = 0.f;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//protected:
//	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
//	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
//	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMFCToolView();
#ifdef _DEBUG
	//virtual void AssertValid() const;
	//virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // MFCToolView.cpp의 디버그 버전
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

