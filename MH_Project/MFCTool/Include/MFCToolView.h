
// MFCToolView.h : CMFCToolView Ŭ������ �������̽�
//

#pragma once

class CMFCToolDoc;
class CMFC_Camera;
class CMFC_Terrain;
class CMFC_Player;

BEGIN(Engine)

class CLayer;

END

class CMFCToolView : public CScrollView
{
protected: // serialization������ ��������ϴ�.
	CMFCToolView();
	DECLARE_DYNCREATE(CMFCToolView)

// Ư���Դϴ�.
public:
	CMFCToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	list<D3DXMESHCONTAINER_DERIVED*>	Get_MeshContainerList();

public:
	void	Set_RenderTerrain(_uint _iRenderType) { m_iRenderTerrain = _iRenderType; }
	void	Set_ChangeColType(CString cstrName, COLTYPE eColType) { Engine::Set_RenderColType(cstrName, eColType); }
	void	Set_ObjectAniIndex(_uint iIndex, OBJECTADD_MFC eObjType = OBJECTADD_MFC_PLAYER);
	void	Set_ColliderMatrix(_matrix* matInfo, CString cstrColName);
	void	Set_ColliderMatrixInterpolX(_float fX, CString cstrColName);
	void	Set_ColliderMatrixInterpolY(_float fY, CString cstrColName);
	void	Set_ColliderMatrixInterpolZ(_float fZ, CString cstrColName);

public:
	HRESULT	Add_Prototype();
	HRESULT Ready_DefaultSettings();

	HRESULT Add_NewTerrain(_uint iRowX, _uint iColZ, _uint iInterval);

	HRESULT	Add_Object(OBJECTADD_MFC _eObjectType);
	HRESULT	Delete_Object(OBJECTADD_MFC _eObjectType);

	HRESULT Add_Collider(_float fRadius, CString cstrName);
	HRESULT Apply_Collider(_float fColScale, _uint iAniIndex);

private:
	CGraphicDev*		m_pDeviceClass = nullptr;
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	CMFC_Camera*		m_pCamera = nullptr;
	CMFC_Terrain*		m_pTerrain = nullptr;
	CMFC_Player*		m_pPlayer = nullptr;
	CLayer*				m_pLayer = nullptr;

	_uint		m_iRenderTerrain = 0;

	_float		m_fDeltaTime = 0.f;

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//protected:
//	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
//	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
//	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CMFCToolView();
#ifdef _DEBUG
	//virtual void AssertValid() const;
	//virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // MFCToolView.cpp�� ����� ����
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

