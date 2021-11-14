#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh : public CComponent
{
private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh();

public:
	const _vec3*			Get_VtxPos(void) { return m_pVtxPos; }
	const _ulong&			Get_Stride(void) { return m_dwStride; }
	const _ulong&			Get_VtxCnt(void) { return m_dwVtxCnt; }

public:
	_bool					Find_Alpha(const char* pFileName);

public:
	HRESULT		Ready_Meshes(const wstring pFilePath, const wstring pFileName);
	void		Render_Meshes();
	void		Render_Meshes(LPD3DXEFFECT& pEffect);

private:
	LPD3DXMESH				m_pMesh;		// �븻������ ������ �޽� ������ ��� �ִ� ��ü
	LPD3DXMESH				m_pOriMesh;		// ���� �ε� ������ �޽� ������ ��� �ִ� ��ü

	LPD3DXBUFFER			m_pAdjacency;	// �޽��� ���� ������ �ٰŷ� �̷��� ������ ������ ���� ������ �����ϱ� ���� ��
	LPD3DXBUFFER			m_pSubset;		// �޽��� �����ϴ� �����ݿ� �ش��ϴ� ����
											// ������� ���� == ������ ���� == �ؽ�ó�� ����
	D3DXMATERIAL*			m_pMtrl;		// �ؽ�ó�� �̸��� ������ ���� ������ �����ϱ� ���� ����ü
	_ulong					m_dwSubsetCnt;	// ������� ����
	LPDIRECT3DTEXTURE9*		m_ppTexture;	// �������� �޽� �ؽ�ó�� ���� �迭�� ���·� �����ϱ� ���� ������
	LPDIRECT3DTEXTURE9*		m_ppNormalTexture;

	_vec3*					m_pVtxPos;		// �޽��� ���� ������ ��ġ������ ���� �Ҵ�(�迭)�� �޸� ������ �����ϱ� ���� ������
	_ulong					m_dwStride;		// ������ ũ�⸦ �����ϱ� ���� ����
	_ulong					m_dwVtxCnt;		// �޽��� ���� ������ ������ ����


public:
	static	CStaticMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath, const wstring pFileName);
	virtual CComponent *	Clone(void) override;

private:
	virtual void	Free();
};

END