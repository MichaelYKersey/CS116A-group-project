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
    vec4 const_vec = vec4(12.9898,78.233,-9898.12,-233.78);
    return fract(sin(dot(co.xyzw+const_vec,-const_vec.yxwz)) * 43758.5453);
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
    return normalize(rand_vec);
}

//dot product of rand vector at gradient sample and perlin smaple position 
float dot_sample(vec4 rand_val_pos, vec4 perlin_pos)
{
    vec4 rand_vec = get_rand_unit_vec(rand_val_pos);
    vec4 dist_stance = perlin_pos-rand_val_pos;
    return dot(rand_vec,dist_stance);
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

float compute_perlin_noise(vec4 perlin_pos)
{   
    vec4 perlin_pos_int0 = ivec4(perlin_pos);
    vec4 perlin_pos_int1 = perlin_pos_int0 + ivec4(1,1,1,1);

    ivec4 v0000 = ivec4(perlin_pos_int0.x,perlin_pos_int0.y,perlin_pos_int0.z,perlin_pos_int0.w);
    float s0000 = dot_sample(v0000,perlin_pos);

    ivec4 v0001 = ivec4(perlin_pos_int0.x,perlin_pos_int0.y,perlin_pos_int0.z,perlin_pos_int1.w);
    float s0001 = dot_sample(v0001,perlin_pos);

    ivec4 v0010 = ivec4(perlin_pos_int0.x,perlin_pos_int0.y,perlin_pos_int1.z,perlin_pos_int0.w);
    float s0010 = dot_sample(v0010,perlin_pos);

    ivec4 v0011 = ivec4(perlin_pos_int0.x,perlin_pos_int0.y,perlin_pos_int1.z,perlin_pos_int1.w);
    float s0011 = dot_sample(v0011,perlin_pos);

    ivec4 v0100 = ivec4(perlin_pos_int0.x,perlin_pos_int1.y,perlin_pos_int0.z,perlin_pos_int0.w);
    float s0100 = dot_sample(v0100,perlin_pos);

    ivec4 v0101 = ivec4(perlin_pos_int0.x,perlin_pos_int1.y,perlin_pos_int0.z,perlin_pos_int1.w);
    float s0101 = dot_sample(v0101,perlin_pos);

    ivec4 v0110 = ivec4(perlin_pos_int0.x,perlin_pos_int1.y,perlin_pos_int1.z,perlin_pos_int0.w);
    float s0110 = dot_sample(v0110,perlin_pos);

    ivec4 v0111 = ivec4(perlin_pos_int0.x,perlin_pos_int1.y,perlin_pos_int1.z,perlin_pos_int1.w);
    float s0111 = dot_sample(v0111,perlin_pos);

    ivec4 v1000 = ivec4(perlin_pos_int1.x,perlin_pos_int0.y,perlin_pos_int0.z,perlin_pos_int0.w);
    float s1000 = dot_sample(v1000,perlin_pos);

    ivec4 v1001 = ivec4(perlin_pos_int1.x,perlin_pos_int0.y,perlin_pos_int0.z,perlin_pos_int1.w);
    float s1001 = dot_sample(v1001,perlin_pos);

    ivec4 v1010 = ivec4(perlin_pos_int1.x,perlin_pos_int0.y,perlin_pos_int1.z,perlin_pos_int0.w);
    float s1010 = dot_sample(v1010,perlin_pos);

    ivec4 v1011 = ivec4(perlin_pos_int1.x,perlin_pos_int0.y,perlin_pos_int1.z,perlin_pos_int1.w);
    float s1011 = dot_sample(v1011,perlin_pos);

    ivec4 v1100 = ivec4(perlin_pos_int1.x,perlin_pos_int1.y,perlin_pos_int0.z,perlin_pos_int0.w);
    float s1100 = dot_sample(v1100,perlin_pos);

    ivec4 v1101 = ivec4(perlin_pos_int1.x,perlin_pos_int1.y,perlin_pos_int0.z,perlin_pos_int1.w);
    float s1101 = dot_sample(v1101,perlin_pos);

    ivec4 v1110 = ivec4(perlin_pos_int1.x,perlin_pos_int1.y,perlin_pos_int1.z,perlin_pos_int0.w);
    float s1110 = dot_sample(v1110,perlin_pos);

    ivec4 v1111 = ivec4(perlin_pos_int1.x,perlin_pos_int1.y,perlin_pos_int1.z,perlin_pos_int1.w);
    float s1111 = dot_sample(v1111,perlin_pos);

    vec4 fract_pos = fract(perlin_pos);

    float s000 = perlin_interpolate(s0000,s0001,fract_pos.x);
    float s001 = perlin_interpolate(s0010,s0011,fract_pos.x);
    float s010 = perlin_interpolate(s0100,s0101,fract_pos.x);
    float s011 = perlin_interpolate(s0110,s0111,fract_pos.x);
    float s100 = perlin_interpolate(s1000,s1001,fract_pos.x);
    float s101 = perlin_interpolate(s1010,s1011,fract_pos.x);
    float s110 = perlin_interpolate(s1100,s1101,fract_pos.x);
    float s111 = perlin_interpolate(s1110,s1111,fract_pos.x);

    float s00 = perlin_interpolate(s000,s001,fract_pos.y);
    float s01 = perlin_interpolate(s010,s011,fract_pos.y);
    float s10 = perlin_interpolate(s100,s101,fract_pos.y);
    float s11 = perlin_interpolate(s110,s111,fract_pos.y);

    float s0 = perlin_interpolate(s00,s01,fract_pos.z);
    float s1 = perlin_interpolate(s10,s11,fract_pos.z);

    float s = perlin_interpolate(s0,s1,fract_pos.w);
    return s;
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
    // float grad = (frag__world_pos.x/0.75*0.5+0.5);
    // FragColor = get_color() * perlin_interpolate(0,1,grad);
    vec4 scaled_world_pos = frag__world_pos * 40;
    ivec4 int_world = ivec4(frag__world_pos);
    vec4 noised = get_rand_unit_vec(ivec4(frag__world_pos*40));
    // float val = dot_sample
    FragColor = get_color() * noised;
    // FragColor = get_color() * val;
    FragColor.w = 1.0f;
}



// Perlin noise sources:
// - https://youtu.be/kCIaHqb60Cw
// - https://youtu.be/JrLSfSh43oA