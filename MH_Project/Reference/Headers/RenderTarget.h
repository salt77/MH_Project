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
	// begin scene 역할의 함수
	void			SetUp_OnGraphicDev(const _uint& iIndex);
	
	// end scned 역할의 함수	
	void			Release_OnGraphicDev(const _uint& iIndex);

	HRESULT			Ready_DebugBuffer(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	void			Render_DebugBuffer(void);

	void			Get_RenderTargetTexture(LPD3DXEFFECT& pEffect, const char* pConstantTable);


private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;

	LPDIRECT3DTEXTURE9			m_pTargetTexture;		// 렌더타겟을 생성하기 위한 컴객체

	LPDIRECT3DSURFACE9			m_pTargetSurface;		// 텍스처 컴객체로부터 렌더타겟 용도로 출력한 텍스쳐 데이터를 저장하기 위한 컴객체
	LPDIRECT3DSURFACE9			m_pOldTargetSurface;	// 기존에 디바이스 장치에 연결되어 있는 렌더타겟을 임시적으로 보관하기 위한 컴객체

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
