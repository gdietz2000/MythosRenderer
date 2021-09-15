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
#include "Graphics/MythosFreeroamCamera.h"

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")
#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")

#include <crtdbg.h>
#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

struct WVP {
	Matrix4 World;
	Matrix4 View;
	Matrix4 Projection;
}MyMatrices;

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
	Mythos::MythosModel* deagle = mythos->LoadMesh("Assets/Models/DesertEagle.txt");
	Mythos::MythosModel* sphere = mythos->LoadMesh("Assets/Models/UvSphere.txt");
	Mythos::MythosModel* cube = mythos->LoadMesh("Assets/Models/Cube.txt");

	unsigned int wid = 512, hei = 512;
	unsigned int wid2 = 64, hei2 = 64;

	if (windowsWindow.GetWindow())
	{

		HRESULT hr;

		BOOL success = mythos->CreateModelBuffers(cube, "vertexBuffer", "indexBuffer");
		if (!success)
			return -1;

		success = mythos->CreateModelBuffers(sphere, "sphereVert", "sphereInd");
		if (!success)
			return -1;

		success = mythos->CreateModelBuffers(deagle, "deagleVert", "deagleInd");
		if (!success)
			return -1;

		Mythos::MythosBufferDescriptor constantDesc;
		constantDesc.data = nullptr;
		constantDesc.byteSize = sizeof(WVP);
		constantDesc.cpuAccess = Mythos::MythosAccessability::MYTHOS_DYNAMIC_ACCESS;

		success = mythos->CreateConstantBuffer(&constantDesc, "constantBuffer");
		if (!success)
			return -1;

		constantDesc.data = nullptr;
		constantDesc.byteSize = sizeof(Math::Vector4);
		success = mythos->CreateConstantBuffer(&constantDesc, "cameraPositionBuffer");
		if (!success)
			return -1;

		success = mythos->CreateMainRenderTarget("mainRenderTarget");
		if (!success)
			return -1;

		Mythos::MythosTextureDescriptor rtDesc;
		rtDesc.bindFlags = Mythos::MYTHOS_BIND_RENDER_TARGET | Mythos::MYTHOS_BIND_SHADER_RESOURCE;
		rtDesc.cpuAccess = Mythos::MYTHOS_DEFAULT_ACCESS;
		rtDesc.format = Mythos::MYTHOS_FORMAT_32_FLOAT4;
		rtDesc.width = mythos->GetViewport().Width;
		rtDesc.height = mythos->GetViewport().Height;
		rtDesc.mipLevels = 1;
		rtDesc.sampleCount = 1;
		rtDesc.sampleQuality = 0;

		success = mythos->CreateRenderTarget(&rtDesc, "tempTexture", "newRTV");
		if (!success)
			return -1;

		const wchar_t* vertexShaderFilePath = L"Assets/Shaders/MainVertexShader.hlsl";
		const wchar_t* pixelShaderFilePath = L"Assets/Shaders/PBRPixelShader.hlsl";
		const char* entryPoint = "main";
		const char* vertexShaderModel = "vs_4_0";
		const char* pixelShaderModel = "ps_4_0";

		success = mythos->CreateVertexShader(vertexShaderFilePath, entryPoint, vertexShaderModel, "vertexShader");
		if (!success)
			return -1;

		success = mythos->CreateVertexShader(L"Assets/Shaders/DiffuseIBLVertexShader.hlsl", entryPoint, vertexShaderModel, "diffuseIBLVertex");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(pixelShaderFilePath, entryPoint, pixelShaderModel, "pixelShader");
		if (!success)
			return -1;
		
		success = mythos->CreatePixelShader(L"Assets/Shaders/SkyboxPixelShader.hlsl", entryPoint, pixelShaderModel, "skyboxShader");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/EquirectangularToCubeMap.hlsl", entryPoint, pixelShaderModel, "cubemapCreator");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/IrradiencePreCompute.hlsl", entryPoint, pixelShaderModel, "diffuseIBLPixel");
		if (!success)
			return -1;

		Mythos::MythosInputElement layoutDesc[] = {
			{"POSITION", 0, Mythos::MYTHOS_FORMAT_32_FLOAT3},
			{"UV", 0, Mythos::MYTHOS_FORMAT_32_FLOAT2},
			{"NORMAL", 0, Mythos::MYTHOS_FORMAT_32_FLOAT3}
		};

		success = mythos->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), "vertexShader", "inputLayout");
		if (!success)
			return -1;

		Mythos::MythosRasterizerDescriptor rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.CullMode = Mythos::MYTHOS_CULL_NONE;
		rasterDesc.FillMode = Mythos::MYTHOS_FILL_SOLID;

		success = mythos->CreateRasterizerState(&rasterDesc, "simpleRasterizer");
		if (!success)
			return -1;

		success = mythos->CreateDepthBuffer("depthTexture", "depthBuffer");
		if (!success)
			return -1;

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_BaseColor.dds", "deagleDiffuse");
		if (!success)
			return -1;
		
		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Ambientocculsion.dds", "deagleAO");
		if (!success)
			return -1;

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Metallic.dds", "deagleMetal");
		if (!success)
			return -1;
		
		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Roughness.dds", "deagleRough");
		if (!success)
			return -1;

		Mythos::MythosSamplerDescriptor samplerDesc;
		samplerDesc.AddressMode = Mythos::MYTHOS_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor = Math::Vector4(1, 1, 1, 1);
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MaxLOD = 1;
		samplerDesc.MinLOD = 0;
		samplerDesc.MipLODBias = 1.0f;
		success = mythos->CreateTextureSampler(&samplerDesc, "samplerState");
		if (!success)
			return -1;
	}

	UINT strides[] = { sizeof(Mythos::MythosVertex) };
	UINT offset[] = { 0 };

	Vector3 Eye = { 0,0,-10 };
	Vector3 Target = { 0,0,1 };
	Vector3 Up = { 0,1,0 };

	Mythos::MythosFreeroamCamera freeCamera;

	Matrix4 World = Matrix4::Identity;
	//Matrix4 World = Matrix4::Scale(10) * Matrix4::RotateY(-3.14 / 2.0);

	auto temp = mythos->GetViewport();

	freeCamera.SetCamera(Eye, Target, Up);
	freeCamera.SetProjection(PI / 3.0f, temp.Width / temp.Height, 0.1f, 1000.0f);

	MyMatrices.World = Matrix4::Identity;
	MyMatrices.View = freeCamera.GetCamera();
	MyMatrices.Projection = freeCamera.GetProjection();

	mythos->SetTopology(Mythos::MYTHOS_TRIANGLE_LIST);
	mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource("inputLayout")->GetData());

	mythos->CreateSkyboxFromEquirectangularTexture(wid, hei, L"Assets/Textures/Newport_Loft.dds", "skybox");
	mythos->ConvoluteSkybox(wid2, hei2, "skybox", "convoluted");

	//MyMatrices.World = Matrix4::Scale(10) * Matrix4::RotateY(PI / -2);
	MyMatrices.View = freeCamera.GetCamera();
	MyMatrices.Projection = freeCamera.GetProjection();
	mythos->UpdateMythosResource("constantBuffer", &MyMatrices, sizeof(WVP));


	mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());

	float rotation = 0.0;

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

		MyMatrices.World = Matrix4::RotateY(rotation);
		rotation += 0.005;
		freeCamera.GetCameraInput();
		MyMatrices.View = freeCamera.GetCamera();
		Math::Vector4 camPos = Math::Vector4(freeCamera.GetPosition(), 1);
		mythos->UpdateMythosResource("cameraPositionBuffer", &camPos, sizeof(Math::Vector4));

		ID3D11RenderTargetView* targets = { (ID3D11RenderTargetView*)mythos->GetResource("mainRenderTarget")->GetData() };
		mythos->SetClearRenderTargetColor({ 0.075,0.075,0.075,1 });
		mythos->ClearRenderTarget("mainRenderTarget");
		mythos->ClearDepthBuffer("depthBuffer");
		mythos->GetContext()->OMSetRenderTargets(1, &targets, nullptr);
		mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());


		mythos->GetContext()->OMSetRenderTargets(1, &targets, (ID3D11DepthStencilView*)mythos->GetResource("depthBuffer")->GetData());

		mythos->UpdateMythosResource("constantBuffer", &MyMatrices, sizeof(WVP));

		ID3D11Buffer* buffers[] = { (ID3D11Buffer*)mythos->GetResource("deagleVert")->GetData() };
		ID3D11Buffer* constBuffers[] = { (ID3D11Buffer*)mythos->GetResource("constantBuffer")->GetData() };

		mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource("inputLayout")->GetData());
		mythos->GetContext()->IASetVertexBuffers(0, 1, buffers, strides, offset);
		mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource("deagleInd")->GetData(), DXGI_FORMAT_R32_UINT, 0);
		mythos->GetContext()->VSSetShader((ID3D11VertexShader*)mythos->GetResource("vertexShader")->GetData(), nullptr, NULL);
		mythos->GetContext()->VSSetConstantBuffers(0, 1, constBuffers);
		//mythos->GetContext()->RSSetState((ID3D11RasterizerState*)mythos->GetResource("simpleRasterizer")->GetData());

		ID3D11Buffer* pixelConstantBuffers[] = { (ID3D11Buffer*)mythos->GetResource("cameraPositionBuffer")->GetData() };
		ID3D11SamplerState* samplers[] = { (ID3D11SamplerState*)mythos->GetResource("samplerState")->GetData() };
		ID3D11ShaderResourceView* srvs[] = 
		{
			(ID3D11ShaderResourceView*)mythos->GetResource("convoluted")->GetData(),
			(ID3D11ShaderResourceView*)mythos->GetResource("deagleDiffuse")->GetData(),
			(ID3D11ShaderResourceView*)mythos->GetResource("deagleAO")->GetData(),
			(ID3D11ShaderResourceView*)mythos->GetResource("deagleMetal")->GetData(),
			(ID3D11ShaderResourceView*)mythos->GetResource("deagleRough")->GetData(),
		};

		mythos->GetContext()->PSSetShader((ID3D11PixelShader*)mythos->GetResource("pixelShader")->GetData(), nullptr, NULL);
		mythos->GetContext()->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		mythos->GetContext()->PSSetConstantBuffers(0, 1, pixelConstantBuffers);
		mythos->GetContext()->PSSetSamplers(0, 1, samplers);

		mythos->GetContext()->DrawIndexed(deagle->m_TotalNumIndices, 0, 0);

		mythos->Present();
	}

	delete mythos;
	delete deagle;
	delete sphere;
	delete cube;

	return (int)msg.wParam;
}

#endif // WINDOWS
