#version 450



struct PointLight
{
	vec4 mLightColor;
	vec3 mPosition;
	float mRange;
	vec3 mAtten;
};

struct DirectionalLight
{
	vec4 mLightColor;
	vec3 mDirection;
};

struct SpotLight
{
	vec4 mLightColor;
	vec3 mPosition;
	float mRange;
	vec3 mDirection;
	float mSpot;
	vec3 mAtten;
	float mPadding;
};




uniform sampler3D u_WorldIntensityTexture;
uniform DirectionalLight dirLight;

#define NR_POINT_LIGHTS 64  
uniform PointLight pointLights[NR_POINT_LIGHTS];


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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.mPosition - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float dist = length(light.mPosition - fragPos);
    float attenuation = 1.0 / (light.mAtten.x + light.mAtten.y * dist + light.mAtten.z * (dist * dist));    
    // combine results
    diff *= attenuation;
    spec *= attenuation;

   return vec3(diff + spec);
}



layout(location=0) out vec4 FragColor;
in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_viewDir;


void main()
{
	//FragColor = v_Color;
	FragColor = vec4(0.2f,0.2f,0.2f,1.0f);
	FragColor = vec4(0.1f,0.1f,0.1f,1.0f);
	FragColor.xyz *= CalcDirLight(dirLight,v_Normal,v_viewDir);

	vec3 new_coord = v_WorldPosition.xyz;
	new_coord /= 128.0f;
	FragColor.xyz += texture(u_WorldIntensityTexture,new_coord).xyz*v_WorldPosition.xyz/100.0f;
	//FragColor.x = ;
	//FragColor = vec4(1,1,1,1);
}
