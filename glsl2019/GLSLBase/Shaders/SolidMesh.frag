#version 450
struct DirectionalLight {
	vec4 mLightColor;
	vec3 mDirection;
};

layout(location=0) out vec4 FragColor;

uniform DirectionalLight dirLight;

in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_viewDir;
in vec4 v_WorldPosition;

const int shininess = 32;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.mDirection);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    return vec3(diff + spec);
}


void main()
{
	FragColor = vec4(0.1, 0.1, 0.1, 0);
	FragColor += vec4(CalcDirLight(dirLight,v_Normal,v_viewDir), 1);
	//FragColor.xyzw = vec4(1);
}