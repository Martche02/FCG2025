#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp" (array color_coefficients).
in vec4 cor_interpolada_pelo_rasterizador;
in vec4 position_model;
in vec4 position_world;
in vec3 normal_world;
in vec3 gouraud_color;
in vec2 texcoords_frag;

uniform sampler2D texture_image;
uniform sampler2D texture_roda;
uniform sampler2D texture_bunny;
uniform int object_id;

uniform vec4 camera_position;
uniform vec4 light_position;
uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

out vec4 color;

void main()
{
    vec4 tex_color;
    vec2 scaled_uv;
    vec3 pos = position_model.xyz;

    if (object_id == 0 || object_id == 2 || object_id == 1 || object_id == 3 || object_id == 4) {
        vec3 abs_pos = abs(pos);
        float u, v;
        if (abs_pos.x >= abs_pos.y && abs_pos.x >= abs_pos.z)
        {
            u = pos.z;
            v = pos.y;
        }
        else if (abs_pos.y >= abs_pos.x && abs_pos.y >= abs_pos.z)
        {
            u = pos.x;
            v = pos.z;
        }
        else
        {
            // Eixo Z dominante → projeção no plano XY
            u = pos.x;
            v = pos.y;
        }


        scaled_uv = fract(vec2(u, v) * 3.0);
    }
    else {
        scaled_uv = texcoords_frag;
    }
    if (object_id == 0 || object_id == 2 || object_id == 4)
        tex_color = texture(texture_image, scaled_uv);
    else if (object_id == 1)
        tex_color = texture(texture_roda, scaled_uv);
    else if (object_id == 3)
        tex_color = texture(texture_bunny, scaled_uv);
    else if (object_id == 999) {
        color.rgb = vec3(1.0);
        color.a = 1.0;
        return;
    }
    else
        tex_color = vec4(1.0);
    vec3 P = position_world.xyz;
    vec3 N = normalize(normal_world);
    vec3 L = normalize(light_position.xyz - P);
    vec3 V = normalize(camera_position.xyz - P);
    vec3 R = reflect(-L, N);
    vec3 H = normalize(L + V);

    vec3 ka = vec3(2.6);
    vec3 kd = vec3(1.0);
    vec3 ks = vec3(0.5);
    float shininess = 32.0;

    float lambert = max(dot(N, L), 0.0);
    float blinn   = pow(max(dot(N, H), 0.0), shininess);
    float phong   = pow(max(dot(R, V), 0.0), shininess);

    vec3 ambient = Ia * ka;
    vec3 diffuse = Id * kd * lambert;
    vec3 specular_blinn = Is * ks * blinn;
    vec3 specular_phong = Is * ks * phong;

    vec3 color_total;

    if (object_id == 1) {
        // Roda: apenas iluminação ambiente
        color_total = ambient;
        color = vec4(color_total, 1.0) * tex_color;
    } else {
        color_total = ambient + diffuse + 0.5 * (specular_blinn + specular_phong);
        color = vec4(color_total, 1.0) * tex_color + vec4(gouraud_color, 0.0);
    }
}

