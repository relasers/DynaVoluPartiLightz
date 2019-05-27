#pragma once

struct Material
{
	glm::vec3 mDiffuseColor;
	float mSpecularFactor;
	float mSpecularPower;

	void SetColor(glm::vec3 _diffColor)
	{
		mDiffuseColor = _diffColor;
	}
	void SetSpecFactor(float _specFactor)
	{
		mSpecularFactor = _specFactor;
	}
	void SetSpecPower(float _specPower)
	{
		mSpecularPower = _specPower;
	}

	Material& operator= (const Material& _Material)
	{
		mDiffuseColor = _Material.mDiffuseColor;
		mSpecularFactor = _Material.mSpecularFactor;
		mSpecularPower = _Material.mSpecularPower;
		return *this;
	}
};