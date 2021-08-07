// Windows Header Files
#include "Resource.h"
#include <windows.h>

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
using namespace Math;

#include "Windows/WindowsApplication.h"
#include "Graphics/DirectX/Buffer.h"

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")
#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")

//Temporary Vertex
struct TempVertex {
	Vector4 position;
	Vector4 color;
};

struct WVP {
	Matrix4 World;
	Matrix4 View;
	Matrix4 Projection;
}MyMatrices;

Matrix4 Camera;
Vector4 CameraPosition;
float PitchClamp = 0.0f;
float YawClamp = 0.0f;

void CameraInput()
{
	float LocalX = 0, LocalZ = 0;
	float GlobalY = 0;
	float camYaw = 0;
	float camPitch = 0;
	//Movement Inputs
	if (GetAsyncKeyState(0x41))
	{
		LocalX -= 0.1f;
	}

	if (GetAsyncKeyState(0x44))
	{
		LocalX += 0.1f;
	}

	if (GetAsyncKeyState(VK_SHIFT))
	{
		GlobalY -= 0.1f;
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		GlobalY += 0.1f;
	}

	if (GetAsyncKeyState(0x53))
	{
		LocalZ -= 0.1f;
	}

	if (GetAsyncKeyState(0x57))
	{
		LocalZ += 0.1f;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		if (PitchClamp > -1.4f)
		{
			camPitch -= 0.005f;
			PitchClamp -= 0.005f;
		}
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		if (PitchClamp < 1.4f)
		{
			camPitch += 0.005f;
			PitchClamp += 0.005f;
		}
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		camYaw -= 0.005f;
		YawClamp -= 0.005f;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		camYaw += 0.005f;
		YawClamp += 0.005f;
	}

	Camera = Camera.Inverse();

	Matrix4 temp = Matrix4::Translate(LocalX, 0, LocalZ);
	Camera = temp * Camera;
	temp = Matrix4::Translate(0, GlobalY, 0);
	Camera = Camera * temp;

	Vector4 tempPosition = Camera.row3;

	Camera.row3 = { 0,0,0,1 };

	temp = Matrix4::RotateY(camYaw);
	Camera = Camera * temp;
	Camera.row3 = tempPosition;
	Camera = Matrix4::RotateX(camPitch) * Camera;

	CameraPosition = Camera.row3;

	Camera = Camera.Inverse();

	MyMatrices.View = Camera;
}


#ifdef _WIN32

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    auto windowsWindow = WindowsApplication(hInstance, L"Mythos", L"Mythos Class", nCmdShow);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYTHOS));

    MSG msg;

	//Default DirectX Rendering Necessities
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Context = nullptr;
	IDXGISwapChain* m_Swap = nullptr;
	ID3D11RenderTargetView* m_RTV = nullptr;
	D3D11_VIEWPORT m_Viewport;

	//Rasterization Stuff
	ID3D11RasterizerState* rasterState = nullptr;
	ID3D11Texture2D* zBufferTexture = nullptr;
	ID3D11DepthStencilView* depthBuffer = nullptr;

	//Rendering the Triangle
	//ID3D11Buffer* vertexBuffer = nullptr;
	//ID3D11Buffer* indexBuffer = nullptr;
	//ID3D11Buffer* constantBuffer = nullptr;

	Buffer vertexBuffer;
	Buffer indexBuffer;
	Buffer constantBuffer;

	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	if (windowsWindow.GetWindow())
	{
		RECT rect;
		GetClientRect(windowsWindow.GetWindow(), &rect);
		m_Viewport.Width = rect.right - rect.left;
		m_Viewport.Height = rect.bottom - rect.top;
		m_Viewport.TopLeftX = m_Viewport.TopLeftY = 0;
		m_Viewport.MinDepth = 0; m_Viewport.MaxDepth = 1;

		D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferDesc.Width = m_Viewport.Width;
		swapDesc.BufferDesc.Height = m_Viewport.Height;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = windowsWindow.GetWindow();
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Windowed = true;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
			&dx11, 1, D3D11_SDK_VERSION, &swapDesc, &m_Swap, &m_Device, 0, &m_Context);

		if (FAILED(hr)) {
			return -1;
		}

		ID3D11Resource* backbuffer;
		hr = m_Swap->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);
		if (FAILED(hr)) {
			return -1;
		}

		hr = m_Device->CreateRenderTargetView(backbuffer, nullptr, &m_RTV);
		if (FAILED(hr)) {
			return -1;
		}

		backbuffer->Release();

		TempVertex triangle[] = {
			{{1,  -1, -1, 1}, {1,0,0,1}},
			{{-1, -1, -1, 1}, {0.77,0.77,0,1}},
			{{-1,  1, -1, 1}, {0,1,0,1}},
			{{1,   1, -1,1}, {0,0.77,0.77,1}},

			{{1,  -1, 1, 1}, {0,0,1,1}},
			{{-1, -1, 1, 1}, {0.77,0,0.77,1}},
			{{-1,  1, 1, 1}, {1,1,1,1}},
			{{1,   1, 1,1}, {0.23,0.23,0.23,1}},
		};

		vertexBuffer = Buffer(BufferType::VertexBuffer, UsageType::UsageDefault, triangle, sizeof(TempVertex) * ARRAYSIZE(triangle), m_Device);

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));

		/*D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(TempVertex) * ARRAYSIZE(triangle);
		vertexDesc.CPUAccessFlags = 0;
		vertexDesc.MiscFlags = 0;
		vertexDesc.StructureByteStride = 0;
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;

		subData.pSysMem = triangle;

		hr = m_Device->CreateBuffer(&vertexDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return -1;*/

		int triangleIndices[] = {
			0,1,2,
			0,2,3,
			1,5,6,
			1,6,2,
			5,4,7,
			5,7,6,
			4,0,3,
			4,3,7,
			3,2,6,
			3,6,7,
			0,5,1,
			0,4,5
		};

		indexBuffer = Buffer(BufferType::IndexBuffer, UsageType::UsageDefault, triangleIndices, sizeof(int) * ARRAYSIZE(triangleIndices), m_Device);

		/*D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(int) * ARRAYSIZE(triangleIndices);
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		indexDesc.Usage = D3D11_USAGE_DEFAULT;

		subData.pSysMem = triangleIndices;

		hr = m_Device->CreateBuffer(&indexDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return -1;*/

		constantBuffer = Buffer(BufferType::ConstantBuffer, UsageType::UsageDynamic, nullptr, sizeof(WVP), m_Device);

		/*D3D11_BUFFER_DESC constantDesc;
		ZeroMemory(&constantDesc, sizeof(constantDesc));
		constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantDesc.ByteWidth = sizeof(WVP);
		constantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantDesc.MiscFlags = 0;
		constantDesc.StructureByteStride = 0;
		constantDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = m_Device->CreateBuffer(&constantDesc, NULL, &constantBuffer);
		if (FAILED(hr))
			return -1;*/

		const wchar_t* vertexShaderFilePath = L"Assets/Shaders/VertexShader.hlsl";
		const wchar_t* pixelShaderFilePath = L"Assets/Shaders/PixelShader.hlsl";
		const char* entryPoint = "main";
		const char* vertexShaderModel = "vs_4_0";
		const char* pixelShaderModel = "ps_4_0";

		ID3D10Blob* errorBlob;
		ID3D10Blob* vertexBlob;
		ID3D10Blob* pixelBlob;

		hr = D3DCompileFromFile(vertexShaderFilePath, NULL, NULL, entryPoint, vertexShaderModel, NULL, NULL, &vertexBlob, &errorBlob);

		if (FAILED(hr))
			return -1;

		hr = D3DCompileFromFile(pixelShaderFilePath, NULL, NULL, entryPoint, pixelShaderModel, NULL, NULL, &pixelBlob, &errorBlob);

		if (FAILED(hr))
			return -1;

		D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		hr = m_Device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);
		if (FAILED(hr))
			return -1;

		hr = m_Device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(hr))
			return -1;

		hr = m_Device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(hr))
			return -1;

		if (errorBlob) errorBlob->Release();
		if (vertexBlob) vertexBlob->Release();
		if (pixelBlob) pixelBlob->Release();

		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;

		hr = m_Device->CreateRasterizerState(&rasterDesc, &rasterState);

		if (FAILED(hr))
			return -1;

		D3D11_TEXTURE2D_DESC zBufferDesc;
		ZeroMemory(&zBufferDesc, sizeof(zBufferDesc));
		zBufferDesc.ArraySize = 1;
		zBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		zBufferDesc.Width = m_Viewport.Width;
		zBufferDesc.Height = m_Viewport.Height;
		zBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		zBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		zBufferDesc.SampleDesc.Count = 1;
		zBufferDesc.MipLevels = 1;

		hr = m_Device->CreateTexture2D(&zBufferDesc, nullptr, &zBufferTexture);
		if (FAILED(hr))
			return -1;

		hr = m_Device->CreateDepthStencilView(zBufferTexture, nullptr, &depthBuffer);
		if (FAILED(hr))
			return -1;
	}

	UINT strides[] = { sizeof(TempVertex) };
	UINT offset[] = { 0 };

	Vector3 Eye = { 0,0,-10};
	Vector3 Target = { 0,0,1};
	Vector3 Up = { 0,1,0};

	Matrix4 World = Matrix4::Identity;
	Camera = Matrix4::LookAtLH(Eye, Target, Up);
	Matrix4 Projection = Matrix4::PerspectiveFovLH(PI / 3.0f, 2.03428578f, 0.1f, 1000.0f);

	MyMatrices.World = World;
	MyMatrices.View = Camera;
	MyMatrices.Projection = Projection;

    // Main message loop:
	while (true)
	{
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		CameraInput();

		float color[] = { 0.0, 0.0, 0.0, 0.0 };
		m_Context->ClearRenderTargetView(m_RTV, color);
		m_Context->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_Context->OMSetRenderTargets(1, &m_RTV, depthBuffer);
		m_Context->RSSetViewports(1, &m_Viewport);

		constantBuffer.SetData(&MyMatrices, sizeof(WVP));
		constantBuffer.Update(m_Context);

		ID3D11Buffer* buffers = { vertexBuffer.GetBufferPointer() };
		ID3D11Buffer* constBuffers{ constantBuffer.GetBufferPointer() };

		m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Context->IASetInputLayout(inputLayout);
		m_Context->IASetVertexBuffers(0, 1, &buffers, strides, offset);
		m_Context->IASetIndexBuffer(indexBuffer.GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
		m_Context->VSSetShader(vertexShader, nullptr, NULL);
		m_Context->VSSetConstantBuffers(0, 1, &constBuffers);
		m_Context->RSSetState(rasterState);


		m_Context->PSSetShader(pixelShader, nullptr, NULL);

		m_Context->DrawIndexed(36, 0, 0);

		m_Swap->Present(0, 0);
	}

	if (m_Device) m_Device->Release();
	if (m_Context) m_Context->Release();
	if (m_RTV) m_RTV->Release();
	if (m_Swap) m_Swap->Release();

	if (rasterState) rasterState->Release();
	if (zBufferTexture) zBufferTexture->Release();
	if (depthBuffer) depthBuffer->Release();

	vertexBuffer.SafeRelease();
	indexBuffer.SafeRelease();
	constantBuffer.SafeRelease();
	if (inputLayout) inputLayout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();


    return (int)msg.wParam;
}

#endif // WINDOWS
