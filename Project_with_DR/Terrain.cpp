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

void Terrain::loadHeight() {

}

float Terrain::GetWidth()const {
	return this->
}

float Terrain::getDepth()const {

}

float Terrain::getHeight(float x, float z)const {

}

void Terrain::setWorld(DirectX::CXMMATRIX M) {

}

DirectX::XMMATRIX Terrain::getWorld()const {

}