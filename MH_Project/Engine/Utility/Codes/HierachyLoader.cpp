#include "HierachyLoader.h"

USING(Engine)

CHierachyLoader::CHierachyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pPath)\
	: m_pGraphicDev(pGraphicDev)
	, m_pPath(pPath)
{
	m_pGraphicDev->AddRef();
}

CHierachyLoader::~CHierachyLoader(void)
{
}


STDMETHODIMP CHierachyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pDerivedMatrix = new D3DXFRAME_DERIVED;
	ZeroMemory(pDerivedMatrix, sizeof(D3DXFRAME_DERIVED));

	Allocate_Name(&pDerivedMatrix->Name, Name);

	pDerivedMatrix->CombinedTransformMatrix = *D3DXMatrixIdentity(&pDerivedMatrix->TransformationMatrix);

	*ppNewFrame = pDerivedMatrix;

	return S_OK;
}

STDMETHODIMP CHierachyLoader::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pDerivedMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	Allocate_Name(&pDerivedMeshContainer->Name, Name);

	pDerivedMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH	pMesh = pMeshData->pMesh;

	// 메쉬가 지닌 폴리곤의 개수를 반환
	_ulong	dwNumFaces = pMesh->GetNumFaces();

	pDerivedMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	memcpy(pDerivedMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	_ulong	dwFVF = pMesh->GetFVF();

	const D3DVERTEXELEMENT9	vertexDecl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	//m_pGraphicDev->CreateVertexDeclaration(vertexDecl, &m_tPNTBT.Decl);

	if (!(dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphicDev, &pDerivedMeshContainer->MeshData.pMesh);
		D3DXComputeNormals(pDerivedMeshContainer->MeshData.pMesh, pDerivedMeshContainer->pAdjacency);
	}
	else
	{
		//pMesh->CloneMesh(pMesh->GetOptions(), vertexDecl, m_pGraphicDev, &pDerivedMeshContainer->MeshData.pMesh);
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pGraphicDev, &pDerivedMeshContainer->MeshData.pMesh);
	}

	//D3DXComputeTangentFrameEx(pDerivedMeshContainer->MeshData.pMesh,
	//						  D3DDECLUSAGE_TEXCOORD, 0,
	//						  D3DDECLUSAGE_BINORMAL, 0,
	//						  D3DDECLUSAGE_TANGENT, 0,
	//						  D3DDECLUSAGE_NORMAL, 0,
	//						  D3DXTANGENT_WRAP_UV | D3DXTANGENT_ORTHOGONALIZE_FROM_V | D3DXTANGENT_CALCULATE_NORMALS | D3DXTANGENT_GENERATE_IN_PLACE,
	//						  (_ulong*)pDerivedMeshContainer->pAdjacency, 0.01f, 0.01f, 0.01f, NULL, NULL);

	pDerivedMeshContainer->NumMaterials = (NumMaterials == 0 ? 1 : NumMaterials);

	pDerivedMeshContainer->pMaterials = new D3DXMATERIAL[pDerivedMeshContainer->NumMaterials];
	ZeroMemory(pDerivedMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * pDerivedMeshContainer->NumMaterials);

	pDerivedMeshContainer->ppTexture = new LPDIRECT3DTEXTURE9[pDerivedMeshContainer->NumMaterials];
	ZeroMemory(pDerivedMeshContainer->ppTexture, sizeof(LPDIRECT3DTEXTURE9) * pDerivedMeshContainer->NumMaterials);

	pDerivedMeshContainer->ppNormalTexture = new LPDIRECT3DTEXTURE9[pDerivedMeshContainer->NumMaterials];
	ZeroMemory(pDerivedMeshContainer->ppNormalTexture, sizeof(LPDIRECT3DTEXTURE9) * pDerivedMeshContainer->NumMaterials);

	pDerivedMeshContainer->pwstrTextureName = new wstring[pDerivedMeshContainer->NumMaterials];
	// wstring이나 vector같은 stl을 사용할 시에는 ZeroMemory()나 memset()을 해서는 안된다. => 자체적으로 동적 할당을 해주기 때문에 메모리 누수가 발생함.
	//ZeroMemory(pDerivedMeshContainer->pwstrTextureName, sizeof(wstring) * pDerivedMeshContainer->NumMaterials);

	if (0 != NumMaterials)
	{
		memcpy(pDerivedMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pDerivedMeshContainer->NumMaterials);

		for (_ulong i = 0; i < pDerivedMeshContainer->NumMaterials; ++i)
		{
			_tchar	szFullPath[MAX_PATH] = L"";
			_tchar	szFileName[256] = L"";

			MultiByteToWideChar(CP_ACP, 0, pDerivedMeshContainer->pMaterials[i].pTextureFilename,
				strlen(pDerivedMeshContainer->pMaterials[i].pTextureFilename), szFileName, 256);

			// 메쉬 텍스처의 최종 경로를 만들어주는 코드
			lstrcpy(szFullPath, m_pPath.c_str());
			lstrcat(szFullPath, szFileName);

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppTexture[i])))
				return E_FAIL;

			pDerivedMeshContainer->pwstrTextureName[i] = szFileName;

			// Player Normal Texture
			if (!lstrcmp(szFileName, L"pc_male_plate_chain.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_chain_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_foot.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_foot_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_hand.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_hand_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_head.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_head_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_head_fur.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_head_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_lower_body.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_lower_body_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_upper_body.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_upper_body_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"pc_male_plate_upper_body02.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"pc_male_plate_upper_body02_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;

			}
			else if (!lstrcmp(szFileName, L"dualsword_vanquisher.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"dualsword_vanquisher_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"sticky_bomb.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"sticky_bomb_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}

			// Ahglan Normal Texture
			else if (!lstrcmp(szFileName, L"golem_ahglan.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"golem_ahglan_inner.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_inner_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"golem_ahglan_ore.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_ore_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"golem_ahglan_Chest.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_Chest_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"golem_ahglan_Head.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_Head_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"golem_ahglan_RHand.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"golem_ahglan_RHand_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"mankind_cloyan.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"mankind_cloyan_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"mankind_cloyan_hair.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"mankind_cloyan_hair_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
			else if (!lstrcmp(szFileName, L"mankind_cloyan_mask.tga"))
			{
				lstrcpy(szFullPath, m_pPath.c_str());
				lstrcpy(szFileName, L"mankind_cloyan_mask_normal.tga");
				lstrcat(szFullPath, szFileName);

				if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pDerivedMeshContainer->ppNormalTexture[i])))
					return E_FAIL;
			}
		}
	}
	// 재질 정보가 없는 경우
	else
	{
		pDerivedMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pDerivedMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pDerivedMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pDerivedMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pDerivedMeshContainer->pMaterials[0].MatD3D.Power = 0.f;

		pDerivedMeshContainer->ppTexture[0] = nullptr;
	}

	if (nullptr == pSkinInfo)
		return S_OK;

	pDerivedMeshContainer->pSkinInfo = pSkinInfo;
	pDerivedMeshContainer->pSkinInfo->AddRef();

	pDerivedMeshContainer->MeshData.pMesh->CloneMeshFVF(pDerivedMeshContainer->MeshData.pMesh->GetOptions(), pDerivedMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphicDev, &pDerivedMeshContainer->pOriMesh);
	//pDerivedMeshContainer->MeshData.pMesh->CloneMesh(pDerivedMeshContainer->MeshData.pMesh->GetOptions(), vertexDecl, m_pGraphicDev, &pDerivedMeshContainer->pOriMesh);


	//D3DXComputeTangentFrameEx(pMesh,
	//	D3DDECLUSAGE_TEXCOORD, 0,
	//	D3DDECLUSAGE_BINORMAL, 0,
	//	D3DDECLUSAGE_TANGENT, 0,
	//	D3DDECLUSAGE_NORMAL, 0,
	//	D3DXTANGENT_WRAP_UV | D3DXTANGENT_ORTHOGONALIZE_FROM_V | D3DXTANGENT_CALCULATE_NORMALS | D3DXTANGENT_GENERATE_IN_PLACE,
	//	pDerivedMeshContainer->pAdjacency, 0.01f, 0.01f, 0.01f, NULL, NULL);

	// 뼈의 개수를 반환
	pDerivedMeshContainer->dwNumBones = pDerivedMeshContainer->pSkinInfo->GetNumBones();

	pDerivedMeshContainer->pFrameOffSetMatrix = new _matrix[pDerivedMeshContainer->dwNumBones];
	ZeroMemory(pDerivedMeshContainer->pFrameOffSetMatrix, sizeof(_matrix) * pDerivedMeshContainer->dwNumBones);

	pDerivedMeshContainer->ppCombinedTransformMatrix = new _matrix*[pDerivedMeshContainer->dwNumBones];
	ZeroMemory(pDerivedMeshContainer->ppCombinedTransformMatrix, sizeof(_matrix*) * pDerivedMeshContainer->dwNumBones);

	pDerivedMeshContainer->pRenderingMatrix = new _matrix[pDerivedMeshContainer->dwNumBones];
	ZeroMemory(pDerivedMeshContainer->pRenderingMatrix, sizeof(_matrix) * pDerivedMeshContainer->dwNumBones);

	for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
		pDerivedMeshContainer->pFrameOffSetMatrix[i] = *pDerivedMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	*ppNewMeshContainer = pDerivedMeshContainer;

	return S_OK;
}

STDMETHODIMP CHierachyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP CHierachyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (_ulong i = 0; i < pDerivedMeshContainer->NumMaterials; ++i)
	{
		Safe_Release(pDerivedMeshContainer->ppTexture[i]);
		Safe_Release(pDerivedMeshContainer->ppNormalTexture[i]);
	}

	Safe_Delete_Array(pDerivedMeshContainer->ppTexture);
	Safe_Delete_Array(pDerivedMeshContainer->ppNormalTexture);
	Safe_Delete_Array(pDerivedMeshContainer->pwstrTextureName);
	Safe_Delete_Array(pDerivedMeshContainer->Name);
	Safe_Delete_Array(pDerivedMeshContainer->pAdjacency);
	Safe_Delete_Array(pDerivedMeshContainer->pMaterials);

	Safe_Delete_Array(pDerivedMeshContainer->pFrameOffSetMatrix);
	Safe_Delete_Array(pDerivedMeshContainer->ppCombinedTransformMatrix);
	Safe_Delete_Array(pDerivedMeshContainer->pRenderingMatrix);

	Safe_Release(pDerivedMeshContainer->pOriMesh);
	Safe_Release(pDerivedMeshContainer->MeshData.pMesh);
	Safe_Release(pDerivedMeshContainer->pSkinInfo);

	Safe_Delete(pDerivedMeshContainer);

	return S_OK;
}

void CHierachyLoader::Allocate_Name(char ** ppName, const char * pFrameName)
{
	if (nullptr == pFrameName)
		return;

	_uint	iLength = strlen(pFrameName);

	*ppName = new char[iLength + 1];

	strcpy_s(*ppName, iLength + 1, pFrameName);
}

CHierachyLoader * CHierachyLoader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pPath)
{
	return new CHierachyLoader(pGraphicDev, pPath);
}

_ulong CHierachyLoader::Release(void)
{
	Safe_Release(m_pGraphicDev);

	delete this;

	return 0;
}
