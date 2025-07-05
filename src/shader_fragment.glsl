#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp" (array color_coefficients).
in vec4 cor_interpolada_pelo_rasterizador;
in vec4 position_world;
in vec3 normal_world;
in vec3 gouraud_color;
// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

uniform vec4 camera_position;//glUniform
uniform vec4 light_position;

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

void main()
{
    vec3 l = normalize(vec3(0.0, 4.0, 4.0) - vec3(position_world));
    vec3 v = normalize(vec3(camera_position) - vec3(position_world));

    //---FONTE: CHATGPT
    vec3 P = position_world.xyz;
    vec3 L = normalize(light_position.xyz - P);
    vec3 V = normalize(camera_position.xyz - P);
    vec3 R = reflect(-L, normal_world);
    vec3 H = normalize(L + V);

    // Coeficientes espectrais (ajuste conforme o objeto)
    vec3 ka = vec3(0.2); // Ambiente
    vec3 kd = cor_interpolada_pelo_rasterizador.rgb;
    vec3 ks = vec3(0.5);
    float shininess = 32.0;

    // Modelo Lambert
    float lambert = max(dot(normal_world, L), 0.0);

    // Blinn-Phong
    float blinn = pow(max(dot(normal_world, H), 0.0), shininess);

    // Phong clássico
    float phong = pow(max(dot(R, V), 0.0), shininess);

    vec3 ambient  = Ia * ka;
    vec3 diffuse  = Id * kd * lambert;
    vec3 specular_blinn  = Is * ks * blinn;
    vec3 specular_phong  = Is * ks * phong;

    vec3 color_total = ambient + diffuse + 0.5 * (specular_blinn + specular_phong);

    // Somar com Gouraud interpolado
    color = vec4(color_total, 1.0) + vec4(gouraud_color, 0.0);
    //---FIM
}

