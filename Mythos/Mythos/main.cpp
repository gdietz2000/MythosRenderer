// Windows Header Files
#include "Resource.h"
#include <windows.h>

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
using namespace Math;

#include "Windows/WindowsApplication.h"
#include "Graphics/Mythos.h"

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
	//Basic DirectX Rendering stuff
	//ID3D11Device* m_Device = nullptr;
	//ID3D11DeviceContext* m_Context = nullptr;
	//IDXGISwapChain* m_Swap = nullptr;
	//ID3D11RenderTargetView* m_RTV = nullptr;
	//D3D11_VIEWPORT m_Viewport;

	Mythos::Mythos* mythos = new Mythos::Mythos(&windowsWindow.GetWindow());

	//Rasterization Stuff
	ID3D11RasterizerState* rasterState = nullptr;
	ID3D11Texture2D* zBufferTexture = nullptr;
	ID3D11DepthStencilView* depthBuffer = nullptr;

	//Rendering the Triangle
	//ID3D11Buffer* vertexBuffer = nullptr;
	//ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;

	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	if (windowsWindow.GetWindow())
	{

		HRESULT hr;

		/*RECT rect;
		GetClientRect(windowsWindow.GetWindow(), &rect);
		
		D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(scDesc));

		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.BufferDesc.Width = rect.right - rect.left;
		scDesc.BufferDesc.Height = rect.bottom - rect.top;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.OutputWindow = windowsWindow.GetWindow();
		scDesc.SampleDesc.Count = 1;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.Windowed = true;

		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
			&dx11, 1, D3D11_SDK_VERSION, &scDesc, &m_Swap, &m_Device, nullptr, &m_Context);

		m_Viewport.Width = (FLOAT)scDesc.BufferDesc.Width;
		m_Viewport.Height = (FLOAT)scDesc.BufferDesc.Height;
		m_Viewport.TopLeftX = m_Viewport.TopLeftY = 0;
		m_Viewport.MinDepth = 0; m_Viewport.MaxDepth = 1;

		ID3D11Resource* backbuffer;

		hr = m_Swap->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);
		hr = mythos->GetCreator()->CreateRenderTargetView(backbuffer, nullptr, &m_RTV);

		backbuffer->Release();*/

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

		

		/*D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(TempVertex) * ARRAYSIZE(triangle);
		vertexDesc.CPUAccessFlags = 0;
		vertexDesc.MiscFlags = 0;
		vertexDesc.StructureByteStride = 0;
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = triangle;

		hr = mythos->GetCreator()->CreateBuffer(&vertexDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return -1;*/

		BOOL success = mythos->CreateVertexBuffer(triangle, sizeof(TempVertex) * ARRAYSIZE(triangle));
		if (!success)
			return -1;

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

		/*D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(int) * ARRAYSIZE(triangleIndices);
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		indexDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = triangleIndices;

		hr = mythos->GetCreator()->CreateBuffer(&indexDesc, &subData, &indexBuffer);
		if (FAILED(hr))
			return -1;*/

		success = mythos->CreateIndexBuffer(triangleIndices, sizeof(int) * ARRAYSIZE(triangleIndices));
		if (!success)
			return -1;

		/*D3D11_BUFFER_DESC constantDesc;
		ZeroMemory(&constantDesc, sizeof(constantDesc));
		constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantDesc.ByteWidth = sizeof(WVP);
		constantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantDesc.MiscFlags = 0;
		constantDesc.StructureByteStride = 0;
		constantDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = mythos->GetCreator()->CreateBuffer(&constantDesc, NULL, &constantBuffer);
		if (FAILED(hr))
			return -1;*/

		success = mythos->CreateConstantBuffer(nullptr, sizeof(WVP));
		if (!success)
			return -1;

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

		hr = mythos->GetCreator()->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);
		if (FAILED(hr))
			return -1;

		hr = mythos->GetCreator()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(hr))
			return -1;

		hr = mythos->GetCreator()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(hr))
			return -1;

		if (errorBlob) errorBlob->Release();
		if (vertexBlob) vertexBlob->Release();
		if (pixelBlob) pixelBlob->Release();

		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;

		hr = mythos->GetCreator()->CreateRasterizerState(&rasterDesc, &rasterState);

		if (FAILED(hr))
			return -1;

		D3D11_VIEWPORT view = mythos->GetViewport();

		D3D11_TEXTURE2D_DESC zBufferDesc;
		ZeroMemory(&zBufferDesc, sizeof(zBufferDesc));
		zBufferDesc.ArraySize = 1;
		zBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		zBufferDesc.Width = view.Width;
		zBufferDesc.Height = view.Height;
		zBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		zBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		zBufferDesc.SampleDesc.Count = 1;
		zBufferDesc.MipLevels = 1;

		hr = mythos->GetCreator()->CreateTexture2D(&zBufferDesc, nullptr, &zBufferTexture);
		if (FAILED(hr))
			return -1;

		hr = mythos->GetCreator()->CreateDepthStencilView(zBufferTexture, nullptr, &depthBuffer);
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
		ID3D11RenderTargetView* targets = { mythos->GetMainRenderTarget() };
		mythos->GetContext()->ClearRenderTargetView(mythos->GetMainRenderTarget(), color);
		mythos->GetContext()->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
		mythos->GetContext()->OMSetRenderTargets(1, &targets, depthBuffer);
		mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());

		mythos->UpdateMythosResource(2, &MyMatrices, sizeof(WVP));

		ID3D11Buffer* buffers = { (ID3D11Buffer*)mythos->GetResource(0)->GetData() };
		ID3D11Buffer* constBuffers{ (ID3D11Buffer*)mythos->GetResource(2)->GetData() };

		mythos->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mythos->GetContext()->IASetInputLayout(inputLayout);
		mythos->GetContext()->IASetVertexBuffers(0, 1, &buffers, strides, offset);
		mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource(1)->GetData(), DXGI_FORMAT_R32_UINT, 0);
		mythos->GetContext()->VSSetShader(vertexShader, nullptr, NULL);
		mythos->GetContext()->VSSetConstantBuffers(0, 1, &constBuffers);
		mythos->GetContext()->RSSetState(rasterState);


		mythos->GetContext()->PSSetShader(pixelShader, nullptr, NULL);

		mythos->GetContext()->DrawIndexed(36, 0, 0);

		mythos->Present();
	}

	/*if (m_Device) mythos->GetCreator()->Release();
	if (m_Context) mythos->GetContext()->Release();
	if (m_Swap) m_Swap->Release();
	if (m_RTV) m_RTV->Release();*/

	if (rasterState) rasterState->Release();
	if (zBufferTexture) zBufferTexture->Release();
	if (depthBuffer) depthBuffer->Release();

	//if (vertexBuffer) vertexBuffer->Release();
	//if (indexBuffer) indexBuffer->Release();
	if (constantBuffer) constantBuffer->Release();
	if (inputLayout) inputLayout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();

	delete mythos;

    return (int)msg.wParam;
}

#endif // WINDOWS
