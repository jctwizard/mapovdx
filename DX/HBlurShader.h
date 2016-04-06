// texture shader.h
#ifndef _HBlurShader_H_
#define _HBlurShader_H_

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class HBlurShader: public BaseShader
{
public:
	HBlurShader(ID3D11Device* device, HWND hwnd);
	~HBlurShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float textureWidth, float textureHeight);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);
	
private:
	struct TextureSizeBufferType
	{
		float textureWidth;
		float textureHeight;
		XMFLOAT2 padding;
	};

	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_TextureSizeBuffer;
};

#endif