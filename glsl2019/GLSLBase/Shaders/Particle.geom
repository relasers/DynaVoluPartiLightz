#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 v_Pos[];
in vec3 v_Dir[];
in float f_Speed[];
in float f_CollideTime[];

void main()
{
	vec4 newpos = gl_in[0].gl_Position;

	gl_Position = newpos + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:bottom-left
    EmitVertex();   
    gl_Position = newpos + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:bottom-right
    EmitVertex();
    gl_Position = newpos + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:top-left
    EmitVertex();
    gl_Position = newpos + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:top-right
    EmitVertex();

    EndPrimitive();
}
