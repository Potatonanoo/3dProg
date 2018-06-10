#include "Application.h"

Application::Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, HWND wndHandle, int width, int height)
{
	g_SwapChain = nullptr;
	g_Device = nullptr;
	g_DeviceContext = nullptr;
	g_RenderTargetView = nullptr;
	g_VertexBuffer = nullptr;
	g_VertexLayout = nullptr;
	g_DeferredVertexLayout = nullptr;
	g_VertexShader = nullptr;
	g_DefVertexShader = nullptr;
	g_PixelShader = nullptr;
	g_DefPixelShader = nullptr;
	g_GeometryShader = nullptr;
	g_DepthStencilView = nullptr;
	g_DepthStencilBuffer = nullptr;

	this->width = width;
	this->height = height;

	// Input matrix data
	ObjData.WorldMatrix = XMMatrixIdentity();
	ObjData.ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PI*0.45f, (float)width / (float)height, 0.1f, 100.f);
	ObjData.LightViewMatrix = XMMatrixLookAtLH({ 0.f, 0.f, -5.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f });
	ObjData.LightProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PI*0.45f, 500.f / 500.f, 0.1f, 50.f);

	g_hInstance = hInstance;
	g_hPrevInstance = hPrevInstance;
	g_lpCmdLine = lpCmdLine;
	g_nCmdShow = nCmdShow;
	g_hwnd = wndHandle;
}

Application::~Application()
{
	g_VertexLayout->Release();
	g_VertexShader->Release();
	g_RenderTargetView->Release();
	g_SwapChain->Release();
	g_Device->Release();
	g_DeviceContext->Release();
	g_DepthStencilView->Release();
	g_DepthStencilBuffer->Release();
}

/* [Initialise anropas när programmet startas. Här sker alla förberedelser för att kunna köra programmet.] */
bool Application::Initialise()
{
	bool result = true;

	HRESULT hr = CreateDirect3DContext();
	if (FAILED(hr))
		result = false;

	obj = new Object*[NUM_OBJ];

	obj[0] = new Object(g_Device, "cube", L"brick.dds");
	//obj[0]->rotate(-0.2f, 0.f, 0.f);
	obj[0]->translate(-2.5f, 0.f, 0.f);

	obj[1] = new Object(g_Device, "cube", L"brick.dds");
	//obj[1]->rotate(0.f, 1.f, 0.f);
	obj[1]->translate(2.f, 2.f, 1.f);

	obj[2] = new Object(g_Device, "cube", L"brick.dds");
	//obj[2]->rotate(0.f, 1.f, 0.f);
	obj[2]->translate(0.f, 5.f, 4.f);

	obj[3] = new Object(g_Device, "cube", L"brick.dds");
	//obj[3]->rotate(0.f, 1.f, 0.f);
	obj[3]->translate(1.2f, -2.f, 3.f);

	obj[4] = new Object(g_Device, "cube", L"brick.dds");
	//obj[4]->rotate(0.f, 1.f, 0.f);
	obj[4]->translate(-1.f, -1.f, 2.f);

	obj[5] = new Object(g_Device, "notcube", L"brick.dds");
	obj[5]->translate(0.f, 0.f, 8.f);

	camera = new Camera({ 0.f, 0.f, -5.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 1.f, 0.f, 1.f });

	result = CreateDepthBuffer();
	SetViewport();
	result = CreateShaders();
	CreateQuadBuffer();
	result = CreateConstantBuffer();
	result = CreateGBuffer();
	createSamplerState();
	result = CreateShadowMap();

	return result;
}

/* [Uppdaterar allt i våran scen. (Rotation, rörelser, osv)] */
bool Application::Update(float dt)
{
	camera->update(dt);
	return true;
}

void Application::Render()
{
	UINT vertexSize = sizeof(float) * 8; // x, y, z, u, v, nx, ny, nz
	UINT offset = 0;
	UINT vertexCount = 0;

	float color[4]{ 0.f, 0.f, 1.f, 1.f };
	g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, color);
	for (int i = 0; i < 3; i++)
		g_DeviceContext->ClearRenderTargetView(g_GBufferRTV[i], color);

	g_DeviceContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	g_DeviceContext->ClearDepthStencilView(g_SM_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//** SHADOW MAPPING **//
	g_DeviceContext->OMSetRenderTargets(1, &g_SM_RTV, g_SM_DSV);
	SetShadowMapViewport();

	g_DeviceContext->IASetInputLayout(g_SM_VertexLayout);
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < NUM_OBJ; i++)
	{
		//Get the World Matrix from the obj class
		ObjData.WorldMatrix = obj[i]->getWorldMatrix();

		D3D11_MAPPED_SUBRESOURCE dataPtr;
		g_DeviceContext->Map(g_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		memcpy(dataPtr.pData, &ObjData, sizeof(ConstantBuffer));
		g_DeviceContext->Unmap(g_ConstantBuffer, 0);

		obj[i]->setVertexBuffer(g_DeviceContext);

		g_DeviceContext->VSSetShader(g_SM_VertexShader, nullptr, 0);
		g_DeviceContext->GSSetShader(nullptr, nullptr, 0);
		g_DeviceContext->PSSetShader(g_SM_PixelShader , nullptr, 0);
		g_DeviceContext->HSSetShader(nullptr, nullptr, 0);
		g_DeviceContext->DSSetShader(nullptr, nullptr, 0);

		g_DeviceContext->VSSetConstantBuffers(0, 1, &g_ConstantBuffer);

		vertexCount = obj[i]->getVertexCount();

		g_DeviceContext->Draw(vertexCount, 0);
	}


	//** DEFERRED RENDERING **//

	// Preperation for deferred draw call
	g_DeviceContext->OMSetRenderTargets(4, g_GBufferRTV, g_DepthStencilView);
	g_DeviceContext->IASetInputLayout(g_DeferredVertexLayout);
	SetViewport();


	for (int i = 0; i < NUM_OBJ; i++)
	{
		//Get the World Matrix from the obj class
		ObjData.WorldMatrix = obj[i]->getWorldMatrix();
		ObjData.ViewMatrix = camera->getViewMatrix();

		D3D11_MAPPED_SUBRESOURCE dataPtr;
		g_DeviceContext->Map(g_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		memcpy(dataPtr.pData, &ObjData, sizeof(ConstantBuffer));
		g_DeviceContext->Unmap(g_ConstantBuffer, 0);

		obj[i]->setVertexBuffer(g_DeviceContext);
		obj[i]->setShaderResourceView(g_DeviceContext); // Texture
		obj[i]->setSamplerState(g_DeviceContext);
		g_DeviceContext->GSSetConstantBuffers(0, 1, &g_ConstantBuffer);

		g_DeviceContext->VSSetShader(g_DeferredVertexShader, nullptr, 0);
		g_DeviceContext->GSSetShader(g_GeometryShader, nullptr, 0);
		g_DeviceContext->PSSetShader(g_DeferredPixelShader, nullptr, 0);
		g_DeviceContext->HSSetShader(nullptr, nullptr, 0);
		g_DeviceContext->DSSetShader(nullptr, nullptr, 0);

		vertexCount = obj[i]->getVertexCount();
		g_DeviceContext->Draw(vertexCount, 0);
	}

	//** NORMAL RENDERING **//
	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);

	vertexSize = sizeof(float) * 8; // x, y, z, u, v, nx, ny, nz
	g_DeviceContext->IASetVertexBuffers(0, 1, &g_QuadBuffer, &vertexSize, &offset);

	g_DeviceContext->VSSetShader(g_VertexShader, nullptr, 0);
	g_DeviceContext->PSSetShader(g_PixelShader, nullptr, 0);
	g_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	g_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	g_DeviceContext->GSSetShader(nullptr, nullptr, 0);

	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_DeviceContext->IASetInputLayout(g_VertexLayout);
	
	g_DeviceContext->PSSetShaderResources(0, 1, &g_GBufferSRV[0]);
	g_DeviceContext->PSSetShaderResources(1, 1, &g_GBufferSRV[1]);
	g_DeviceContext->PSSetShaderResources(2, 1, &g_GBufferSRV[2]);
	g_DeviceContext->PSSetShaderResources(3, 1, &g_GBufferSRV[3]);
	g_DeviceContext->PSSetShaderResources(4, 1, &g_SM_SRV);

	g_DeviceContext->PSSetSamplers(0, 1, &g_SampleStateWrap);
	g_DeviceContext->PSSetSamplers(1, 1, &g_SampleStateClamp); // For Shadow Mapping

	g_DeviceContext->Draw(4, 0);

	g_SwapChain->Present(0, 0);
}

/* [Skapar g_Device, g_DeviceContext, g_SwapChain, g_RenderTargetView (backbufferRTV)] */
bool Application::CreateDirect3DContext()
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = g_hwnd;								// the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL,
		NULL, D3D11_SDK_VERSION, &scd, &g_SwapChain, &g_Device, NULL, &g_DeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		// use the back buffer address to create the render target
		g_Device->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);
		pBackBuffer->Release();
	}
	return hr;
}

bool Application::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = (float)width;
	depthStencilBufferDesc.Height = (float)height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;
	
	HRESULT hr = g_Device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &g_DepthStencilBuffer);
	if (FAILED(hr))
		return false;
	
	hr = g_Device->CreateDepthStencilView(g_DepthStencilBuffer, nullptr, &g_DepthStencilView);
	if (FAILED(hr))
		return false;
	
	return true;
}

void Application::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DeviceContext->RSSetViewports(1, &vp);
}

void Application::SetShadowMapViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = 500.f;
	vp.Height = 500.f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DeviceContext->RSSetViewports(1, &vp);
}

/* [Skapar våra shaders VertexShader(behövs denna?), DeferredVertexShader, GeometryShader, PixelShader, DeferredPixelShader] */
bool Application::CreateShaders()
{
	//---------- Vertex Shader ----------//

	ID3DBlob* pVS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr;

	hr = D3DCompileFromFile( L"Vertex.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, 0, &pVS, &error );
	if (FAILED(hr))
		return false;

	hr = g_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &g_VertexShader);
	if (FAILED(hr))
			return false;

	D3D11_INPUT_ELEMENT_DESC inputDesc1[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = g_Device->CreateInputLayout(inputDesc1, ARRAYSIZE(inputDesc1), pVS->GetBufferPointer(), pVS->GetBufferSize(), &g_VertexLayout);
	if (FAILED(hr))
		return false;


	//---------- Deferred Vertex Shader ----------//

	pVS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile( L"DefVertex.hlsl", nullptr, nullptr, "VS_main", "vs_5_0", 0, 0, &pVS, &error );
	if (FAILED(hr))
		return false;

	hr = g_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &g_DeferredVertexShader);
	if (FAILED(hr))
		return false;

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = g_Device->CreateInputLayout(inputDesc2, ARRAYSIZE(inputDesc2), pVS->GetBufferPointer(), pVS->GetBufferSize(), &g_DeferredVertexLayout);
	if (FAILED(hr))
		return false;

	//---------- Geometry Shader ----------//

	ID3DBlob* pGS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile( L"Geometry.hlsl", nullptr, nullptr, "GS_main", "gs_5_0", 0, 0, &pGS, &error	);
	if (FAILED(hr))
		return false;

	hr = g_Device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &g_GeometryShader);
	if (FAILED(hr))
		return false;

	//---------- Pixel Shader ----------//

	ID3DBlob* pPS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile(L"Pixel.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, &error);
	if (FAILED(hr))
		return false;

	hr = g_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &g_PixelShader);

	if (FAILED(hr))
		return false;

	//---------- Deferred Pixel Shader ----------//

	pPS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile( L"DefPixel.hlsl", nullptr, nullptr, "PS_main", "ps_5_0", 0, 0, &pPS, &error );
	if (FAILED(hr))
		return false;

	hr = g_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &g_DeferredPixelShader);

	if (FAILED(hr))
		return false;

	//---------- ShadowMapping Vertex Shader ----------//

	pVS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile(L"ShadowMappingVS.hlsl", nullptr, nullptr, "ShadowMapVS", "vs_5_0", 0, 0, &pVS, &error);
	if (FAILED(hr))
		return false;

	hr = g_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &g_SM_VertexShader);
	if (FAILED(hr))
		return false;
	D3D11_INPUT_ELEMENT_DESC inputDesc3[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = g_Device->CreateInputLayout(inputDesc3, ARRAYSIZE(inputDesc3), pVS->GetBufferPointer(), pVS->GetBufferSize(), &g_SM_VertexLayout);
	if (FAILED(hr))
		return false;

	//---------- ShadowMapping Pixel Shader ----------//

	pPS = nullptr;
	error = nullptr;
	hr = D3DCompileFromFile(L"ShadowMappingPS.hlsl", nullptr, nullptr, "ShadowMapPS", "ps_5_0", 0, 0, &pPS, &error);
	if (FAILED(hr))
		return false;

	hr = g_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &g_SM_PixelShader);

	if (FAILED(hr))
		return false;

	pVS->Release();
	pGS->Release();
	pPS->Release();
	return true;
}

/* [Skapar våran konstanta buffer med structen ConstantBuffer1 (Application.h). Här behöver vi implementera för ConstantBuffer2 också. (eller kombinera dessa till en enda struct)] */
bool Application::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	
	g_Device->CreateBuffer(&cbDesc, nullptr, &g_ConstantBuffer);

	return true;
}

/* [Skapar våran G-Buffer som vi renderar till i Render()] */
bool Application::CreateGBuffer()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = 0;
	textureDesc.CPUAccessFlags = 0;

	for (int i = 0; i < 4; i++)
	{
		g_Device->CreateTexture2D(&textureDesc, NULL, &g_GBufferTEX[i]);
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 4; i++)
	{
		g_Device->CreateRenderTargetView(g_GBufferTEX[i], &renderTargetViewDesc, &g_GBufferRTV[i]);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	for (int i = 0; i < 4; i++)
	{
		g_Device->CreateShaderResourceView(g_GBufferTEX[i], &shaderResourceViewDesc, &g_GBufferSRV[i]);
	}

	return true;
}

void Application::CreateQuadBuffer()
{	
	VertexData quadVertices[6] =
	{
		-1.f, 1.f, 0.f,	// v0
		0.f, 0.f,			// q0
		0.f, 0.f, -1.f,

		1.f, 1.f, 0.0f,		// v1
		1.f, 0.f,			// q1
		0.f, 0.f, -1.f,

		-1.f, -1.f, 0.f,	// v2
		0.f, 1.f,			// q2
		0.f, 0.f, -1.f,

		1.f, -1.f, 0.f,		// v3
		1.f, 1.f,			// q3
		0.f, 0.f, -1.f,
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(quadVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quadVertices;
	g_Device->CreateBuffer(&bufferDesc, &data, &g_QuadBuffer);
}

void Application::createSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_Device->CreateSamplerState(&sampDesc, &g_SampleStateWrap);

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	g_Device->CreateSamplerState(&sampDesc, &g_SampleStateClamp);
}

bool Application::CreateShadowMap()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = 500.f;
	textureDesc.Height = 500.f;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	g_Device->CreateTexture2D(&textureDesc, NULL, &g_SM_RTTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	g_Device->CreateRenderTargetView(g_SM_RTTexture, &rtvDesc, &g_SM_RTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	g_Device->CreateShaderResourceView(g_SM_RTTexture, &srvDesc, &g_SM_SRV);


	D3D11_TEXTURE2D_DESC depthBufferDesc;
	depthBufferDesc.Width = 500.f;
	depthBufferDesc.Height = 500.f;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT hr = g_Device->CreateTexture2D(&depthBufferDesc, nullptr, &g_SM_DSTexture);
	if (FAILED(hr))
		return false;

	hr = g_Device->CreateDepthStencilView(g_SM_DSTexture, nullptr, &g_SM_DSV);
	if (FAILED(hr))
		return false;

	return true;
}