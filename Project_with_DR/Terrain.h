#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d9types.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <DirectXPackedVector.h>
#include <algorithm>
#include <iostream>

using namespace DirectX;

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

		struct VertexData
		{
			float x, y, z;
			float u, v;
			float nx, ny, nz;
		};

	bool inBounds(int i, int j);
	float Average(int i, int j);

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
	XMMATRIX WorldMatrix;
public:
	
	Terrain(ID3D11Device* g_Device);
	~Terrain();
	void Update();
	void Smooth();

	void LoadHeightmap(ID3D11Device* g_Device);
	void BuildQuadPatchVB(ID3D11Device* device); // vertex buffer
	void BuildQuadPatchIB(ID3D11Device* device); // index buffer
	void BuildHeightmapSRV(ID3D11Device* device); // Shader resource View
	void loadTexture(ID3D11Device* g_Device, wchar_t* filename);
	std::vector<float> heightMap;

	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;
	//ID3D11ShaderResourceView* heightmapSRV;
	//ID3D11ShaderResourceView* terrainResource;
	//ID3D11ShaderResourceView* grassResource;

	ID3D11ShaderResourceView* textureSRV;
	ID3D11SamplerState* textureSamplerState;
		ID3D11ShaderResourceView* SRV;
		ID3D11Texture2D* texture;
		ID3D11Buffer* vertexBuffer;

		ID3D11Buffer* getVertexBuffer();
		ID3D11ShaderResourceView* getShaderResourceView();

		void setVertexBuffer(ID3D11DeviceContext* g_DeviceContext);
		void setResourceView(ID3D11DeviceContext* g_DeviceContext);

		void setTextureSRV(ID3D11DeviceContext* g_DeviceContext);
		void setTextureSamplerState(ID3D11DeviceContext* g_DeviceContext);

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

