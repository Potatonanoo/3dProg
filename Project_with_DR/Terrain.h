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
		// filename of the heightmap
		std::wstring HeightMapFilename;

		// height and width of the map
		UINT hMapWidth;
		UINT hMapHeight;

		// height scale
		float hScale;

		// The spacing along the x and z-axes (length between vertices)
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
	
	static const int CellsPerPatch = 64;
	int NumbPatchVertRows;
	int NumbPatchVertCols;
	int NumbPatchVertices;
	int NumbPatchQuadFaces;

	initInfo terrain_info;
	//DirectX::XMFLOAT4X4 world;
	//std::vector<DirectX::XMFLOAT2>PatchBoundsY;

	// one objstruct = one vertex
	unsigned int indexCounter;
	
	
	//ID3D11ShaderResourceView* mBlendMapSRV;
	//ID3D11ShaderResourceView* mLayerMapArraySRV;
	DirectX::XMMATRIX WorldMatrix;
public:
	
	Terrain();
	Terrain(ID3D11Device* device, std::wstring filename);
	~Terrain();
	void Update();

	std::vector<float> heightMap;

	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;
	ID3D11ShaderResourceView* heightmapSRV;

	float getWidth()const;
	float getDepth()const;
	unsigned int getIndexCounter()const;

	//ID3D11Buffer* getVertexBuffer();
	//ID3D11Buffer* getIndexBuffer();
	//ID3D11ShaderResourceView* getShaderResourceView();
	//ID3D11SamplerState* getSamplerState();

	float getHeight(float x, float z)const;
	//void setWorld(DirectX::CXMMATRIX M);
	DirectX::XMMATRIX getWorld()const;
	void translate(float x, float y, float z);

};

