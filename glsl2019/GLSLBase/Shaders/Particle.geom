#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 u_ProjView;
uniform vec3 u_CameraPos;

in vec3 v_Pos[];
in vec3 v_Dir[];
in float f_Speed[];
in float f_CollideTime[];

out vec3 vPos;
out vec3 vDir;
out float fSpeed;
out float fCollideTime;

const float size = 0.001f;

void main()
{
	fCollideTime = f_CollideTime[0];

	vec3 base = gl_in[0].gl_Position.xyz;
	vec3 up = vec3(0, 1, 0);
	vec3 to_camera = normalize(u_CameraPos - base);
	vec3 right = cross(up, to_camera);
	up = cross(to_camera, right);


	gl_Position = u_ProjView * vec4(base + ((right + up) * size), 1);
    EmitVertex();   
    gl_Position = u_ProjView * vec4(base + ((right - up) * size), 1);
    EmitVertex();
    gl_Position = u_ProjView * vec4(base + ((-right + up) * size), 1);
    EmitVertex();
    gl_Position = u_ProjView * vec4(base + ((-right - up) * size), 1);
    EmitVertex();

    EndPrimitive();
}
