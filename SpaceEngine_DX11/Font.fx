cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType FontVertex(VertexInputType input)
{
	PixelInputType output;


	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	return output;
}

Texture2D ShaderFonture;
SamplerState SampleType;

cbuffer PixelBuffer
{
	float4 pixelColor;
};

float4 FontPixel(PixelInputType input) : SV_TARGET
{
	float4 color;

	color = ShaderFonture.Sample(SampleType, input.tex);

	if (color.r == 0.0f)
	{
		color.a = 0.0f;
	}

	else
	{
		color.rgb = pixelColor.rgb;
		color.a = 1.0f;
	}

	return color;
}
