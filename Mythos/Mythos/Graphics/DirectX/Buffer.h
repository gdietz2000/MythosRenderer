#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

enum class BufferType
{
	VertexBuffer, IndexBuffer, ConstantBuffer
};

enum class UsageType
{
	UsageDefault, UsageImmutable, UsageDynamic
};

class Buffer
{
public:
	Buffer();
	Buffer(BufferType type, UsageType usage, void* inputData, unsigned int sizeInBytes, ID3D11Device* device);
	void SetData(void* inputData, unsigned int sizeInBytes);
	void* GetData();
	ID3D11Buffer* GetBufferPointer();
	inline void SafeRelease() {
		if (buffer) {
			buffer->Release();
			buffer = nullptr;
		}
	}
	void Update(ID3D11DeviceContext* context);
private:
	unsigned int dataSize = 0;
	void* data = nullptr;
	ID3D11Buffer* buffer = nullptr;
};