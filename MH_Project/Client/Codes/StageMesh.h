#pragma once

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CStaticMesh;
class COptimization;
class CShader;

END

class CStageMesh : public CGameObject
{
private:
	explicit CStageMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStageMesh(const CStageMesh& rhs);
	virtual ~CStageMesh(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_Material();
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CStaticMesh*	m_pMeshCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	//COptimization*	m_pOptimizationCom = nullptr;

	_vec3		m_vDir;
	_bool		m_bColl = false;

public:
	static CStageMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void);
};
