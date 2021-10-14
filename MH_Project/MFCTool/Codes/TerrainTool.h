#pragma once


// CTerrainTool ��ȭ �����Դϴ�.

class CTerrainTool : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainTool)

public:
	CTerrainTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTerrainTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedApplyRenderType();
	afx_msg void OnBnClickedApply();
	afx_msg void RadioCtrl(UINT ID);

public:
	BOOL m_bRenderSoild;
	bool m_bRenderWireFrame;
	bool m_bRenderNone;
	_uint m_iRenderType = 2;

	_int m_iRowX;
	_int m_iColZ;
	_int m_iInterval;
	_int m_iDetail;
};
