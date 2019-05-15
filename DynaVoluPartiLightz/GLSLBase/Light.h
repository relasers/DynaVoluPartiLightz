#pragma once

typedef enum LightType
{
	Point, Spot, Directional
}LightType;

struct LightData
{
	glm::vec4 mPosition;
	glm::vec4 mLightColor;

	LightType mType; // 0 : PointLight , 1 : SpotLight, 2 : Directional light
	float mRange;
	float mFalloff;
	float mAtten0;
	
	float mAtten1;
	float mAtten2;
	float mTheta;
	float mPhi;

};

struct PointLight
{
	glm::float4 mLightColor;
	glm::float3 mPosition;
	float mRange;
	glm::float3 mAtten;
};


struct PointLight_p
{
	glm::float4 mLightColor;

	glm::float3 mPosition;
	float mRange;

	glm::float3 mAtten;
	float mPadding;
};



struct DirectionalLight
{
	glm::float4 mLightColor;
	glm::float3 mDirection;
};

struct DirectionalLight_p
{
	glm::float4 mLightColor;

	glm::float3 mDirection;
	float mPadding;
};

struct SpotLight
{
	glm::float4 mLightColor;
	glm::float3 mPosition;
	float mRange;
	glm::float3 mDirection;
	float mSpot;
	glm::float3 mAtten;
	float mPadding;
};

struct SpotLight_p
{
	glm::float4 mLightColor;

	glm::float3 mPosition;
	float mRange;

	glm::float3 mDirection;
	float mSpot;

	glm::float3 mAtten;
};
