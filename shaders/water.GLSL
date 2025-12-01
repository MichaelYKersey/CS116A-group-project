#Shader Vertex
#version 330 core
layout (location = 0) in vec3 vertex_world_pos;
out vec4 perlin_pos;
uniform float perlin_progress;
uniform vec3 perlin_offset;
void main()
{
    perlin_pos = vec4(vertex_world_pos+perlin_offset, perlin_progress);
    gl_Position = vec4(vertex_world_pos, 1.0);
}

#Shader Fragment
#version 330 core
out vec4 FragColor;
in vec4 perlin_pos;

// hash ripped form internet (https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl)
// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( hash(v.x) ^ v.y); }
uint hash( uvec3 v ) { return hash( hash(v.xy) ^ v.z); }
uint hash( uvec4 v ) { return hash( hash(v.xyz) ^ v.w); }
// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}
// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
//end of RNG generator code copied

//3d + time is 4 dimessions
vec4 get_rand_unit_vec4(vec4 in_vec)
{
    //gen rand values then normalize (it's not uniform but oh well)
    vec4 rand_vec = vec4 (
        random(in_vec.wxyz),
        random(in_vec.zwxy),
        random(in_vec.yzwx),
        random(in_vec.wzyx)
    );
    //map from 0 to 1 to -1 to 1
    rand_vec = rand_vec*2.0f-1.0f;
    return normalize(rand_vec);
}
vec2 get_rand_unit_vec2(vec2 p)
{
    vec4 norm_vec = get_rand_unit_vec4(vec4(p,0.0f,0.0f));
    return normalize(norm_vec.xy);
}

//dot product of rand vector at gradient sample and perlin smaple position 
float dot_sample(vec4 gradient_position, vec4 perlin_pos)
{
    vec4 rand_vec = get_rand_unit_vec4(gradient_position);
    // float a = random(gradient_position.xy) * 6.28318530718; // 2*pi
    // vec4 rand_vec = vec4(cos(a), sin(a),0.0f,0.0f);
    vec4 diff_vec = perlin_pos - gradient_position;
    return dot(rand_vec,diff_vec);
}
// interpolating a to b using 6t^4 - 15t^5 + 10t^6
float perlin_interpolate(float a, float b, float t)
{
    // float t2 = t*t;
    // float pt = t2*t2;
    // float overall_t = 6.0f * pt;
    // pt *= t;
    // overall_t += -15.0f * pt;
    // pt *= t;
    // overall_t += 10.0f * pt;
    // float overall_t = t*t*t;
    float overall_t = t*t*t*(t*(t*6 - 15) + 10);
    return mix(a,b,overall_t);
}
float compute_perlin_noise_2d(vec2 perlin_pos_2d)
{
    vec4 perlin_pos = vec4(perlin_pos_2d.xy,0.0f,0.0f);
    vec4 ppi0 = floor(perlin_pos);
    vec4 ppi1 = ppi0 + vec4(1,1,0,0);
    vec4 fract_pos = fract(perlin_pos);
    
    vec4 v00 = vec4(ppi0.x,ppi0.y,0.0f,0.0f);
    vec4 v01 = vec4(ppi0.x,ppi1.y,0.0f,0.0f);
    vec4 v10 = vec4(ppi1.x,ppi0.y,0.0f,0.0f);
    vec4 v11 = vec4(ppi1.x,ppi1.y,0.0f,0.0f);

    float s00 = dot_sample(v00,perlin_pos);
    float s01 = dot_sample(v01,perlin_pos);
    float s10 = dot_sample(v10,perlin_pos);
    float s11 = dot_sample(v11,perlin_pos);

    float i0 = perlin_interpolate(s00,s01,fract_pos.y);
    float i1 = perlin_interpolate(s10,s11,fract_pos.y);
    float i = perlin_interpolate(i0,i1,fract_pos.x);
    return i;
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

    float s000 = perlin_interpolate(s0000,s0001,fract_pos.w);
    float s001 = perlin_interpolate(s0010,s0011,fract_pos.w);
    float s010 = perlin_interpolate(s0100,s0101,fract_pos.w);
    float s011 = perlin_interpolate(s0110,s0111,fract_pos.w);
    float s100 = perlin_interpolate(s1000,s1001,fract_pos.w);
    float s101 = perlin_interpolate(s1010,s1011,fract_pos.w);
    float s110 = perlin_interpolate(s1100,s1101,fract_pos.w);
    float s111 = perlin_interpolate(s1110,s1111,fract_pos.w);

    float s00 = perlin_interpolate(s000,s001,fract_pos.z);
    float s01 = perlin_interpolate(s010,s011,fract_pos.z);
    float s10 = perlin_interpolate(s100,s101,fract_pos.z);
    float s11 = perlin_interpolate(s110,s111,fract_pos.z);

    float s0 = perlin_interpolate(s00,s01,fract_pos.y);
    float s1 = perlin_interpolate(s10,s11,fract_pos.y);

    float s = perlin_interpolate(s0,s1,fract_pos.x);
    return s;
}

vec4 get_color(float val)
{
    //color technique from here: https://www.reddit.com/r/gamedev/comments/1kd36md/tiny_tip_on_how_to_quickly_use_perlin_noise_to/
    if (val < 0.55 && 0.52 < val) {
        return vec4(1.0f,1.0f,1.0f,1.0f);
    } else {
        return vec4(0.0f,0.0f,0.8,1.0f);
    }
    // return vec4(1.0f,1.0f,1.0f,1.0f); 
}

void main()
{
    vec4 scaled_world_pos = (perlin_pos*0.5+0.5) * 40;
    ivec4 int_world_pos = ivec4(scaled_world_pos);

    // vec4 noised = get_rand_unit_vec4(scaled_world_pos);
    // FragColor = get_color() * noised;
    // float val = dot_sample(int_world_pos,scaled_world_pos);
    // float val = dot_sample(int_world_pos+ivec4(1,1,1,1),scaled_world_pos);
    float val = compute_perlin_noise(scaled_world_pos);
    val = val *0.5+0.5;
    FragColor = get_color(val);
    FragColor.w = 1.0f;
}



// Perlin noise sources:
// - https://youtu.be/kCIaHqb60Cw
// - https://youtu.be/JrLSfSh43oA