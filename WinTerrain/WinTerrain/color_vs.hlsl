// Simple vertex shader
	
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;    
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;
	
    //input.position *= 2;
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.colour = input.colour;
    
    output.tex = input.tex;
    
    output.normal = input.normal;

    return output;
}