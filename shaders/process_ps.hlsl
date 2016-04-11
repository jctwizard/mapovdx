// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

#define EFFECT_NONE			0x00
#define EFFECT_BLUR			0x01
#define EFFECT_EDGEDETECT	0x02
#define EFFECT_POSTERISE	0x04
#define EFFECT_INVERT		0x08

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
	uint effectFlags : INT;
};

float4 main(InputType input) : SV_TARGET
{
	float4 colour, blurredColour, originalColour;

	// Initialize the colour to black.
	originalColour = colour = texture0.Sample(Sampler0, input.tex);
	
	float value = colour.r * 0.333f + colour.g * 0.333f + colour.b * 0.333f;
	
	// Use subtractive edge detection
	if ((input.effectFlags & EFFECT_BLUR) == EFFECT_BLUR ||
		(input.effectFlags & EFFECT_EDGEDETECT) == EFFECT_EDGEDETECT)
	{
		// Create the weights that each neighbor pixel will contribute to the blur.
		float weight4 = 0.382928;
		float weight3 = 0.241732;
		float weight2 = 0.060598;
		float weight1 = 0.005977;
		float weight0 = 0.000229;

		blurredColour = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Add the nine vertical pixels to the colour by the specific weight of each.
		blurredColour += texture0.Sample(Sampler0, input.texCoord1);
		blurredColour += texture0.Sample(Sampler0, input.texCoord2);
		blurredColour += texture0.Sample(Sampler0, input.texCoord3);
		blurredColour += texture0.Sample(Sampler0, input.texCoord4);
		blurredColour += texture0.Sample(Sampler0, input.texCoord5);
		blurredColour += texture0.Sample(Sampler0, input.texCoord6);
		blurredColour += texture0.Sample(Sampler0, input.texCoord7);
		blurredColour += texture0.Sample(Sampler0, input.texCoord8);
		blurredColour += texture0.Sample(Sampler0, input.texCoord9);
		
		blurredColour /= 9;

		colour = blurredColour;
	}
	if ((input.effectFlags & EFFECT_EDGEDETECT) == EFFECT_EDGEDETECT)
	{
		float blurredColourValue = blurredColour.r * 0.333f + blurredColour.g * 0.333f + blurredColour.b * 0.333f;

		float edgeDetectedValue = blurredColourValue - value;

		float threshold = 0.004f;

		if (edgeDetectedValue > threshold)
		{
			edgeDetectedValue = 1.0f;
		}
		else
		{
			edgeDetectedValue = 0.0f;
		}

		value = edgeDetectedValue;
		colour = float4(value, value, value, 1.0f);
	}
	// Reduce the colours
	if ((input.effectFlags & EFFECT_POSTERISE) == EFFECT_POSTERISE)
	{
		float colourCount = 5;

		if ((input.effectFlags & EFFECT_EDGEDETECT) != EFFECT_EDGEDETECT)
		{
			value = 0.0f;
		}

		colour = float4(
			floor(originalColour.r * (float(colourCount) - 0.01f)) / colourCount * (1 - value),
			floor(originalColour.g * (float(colourCount) - 0.01f)) / colourCount * (1 - value),
			floor(originalColour.b * (float(colourCount) - 0.01f)) / colourCount * (1 - value),
			1.0f);
	}
	if ((input.effectFlags & EFFECT_INVERT) == EFFECT_INVERT)
	{
		colour = float4(1.0f - colour.r, 1.0f - colour.g, 1.0f - colour.b, 1.0f);
	}

	return colour;
}