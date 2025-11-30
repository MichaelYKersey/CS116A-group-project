#Shader Vertex
#version 330 core
layout (location = 0) in float vertex_data;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;
uniform float time;  // Animation time
out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out float IsWater;  // Flag to identify water in fragment shader

void main()
{
    //Deconstruct vertex_data
    int aPos = int(vertex_data) & ((1 << 18) - 1); //First 18 bits
    int aNormal = (int(vertex_data) >> 18) & ((1 << 6) - 1); //Next 6 bits
    int color = (int(vertex_data) >> 24) & ((1 << 3) - 1); //Next 3 bits (0-7)

    //Deconstruct position integer into x, y, z components
    int x = aPos & ((1 << 6) - 1); 
    int y = ( aPos & (((1 << 6) - 1) << 6)) >> 6;
    int z = ( aPos & (((1 << 6) - 1) << 12)) >> 12;

    //Convert normal bits to normal vector [0, 1, 2] -> [0, 1, -1]
    int nx = ((( aNormal & ((1 << 2) - 1)) + 1) % 3) - 1; 
    int ny = (((( aNormal & (((1 << 2) - 1) << 2)) >> 2) + 1) % 3) - 1;
    int nz = (((( aNormal & (((1 << 2) - 1) << 4)) >> 4) + 1) % 3) - 1;

    // Convert color bits to its corresponding color
    vec3 aColor;
    bool isWater = false;
    if(color == 0){
        aColor = vec3(0.761,0.698,0.502);  // SAND (beige)
    }else if(color == 1){
        aColor = vec3(0.04,0.44,0.15);     // GRASS (green)
    }else if(color == 2){
        aColor = vec3(0.572,0.557,0.522);  // STONE/SNOW (gray/white)
    }else if(color == 3){
        aColor = vec3(0.1, 0.3, 0.8);      // WATER - Dark blue (static blocks)
        isWater = true;
    }else if(color == 4){
        aColor = vec3(0.4, 0.85, 1.0);     // WATER PARTICLES - Light cyan (fluid physics!)
        isWater = true;
    }else{
        aColor = vec3(1.0, 0.0, 1.0);      // ERROR - magenta (shouldn't happen)
    }

    vec4 normalizedPos = vec4(x / 32.0 - 0.5, y / 32.0 - 0.5, z / 32.0 - 0.5, 1.0);

    // WATER ANIMATION - different effects for different water blocks!
    if(isWater) {
        if(ny > 0) {
            // Top faces (lake surface) - gentle waves
            normalizedPos.y += sin(time * 2.0 + normalizedPos.x * 10.0 + normalizedPos.z * 8.0) * 0.015;
        } else if(ny == 0) {
            // Vertical/side faces (falling water) - DOWNWARD FLOW EFFECT!
            float flowSpeed = time * 2.5;

            // Create downward scrolling pattern using Y coordinate
            float scrollOffset = mod(flowSpeed + normalizedPos.y * 5.0, 6.28318);  // 2*PI for smooth repeat

            // Rippling waterfall effect - moves downward
            normalizedPos.y += sin(scrollOffset) * 0.025;
            normalizedPos.x += sin(flowSpeed * 1.5 + normalizedPos.y * 12.0) * 0.015;  // Horizontal turbulence
            normalizedPos.z += cos(flowSpeed * 1.3 + normalizedPos.y * 10.0) * 0.015;  // Depth variation
        }
    }

    vec4 worldPosition = model * normalizedPos;
    gl_ClipDistance[0] = dot(worldPosition, plane);
    gl_Position = projection * view * worldPosition;
    FragPos = vec3(model * normalizedPos);
    Normal = vec3(nx, ny, nz);
    Color = aColor;
    IsWater = isWater ? 1.0 : 0.0;  // Pass water flag to fragment shader
}
#Shader Fragment
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in float IsWater;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float time;
out vec4 FragColor;
void main()
{   
    //Find ambient lighting factor
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    //Find diffuse lighting factor
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //Find Specular lighting factor
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    //Calculate result
    vec3 result = (ambient + diffuse + specular) * Color;

    // WATER FLOW EFFECT - Create visible downward-scrolling brightness waves
    if(IsWater > 0.5) {
        // Only apply to vertical faces (not top surface)
        if(abs(Normal.y) < 0.5) {
            // Create scrolling brightness pattern moving DOWN
            float flowSpeed = time * 3.0;  // Speed of flow
            float pattern = sin((FragPos.y * 8.0) - flowSpeed) * 0.5 + 0.5;  // 0 to 1

            // Add bright streaks that move downward
            float streaks = sin((FragPos.y * 15.0) - flowSpeed * 1.5);
            streaks = pow(max(streaks, 0.0), 3.0) * 0.4;  // Sharp bright lines

            // Brighten the water with animated pattern
            result = result * (0.8 + pattern * 0.4 + streaks);
        }
    }

    FragColor = vec4(result, 1.0);
}