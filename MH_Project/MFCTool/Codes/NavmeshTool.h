#pragma once

#include "afxcmn.h"

#include "MFC_Terrain.h"

// CNavmeshTool ��ȭ �����Դϴ�.

USING(Engine)

class CNavmeshTool : public CDialogEx
{
	DECLARE_DYNAMIC(CNavmeshTool)

public:
	CNavmeshTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNavmeshTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	HRESULT		 AddNaviMeshString(_uint iVecPointSize);

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnTreeSelChangeNavmesh(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDeleteNav();
	afx_msg void OnBnClickedDeleteAllNav();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();

public:
	vector<_matrix>		m_vecSavePoint;

	CTreeCtrl m_TreeNavmesh;

	//CMFC_Terrain*	m_pTerrain = nullptr;
	CTerrainTex*	m_pTerrainTex = nullptr;
	CTransform*		m_pTerrainTrans = nullptr;
	CCalculator*	m_pCalculatorCom = nullptr;
};
