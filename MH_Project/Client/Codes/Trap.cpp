#include "stdafx.h"
#include "Trap.h"

#include "Balista.h"


CTrap::CTrap(LPDIRECT3DDEVICE9 pGraghicDev)
	: CGameObject(pGraghicDev)
{
}

CTrap::CTrap(const CTrap & rhs)
	: CGameObject(rhs)
{
}

CTrap::~CTrap()
{
}


HRESULT CTrap::Ready_Object()
{
	FAILED_CHECK_RETURN(CGameObject::Ready_Object(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(&TRAP_POS);
	m_pTransformCom->Set_Scale(SCALE_NORMAL * 2.f, SCALE_NORMAL * 2.f, SCALE_NORMAL * 2.f);

	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90));

	return S_OK;
}

HRESULT CTrap::LateReady_Object()
{
	FAILED_CHECK_RETURN(CGameObject::LateReady_Object(), E_FAIL);

	m_pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pPlayerTrans, E_FAIL);

	return S_OK;
}

_int CTrap::Update_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::Update_Object(fTimeDelta);

	Animation_Control();

	m_pMeshCom->Set_TrackSpeed(0.5f);
	m_pMeshCom->Set_AnimationIndex(m_iAniIndex);
	m_pMeshCom->Play_Animation(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

_int CTrap::LateUpdate_Object(const _float & fTimeDelta)
{
	_int iExit = CGameObject::LateUpdate_Object(fTimeDelta);

	return iExit;
}

void CTrap::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint iMaxPass = 0;

	pEffect->Begin(&iMaxPass, NULL);		// 1인자 : 현재 쉐이더 파일이 반환하는 pass의 최대 개수
											// 2인자 : 시작하는 방식을 묻는 FLAG
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CTrap::Add_Component()
{
	CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<CDynamicMesh*>(Clone_Prototype(L"Proto_Mesh_Trap"));
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

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Prototype(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CTrap::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}

void CTrap::Fire()
{
	if (!m_bFire)
	{
		m_bFire = true;

		_uint	iFireCount = 0;

		map<const wstring, CGameObject*> mapObject = Engine::Get_MapObject(L"Balista");
		map<const wstring, CGameObject*>::iterator	iter_Balista = mapObject.begin();

		for (; iter_Balista != mapObject.end(); ++iter_Balista)
		{
			CBalista*	pBalista = static_cast<CBalista*>(iter_Balista->second);

			if (pBalista)
			{
				if (POOLING_POS == *static_cast<CTransform*>(pBalista->Get_Component(L"Com_Transform", ID_DYNAMIC))->Get_Info(INFO_POS))
				{
					_vec3	vPos = *m_pTransformCom->Get_Info(INFO_POS);
					vPos.y += 0.5f;

					switch (iFireCount)
					{
					case 0:
						vPos += -*m_pTransformCom->Get_Info(INFO_LOOK) * 55.f;
						break;

					case 1:
						vPos += -*m_pTransformCom->Get_Info(INFO_LOOK) * 23.f;
						break;

					case 2:
						vPos += *m_pTransformCom->Get_Info(INFO_LOOK) * 23.f;
						break;

					case 3:
						vPos += *m_pTransformCom->Get_Info(INFO_LOOK) * 55.f;
						break;
					}

					pBalista->Set_EnableBalista(vPos, *m_pTransformCom->Get_Info(INFO_RIGHT));

					++iFireCount;
				}
			}

			if (4 <= iFireCount)
				break;
		}
	}
}

void CTrap::Animation_Control()
{
	m_fAniTime = m_pMeshCom->Get_AniFrameTime();

	m_eCurState = (TRAP_STATE)m_iAniIndex;

	if (m_ePreState != m_eCurState)
	{
		m_fAniTime = 0.f;
		m_lfAniEnd = m_pMeshCom->Get_AniFrameEndTime();

		switch (m_eCurState)
		{
		case CTrap::TRAP_FIRE:
			break;

		case CTrap::TRAP_IDLE:
			m_bFire = false;
			break;
		}

		m_ePreState = m_eCurState;
	}

	switch (m_eCurState)
	{
	case CTrap::TRAP_FIRE:
		if (m_fAniTime > m_lfAniEnd * 0.3f)
		{
			Fire();
		}
		break;

	case CTrap::TRAP_IDLE:
		break;
	}

	if (m_fAniTime >= m_lfAniEnd)
	{
		switch (m_eCurState)
		{
		case CTrap::TRAP_FIRE:
			m_iAniIndex = TRAP_IDLE;
			break;
		case CTrap::TRAP_IDLE:
			m_iAniIndex = TRAP_FIRE;
			break;
		}
	}
}


CTrap * CTrap::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrap*	pInstance = new CTrap(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrap::Free()
{
	CGameObject::Free();
}
