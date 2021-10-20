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
#pragma warning(disable:4996)

#include <iostream>
#include <chrono>

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
	Mythos::MythosModel* cube = mythos->LoadMesh("Assets/Models/Cube.txt");
	Mythos::MythosModel* plane = mythos->LoadMesh("Assets/Models/Plane.txt");

	unsigned int wid = 512, hei = 512;
	unsigned int wid2 = 32, hei2 = 32;

	Mythos::MythosID cubeVertexID, cubeIndexID, deagleVertexID, deagleIndexID, planeVertexID, planeIndexID, constantBufferID, cameraPositionID, lightBufferID, mainRenderTargetID, vertexShaderID, pixelShaderID, skyboxShaderID,
		inputLayoutID, depthTextureID, depthBufferID, samplerStateID, skyboxTextureID, convolutedID, prefilteredEnvironmentID, brdfID;

	Mythos::MythosID deagleD, deagleAO, deagleN, deagleM, deagleR, white, grey, black;


	Mythos::MythosLight lights[5];
	Mythos::MythosID directionalLightID, pointLightID, spotLightID;

	Mythos::MythosMaterial deagleMaterial, standardMaterial;

	if (windowsWindow.GetWindow())
	{
		HRESULT hr;

		BOOL success = mythos->CreateModelBuffers(cube, cubeVertexID, cubeIndexID);
		if (!success)
			return -1;

		success = mythos->CreateModelBuffers(deagle, deagleVertexID, deagleIndexID);
		if (!success)
			return -1;

		success = mythos->CreateModelBuffers(plane, planeVertexID, planeIndexID);
		if (!success)
			return -1;

		Mythos::MythosBufferDescriptor constantDesc;
		constantDesc.data = nullptr;
		constantDesc.byteSize = sizeof(WVP);
		constantDesc.cpuAccess = Mythos::MythosAccessability::MYTHOS_DYNAMIC_ACCESS;

		success = mythos->CreateConstantBuffer(&constantDesc, constantBufferID);
		if (!success)
			return -1;

		constantDesc.data = nullptr;
		constantDesc.byteSize = sizeof(Math::Vector4);
		success = mythos->CreateConstantBuffer(&constantDesc, cameraPositionID);
		if (!success)
			return -1;

		constantDesc.data = nullptr;
		constantDesc.byteSize = sizeof(Mythos::MythosLight) * 5;
		success = mythos->CreateConstantBuffer(&constantDesc, lightBufferID);
		if (!success)
			return -1;

		success = mythos->CreateMainRenderTarget(mainRenderTargetID);
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

		const wchar_t* vertexShaderFilePath = L"Assets/Shaders/MainVertexShader.hlsl";
		const wchar_t* pixelShaderFilePath = L"Assets/Shaders/PBRPixelShader.hlsl";
		const char* entryPoint = "main";
		const char* vertexShaderModel = "vs_4_0";
		const char* pixelShaderModel = "ps_4_0";

		success = mythos->CreateVertexShader(vertexShaderFilePath, entryPoint, vertexShaderModel,vertexShaderID);
		if (!success)
			return -1;

		//success = mythos->CreateVertexShader(L"Assets/Shaders/DiffuseIBLVertexShader.hlsl", entryPoint, vertexShaderModel, "diffuseIBLVertex");
		//if (!success)
		//	return -1;

		//success = mythos->CreateVertexShader(L"Assets/Shaders/TextureVertexShader.hlsl", entryPoint, vertexShaderModel, "textureVertex");
		//if (!success)
		//	return -1;

		success = mythos->CreatePixelShader(pixelShaderFilePath, entryPoint, pixelShaderModel, pixelShaderID);
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/SkyboxPixelShader.hlsl", entryPoint, pixelShaderModel, skyboxShaderID);
		if (!success)
			return -1;

		/*success = mythos->CreatePixelShader(L"Assets/Shaders/EquirectangularToCubeMap.hlsl", entryPoint, pixelShaderModel, "cubemapCreator");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/IrradiencePreCompute.hlsl", entryPoint, pixelShaderModel, "diffuseIBLPixel");
		if (!success)
			return -1;
		
		success = mythos->CreatePixelShader(L"Assets/Shaders/PrefilterEnvironment.hlsl", entryPoint, pixelShaderModel, "prefilterShader");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/BRDF_LUT.hlsl", entryPoint, pixelShaderModel, "brdf_lut");
		if (!success)
			return -1;
		
		success = mythos->CreatePixelShader(L"Assets/Shaders/Color.hlsl", entryPoint, pixelShaderModel, "color");
		if (!success)
			return -1;

		success = mythos->CreatePixelShader(L"Assets/Shaders/TexturedObject.hlsl", entryPoint, pixelShaderModel, "textureShader");
		if (!success)
			return -1;*/

		Mythos::MythosInputElement layoutDesc[] = {
			{"POSITION", 0, Mythos::MYTHOS_FORMAT_32_FLOAT3},
			{"UV", 0, Mythos::MYTHOS_FORMAT_32_FLOAT2},
			{"NORMAL", 0, Mythos::MYTHOS_FORMAT_32_FLOAT3}
		};

		success = mythos->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vertexShaderID, inputLayoutID);
		if (!success)
			return -1;

		success = mythos->CreateDepthBuffer(depthTextureID, depthBufferID);
		if (!success)
			return -1;

		Math::Vector4 whiteColor = Math::Vector4(1.0);
		Math::Vector4 greyColor = Math::Vector4(0.5);
		Math::Vector4 blackColor = Math::Vector4(0.0);

		Mythos::MythosTextureDescriptor textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.bindFlags = Mythos::MYTHOS_BIND_SHADER_RESOURCE;
		textureDesc.cpuAccess = Mythos::MYTHOS_DEFAULT_ACCESS;
		textureDesc.format = Mythos::MYTHOS_FORMAT_32_FLOAT4;
		textureDesc.width = 1;
		textureDesc.height = 1;
		textureDesc.mipLevels = 0;
		textureDesc.sampleCount = 1;
		textureDesc.sampleQuality = 0;

		Mythos::MythosID tempID;

		mythos->CreateTexture2D(&textureDesc, &whiteColor, tempID);
		mythos->CreateShaderResource(mythos->GetResource(tempID), white);
		mythos->CreateTexture2D(&textureDesc, &greyColor, tempID);
		mythos->CreateShaderResource(mythos->GetResource(tempID), grey);
		mythos->CreateTexture2D(&textureDesc, &blackColor, tempID);
		mythos->CreateShaderResource(mythos->GetResource(tempID), black);

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_BaseColor.dds", deagleD);
		if (!success)
			return -1;

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Ambientocculsion.dds", deagleAO);
		if (!success)
			return -1;

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Normal.dds", deagleN);
		if (!success)
			return -1;
		
		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Metallic.dds", deagleM);
		if (!success)
			return -1;

		success = mythos->CreateShaderResource(L"Assets/Textures/Deagle_Roughness.dds", deagleR);
		if (!success)
			return -1;

		Mythos::MythosSamplerDescriptor samplerDesc;
		samplerDesc.AddressMode = Mythos::MYTHOS_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor = Math::Vector4(1, 1, 1, 1);
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MipLODBias = 0.0f;
		success = mythos->CreateTextureSampler(&samplerDesc, samplerStateID);
		if (!success)
			return -1;

		success = mythos->CreateDirectionalLight(Math::Vector3(0,-1,1), Math::Vector3(1.0, 0.9568627, 0.8392157), 1, directionalLightID);
		//success = mythos->CreateDirectionalLight(Math::Vector3(0,-1,1), Math::Vector3(0,1,1), 1, directionalLightID);
		if (!success)
			return -1;

		success = mythos->CreatePointLight(Math::Vector3(-1, 0, 0), Math::Vector3(0, 1, 1), 5, 3, pointLightID);
		if (!success)
			return -1;

		success = mythos->CreateSpotLight(Math::Vector3(0, 0, -5), Math::Vector3(0, -0.25f, 1), Math::Vector3(1, 0, 0), 10, Math::radians(5.0f), Math::radians(2.5f), Math::radians(7.5f), spotLightID);
		if (!success)
			return -1;

		lights[0] = *mythos->GetLight(directionalLightID);
		lights[1] = *mythos->GetLight(pointLightID);
		lights[2] = *mythos->GetLight(spotLightID);

		deagleMaterial = Mythos::MythosMaterial(deagleD, deagleAO, deagleN, deagleM, deagleR, NULL);
		standardMaterial = Mythos::MythosMaterial(white, white, black, white, black, NULL);
	}


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
	mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource(inputLayoutID)->GetData());

	//mythos->CreateSkyboxFromEquirectangularTexture(wid, hei, L"Assets/Textures/Newport_Loft.dds", skyboxTextureID);
	mythos->CreateSkyboxFromEquirectangularTexture(wid, hei, L"Assets/Textures/Arches_PineTree.dds", skyboxTextureID);
	//mythos->CreateSkyboxFromEquirectangularTexture(wid, hei, L"Assets/Textures/HDRI.dds", skyboxTextureID);
	mythos->ConvoluteSkybox(wid2, hei2, skyboxTextureID, convolutedID);
	mythos->CreatePrefilteredEnvironment(256, 256, skyboxTextureID, prefilteredEnvironmentID);
	mythos->CreateBRDFTexture(wid2, hei2, brdfID);

	//MyMatrices.World = Matrix4::Scale(10) * Matrix4::RotateY(PI / -2);
	MyMatrices.View = freeCamera.GetCamera();
	MyMatrices.Projection = freeCamera.GetProjection();
	mythos->UpdateMythosResource(constantBufferID, &MyMatrices, sizeof(WVP));
	mythos->UpdateMythosResource(lightBufferID, lights, sizeof(Mythos::MythosLight) * 5);


	mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());

	float rotation = 0.0;
	float deltaTime = 0.0;

	double totalTime = 0.0;
	int frames = 0;

	std::chrono::steady_clock timer;

	// Main message loop:
	while (true)
	{
		auto start = timer.now();

		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		freeCamera.GetCameraInput(deltaTime);
		MyMatrices.View = freeCamera.GetCamera();
		Math::Vector4 camPos = Math::Vector4(freeCamera.GetPosition(), 1);
		mythos->UpdateMythosResource(cameraPositionID, &camPos, sizeof(Math::Vector4));

		ID3D11RenderTargetView* targets = { (ID3D11RenderTargetView*)mythos->GetResource(mainRenderTargetID)->GetData() };
		mythos->SetClearRenderTargetColor({ 0.075,0.075,0.075,1 });
		mythos->ClearRenderTarget(mainRenderTargetID);
		mythos->ClearDepthBuffer(depthBufferID);
		mythos->GetContext()->RSSetViewports(1, &mythos->GetViewport());

		UINT strides[] = { sizeof(Mythos::MythosVertex) };
		UINT offset[] = { 0 };

		{
			MyMatrices.World = Matrix4::Scale(1, 1, -1) * Matrix4::Translate(freeCamera.GetPosition());
			mythos->UpdateMythosResource(constantBufferID, &MyMatrices, sizeof(WVP));

			mythos->GetContext()->OMSetRenderTargets(1, &targets, nullptr);

			ID3D11Buffer* buffers[] = { (ID3D11Buffer*)mythos->GetResource(cubeVertexID)->GetData() };
			ID3D11Buffer* constBuffers[] = { (ID3D11Buffer*)mythos->GetResource(constantBufferID)->GetData() };

			mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource(inputLayoutID)->GetData());
			mythos->GetContext()->IASetVertexBuffers(0, 1, buffers, strides, offset);
			mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource(cubeIndexID)->GetData(), DXGI_FORMAT_R32_UINT, 0);

			mythos->GetContext()->VSSetShader((ID3D11VertexShader*)mythos->GetResource(vertexShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->VSSetConstantBuffers(0, 1, constBuffers);

			ID3D11Buffer* pixelConstantBuffers[] = { (ID3D11Buffer*)mythos->GetResource(cameraPositionID)->GetData()};
			ID3D11SamplerState* samplers[] = { (ID3D11SamplerState*)mythos->GetResource(samplerStateID)->GetData() };
			ID3D11ShaderResourceView* srvs[] =
			{
				(ID3D11ShaderResourceView*)mythos->GetResource(skyboxTextureID)->GetData(),
			};

			mythos->GetContext()->PSSetShader((ID3D11PixelShader*)mythos->GetResource(skyboxShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
			mythos->GetContext()->PSSetConstantBuffers(0, 1, pixelConstantBuffers);
			mythos->GetContext()->PSSetSamplers(0, 1, samplers);

			mythos->GetContext()->DrawIndexed(cube->m_TotalNumIndices, 0, 0);
		}

		{
			mythos->GetContext()->OMSetRenderTargets(1, &targets, (ID3D11DepthStencilView*)mythos->GetResource(depthBufferID)->GetData());

			MyMatrices.World = Matrix4::RotateY(rotation);
			rotation += 0.002 * deltaTime;
			mythos->UpdateMythosResource(constantBufferID, &MyMatrices, sizeof(WVP));

			ID3D11Buffer* buffers[] = { (ID3D11Buffer*)mythos->GetResource(deagleVertexID)->GetData() };
			ID3D11Buffer* constBuffers[] = { (ID3D11Buffer*)mythos->GetResource(constantBufferID)->GetData() };

			mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource(inputLayoutID)->GetData());
			mythos->GetContext()->IASetVertexBuffers(0, 1, buffers, strides, offset);
			mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource(deagleIndexID)->GetData(), DXGI_FORMAT_R32_UINT, 0);

			mythos->GetContext()->VSSetShader((ID3D11VertexShader*)mythos->GetResource(vertexShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->VSSetConstantBuffers(0, 1, constBuffers);

			ID3D11Buffer* pixelConstantBuffers[] = { (ID3D11Buffer*)mythos->GetResource(cameraPositionID)->GetData(), (ID3D11Buffer*)mythos->GetResource(lightBufferID)->GetData() };
			ID3D11SamplerState* samplers[] = { (ID3D11SamplerState*)mythos->GetResource(samplerStateID)->GetData() };
			ID3D11ShaderResourceView* srvs[] =
			{
				(ID3D11ShaderResourceView*)mythos->GetResource(convolutedID)->GetData(),
				(ID3D11ShaderResourceView*)mythos->GetResource(prefilteredEnvironmentID)->GetData(),
				(ID3D11ShaderResourceView*)mythos->GetResource(brdfID)->GetData(),
			};

			mythos->GetContext()->PSSetShader((ID3D11PixelShader*)mythos->GetResource(pixelShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
			mythos->SetMaterial(3, deagleMaterial);
			mythos->GetContext()->PSSetConstantBuffers(0, ARRAYSIZE(pixelConstantBuffers), pixelConstantBuffers);
			mythos->GetContext()->PSSetSamplers(0, 1, samplers);

			mythos->GetContext()->DrawIndexed(deagle->m_TotalNumIndices, 0, 0);
		}

		{
			mythos->GetContext()->OMSetRenderTargets(1, &targets, (ID3D11DepthStencilView*)mythos->GetResource(depthBufferID)->GetData());

			MyMatrices.World = Matrix4::Translate(0, -1.5, 0);
			mythos->UpdateMythosResource(constantBufferID, &MyMatrices, sizeof(WVP));

			ID3D11Buffer* buffers[] = { (ID3D11Buffer*)mythos->GetResource(planeVertexID)->GetData() };
			ID3D11Buffer* constBuffers[] = { (ID3D11Buffer*)mythos->GetResource(constantBufferID)->GetData() };

			mythos->GetContext()->IASetInputLayout((ID3D11InputLayout*)mythos->GetResource(inputLayoutID)->GetData());
			mythos->GetContext()->IASetVertexBuffers(0, 1, buffers, strides, offset);
			mythos->GetContext()->IASetIndexBuffer((ID3D11Buffer*)mythos->GetResource(planeIndexID)->GetData(), DXGI_FORMAT_R32_UINT, 0);

			mythos->GetContext()->VSSetShader((ID3D11VertexShader*)mythos->GetResource(vertexShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->VSSetConstantBuffers(0, 1, constBuffers);

			ID3D11Buffer* pixelConstantBuffers[] = { (ID3D11Buffer*)mythos->GetResource(cameraPositionID)->GetData(), (ID3D11Buffer*)mythos->GetResource(lightBufferID)->GetData() };
			ID3D11SamplerState* samplers[] = { (ID3D11SamplerState*)mythos->GetResource(samplerStateID)->GetData() };
			ID3D11ShaderResourceView* srvs[] =
			{
				(ID3D11ShaderResourceView*)mythos->GetResource(convolutedID)->GetData(),
				(ID3D11ShaderResourceView*)mythos->GetResource(prefilteredEnvironmentID)->GetData(),
				(ID3D11ShaderResourceView*)mythos->GetResource(brdfID)->GetData(),
			};

			mythos->GetContext()->PSSetShader((ID3D11PixelShader*)mythos->GetResource(pixelShaderID)->GetData(), nullptr, NULL);
			mythos->GetContext()->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
			mythos->GetContext()->PSSetConstantBuffers(0, ARRAYSIZE(pixelConstantBuffers), pixelConstantBuffers);
			mythos->SetMaterial(3, standardMaterial);
			mythos->GetContext()->PSSetSamplers(0, 1, samplers);

			mythos->GetContext()->DrawIndexed(plane->m_TotalNumIndices, 0, 0);
		}

		mythos->Present();

		auto end = timer.now();
		deltaTime = deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		totalTime += deltaTime / 1000;

		frames++;

		if (totalTime >= 1.0) {
			totalTime -= 1.0;
			frames = 0;
		}
	}

	delete mythos;
	delete deagle;
	delete cube;
	delete plane;

	return (int)msg.wParam;
}

#endif // WINDOWS
