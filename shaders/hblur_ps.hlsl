// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
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
};

float4 main(InputType input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3, weight4, weight5;
	float4 colour, blurredColour;

	// Create the weights that each neighbor pixel will contribute to the blur.
	weight4 = 0.382928;
	weight3 = 0.241732;
	weight2 = 0.060598;
	weight1 = 0.005977;
	weight0 = 0.000229;

	// Initialize the colour to black.
	colour = texture0.Sample(Sampler0, input.tex);

	blurredColour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Add the nine vertical pixels to the colour by the specific weight of each.
	blurredColour += texture0.Sample(Sampler0, input.texCoord1) * weight0;
	blurredColour += texture0.Sample(Sampler0, input.texCoord2) * weight1;
	blurredColour += texture0.Sample(Sampler0, input.texCoord3) * weight2;
	blurredColour += texture0.Sample(Sampler0, input.texCoord4) * weight3;
	blurredColour += texture0.Sample(Sampler0, input.texCoord5) * weight4;
	blurredColour += texture0.Sample(Sampler0, input.texCoord6) * weight3;
	blurredColour += texture0.Sample(Sampler0, input.texCoord7) * weight2;
	blurredColour += texture0.Sample(Sampler0, input.texCoord8) * weight1;
	blurredColour += texture0.Sample(Sampler0, input.texCoord9) * weight0;

	blurredColour.a = 1.0f;

	colour = blurredColour;

	return colour;
}