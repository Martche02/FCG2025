#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp" (array color_coefficients).
in vec4 cor_interpolada_pelo_rasterizador;
in vec4 position_world;
in vec3 normal_world;
// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

uniform vec4 camera_position;//glUniform

void main()
{
    vec3 l = normalize(vec3(0.0, 4.0, 4.0) - vec3(position_world));
    vec3 v = normalize(vec3(camera_position) - vec3(position_world));

    //---FONTE: CHATGPT
    // Iluminação ambiente
    vec3 ambient = vec3(0.2, 0.2, 0.2);

    // Iluminação difusa (Lambert)
    float diff = max(dot(normal_world, l), 0.0);
    vec3 kd = vec3(0.8, 0.4, 0.08); // cor difusa
    vec3 diffuse = kd * diff;

    // Iluminação especular (Blinn-Phong)
    vec3 h = normalize(l + v);
    float spec = pow(max(dot(normal_world, h), 0.0), 32.0);
    vec3 ks = vec3(0.8, 0.8, 0.8); // cor especular
    vec3 specular = ks * spec;

    color = vec4(ambient + diffuse + specular, 1.0);
    //---FIM
}

