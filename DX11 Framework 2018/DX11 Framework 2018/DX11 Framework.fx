//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);



//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float3 LightVecW;
	float Pad;
	float4 AmbientLight;
	float4 AmbientMtrl;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW;
}

struct VS_INPUT
{
	float4 Pos		: POSITION;
	float3 Norm		: NORMAL;
	float2 Tex		: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos		: SV_POSITION;
	float3 Norm		: NORMAL;
	float2 Tex		: TEXCOORD0;
	float3 PosW		: POSITION0;
    
};



VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(input.Pos, World);
	output.PosW = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;
	normalW = normalize(normalW);

	output.Norm = normalW;
	output.Tex = input.Tex;

	return output;
}




//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float3 toEye = normalize(EyePosW - input.PosW.xyz);
	float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;
	normalW = normalize(normalW);
	
	float3 lightVec = normalize(LightVecW);

	float3 r = reflect(-lightVec, normalW);
	float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

	// Compute Colour using Diffuse lighting only
	float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);
	float diffuseAmount = max(dot(lightVec, normalW), 0.0f);
	float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
	float3 ambient = (AmbientMtrl * AmbientLight).rgb;
	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;
	
		
	float4 color;

	color.rgb = (textureColour * (ambient + diffuse) )+ specular;
	color.a = DiffuseMtrl.a;
	
	clip(textureColour.a - 0.25f);

    return color;
}





//
//
