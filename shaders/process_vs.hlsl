// texture vertex shader
// Basic shader for rendering textured geometry
cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TextureSizeBuffer : register(cb1)
{
	float textureWidth;
	float textureHeight;
	uint effectFlags;
	float texSizePadding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
	uint effectFlags : INT;
};

OutputType main(InputType input)
{
	OutputType output;
	float texelWidth, texelHeight;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	output.effectFlags = effectFlags;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	// Determine the floating point size of a texel for a screen with this specific height.
	texelWidth = 1.0f / textureWidth;
	texelHeight = 1.0f / textureHeight;

	// Create UV coordinates for the pixel and its four vertical neighbors on either side.
	output.texCoord1 = input.tex + float2(texelWidth * -1.0f, texelHeight * -1.0f);
	output.texCoord2 = input.tex + float2(texelWidth * -1.0f, texelHeight * 0.0f);
	output.texCoord3 = input.tex + float2(texelWidth * -1.0f, texelHeight * 1.0f);
	output.texCoord4 = input.tex + float2(texelWidth * 0.0f, texelHeight * -1.0f);
	output.texCoord5 = input.tex + float2(texelWidth * 0.0f, texelHeight *  0.0f);
	output.texCoord6 = input.tex + float2(texelWidth * 0.0f, texelHeight *  1.0f);
	output.texCoord7 = input.tex + float2(texelWidth * 1.0f, texelHeight *  -1.0f);
	output.texCoord8 = input.tex + float2(texelWidth * 1.0f, texelHeight *  0.0f);
	output.texCoord9 = input.tex + float2(texelWidth * 1.0f, texelHeight *  1.0f);

	output.effectFlags = effectFlags;

    return output;
}