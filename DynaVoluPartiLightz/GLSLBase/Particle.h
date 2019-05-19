#pragma once

struct ParticlePosition
{
	glm::float4 mPosition;
};

struct ParticleVelocity
{
	glm::float4 mVelocity;
};

struct ParticleColor
{
	glm::float4 mColor;
};

constexpr unsigned int NUM_PARTICLES = 2048*2048;
constexpr unsigned int WORK_GROUP_SIZE = 128;