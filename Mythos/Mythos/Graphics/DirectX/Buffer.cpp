#include "Buffer.h"

Buffer::Buffer()
{
	
}

Buffer::Buffer(BufferType type, UsageType usage, void* inputData, unsigned int sizeInBytes, ID3D11Device* device)
{
	dataSize = sizeInBytes;
	data = inputData;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeInBytes;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	switch (type)
	{
	case BufferType::VertexBuffer:
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	case BufferType::IndexBuffer:
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		break;
	case BufferType::ConstantBuffer:
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		break;
	default:
		throw "Invalid Buffer Type";
	}

	switch (usage)
	{
	case UsageType::UsageDefault:
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		break;
	case UsageType::UsageImmutable:
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		break;
	case UsageType::UsageDynamic:
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		break;
	default:
		throw "Invalid Usage Type";
	}

	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(D3D11_SUBRESOURCE_DATA));
	subData.pSysMem = inputData;

	HRESULT hr;

	if (inputData)
		hr = device->CreateBuffer(&bufferDesc, &subData, &buffer);
	else
		hr = device->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(hr))
		throw "Buffer Failed to be Created";
}

void Buffer::SetData(void* inputData, unsigned int sizeInBytes)
{
	dataSize = sizeInBytes;
	data = inputData;
}

void* Buffer::GetData()
{
	return data;
}

ID3D11Buffer* Buffer::GetBufferPointer()
{
	return buffer;
}

void Buffer::Update(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	memcpy(gpuBuffer.pData, data, dataSize);
	context->Unmap(buffer, 0);
}

