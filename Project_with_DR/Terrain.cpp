#include "Terrain.h"



Terrain::Terrain() {
	terrain_info.HeightMapFilename = L"Textures\\HMap.raw";
	terrain_info.hMapHeight = 64;
	terrain_info.hMapWidth = 64;

	terrain_info.quadSize = 1;
	terrain_info.hScale = 15;

	NumbPatchVertRows = (terrain_info.hMapHeight - 1) / CellsPerPatch;
	NumbPatchVertCols = (terrain_info.hMapWidth - 1) / CellsPerPatch;

	NumbPatchVertices = NumbPatchVertRows * NumbPatchVertCols;
	NumbPatchQuadFaces = (NumbPatchVertRows - 1) * (NumbPatchVertCols - 1);
}


Terrain::~Terrain() {
	// Release mQuadPatchVB
	// Release mQuadPatchIB
	// Release heightmapSRV
}

bool Terrain::inBounds(int i, int j) {
	return true;
}

float Terrain::Average(int i, int j) {

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

	// Replace the old heightmap with the filtered one
	heightMap = dest;

}

void Terrain::BuildQuadPatchVB(ID3D11Device* device) {

}

void Terrain::BuildQuadPatchIB(ID3D11Device* device) {

}

void Terrain::BuildHeightMapSRV(ID3D11Device* device) {

}

float Terrain::GetWidth()const {

}

float Terrain::getDepth()const {

}

float Terrain::getHeight(float x, float z)const {

}

void Terrain::setWorld(DirectX::CXMMATRIX M) {

}

DirectX::XMMATRIX Terrain::getWorld()const {

}