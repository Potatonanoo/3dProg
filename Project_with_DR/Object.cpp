#include "Object.h"
#include "DDSTextureLoader.h"

Object::Object()
{
	objAngle = 0;
	vertexCount = 0;
	g_VertexBuffer = nullptr;
	g_ShaderResourceView = nullptr;
	WorldMatrix = XMMatrixIdentity();
}

Object::Object(ID3D11Device* g_Device, string filename, wchar_t* texture)
{
	objAngle = 0;
	vertexCount = 0;
	WorldMatrix = XMMatrixIdentity();
	createVertexBuffer(g_Device, filename);
	createTexture(g_Device, texture);
}

Object::~Object()
{

}

ID3D11Buffer* Object::getVertexBuffer()
{
	return this->g_VertexBuffer;
}


ID3D11ShaderResourceView* Object::getShaderResourceView()
{
	return this->g_ShaderResourceView;
}

ID3D11SamplerState* Object::getSamplerState()
{
	return this->g_SamplerState;
}

void Object::createVertexBuffer(ID3D11Device* g_Device, string filename)
{
	VertexData triangleVertices[36] =
	{
		-0.5f, 0.5f, -0.5f,		0.f, 0.f,	0.f, 0.f, -1.f,
		0.5f, 0.5f, -0.5f,		1.f, 0.f,	0.f, 0.f, -1.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	0.f, 0.f, -1.f,
		0.5f, 0.5f, -0.5f,		1.f, 0.f,	0.f, 0.f, -1.f,
		0.5f, -0.5f, -0.5f,		1.f, 1.f,	0.f, 0.f, -1.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	0.f, 0.f, -1.f,

		-0.5f, -0.5f, 0.5f,		0.f, 1.f,	0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,		0.f, 0.f,	0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,		0.f, 1.f,	0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,		1.f, 1.f,	0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,		0.f, 0.f,	1.f, 0.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	1.f, 0.f, 0.f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f,	1.f, 0.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	1.f, 0.f, 0.f,
		0.5f, -0.5f, 0.5f,		1.f, 1.f,	1.f, 0.f, 0.f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f,	1.f, 0.f, 0.f,

		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	-1.f, 0.f, 0.f,
		-0.5f, 0.5f, 0.5f,		1.f, 0.f,	-1.f, 0.f, 0.f,
		-0.5f, 0.5f, -0.5f,		0.f, 0.f,	-1.f, 0.f, 0.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	-1.f, 0.f, 0.f,
		-0.5f, -0.5f, 0.5f,		1.f, 1.f,	-1.f, 0.f, 0.f,
		-0.5f, 0.5f, 0.5f,		1.f, 0.f,	-1.f, 0.f, 0.f,

		-0.5f, 0.5f, 0.5f,		0.f, 0.f,	0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	0.f, 1.f, 0.f,
		-0.5f, 0.5f, -0.5f,		0.f, 1.f,	0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,	0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f,	0.f, 1.f, 0.f,
		-0.5f, 0.5f, -0.5f,		0.f, 1.f,	0.f, 1.f, 0.f,

		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	0.f, -1.f, 0.f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f,	0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,		0.f, 0.f,	0.f, -1.f, 0.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,	0.f, -1.f, 0.f,
		0.5f, -0.5f, -0.5f,		1.f, 1.f,	0.f, -1.f, 0.f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f,	0.f, -1.f, 0.f,
		
	};

	vertexCount = 36;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	g_Device->CreateBuffer(&bufferDesc, &data, &g_VertexBuffer);

	vertexSize = sizeof(float) * 8; // x, y, z, u, v, nx, ny, nz
	offset = 0;
}

void Object::createTexture(ID3D11Device* g_Device, wchar_t* filename)
{
	ID3D11Resource* pTexture = nullptr;
	DirectX::CreateDDSTextureFromFile(g_Device, filename, &pTexture, &g_ShaderResourceView);

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	g_Device->CreateSamplerState(&sampDesc, &g_SamplerState);
}

void Object::setVertexBuffer(ID3D11DeviceContext* g_DeviceContext)
{
	g_DeviceContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &vertexSize, &offset);
}

void Object::setShaderResourceView(ID3D11DeviceContext* g_DeviceContext)
{
	g_DeviceContext->PSSetShaderResources(0, 1, &g_ShaderResourceView);
}

void Object::setSamplerState(ID3D11DeviceContext* g_DeviceContext)
{
	g_DeviceContext->PSSetSamplers(0, 1, &g_SamplerState);
}

void Object::update(float dt)
{
}

void Object::translate(float x, float y, float z)
{
	WorldMatrix = WorldMatrix * XMMatrixTranslation(x, y, z);
}

void Object::rotate(float pitch, float yaw, float roll)
{
	WorldMatrix = WorldMatrix * XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

XMMATRIX Object::getWorldMatrix()
{
	return WorldMatrix;
}

UINT Object::getVertexCount()
{
	return vertexCount;
}