#Shader Vertex
#version 330 core
layout (location = 0) in vec3 vertex_world_pos;
out vec4 frag__world_pos;
void main()
{
    frag__world_pos = vec4(vertex_world_pos,1.0f);
    gl_Position = vec4(vertex_world_pos, 1.0);
}

#Shader Fragment
#version 330 core
out vec4 FragColor;
in vec4 frag__world_pos;

//popular random num generator expaned to vec4
float rand(vec4 co){
  return fract(sin(dot(co.xyzw ,vec4(12.9898,78.233,-9898.12,-233.78))) * 43758.5453);
}

//3d + time is 4 dimessions
vec4 get_rand_unit_vec(vec4 in_vec)
{
    //gen rand values then normalize (it's not uniform but oh well)
    vec4 rand_vec = vec4 (
        rand(in_vec.wxyz),
        rand(in_vec.zwxy),
        rand(in_vec.yzwx),
        rand(in_vec.wzyx)
    );    
    return normalize(in_vec);
}

//dot product of rand vector at gradient sample and perlin smaple position 
float dot_sample(vec4 rand_val_pos, vec4 perlin_pos)
{
    vec4 a = get_rand_unit_vec(rand_val_pos);
    vec4 b = perlin_pos-rand_val_pos;
    return dot(a,b);
}
// interpolating a to b using 6t^4 - 15t^5 + 10t^6
float perlin_interpolate(float a, float b, float t)
{
    float t2 = t*t;
    float pt = t2*t2;
    float overall_t = 6.0f * pt;
    pt *= t;
    overall_t += -15.0f * pt;
    pt *= t;
    overall_t += 10.0f * pt;
    return (a *overall_t) + (b * (1-overall_t));
}

vec4 get_color()
{
    return vec4(1.0f,1.0f,1.0f, 1.0f); 
}

void main()
{
    // FragColor = vec4(1.0f,1.0f,1.0f, 1.0f);
    // FragColor = get_color();
    // FragColor = vec4(frag__world_pos.xy,1.0f,1.0f);
    float grad = (frag__world_pos.x/0.75*0.5+0.5);
    FragColor = get_color() * perlin_interpolate(0,1,grad);

}



// Perlin noise sources:
// - https://youtu.be/kCIaHqb60Cw
// - https://youtu.be/JrLSfSh43oA