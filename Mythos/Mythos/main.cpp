// Windows Header Files
#include "Resource.h"
#include <windows.h>

#include "Vector2.h"
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

#include <crtdbg.h>
#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

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

	ENABLE_LEAK_DETECTION();

	// TODO: Place code here.
	auto windowsWindow = WindowsApplication(hInstance, L"Mythos", L"Mythos Class", nCmdShow);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYTHOS));

	MSG msg;

	Mythos::Mythos* mythos = new Mythos::Mythos(&windowsWindow.GetWindow());

	if (windowsWindow.GetWindow())
	{

		HRESULT hr;

		TempVertex triangle[] = {
			{{1,  -1, -1, 1}, {1,0,0,1}},
			{{-1, -1, -1, 1}, {0.77,0.77,0,1}},
			{{-1,  1, -1, 1}, {1,1,0,1}},
			{{1,   1, -1, 1}, {0,1,0,1}},

			{{1,  -1, 1, 1}, {0,0.77,0.77,1}},
			{{-1, -1, 1, 1}, {0,1,1,1}},
			{{-1,  1, 1, 1}, {0,0,1,1}},
			{{1,   1, 1, 1}, {0.77,0,0.77,1}},
		};

		BOOL success = mythos->CreateVertexBuffer(triangle, sizeof(TempVertex) * ARRAYSIZE(triangle), "vertexBuffer");
		if (!success)
			return -1;

		success = mythos->CreateMainRenderTarget("mainRenderTarget");
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

		success = mythos->CreateIndexBuffer(triangleIndices, sizeof(int) * ARRAYSIZE(triangleIndices), "indexBuffer");
		if (!success)
			return -1;

		success = mythos->CreateConstantBuffer(nullptr, sizeof(WVP), "constantBuffer");
		if (!success)
			return -1;

		const wchar_t* vertexShaderFilePath = L"Assets/Shaders/VertexShader.hlsl";
		const wchar_t* pixelShaderFilePath = L"Assets/Shaders/PixelShader.hlsl";
		const char* entryPoint = "main";
		const char* vertexShaderModel = "vs_4_0";
		const char* pixelShaderModel = "ps_4_0";

		success = mythos->CreateVertexShader(vertexShaderFilePath, entryPoint, vertexShaderModel, "vertexShader");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(pixelShaderFilePath, entryPoint, pixelShaderModel, "pixelShader");
		if (!success)
			return -1;

		success = mythos->CreateTexture2D(L"Assets/Textures/CastleFlag.dds", "CastleFlagTexture");
		if (!success)
			return -1;

		success = mythos->CreateShaderResource("CastleFlagTexture", "CastleFlag");
		if (!success)
			return -1;

		Mythos::MythosInputElement layoutDesc[] = {
			{"POSITION", 0, Mythos::FLOAT4},
			{"COLOR", 0, Mythos::FLOAT4}
		};

		success = mythos->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), "vertexShader", "inputLayout");
		if (!success)
			return -1;

		success = mythos->CreateSimpleRasterizerState("simpleRasterizer");
		if (!success)
			return -1;

		success = mythos->CreateDepthBuffer("depthTexture", "depthBuffer");
		if (!success)
			return -1;

		success = mythos->CreateTextureSampler("samplerState");
		if (!success)
			return -1;
	}

	UINT strides[] = { sizeof(TempVertex) };
	UINT offset[] = { 0 };

	Vector3 Eye = { 0,0,-10 };
	Vector3 Target = { 0,0,1 };
	Vector3 Up = { 0,1,0 };

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

		ID3D11RenderTargetView* targets = { (ID3D11RenderTargetView*)mythos->GetResource("mainRenderTarget")->GetData() };
		mythos->ClearRenderTarget("mainRenderTarget");
		mythos->ClearDepthBuffer("depthBuffer");
		mythos->GetContext()->OMSetRenderTargets(1, &targets, (ID3D11DepthStencilView*)mythos->GetResource("depthBuffer")->GetData());
		mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());

		mythos->UpdateMythosResource("constantBuffer", &MyMatrices, sizeof(WVP));

		ID3D11Buffer* buffers = { (ID3D11Buffer*)mythos->GetResource("vertexBuffer")->GetData() };
		ID3D11Buffer* constBuffers{ (ID3D11Buffer*)mythos->GetResource("constantBuffer")->GetData() };

		mythos->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource("inputLayout")->GetData());
		mythos->GetContext()->IASetVertexBuffers(0, 1, &buffers, strides, offset);
		mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource("indexBuffer")->GetData(), DXGI_FORMAT_R32_UINT, 0);
		mythos->GetContext()->VSSetShader((ID3D11VertexShader*)mythos->GetResource("vertexShader")->GetData(), nullptr, NULL);
		mythos->GetContext()->VSSetConstantBuffers(0, 1, &constBuffers);
		mythos->GetContext()->RSSetState((ID3D11RasterizerState*)mythos->GetResource("simpleRasterizer")->GetData());


		//ID3D11ShaderResourceView* resources = { (ID3D11ShaderResourceView*)mythos->GetResource("CastleFlag")->GetData() };
		mythos->GetContext()->PSSetShader((ID3D11PixelShader*)mythos->GetResource("pixelShader")->GetData(), nullptr, NULL);
		ID3D11SamplerState* samplers = { (ID3D11SamplerState*)mythos->GetResource("samplerState")->GetData() };
		mythos->GetContext()->PSSetSamplers(0, 1, &samplers);
		//mythos->GetContext()->PSSetShaderResources(0, 1, &resources);

		mythos->GetContext()->DrawIndexed(36, 0, 0);

		mythos->Present();
	}

	delete mythos;

	return (int)msg.wParam;
}

#endif // WINDOWS
