#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 color_coefficients;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 cor_interpolada_pelo_rasterizador;
out vec4 position_world;
out vec3 normal_world;
out vec3 gouraud_color;
// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 camera_position;
uniform vec4 light_position;
uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;
// Variável booleana no código C++ também enviada para a GPU
uniform bool render_as_black;

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente está entre -1 e 1.  (Veja {+NDC2+}).
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    position_world = model * model_coefficients;
    normal_world = normalize(transpose(inverse(mat3(model))) * vec3(0.0, 0.0, 1.0));

    //---FONTE: CHATGPT
    vec3 P = (model * model_coefficients).xyz;
    vec3 N = normalize(transpose(inverse(mat3(model))) * vec3(0.0, 0.0, 1.0));
    vec3 L = normalize(light_position.xyz - P);
    vec3 V = normalize(camera_position.xyz - P);
    vec3 R = reflect(-L, N);

    // Gouraud: computa iluminação no vértice
    vec3 ka = vec3(0.2, 0.2, 0.2); // ou baseado no objeto
    vec3 kd = color_coefficients.rgb;
    vec3 ks = vec3(0.5);
    float shininess = 32.0;

    float lambert = max(dot(N, L), 0.0);
    float phong = pow(max(dot(R, V), 0.0), shininess);

    gouraud_color = Ia * ka + Id * kd * lambert + Is * ks * phong;
    //---FIM

    if ( render_as_black )
    {
        // Ignoramos o atributo cor dos vértices, colocando a cor final como
        // preta. Utilizamos isto para renderizar as arestas pretas dos cubos.
        cor_interpolada_pelo_rasterizador = vec4(0.0f,0.0f,0.0f,1.0f);
    }
    else
    {
        // Copiamos o atributo cor (de entrada) de cada vértice para a variável
        // "cor_interpolada_pelo_rasterizador". Esta variável será interpolada pelo
        // rasterizador, gerando valores interpolados para cada fragmento!  Veja o
        // arquivo "shader_fragment.glsl".
        cor_interpolada_pelo_rasterizador = color_coefficients;
    }
}

