// texture shader.cpp
#include "ProcessShader.h"

ProcessShader::ProcessShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"../shaders/process_vs.hlsl", L"../shaders/process_ps.hlsl");
}

ProcessShader::~ProcessShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the light constant buffer.
	if (m_TextureSizeBuffer)
	{
		m_TextureSizeBuffer->Release();
		m_TextureSizeBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void ProcessShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC textureSizeBufferDesc;
	D3D11_BUFFER_DESC effectFlagsBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	textureSizeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureSizeBufferDesc.ByteWidth = sizeof(TextureSizeBufferType);
	textureSizeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	textureSizeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureSizeBufferDesc.MiscFlags = 0;
	textureSizeBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&textureSizeBufferDesc, NULL, &m_TextureSizeBuffer);
}

void ProcessShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, float textureWidth, float textureHeight, UINT effectFlags)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TextureSizeBufferType* textureSizePtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Send texture size data to vertex shader
	deviceContext->Map(m_TextureSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	textureSizePtr = (TextureSizeBufferType*)mappedResource.pData;
	textureSizePtr->textureWidth = textureWidth;
	textureSizePtr->textureHeight = textureHeight;
	textureSizePtr->effectFlags = effectFlags;
	textureSizePtr->padding = 1.0f;
	deviceContext->Unmap(m_TextureSizeBuffer, 0);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_TextureSizeBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
}

void ProcessShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



