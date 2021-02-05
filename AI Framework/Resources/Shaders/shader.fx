//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};



struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

LightingResult ComputeLighting(float4 vertexPos, float3 N)
{
	LightingResult totalResult = { { 1, 1, 1, 1 },{ 0, 0, 0, 0 } };

	totalResult.Diffuse = saturate(totalResult.Diffuse);
	totalResult.Specular = saturate(totalResult.Specular);

	return totalResult;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
	output.worldPos = output.Pos;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	// multiply the normal by the world transform (to go from model space to world space)
	output.Norm = mul(float4(input.Norm, 1), World).xyz;

	output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT IN) : SV_TARGET
{
	LightingResult lit = ComputeLighting(IN.worldPos, normalize(IN.Norm));

	float4 ambient = float4(0.2, 0.2, 0.2, 1);
	float4 diffuse = lit.Diffuse;
	float4 specular = lit.Specular;

	float4 texColor = txDiffuse.Sample(samLinear, IN.Tex);

	float4 finalColor = (ambient + diffuse + specular) * texColor;
	
    if ( finalColor.r >= 0.95f &&
		finalColor.g >= 0.95f &&
		finalColor.b >= 0.95f )
        discard;
	
    return finalColor;
}

