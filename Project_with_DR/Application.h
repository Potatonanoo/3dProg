#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>
#include <DirectXMath.h> 
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d9types.h>
#include "DDSTextureLoader.h"
#include "Object.h"
#include "Camera.h"

using namespace DirectX;

#define NUM_OBJ 6

class Application
{
private:
	HINSTANCE	g_hInstance;
	HINSTANCE	g_hPrevInstance;
	LPWSTR		g_lpCmdLine;
	int			g_nCmdShow;
	HWND		g_hwnd;
	int width;
	int height;

public:
	Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, HWND wndHandle, int width, int height);
	~Application();

	bool Initialise();
	bool Update(float dt);
	void Render();
	void createSamplerState();

private:
	bool CreateDirect3DContext();
	bool CreateDepthBuffer();
	void SetViewport();
	void SetShadowMapViewport();
	bool CreateShaders();
	bool CreateConstantBuffer();
	bool CreateGBuffer();
	void CreateQuadBuffer();
	bool CreateShadowMap();

private:
	Object** obj;
	Camera* camera;

	IDXGISwapChain*	g_SwapChain;
	ID3D11Device* g_Device;
	ID3D11DeviceContext* g_DeviceContext;
	ID3D11RenderTargetView* g_RenderTargetView;

	ID3D11SamplerState* g_SampleStateWrap;
	ID3D11SamplerState* g_SampleStateClamp;

	// Normal Rendering
	ID3D11InputLayout* g_VertexLayout;
	ID3D11VertexShader* g_VertexShader;
	ID3D11PixelShader* g_PixelShader;

	// Deferred Rendering
	ID3D11InputLayout* g_DeferredVertexLayout;
	ID3D11VertexShader* g_DeferredVertexShader;
	ID3D11PixelShader* g_DeferredPixelShader;
	ID3D11GeometryShader* g_GeometryShader;

	ID3D11Buffer* g_QuadBuffer;

	// Matrix data
	ID3D11Buffer* g_ConstantBuffer;

	// Depth Buffer
	ID3D11DepthStencilView* g_DepthStencilView;
	ID3D11Texture2D* g_DepthStencilBuffer;

	// Shadow Mapping
	ID3D11InputLayout*			g_SM_VertexLayout;
	ID3D11VertexShader*			g_SM_VertexShader;
	ID3D11PixelShader*			g_SM_PixelShader;

	ID3D11Texture2D*			g_SM_RTTexture;
	ID3D11RenderTargetView*		g_SM_RTV;
	ID3D11ShaderResourceView*	g_SM_SRV;

	ID3D11DepthStencilView*		g_SM_DSV;
	ID3D11Texture2D*			g_SM_DSTexture;

	// G-Buffer
	ID3D11Texture2D* g_GBufferTEX[4];
	ID3D11RenderTargetView* g_GBufferRTV[4];
	ID3D11ShaderResourceView* g_GBufferSRV[4];

	struct ConstantBuffer
	{
		XMMATRIX WorldMatrix;
		XMMATRIX ViewMatrix;
		XMMATRIX ProjectionMatrix;
		XMMATRIX LightViewMatrix;
		XMMATRIX LightProjectionMatrix;
	};
	ConstantBuffer ObjData;

	struct VertexData
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};
};

#endif