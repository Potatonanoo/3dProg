#include "Object.h"
#include "bth_image.h"


Object::Object()
{
	g_VertexBuffer = nullptr;
	g_ShaderResourceView = nullptr;
}

Object::Object(ID3D11Device* g_Device)
{
	createVertexBuffer(g_Device);
	createTexture(g_Device);
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

void Object::createVertexBuffer(ID3D11Device* g_Device)
{
	VertexData triangleVertices[6] =
	{
		-0.5f, 0.5f, 0.0f,	// v0
		0.f, 0.f,			// q0
		0.f, 0.f, -1.f,

		0.5f, 0.5f, 0.0f,	// v1
		1.f, 0.f,			// q1
		0.f, 0.f, -1.f,

		-0.5f, -0.5f, 0.0f,	// v2
		0.f, 1.f,			// q2
		0.f, 0.f, -1.f,

		0.5f, 0.5f, 0.0f,	// v1
		1.f, 0.f,			// q1
		0.f, 0.f, -1.f,

		0.5f, -0.5f, 0.0f,	// v3
		1.f, 1.f,			// q3
		0.f, 0.f, -1.f,

		-0.5f, -0.5f, 0.0f,	// v4
		0.f, 1.f,			// q4
		0.f, 0.f, -1.f,
	};

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

void Object::createTexture(ID3D11Device* g_Device)
{
	ID3D11Texture2D* texture;

	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = BTH_IMAGE_WIDTH;
	texDesc.Height = BTH_IMAGE_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Data from the image
	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &BTH_IMAGE_DATA;
	subData.SysMemPitch = sizeof(char) * 4 * BTH_IMAGE_WIDTH;
	subData.SysMemSlicePitch = sizeof(char) * 4 * BTH_IMAGE_WIDTH * BTH_IMAGE_HEIGHT;

	g_Device->CreateTexture2D(&texDesc, &subData, &texture);
	g_Device->CreateShaderResourceView(texture, NULL, &g_ShaderResourceView);

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