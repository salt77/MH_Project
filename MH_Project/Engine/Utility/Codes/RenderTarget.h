#ifndef RenderTarget_h__
#define RenderTarget_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CRenderTarget : public CBase
{
private:
	explicit CRenderTarget(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderTarget(void);

public:
	HRESULT			Ready_RenderTarget(const _uint& iWidth,
										const _uint& iHeight,
										D3DFORMAT Format,
										D3DXCOLOR Color);

	// clear
	void			Clear_RenderTarget(void);
	// begin scene ������ �Լ�
	void			SetUp_OnGraphicDev(const _uint& iIndex);
	
	// end scned ������ �Լ�	
	void			Release_OnGraphicDev(const _uint& iIndex);

	HRESULT			Ready_DebugBuffer(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	void			Render_DebugBuffer(void);

	void			Get_RenderTargetTexture(LPD3DXEFFECT& pEffect, const char* pConstantTable);


private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;

	LPDIRECT3DTEXTURE9			m_pTargetTexture;		// ����Ÿ���� �����ϱ� ���� �İ�ü

	LPDIRECT3DSURFACE9			m_pTargetSurface;		// �ؽ�ó �İ�ü�κ��� ����Ÿ�� �뵵�� ����� �ؽ��� �����͸� �����ϱ� ���� �İ�ü
	LPDIRECT3DSURFACE9			m_pOldTargetSurface;	// ������ ����̽� ��ġ�� ����Ǿ� �ִ� ����Ÿ���� �ӽ������� �����ϱ� ���� �İ�ü

	D3DXCOLOR					m_ClearColor;

	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;


public:
	static CRenderTarget*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
										const _uint& iWidth,
										const _uint& iHeight, 
										D3DFORMAT Format, 
										D3DXCOLOR Color);
	
	virtual void				Free(void);
};

END
#endif // RenderTarget_h__
