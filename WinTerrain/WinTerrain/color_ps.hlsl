// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

struct InputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};


float4 main(InputType input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return input.colour;
}