#include "Terrain.h"
#include "DDSTextureLoader.h"

Terrain::Terrain(ID3D11Device* g_Device) {
	//heightmapSRV = nullptr;
	//terrainResource = nullptr;
	//mQuadPatchVB = nullptr;
	//mQuadPatchIB = nullptr;
	//indexCounter = 0;
	//MessageBoxA(NULL, "Missing filename for Heightmap", "Error", MB_OK | MB_ICONEXCLAMATION);

	vertexBuffer = nullptr;

	terrain_info.HeightMapFilename = L"Textures\\HMap.raw";
	terrain_info.hMapHeight = 464;
	terrain_info.hMapWidth = 464;

	WorldMatrix = XMMatrixIdentity();

	//terrain_info.quadSize = 1;
	//terrain_info.hScale = 1;

	//NumbPatchVertRows = (terrain_info.hMapHeight - 1);
	//NumbPatchVertCols = (terrain_info.hMapWidth - 1);

	//NumbPatchVertices = NumbPatchVertRows * NumbPatchVertCols;
	//NumbPatchQuadFaces = (NumbPatchVertRows - 1) * (NumbPatchVertCols - 1);

	BuildHeightmapSRV(g_Device);
	LoadHeightmap(g_Device);
}

Terrain::~Terrain() {
	//mQuadPatchIB->Release();
	//mQuadPatchVB->Release();
	//terrainResources->Release();
	//heightmapSRV->Release();
}

void Terrain::Update() {

}

void Terrain::LoadHeightmap(ID3D11Device* g_Device)
{
	ID3D11Resource* pTexture = nullptr;
	DirectX::CreateDDSTextureFromFile(g_Device, L"dds", &pTexture, &SRV);

	//// Takes in a height per vertex
	//std::vector<unsigned char> in(terrain_info.hMapWidth * terrain_info.hMapHeight);

	//std::ifstream inFile;
	//inFile.open(terrain_info.HeightMapFilename.c_str(), std::ios_base::binary);

	//if (inFile)
	//{
	//	// Read RAW file.
	//	inFile.read((char*)&in[0], (std::streamsize)in.size());
	//	inFile.close();
	//}
	//else {
	//	//std::cout << "File not found\n";
	//	MessageBoxA(NULL, "Heightmap file not found", "Error", MB_OK | MB_ICONEXCLAMATION);

	//}

	//// Copy array to float array and scale.
	//heightMap.resize(terrain_info.hMapHeight * terrain_info.hMapWidth, 0);

	//for (unsigned int i = 0; i < terrain_info.hMapHeight * terrain_info.hMapWidth; i++) {
	//	heightMap[i] = (in[i])*terrain_info.hScale;
	//}
}

bool Terrain::inBounds(int i, int j) {
	/*
	In case that we are on the edge of the hmap, 
	where a pixel does not have eight neighbouring pixels, 
	then we just take the average with as many neighbour pixels as possible
	*/

	if (i >= 0 && i < (int)terrain_info.hMapHeight && j >= 0 && j < (int)terrain_info.hMapWidth) {
		return true;
	}
	else {
		return false;
	}

	// returns true is the entry is on the hmap otherwise false
}

// averages by nearby points
float Terrain::Average(int i, int j) {

	// Function calculates average height of the ij element.
	// It averages itself with its eight neighbor pixels.
	// if a pixel is missing neighbor we don't include it in the average (edge pixels)

	float avg = 0.0f;
	float num = 0.0f;

	// int allows negatives.
	// no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; m++)
	{
		for (int n = j - 1; n <= j + 1; n++)
		{
			if (inBounds(m, n))
			{
				avg += heightMap[m*terrain_info.hMapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

// Smoothing the terrain
void Terrain::Smooth() {
	std::vector<float> dest(heightMap.size());

	for (unsigned int i = 0; i < terrain_info.hMapHeight; i++)
	{
		for (unsigned int j = 0; j < terrain_info.hMapWidth; j++)
		{
			dest[i*terrain_info.hMapWidth + j] = Average(i, j);
		}
	}

	// Replace old heightmap with the filtered one
	heightMap = dest;

}

//Create vertex buffer
void Terrain::BuildQuadPatchVB(ID3D11Device* g_Device) {
	
	float width = terrain_info.hMapWidth/2;
	float height = terrain_info.hMapHeight/2;

	VertexData triangleVertices[6] =
	{
		-width, -height, 0.f,		0.f, 1.f,	0.f, 0.f, 1.f,
		width, height, 0.f,			1.f, 0.f,	0.f, 0.f, 1.f,
		-width, height, 0.f,		0.f, 0.f,	0.f, 0.f, 1.f,
		-width, -height, 0.f,		0.f, 1.f,	0.f, 0.f, 1.f,
		width, -height, 0.f,		1.f, 1.f,	0.f, 0.f, 1.f,
		width, height, 0.f,			1.f, 0.f,	0.f, 0.f, 1.f,
	};
	
	int vertexCount = 6;

	D3D11_BUFFER_DESC bufDesc;
	memset(&bufDesc, 0, sizeof(bufDesc));
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	g_Device->CreateBuffer(&bufDesc, &data, &vertexBuffer);
	
	
	
	// nr of vertices
	//std::vector<VTerr> patchVertices(NumbPatchVertRows*NumbPatchVertCols);

	//float halfWidth = 0.5f*getWidth();
	//float halfDepth = 0.5f*getDepth();

	//float patchWidth = getWidth() / (NumbPatchVertCols - 1);
	//float patchDepth = getDepth() / (NumbPatchVertRows - 1);
	//float du = 1.0f / (NumbPatchVertCols - 1);
	//float dv = 1.0f / (NumbPatchVertRows - 1);

	//for (unsigned int i = 0; i < NumbPatchVertRows; i++)
	//{
	//	float z = halfDepth - i * patchDepth;
	//	for (unsigned int j = 0; j < NumbPatchVertCols; j++)
	//	{
	//		float x = -halfWidth + j * patchWidth;
	//		// float y = heightMap[(i*NumPatchVertCols + j)];
	//		patchVertices[i*NumbPatchVertCols + j].Pos = DirectX::XMFLOAT3(x, 0.0f, z);

	//		//Strech texture over grid
	//		patchVertices[i*NumbPatchVertCols + j].Tex.x = j * du;
	//		patchVertices[i*NumbPatchVertCols + j].Tex.y = i * dv;
	//	}
	//}

	////vertex buffer desc
	//D3D11_BUFFER_DESC vbd;
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = sizeof(VTerr) * patchVertices.size();
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.CPUAccessFlags = 0;
	//vbd.MiscFlags = 0;
	//vbd.StructureByteStride = 0;

	//D3D11_SUBRESOURCE_DATA vInitData;
	//vInitData.pSysMem = &patchVertices[0];
	//HRESULT hr = device->CreateBuffer(&vbd, &vInitData, &mQuadPatchVB);

	//if (hr != S_OK)
	//{
	//	//std::cout << "Error Terrain Vertex Buffer";
	//	MessageBoxA(NULL, "Error in Terrain Vertex Buffer", "Error", MB_OK | MB_ICONEXCLAMATION);
	//}

}

// create index buffer
void Terrain::BuildQuadPatchIB(ID3D11Device* device){

	//indices is the vertices position
	//std::vector<int> indices(NumbPatchQuadFaces * 4);
	//// 4 indices each quad

	////Iterate over each quad, compute indicees
	//int k = 0;
	//for (UINT i = 0; i < NumbPatchVertRows - 1; i++)
	//{
	//	for (UINT j = 0; j < NumbPatchVertCols - 1; j++)
	//	{
	//		// Top row of 2 x 2 quad patch
	//		indices[k] = i * NumbPatchVertCols + j;
	//		indices[k + 1] = i * NumbPatchVertCols + j + 1;

	//		// Bottom
	//		indices[k + 2] = (i + 1)*NumbPatchVertCols + j;
	//		indices[k + 3] = (i + 1)*NumbPatchVertRows + j + 1;
	//		k += 4; // selection of next quad
	//		indexCounter += 4;
	//	}
	//}

	////index buffer desc
	//D3D11_BUFFER_DESC ibd;
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.ByteWidth = sizeof(USHORT)*indices.size();
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ibd.CPUAccessFlags = 0;
	//ibd.MiscFlags = 0;
	//ibd.StructureByteStride = 0;

	//D3D11_SUBRESOURCE_DATA iInitData;
	//iInitData.pSysMem = &indices[0];
	//HRESULT hr = device->CreateBuffer(&ibd, &iInitData, &mQuadPatchIB);

	//if (hr != S_OK)
	//{
	//	//std::cout << "Error Terrain Index Buffer";
	//	MessageBoxA(NULL, "Error in Terrain Index Buffer", "Error", MB_OK | MB_ICONEXCLAMATION);
	//}

}

void Terrain::BuildHeightmapSRV(ID3D11Device* g_Device) 
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = terrain_info.hMapWidth;
	texDesc.Height = terrain_info.hMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	g_Device->CreateTexture2D(&texDesc, NULL, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	g_Device->CreateShaderResourceView(texture, &srvDesc, &SRV);



	//std::vector<DirectX::PackedVector::HALF> hmap(heightMap.size());
	//std::transform(heightMap.begin(), heightMap.end(), hmap.begin(), DirectX::PackedVector::XMConvertFloatToHalf);

	//D3D11_SUBRESOURCE_DATA data;
	//data.pSysMem = &hmap[0];
	//data.SysMemPitch = terrain_info.hMapHeight * sizeof(DirectX::PackedVector::HALF);
	//data.SysMemSlicePitch = 0;
	//ID3D11Texture2D* hmapTex = 0;
	//HRESULT hr;
	//hr = device->CreateTexture2D(&texDesc, &data, &hmapTex); // create texture

	//if (hr != S_OK)
	//{
	//	//std::cout << "Error Terrain Shader Resource View\n";
	//	MessageBoxA(NULL, "Error in Terrain Shader Resource View", "Error", MB_OK | MB_ICONEXCLAMATION);
	//}

	//ReleaseCOM(hmapTex);
}

float Terrain::getWidth()const {
	// Total terrain width
	return (terrain_info.hMapWidth - 1)*terrain_info.quadSize;
}

float Terrain::getDepth()const {
	// Total terrain depth
	return (terrain_info.hMapHeight - 1)*terrain_info.quadSize;

}

unsigned int Terrain::getIndexCounter()const {
	return this->indexCounter;
}

float Terrain::getHeight(float x, float z)const
{
	// transform terrain local space to "cell" space
	float c = (x + 0.5f*getWidth()) / terrain_info.quadSize;
	float d = (z - 0.5f*getDepth()) / -terrain_info.quadSize;

	// What row and column the coords are in
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// fetches the heights of the cell
	float A = heightMap[row*terrain_info.hMapWidth + col];
	float B = heightMap[row*terrain_info.hMapWidth + col + 1];
	float C = heightMap[(row + 1)*terrain_info.hMapWidth + col];
	float D = heightMap[(row + 1)*terrain_info.hMapWidth + col + 1];

	// camera relative to the cell
	float s = c - (float)col;
	float t = d - (float)row;

	// upper triangle
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

DirectX::XMMATRIX Terrain::getWorld()const 
{
	return WorldMatrix;
}

void Terrain::translate(float x, float y, float z)
{
	WorldMatrix = WorldMatrix * DirectX::XMMatrixTranslation(x, y, z);
}

ID3D11Buffer* Terrain::getVertexBuffer()
{
	return this->vertexBuffer;
}

ID3D11ShaderResourceView* Terrain::getShaderResourceView()
{
	return this->SRV;
}

// Access reading error!
void Terrain::setVertexBuffer(ID3D11DeviceContext* g_DeviceContext)
{
	UINT32 size = sizeof(VertexData);// *8; // x,y,z  x,y  nx,ny,nz
	UINT32 offset = 0;
	g_DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &size, &offset);
}

void Terrain::setResourceView(ID3D11DeviceContext* g_DeviceContext)
{
	g_DeviceContext->GSSetShaderResources(0, 1, &SRV);
}