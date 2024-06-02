#version 330


layout (lines_adjacency) in;
layout (triangle_strip) out;
layout (max_vertices = 12) out;


uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 color=vec4(1,0.5,1,1);
uniform vec4 last = vec4(0,1,0,1);


void main(void) {
	
	vec4 x[5];



	for( int i =0; i<gl_in.length();i++)
	{
		x[i] = P * V * M *gl_in[i].gl_Position;
	}
	x[4] = P * V * M * last;
	
	gl_Position = x[0];
	EmitVertex();
	gl_Position = x[1];
	EmitVertex();
	gl_Position = x[4];
	EmitVertex();
	EndPrimitive();



	gl_Position = x[1];
	EmitVertex();
	gl_Position = x[2];
	EmitVertex();
	gl_Position = x[4];
	EmitVertex();
	EndPrimitive();



	gl_Position = x[2];
	EmitVertex();
	gl_Position = x[3];
	EmitVertex();
	gl_Position = x[4];
	EmitVertex();
	EndPrimitive();

	gl_Position = x[3];
	EmitVertex();
	gl_Position = x[0];
	EmitVertex();
	gl_Position = x[4];
	EmitVertex();
	EndPrimitive();

}
