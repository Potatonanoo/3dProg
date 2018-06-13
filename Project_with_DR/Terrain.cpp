#include "Terrain.h"

Terrain::Terrain() {
	terrain_info.HeightMapFilename = L"Textures\\HMap.raw";
	terrain_info.hMapHeight = 4;
	terrain_info.hMapWidth = 4;

	terrain_info.quadSize = 1;
	terrain_info.hScale = 1;

	NumbPatchVertRows = (terrain_info.hMapHeight - 1);
	NumbPatchVertCols = (terrain_info.hMapWidth - 1);

	NumbPatchVertices = NumbPatchVertRows * NumbPatchVertCols;
	NumbPatchQuadFaces = (NumbPatchVertRows - 1) * (NumbPatchVertCols - 1);
}


Terrain::~Terrain() {
	// Release mQuadPatchVB
	// Release mQuadPatchIB
	// Release heightmapSRV
}

bool Terrain::inBounds(int i, int j) {
	/*
	In case that we are on the edge of the hmap, 
	where a pixel does not have eight neighbouring pixels, 
	then we just take the average with as many neighbour pixels as possible
	*/

	return (i >= 0 && i < (int)terrain_info.hMapHeight && j >= 0 && j < (int)terrain_info.hMapWidth);
	// returns true is the entry is on the hmap otherwise false
}

float Terrain::Average(int i, int j) {

	// Function calculates average height of the ij element.
	// It averages itself with its eight neighbor pixels.
	// if a pixel is missing neighbor we don't include it in the average (edge pixels)

	float avg = 0.0f;
	float num = 0.0f;

	// int allows negatives.
	// no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
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

void Terrain::LoadHeightmap()
{
	// A height per vertex
	std::vector<unsigned char> in(terrain_info.hMapWidth * terrain_info.hMapHeight);

	std::ifstream inFile;
	inFile.open(terrain_info.HeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read RAW.
		inFile.read((char*)&in[0], (std::streamsize)in.size());
		inFile.close();
	}

	// Copy array to float array and scale.
	heightMap.resize(terrain_info.hMapHeight * terrain_info.hMapWidth, 0);

	for (UINT i = 0; i < terrain_info.hMapHeight * terrain_info.hMapWidth; ++i){
		heightMap[i] = (in[i] / 255.0f)*terrain_info.hScale;
	}
}

void Terrain::Smooth() {
	std::vector<float> dest(heightMap.size());

	for (UINT i = 0; i < terrain_info.hMapHeight; ++i)
	{
		for (UINT j = 0; j < terrain_info.hMapWidth; ++j)
		{
			dest[i*terrain_info.hMapWidth + j] = Average(i, j);
		}
	}

	// Replace old heightmap with the filtered one
	heightMap = dest;

}

void Terrain::BuildQuadPatchVB(ID3D11Device* device) {
	// nr of vertices
	std::vector<VTerr> patchVertices(NumbPatchVertRows*NumbPatchVertCols);

	float halfWidth = 0.5f*getWidth();
	float halfDepth = 0.5f*getDepth();

	float patchWidth = getWidth() / (NumbPatchVertCols - 1);
	float patchDepth = getDepth() / (NumbPatchVertRows - 1);
	float du = 1.0f / (NumbPatchVertCols - 1);
	float dv = 1.0f / (NumbPatchVertRows - 1);

	for (UINT i = 0; i < NumbPatchVertRows; i++)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < NumbPatchVertCols; j++)
		{
			float x = -halfWidth + j * patchWidth;
			// float y = heightMap[(i*NumPatchVertCols + j)];
			patchVertices[i*NumbPatchVertCols + j].Pos = DirectX::XMFLOAT3(x, 0.0f, z);

			//Strech texture over grid
			patchVertices[i*NumbPatchVertCols + j].Tex.x = j * du;
			patchVertices[i*NumbPatchVertCols + j].Tex.y = i * dv;
		}
	}

	// Store aligned bounding box y-bouds upper left patch corner.
	for (UINT i = 0; i < NumbPatchVertRows - 1; i++)
	{
		for (UINT j = 0; j < NumbPatchVertCols - 1; j++)
		{
			UINT patchID = i * (NumbPatchVertCols - 1) + j;
			patchVertices[i*NumbPatchVertCols + j].BoundsY = PatchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VTerr) * patchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &patchVertices[0];
	HRESULT hr = device->CreateBuffer(&vbd, &vInitData, &mQuadPatchVB);

	if (hr != S_OK)
	{
		std::cout << "Error Terrain Vertex Buffer";
	}

}

void Terrain::BuildQuadPatchIB(ID3D11Device* device){

	//indices is the vertices position
	std::vector<int> indices(NumbPatchQuadFaces * 4);
	// 4 indices each quad

	//Iterate over each quad, compute indicees
	int k = 0;
	for (UINT i = 0; i < NumbPatchVertRows - 1; i++)
	{
		for (UINT j = 0; j < NumbPatchVertCols - 1; j++)
		{
			// Top row of 2 x 2 quad patch
			indices[k] = i * NumbPatchVertCols + j;
			indices[k + 1] = i * NumbPatchVertCols + j + 1;

			// Bottom
			indices[k + 2] = (i + 1)*NumbPatchVertCols + j;
			indices[k + 3] = (i + 1)*NumbPatchVertRows + j + 1;
			k += 4; // selection of next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT)*indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices[0];
	HRESULT hr = device->CreateBuffer(&ibd, &iInitData, &mQuadPatchIB);

	if (hr != S_OK)
	{
		std::cout << "Error Terrain Index Buffer";
	}

}

void Terrain::BuildHeightmapSRV(ID3D11Device* device) {
	// Function for supporting tessellation and displacement mapping
	// We sample the heightmap in our shader programs. 
	// For this we must create shader resource view to the hmap.
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

	std::vector<DirectX::PackedVector::HALF> hmap(heightMap.size());
	std::transform(heightMap.begin(), heightMap.end(), hmap.begin(), DirectX::PackedVector::XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = terrain_info.hMapHeight * sizeof(DirectX::PackedVector::HALF);
	data.SysMemSlicePitch = 0;
	ID3D11Texture2D* hmapTex = 0;
	HRESULT hr;
	hr = device->CreateTexture2D(&texDesc, &data, &hmapTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hr = device->CreateShaderResourceView(hmapTex, &srvDesc, &heightmapSRV);

	if (hr != S_OK)
	{
		std::cout << "Error Terrain Shader Resource View";
	}

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
