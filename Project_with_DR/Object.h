#ifndef OBJECT_H
#define OBJECT_H

#include <windows.h>
#include <DirectXMath.h> 
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d9types.h>
#include "DDSTextureLoader.h"

using namespace DirectX;

class Object
{
public:
	Object();
	Object(ID3D11Device* g_Device);
	~Object();
	
	ID3D11Buffer* getVertexBuffer();
	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11SamplerState* getSamplerState();

	void setVertexBuffer(ID3D11DeviceContext* g_DeviceContext);
	void setShaderResourceView(ID3D11DeviceContext* g_DeviceContext);
	void setSamplerState(ID3D11DeviceContext* g_DeviceContext);

private:
	void createVertexBuffer(ID3D11Device* g_Device);
	void createTexture(ID3D11Device* g_Device);

	ID3D11Buffer* g_VertexBuffer;
	ID3D11ShaderResourceView* g_ShaderResourceView;
	ID3D11SamplerState* g_SamplerState;

	struct VertexData
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

	UINT vertexSize = sizeof(float) * 8; // x, y, z, u, v, nx, ny, nz
	UINT offset = 0;
};

#endif
