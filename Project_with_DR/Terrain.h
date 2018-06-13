#pragma once
#include <d3d11.h>
//#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <DirectXPackedVector.h>
#include <algorithm>
#include <iostream>

struct VTerr
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 Tex;
	DirectX::XMFLOAT2 BoundsY;
};

class Terrain
{
public:

	struct initInfo
	{
		std::wstring HeightMapFilename;
		std::wstring LayerMapFilename0;
		std::wstring LayerMapFilename1;
		std::wstring LayerMapFilename2;
		std::wstring LayerMapFilename3;
		std::wstring LayerMapFilename4;
		std::wstring BlendMapFilename;

		UINT hMapWidth;
		UINT hMapHeight;
		float hScale;

		// The spacing along the x and z-axes
		float quadSize; // aka Cellspacing
	};

private:
	bool inBounds(int i, int j);
	float Average(int i, int j);
	void Smooth();

	void LoadHeightmap();
	void BuildQuadPatchVB(ID3D11Device* device); // vertex buffer
	void BuildQuadPatchIB(ID3D11Device* device); // index buffer
	void BuildHeightmapSRV(ID3D11Device* device); // Shader resource View
	
	//static const int CellsPerPatch = 64;
	int NumbPatchVertRows;
	int NumbPatchVertCols;
	int NumbPatchVertices;
	int NumbPatchQuadFaces;

	initInfo terrain_info;
	//DirectX::XMFLOAT4X4 world;
	std::vector<DirectX::XMFLOAT2>PatchBoundsY;

	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;

	ID3D11ShaderResourceView* heightmapSRV;
	//ID3D11ShaderResourceView* mBlendMapSRV;
	//ID3D11ShaderResourceView* mLayerMapArraySRV;

public:
	
	Terrain();
	~Terrain();

	std::vector<float> heightMap;

	float getWidth()const;
	float getDepth()const;
	//float getHeight(float x, float z)const;
	//void setWorld(DirectX::CXMMATRIX M);
	//DirectX::XMMATRIX getWorld()const;

};

