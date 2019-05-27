#pragma once

struct ParticlePosition
{
	glm::float4 mPosition;
};

struct ParticleVelocity
{
	glm::float4 mVelocity;
};

struct ParticleLightColor
{
	// r,g,b,intensity
	glm::float4 mLightColor;
};

struct ParticleRangeAtten
{
	// atten0, atten1, atten2 , range
	glm::float4 mAttenRange;
};

//ParticleLightColor
//ParticleRangeAtten