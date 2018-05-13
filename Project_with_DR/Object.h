#ifndef OBJECT_H
#define OBJECT_H

#include <windows.h>
#include <DirectXMath.h> 
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d9types.h>
#include <iostream>
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace std;

class Object
{
public:
	Object();
	Object(ID3D11Device* g_Device, string filename, string texture);
	~Object();
	
	ID3D11Buffer* getVertexBuffer();
	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11SamplerState* getSamplerState();

	void setVertexBuffer(ID3D11DeviceContext* g_DeviceContext);
	void setShaderResourceView(ID3D11DeviceContext* g_DeviceContext);
	void setSamplerState(ID3D11DeviceContext* g_DeviceContext);

	void update(float dt);

	void translate(float x, float y, float z);
	void rotate(float pitch, float yaw, float roll);
	XMMATRIX getWorldMatrix();
	UINT getVertexCount();

private:
	void createVertexBuffer(ID3D11Device* g_Device, string filename);
	void createTexture(ID3D11Device* g_Device, wchar_t* filename);

	ID3D11Buffer* g_VertexBuffer;
	ID3D11ShaderResourceView* g_ShaderResourceView;
	ID3D11SamplerState* g_SamplerState;

	struct VertexData
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

	XMMATRIX WorldMatrix;

	UINT vertexSize = sizeof(float) * 8; // x, y, z, u, v, nx, ny, nz
	UINT offset = 0;
	UINT vertexCount;

	float objAngle;
};

#endif
