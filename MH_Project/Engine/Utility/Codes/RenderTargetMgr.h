#ifndef RenderTargetMgr_h__
#define RenderTargetMgr_h__

#include "Engine_Define.h"
#include "Base.h"
#include "RenderTarget.h"


BEGIN(Engine)

class ENGINE_DLL CRenderTargetMgr : public CBase
{
	DECLARE_SINGLETON(CRenderTargetMgr)

private:
	explicit CRenderTargetMgr(void);
	virtual ~CRenderTargetMgr(void);

public:
	HRESULT		Ready_RenderTarget(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pTargetTag, const _uint& iWidth, const _uint& iHeight, D3DFORMAT Format, D3DXCOLOR Color);
	HRESULT		Ready_MRT(const wstring pMRTTag, const wstring pTargetTag);

	HRESULT		Begin_MRT(const wstring pMRTTag);
	HRESULT		End_MRT(const wstring pMRTTag);

	HRESULT		Ready_DebugBuffer(const wstring pTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	void		Render_DebugBuffer(const wstring pMRTTag);

	void		Get_RenderTargetTexture(LPD3DXEFFECT& pEffect, const wstring pTargetTag, const char* pConstantTable);

private:
	CRenderTarget*			Find_RenderTarget(const wstring pTargetTag);
	list<CRenderTarget*>*	Find_MRT(const wstring pMRTTag);

private:
	map<const wstring, CRenderTarget*>			m_mapRenderTarget;
	map<const wstring, list<CRenderTarget*>>	m_mapMRT;

private:
	virtual void	Free(void);
};

END
#endif // RenderTargetMgr_h__
