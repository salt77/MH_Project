#pragma once

#include "afxcmn.h"

#include "MFC_Terrain.h"

// CNavmeshTool 대화 상자입니다.

USING(Engine)

class CNavmeshTool : public CDialogEx
{
	DECLARE_DYNAMIC(CNavmeshTool)

public:
	CNavmeshTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNavmeshTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
