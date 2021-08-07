// Windows Header Files
#include "Resource.h"
#include <windows.h>

#include "Windows/WindowsApplication.h"

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
#include "d3dcompiler.h"
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

//Temporary Vertex
struct TempVertex {
	XMVECTOR position;
};

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

	//Rendering the Triangle
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
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

		float AspectRatio = m_Viewport.Width / m_Viewport.Height;

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
			{{-0.5, -0.5, 1, 1}},
			{{0.5, -0.5, 1, 1}},
			{{0, 0.5, 1, 1}}
		};

		D3D11_BUFFER_DESC vertexDesc;
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

		hr = m_Device->CreateBuffer(&vertexDesc, &subData, &vertexBuffer);
		if (FAILED(hr))
			return -1;

		int triangleIndices[] = {
			1,0,2
		};

		D3D11_BUFFER_DESC indexDesc;
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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA}
		};

		hr = m_Device->CreateInputLayout(layoutDesc, 1, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);
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
	}

    // Main message loop:
	UINT strides[] = { sizeof(TempVertex) };
	UINT offset[] = { 0 };
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
		float color[] = { 1.0, 0.0, 0.0, 0.0 };
		m_Context->ClearRenderTargetView(m_RTV, color);
		m_Context->OMSetRenderTargets(1, &m_RTV, nullptr);
		m_Context->RSSetViewports(1, &m_Viewport);

		m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Context->IASetInputLayout(inputLayout);
		m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offset);
		m_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_Context->VSSetShader(vertexShader, nullptr, NULL);
		m_Context->PSSetShader(pixelShader, nullptr, NULL);

		m_Context->DrawIndexed(3, 0, 0);


		m_Swap->Present(0, 0);
	}

	if (m_Device) m_Device->Release();
	if (m_Context) m_Context->Release();
	if (m_RTV) m_RTV->Release();
	if (m_Swap) m_Swap->Release();

	if (vertexBuffer) vertexBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
	if (inputLayout) inputLayout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();


    return (int)msg.wParam;
}

#endif // WINDOWS
