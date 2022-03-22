// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;    
    float3 normal : NORMAL;
};

cbuffer LightBuffer : register(b0)
{
    float4 ambiantColour;
    float4 diffuseColour;
    float3 lightDirection;
    float padding;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 main(InputType input) : SV_TARGET
{       
    //float4 normal = (input.normal.r, input.normal.g,input.normal.b,1);
    float4 normal = (1, 0,1,0.5);
    //return normal;
    float4 lightColour;
    
    lightColour = calculateLighting(-lightDirection, input.normal, diffuseColour) + ambiantColour;
    
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    
    //return textureColour;
    return saturate(lightColour * textureColour);
}