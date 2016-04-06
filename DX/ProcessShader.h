// texture shader.h
#ifndef _ProcessShader_H_
#define _ProcessShader_H_

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class ProcessShader: public BaseShader
{
public:
	ProcessShader(ID3D11Device* device, HWND hwnd);
	~ProcessShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float textureWidth, float textureHeight, UINT effectFlags);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);
	
private:
	struct TextureSizeBufferType
	{
		float textureWidth;
		float textureHeight;
		UINT effectFlags;
		float padding;
	};

	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_TextureSizeBuffer;
};

#endif