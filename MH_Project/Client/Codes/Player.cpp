#include "stdafx.h"
#include "Player.h"

#include "Export_Function.h"
#include "DynamicMesh.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{

}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{

}

CPlayer::~CPlayer(void)
{

}

HRESULT CPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pMeshCom->Set_AnimationIndex(0);

	return S_OK;
}

_int CPlayer::Update_Object(const _float& fTimeDelta)
{
	SetUp_OnTerrain();
	Key_Input(fTimeDelta);

	_int iExit = CGameObject::Update_Object(fTimeDelta);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CPlayer::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pColliderCom->Render_Collider(COL_FALSE, m_pTransformCom->Get_WorldMatrix());
	
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pMeshCom->Render_Meshes();
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer*	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CPlayer::Free(void)
{
	CGameObject::Free();
}

HRESULT CPlayer::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_Mesh_Player"));
	NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(m_pRendererCom, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Prototype(L"Proto_Calculator"));
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//// Collider
	//pComponent = m_pColliderCom = CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_VtxCnt(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(m_pColliderCom, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(L"Com_Collider", pComponent);

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_INFO(INFO_LOOK, &m_vDir);

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		/*D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_pTransformCom->Move_Pos(&m_vDir, 5.f, fTimeDelta);*/
		m_pMeshCom->Set_AnimationIndex(1);

	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pMeshCom->Set_AnimationIndex(0);

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pMeshCom->Set_AnimationIndex(2);
		//D3DXVec3Normalize(&m_vDir, &m_vDir);
		//m_pTransformCom->Move_Pos(&m_vDir, -5.f, fTimeDelta);
	}
	
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		//m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));
	
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));

	if (Get_DIMouseState(DIM_LB) & 0X80)
	{
		_vec3	vPickPos = PickUp_OnTerrain();
		_vec3	vPlayerPos;
		m_pTransformCom->Get_INFO(INFO_POS, &vPlayerPos);

		_vec3	vDir = *D3DXVec3Normalize(&vDir, &(vPickPos - vPlayerPos));
		m_pTransformCom->Move_Pos(&vDir, 5.f, fTimeDelta);
	}
}

void CPlayer::SetUp_OnTerrain(void)
{
	_vec3		vPos;
	m_pTransformCom->Get_INFO(INFO_POS, &vPos);

	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);
	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

Engine::_vec3 CPlayer::PickUp_OnTerrain(void)
{
	CTerrainTex*	pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	CTransform*	pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}

