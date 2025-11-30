#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 frag_pos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    frag_pos = gl_Position;
}

#Shader Fragment
#version 330 core
out vec4 FragColor;
in vec4 frag_pos;
//popular random num generator expaned to vec4
float rand(vec4 co){
  return fract(sin(dot(co.xyzw ,vec4(12.9898,78.233,-9898.12,-233.78))) * 43758.5453);
}

vec4 get_color()
{
    return vec4(1.0f,0.0f,0.0f, 0.25f); 
}

void main()
{
    // FragColor = vec4(1.0f,1.0f,1.0f, 1.0f);
    // FragColor = get_color();
    // FragColor = vec4(frag_pos.xy,1.0f,1.0f);
    FragColor = get_color() * rand(frag_pos);

}