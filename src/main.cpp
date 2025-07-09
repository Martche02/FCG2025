//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   Trabalho Final: Bruno Bourscheid (550177) e Marcelo Gonda Stangler (587562)
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"


#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>


#include <glm/vec2.hpp>
#include <algorithm>


extern GLuint textVAO; // Tentando resolver problema do textrender
extern GLuint textVBO;
extern GLuint textprogram_id;
extern GLuint texttexture_id;

// Declaração de funções utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);
glm::mat4 g_CannonMatrix;
GLuint g_TextureCubao;
GLuint g_BunnyTexture;
GLuint g_TextureRoda;


// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void DrawCube(GLint render_as_black_uniform); // Desenha um cubo
GLuint BuildTriangles(); // Constrói triângulos para renderização
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU


// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);



// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

struct Mesh //Para desenho dos cilindros
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    int num_indices;
};
Mesh g_CylinderMesh;

struct InputState
{
    bool W = false;
    bool A = false;
    bool S = false;
    bool D = false;
}; // Criei para manter a função de callback das keys, mas tratar a movimentação na main, pois se n a movimentação ficava travada
InputState g_Input;



struct CollisionSides {
    bool left = false;
    bool right = false;
    bool front = false;
    bool back = false;

    bool collided() const {
        return left || right || front || back;
    }
};


struct BoundingObject {
    int id;            // ID único do obstáculo
    float x, z;         // Posição no plano XZ
    float angleY;       // Rotação em torno do eixo Y (em radianos)
    float width;        // Largura (em X)
    float length;       // Comprimento (em Z)
    float height;       // Altura (Y)
    int type; // 1- Box, 2- Sphere, 3- Triangle
    float radius = 0.0f; // Para esfera
    glm::vec2 v0, v1, v2; // coordenadas absolutas dos vértices quando triangulo

    BoundingObject(int id, float x, float z, float angleY, // Construtor
                   float width, float length, float height, int type, float radius,
                   glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
                    : id(id), x(x), z(z), angleY(angleY),
                      width(width), length(length), height(height), type(type),
                      radius(radius), v0(v0), v1(v1), v2(v2) {}
};



// Funções adicionais Header
Mesh CreateCylinderMesh(float radius, float height, int segments);
void DrawCylinder(Mesh& mesh, GLint render_as_black_uniform);
CollisionSides CheckWallCollision(float posX, float posZ, float angleY, float carHalfWidth, float carHalfLength, float mapWidth, float mapDepth);
CollisionSides CheckBoxCollision(float posA_X, float posA_Z, float angleA, float halfWidthA, float halfLengthA, float posB_X, float posB_Z, float angleB, float widthB, float lengthB);
CollisionSides CheckAllCollisions(float carX, float carZ, float carAngleY, float carHalfWidth, float carHalfLength, float mapWidth, float mapDepth, const std::vector<BoundingObject>& obstacles);
void FireCannon(float carX, float carZ, float carAngleY);
bool RaySegmentIntersect(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 p1, glm::vec2 p2, float& out_t);
bool RayHitsBoundingBox(const BoundingObject& obs, glm::vec2 rayOrigin, glm::vec2 rayDir, float& out_t);
float Cross2D(glm::vec2 a, glm::vec2 b);
bool RayHitsSphere(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 center, float radius, float& out_t);
bool RayHitsTriangle(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float& out_t);
BoundingObject MakeTriangle(int id, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2);
BoundingObject MakeSphere(int id, float x, float z, float radius);
BoundingObject MakeBox(int id, float x, float z, float angleY, float width, float length, float height);

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTriangles() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<const char*, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

float g_CarAngleY = 0.0f; // Var para o angulo da direção do carro

// Vars para o disparo do nosso canhão
float lastShotTime = -100.0f;
const float shotCooldown = 3.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

// Variáveis que controlam rotação do antebraço
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variáveis que controlam translação do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;
float g_TorsoPositionZ = -60.0f;


// Variável que controla se o texto informativo será mostrado na tela.
bool g_UseFreeCamera(false);

bool g_Win = false; // var que controla a vitória
float limiar_vitoria = 4.0f; // dist da parede e da linha de chegada

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;

std::vector<BoundingObject> obstacles; //Vetor com os objetos

// Time controller
float g_StartTime = 0.0f;
float g_FinalTime = 0.0f;
bool g_TimerStarted = false;
bool g_TimerStopped = false;



int main()
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 800 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "INF01047 - 550177 e 587562 - Trabalho Final", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    // Construímos a representação de um triângulo
    GLuint vertex_array_object_id = BuildTriangles();

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    int width, height, channels;
    unsigned char *image = stbi_load("../../data/chatgpt_psicodelico.png", &width, &height, &channels, 0);
    if (!image) {
        fprintf(stderr, "Erro ao carregar textura chatgpt_psicodelico.png\n");
        std::exit(EXIT_FAILURE);
    }

    glGenTextures(1, &g_TextureCubao);
    glBindTexture(GL_TEXTURE_2D, g_TextureCubao);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);

        unsigned char *image_bunny = stbi_load("../../data/bunny_texture.jpg", &width, &height, &channels, 0);
    if (!image_bunny) {
        fprintf(stderr, "Erro ao carregar bunny_texture.png\n");
        std::exit(EXIT_FAILURE);
    }

    glGenTextures(1, &g_BunnyTexture);
    glBindTexture(GL_TEXTURE_2D, g_BunnyTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image_bunny);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image_bunny);
    g_CylinderMesh = CreateCylinderMesh(1.0f, 1.0f, 32);

    unsigned char *image_roda = stbi_load("../../data/bunny_texture.jpg", &width, &height, &channels, 0);
    if (!image_roda) {
        fprintf(stderr, "Erro ao carregar textura roda.png\n");
        std::exit(EXIT_FAILURE);
    }

    glGenTextures(1, &g_TextureRoda);
    glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image_roda);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image_roda);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform           = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    GLint view_uniform            = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform      = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint render_as_black_uniform = glGetUniformLocation(g_GpuProgramID, "render_as_black"); // Variável booleana em shader_vertex.glsl
    GLint object_id_uniform = glGetUniformLocation(g_GpuProgramID, "object_id");
    GLint tex_image_uniform = glGetUniformLocation(g_GpuProgramID, "texture_image");
    GLint tex_roda_uniform  = glGetUniformLocation(g_GpuProgramID, "texture_roda");
    GLint tex_bunny_uniform = glGetUniformLocation(g_GpuProgramID, "texture_bunny");
    glUniform1i(tex_bunny_uniform, 3);
    glUniform1i(tex_image_uniform, 0); // GL_TEXTURE0
    glUniform1i(tex_roda_uniform, 1);  // GL_TEXTURE1
    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    float lastTime = glfwGetTime(); // para o delta time visto em aula
    float car_velocity = 0.0f;
    const float max_velocity = 5.0f;
    const float acceleration = 4.0f;
    const float deceleration = 3.0f;
    float rotation_speed = 2.5f; // rad/s


    // Como trabalhamos com um corpo único no movimento, e não roda a roda, vou simular a resistencia na direção pela velocidade angular (+- inércia)
    float angular_velocity = 0.0f;
    const float angular_acceleration = 6.0f; // quanto mais alto, mais responsivo
    float angular_damping = 4.0f;      // resistência da rotação

    float g_WheelAngle = 0.0f; // rotação as rodas para animar (ou tentar)


    // Inserindo obstáculos no vetor obstacles
    obstacles.push_back(MakeBox(1, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 1.5f));
    obstacles.push_back(MakeBox(2, 2.5f, 4.0f, 0.2f, 1.0f, 2.0f, 1.0f));
    obstacles.push_back(MakeBox(3, -3.0f, 4.5f, 0.0f, 1.0f, 1.5f, 1.0f));
    obstacles.push_back(MakeBox(4, 1.5f, 6.0f, 0.0f, 0.8f, 2.5f, 0.8f));
    obstacles.push_back(MakeBox(5, -2.0f, -3.0f, 0.3f, 1.0f, 1.8f, 1.0f));

// --- Coelho OBJ
//---FONTE CHATGPT
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
std::string warn, err;
GLuint g_BunnyVAO;
int g_BunnyNumTriangles;
GLuint g_BunnyTexture;
float bunny_time = 0.0f;
std::vector<glm::vec3> bezier_control_points = {
    glm::vec3(5.0f, 0.0f, 0.0f),
    glm::vec3(6.0f, 0.0f, -4.0f),
    glm::vec3(0.0f, 0.0f, -6.0f),
    glm::vec3(-6.0f, 0.0f, -4.0f),
    glm::vec3(5.0f, 0.0f, 0.0f)
};

bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../../data/bunny.obj", "../../data/");
if (!ret) {

        fprintf(stderr, "Erro ao carregar bunny.obj\n");
    std::exit(1);
}

std::vector<float> bunny_vertices;
std::vector<unsigned int> bunny_indices;

for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
        bunny_vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
        bunny_vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
        bunny_vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

        float u = 0.0f, v = 0.0f;
        if (index.texcoord_index >= 0) {
            u = attrib.texcoords[2 * index.texcoord_index + 0];
            v = attrib.texcoords[2 * index.texcoord_index + 1];
        }
        bunny_vertices.push_back(u);
        bunny_vertices.push_back(v);
    }
}

for (int i = 0; i < bunny_vertices.size() / 5; ++i) {
    bunny_indices.push_back(i);
}

// Enviar para GPU
glGenVertexArrays(1, &g_BunnyVAO);
glBindVertexArray(g_BunnyVAO);

GLuint vbo, ebo;
glGenBuffers(1, &vbo);
glGenBuffers(1, &ebo);

glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, bunny_vertices.size() * sizeof(float), bunny_vertices.data(), GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, bunny_indices.size() * sizeof(unsigned int), bunny_indices.data(), GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(2);

glBindVertexArray(0);
g_BunnyNumTriangles = bunny_indices.size();

//---FIM
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, g_BunnyTexture);


g_CannonMatrix = Matrix_Identity(); // Apenas para não dar problema na câmera quando executa o 1 frame com camfree = false


float block_time = 0.0f; // pos do bloco na curva de bezier

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {


        // Operações de movimentação
        float currentTime = glfwGetTime(); // para o delta time visto em aula
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        g_WheelAngle += car_velocity * deltaTime * -5.0f; // velocidade da roda girando


                // Cube/Map dimensions
        const float MAP_Width = 20.0f;
        const float MAP_Height = 20.0f;
        const float MAP_Depth = 150.0f;

        // Car to Bounding box Limits
        const float CAR_HALF_WIDTH  = 1.0f; // raio + tolerância
        const float CAR_HALF_LENGTH = 2.0f;




        if (g_Input.W)
        {
            car_velocity += acceleration * deltaTime;
        }
        else if (g_Input.S)
        {
            car_velocity -= acceleration * deltaTime;
        }
        else
        {
            // Desaceleração natural
            if (car_velocity > 0.0f)
            {
                car_velocity -= deceleration * deltaTime;
                if (car_velocity < 0.0f) car_velocity = 0.0f;
            }
            else if (car_velocity < 0.0f)
            {
                car_velocity += deceleration * deltaTime;
                if (car_velocity > 0.0f) car_velocity = 0.0f;
            }
        }

        // Limita a velocidade máxima
        if (car_velocity > max_velocity) car_velocity = max_velocity;
        if (car_velocity < -max_velocity) car_velocity = -max_velocity;

        // Aplica translação com base na direção atual do carro
        g_TorsoPositionX += sin(g_CarAngleY) * car_velocity * deltaTime;
        g_TorsoPositionZ += cos(g_CarAngleY) * car_velocity * deltaTime;



        if (!g_TimerStarted && (g_Input.W || g_Input.A || g_Input.S || g_Input.D))
        {
            g_StartTime = glfwGetTime();
            g_TimerStarted = true;
        }



        float carX = g_TorsoPositionX;
        float carZ = g_TorsoPositionZ;



        CollisionSides collision = CheckAllCollisions(
            carX, carZ, g_CarAngleY,
            CAR_HALF_WIDTH, CAR_HALF_LENGTH,
            MAP_Width, MAP_Depth,
            obstacles);




        float rebound_strength = 10.0f * deltaTime; // Força qe o carro volta

        glm::vec2 forwardVec(sin(g_CarAngleY), cos(g_CarAngleY));
        glm::vec2 rightVec(forwardVec.y, -forwardVec.x); // 90 graus à direita

        // Frontal
        if (collision.front) {
            g_TorsoPositionX -= forwardVec.x * rebound_strength;
            g_TorsoPositionZ -= forwardVec.y * rebound_strength;
            car_velocity = -std::abs(car_velocity) * 0.8f;
        }

        // Traseira
        if (collision.back) {
            g_TorsoPositionX += forwardVec.x * rebound_strength;
            g_TorsoPositionZ += forwardVec.y * rebound_strength;
            car_velocity = std::abs(car_velocity) * 0.8f;
        }


        // Lado esquerdo
        if (collision.left) {
            g_TorsoPositionX += rightVec.x * rebound_strength;
            g_TorsoPositionZ += rightVec.y * rebound_strength;
        }

        // Lado direito
        if (collision.right) {
            g_TorsoPositionX -= rightVec.x * rebound_strength;
            g_TorsoPositionZ -= rightVec.y * rebound_strength;
        }



        // Rotação com A e D proporcional à velocidade do carro, utilizei GPT para pesquisar como simulava esses calculos
        if (car_velocity != 0.0f)
        {
            float direction = (car_velocity > 0.0f) ? 1.0f : -1.0f;
            float turn_input = 0.0f;

            if (g_Input.A)
                turn_input = +1.0f;
            if (g_Input.D)
                turn_input = -1.0f;

            float target_angular_velocity = direction * turn_input * rotation_speed * (std::abs(car_velocity) / max_velocity);
            float angular_difference = target_angular_velocity - angular_velocity;
            float max_change = angular_acceleration * deltaTime;
            if (std::abs(angular_difference) > max_change)
                angular_difference = (angular_difference > 0 ? 1 : -1) * max_change;

            angular_velocity += angular_difference;

            // amortecimento para evitar rotação infinita
            angular_velocity -= angular_velocity * angular_damping * deltaTime;

            // angulo final
            g_CarAngleY += angular_velocity * deltaTime;
        }
        else
        {
            // Quando parado, desacelera rotação até parar
            angular_velocity -= angular_velocity * angular_damping * deltaTime;
        }







        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);
        GLint texture_uniform = glGetUniformLocation(g_GpuProgramID, "texture_image");
        glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
        glUniform1i(tex_roda_uniform, 1);
        glUniform1i(texture_uniform, 1);
        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // vértices apontados pelo VAO criado pela função BuildTriangles(). Veja
        // comentários detalhados dentro da definição de BuildTriangles().
        glBindVertexArray(vertex_array_object_id);

        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        glm::vec4 camera_position_c;
        glm::vec4 camera_lookat_l;
        glm::vec4 camera_view_vector;
        if (g_UseFreeCamera)
        {
            float r = g_CameraDistance;
            float y = r*sin(g_CameraPhi);
            float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
            float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

            // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
            // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
            camera_position_c  = glm::vec4(x,y,z,1.0f); // Ponto "c", centro da câmera
            camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
            camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada
        }
        else
        {
            glm::vec4 fogo = g_CannonMatrix * glm::vec4(0.0f, 0.0f, 1.5f, 1.0f);
            glm::vec4 dir = glm::normalize(g_CannonMatrix * glm::vec4(0.0f, 0.0f, 3.0f, 0.0f));
            glm::vec4 up_cannon  = glm::normalize(g_CannonMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

            // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
            // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
            camera_lookat_l     = fogo + up_cannon * 1.0f;
            camera_position_c   = camera_lookat_l - 10.0f * dir;
            camera_view_vector  = dir;
        }


        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

        //---FONTE: CHATGPT
        GLint light_pos_uniform     = glGetUniformLocation(g_GpuProgramID, "light_position");
        GLint camera_pos_uniform    = glGetUniformLocation(g_GpuProgramID, "camera_position");
        GLint Ia_uniform            = glGetUniformLocation(g_GpuProgramID, "Ia");
        GLint Id_uniform            = glGetUniformLocation(g_GpuProgramID, "Id");
        GLint Is_uniform            = glGetUniformLocation(g_GpuProgramID, "Is");

        glUniform4f(light_pos_uniform, 0.0f, 3.0f, 3.0f, 1.0f);
        glUniform4f(camera_pos_uniform, camera_position_c.x, camera_position_c.y, camera_position_c.z, camera_position_c.w);
        glUniform3f(Ia_uniform, 0.2f, 0.2f, 0.2f);
        glUniform3f(Id_uniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(Is_uniform, 1.0f, 1.0f, 1.0f);
        //---FIM
        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -100.0f; // Posição do "far plane"

        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        // Draw# TAREFAS DO LABORATÓRIO 3
        // Cada cópia do cubo possui uma matriz de modelagem independente,
        // já que cada cópia estará em uma posição (rotação, escala, ...)
        // diferente em relação ao espaço global (World Coordinates). Veja
        // slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //
        // Entretanto, neste laboratório as matrizes de modelagem dos cubos
        // serão construídas de maneira hierárquica, tal que operações em
        // alguns objetos influenciem outros objetos. Por exemplo: ao
        // transladar o torso, a cabeça deve se movimentar junto.
        // Veja slides 243-273 do documento Aula_08_Sistemas_de_Coordenadas.pdf
        //
        glm::mat4 model = Matrix_Identity(); // Transformação inicial = identidade.
        // Desenha cubo-pista visto de dentro
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_TextureCubao);
        glUniform1i(glGetUniformLocation(g_GpuProgramID, "texture_image"), 1);


        glActiveTexture(GL_TEXTURE0);

        glDisable(GL_CULL_FACE);



        for (const auto& obj : obstacles) // Desenha todos os obstaculos que são do tipo Box
            {
                if (obj.type == 1)
                {
                    PushMatrix(model);
                        model = model * Matrix_Translate(obj.x, obj.height, obj.z);
                        model = model * Matrix_Scale(obj.width, obj.height, obj.length);
                        model = model * Matrix_Rotate_Y(obj.angleY); // caso tenha rotação
                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                        DrawCube(render_as_black_uniform);
                    PopMatrix(model);
                }
            }

        PushMatrix(model);
            model = model * Matrix_Translate(0.0f, 0.8f, MAP_Depth/2.0f - limiar_vitoria); // eleva faixa de vitória
            model = model * Matrix_Scale(MAP_Width, 0.6f, 0.1f);   // largura, altura, profundidade
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 999);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            DrawCube(render_as_black_uniform);
        PopMatrix(model);

        PushMatrix(model);
            model = model * Matrix_Translate(0.0f, MAP_Height - 0.5f, 0.0f); // eleva o Mapa (Cubo)
            model = model * Matrix_Scale(MAP_Width, MAP_Height, MAP_Depth);   // largura, altura, profundidade
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, g_TextureCubao);
            DrawCube(render_as_black_uniform);
        PopMatrix(model);





        glEnable(GL_CULL_FACE);

        glBindTexture(GL_TEXTURE_2D, 0);
        // Translação inicial do torso
        model = model * Matrix_Translate(g_TorsoPositionX, g_TorsoPositionY + 1.0f, g_TorsoPositionZ + 0.0f);
        model = model * Matrix_Rotate_Y(g_CarAngleY);
        // Guardamos matriz model atual na pilha
        PushMatrix(model);
            // Atualizamos a matriz model (multiplicação à direita) para fazer um escalamento do torso
            model = model * Matrix_Scale(1.2f, 1.2f, 3.0f);
            // Enviamos a matriz "model" para a placa de vídeo (GPU). Veja o
            // arquivo "shader_vertex.glsl", onde esta é efetivamente
            // aplicada em todos os pontos.
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            // Desenhamos um cubo. Esta renderização irá executar o Vertex
            // Shader definido no arquivo "shader_vertex.glsl", e o mesmo irá
            // utilizar as matrizes "model", "view" e "projection" definidas
            // acima e já enviadas para a placa de vídeo (GPU).
            DrawCube(render_as_black_uniform); // Draw TORSO
        // Tiramos da pilha a matriz model guardada anteriormente
        PopMatrix(model);

            PushMatrix(model); // model = base do torso (após translação inicial)
            model = model * Matrix_Translate(0.0f, +0.1f, 1.0f); // Sobe até o topo do torso
            model = model
                  * Matrix_Rotate_Z(g_AngleZ)
                  * Matrix_Rotate_Y(g_AngleY)
                  * Matrix_Rotate_X(g_AngleX); //
            g_CannonMatrix = model;
            model = model * Matrix_Scale(0.1f, 0.1f, 1.5f); // Escala do canhão
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            DrawCube(render_as_black_uniform); // Draw Canhão
        PopMatrix(model);

        PushMatrix(model);
        model = model * Matrix_Translate(-0.7f, -1.05f, 1.0f); // Posição da roda frontal direita
        PushMatrix(model);
            model = model * Matrix_Rotate_Z(glm::half_pi<float>()); // Deita o cilindro no eixo z
            model = model * Matrix_Scale(0.4f, 0.2f, 0.4f); // raio XZ e altura Y
            model = model * Matrix_Rotate_Y(g_WheelAngle); // para animação das rodas girando
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
            DrawCylinder(g_CylinderMesh, render_as_black_uniform);
        PopMatrix(model);
        PopMatrix(model);

    PushMatrix(model); // model = base do torso
        model = model * Matrix_Translate(-0.7f, -1.05f, -1.0f); // Roda Traseira Direita
        PushMatrix(model);
            model = model * Matrix_Rotate_Z(glm::half_pi<float>()); // Deita o cilindro no eixo z
            model = model * Matrix_Scale(0.4f, 0.2f, 0.4f); // raio XZ e altura Y
            model = model * Matrix_Rotate_Y(g_WheelAngle); // para animação das rodas girando
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
            DrawCylinder(g_CylinderMesh, render_as_black_uniform);
        PopMatrix(model);
    PopMatrix(model);


            PushMatrix(model); // model = base do torso
            model = model * Matrix_Translate(+0.7f, -1.05f, 1.0f); // Roda frontal esquerda

            PushMatrix(model);
                model = model * Matrix_Rotate_Z(glm::half_pi<float>()); // Deita o cilindro no eixo z
                model = model * Matrix_Scale(0.4f, 0.2f, 0.4f); // raio XZ e altura Y
                model = model * Matrix_Rotate_Y(g_WheelAngle); // para animação das rodas girando
                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
                DrawCylinder(g_CylinderMesh, render_as_black_uniform);
            PopMatrix(model);
            PopMatrix(model);

            PushMatrix(model); // model = base do torso
            model = model * Matrix_Translate(+0.7f, -1.05f, -1.0f); // Roda traseira esquerda

            PushMatrix(model);
                model = model * Matrix_Rotate_Z(glm::half_pi<float>()); // Deita o cilindro no eixo z
                model = model * Matrix_Scale(0.4f, 0.2f, 0.4f); // raio XZ e altura Y
                model = model * Matrix_Rotate_Y(g_WheelAngle); // para animação das rodas girando
                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(object_id_uniform, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_TextureRoda);
                DrawCylinder(g_CylinderMesh, render_as_black_uniform);
            PopMatrix(model);
            PopMatrix(model);

        PopMatrix(model);
        tinyobj::attrib_t attrib;




// Bézier com 5 pontos
auto bezier = [](std::vector<glm::vec3> cp, float t) -> glm::vec3 {
    std::vector<glm::vec3> temp = cp;
    for (int k = 1; k < cp.size(); ++k)
        for (int i = 0; i < cp.size() - k; ++i)
            temp[i] = (1 - t) * temp[i] + t * temp[i + 1];
    return temp[0];
};

block_time += 0.00005f; // velocidade constante
if (block_time > 1.0f) block_time = 0.0f;

glm::vec3 block_pos = bezier(bezier_control_points, block_time);

// Atualiza o bloco de ID 1
for (auto& obs : obstacles)
{
    if (obs.id == 1 && obs.type == 1)
    {
        obs.x = block_pos.x;
        obs.z = block_pos.z;
        break;
    }
}



glm::vec3 bunny_pos = glm::vec3(-8.0f, 1.0f, 73.0f);
glm::mat4 model_bunny = glm::translate(glm::mat4(1.0f), bunny_pos);
model_bunny = model_bunny * Matrix_Rotate_Y(glm::pi<float>());
glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_bunny));
glUniform1i(object_id_uniform, 4); // bunny object id
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, g_BunnyTexture);
glBindVertexArray(g_BunnyVAO);
glDrawElements(GL_TRIANGLES, g_BunnyNumTriangles, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);

bunny_pos = glm::vec3(8.0f, 1.0f, 73.0f);
model_bunny = glm::translate(glm::mat4(1.0f), bunny_pos);
model_bunny = model_bunny * Matrix_Rotate_Y(glm::pi<float>());
glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_bunny));
glUniform1i(object_id_uniform, 4); // bunny object id
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, g_BunnyTexture);
glBindVertexArray(g_BunnyVAO);
glDrawElements(GL_TRIANGLES, g_BunnyNumTriangles, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);




        // "Desligamos" o VAO, evitando assim que operações posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);


        // Ativando VAO e VBO corretos, shader e textura usados em TextRendering
        extern GLuint textVAO, textVBO, textprogram_id, texttexture_id;

        glUseProgram(textprogram_id);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glActiveTexture(GL_TEXTURE0 + 31); // unidade usada em TextRendering_Init
        glBindTexture(GL_TEXTURE_2D, texttexture_id);


        if (g_Win)
        {
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << "Tempo Final: " << g_FinalTime << "s";

            float lineheight = TextRendering_LineHeight(window);
            float charwidth = TextRendering_CharWidth(window);
            float xpos = -((float)ss.str().length()) * charwidth / 2.0f - 0.02f;
            float ypos = 0.0f;

            TextRendering_PrintString(window, ss.str(), xpos, ypos, 2.0f);
        }
        glUseProgram(0); // limpeza opcional


        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}




void FireCannon(float carX, float carZ, float carAngleY)
{
    printf("Bang!\n");
    printf("Car position: (%.2f, %.2f)\n", carX, carZ);

    if (!g_TimerStarted)
    {
        g_StartTime = glfwGetTime();
        g_TimerStarted = true;
    }

    glm::vec2 rayOrigin = glm::vec2(carX, carZ);
    glm::vec2 rayDir = glm::normalize(glm::vec2(sin(carAngleY), cos(carAngleY)));

    printf("Front direction: (%.2f, %.2f)\n", rayDir.x, rayDir.y);

    int closestIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    for (size_t i = 0; i < obstacles.size(); ++i)
    {
        const auto& obj = obstacles[i];
        float t = -1.0f;

        if (obj.type == 1) {
            if (RayHitsBoundingBox(obj, rayOrigin, rayDir, t) && t < closestT) {
                closestT = t;
                closestIndex = i;
            }
        }
        else if (obj.type == 2) {
            glm::vec2 center(obj.x, obj.z);
            if (RayHitsSphere(rayOrigin, rayDir, center, obj.radius, t) && t < closestT) {
                closestT = t;
                closestIndex = i;
            }
        }
        else if (obj.type == 3) {
            if (RayHitsTriangle(rayOrigin, rayDir, obj.v0, obj.v1, obj.v2, t) && t < closestT) {
                closestT = t;
                closestIndex = i;
            }
        }
    }

    if (closestIndex != -1)
    {
        printf("ATINGIU!! (ID %d, TYPE %d)\n", obstacles[closestIndex].id, obstacles[closestIndex].type);
        obstacles.erase(obstacles.begin() + closestIndex);
    }
}





float Cross2D(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}


bool RaySegmentIntersect(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 p1, glm::vec2 p2, float& out_t)
{
    glm::vec2 v1 = rayOrigin - p1;
    glm::vec2 v2 = p2 - p1;
    glm::vec2 v3(-rayDir.y, rayDir.x); // perpendicular

    float dot = glm::dot(v2, v3);
    if (fabs(dot) < 1e-6f) return false;

    float t1 = Cross2D(v2, v1) / dot;
    float t2 = glm::dot(v1, v3) / dot;

    if (t1 >= 0.0f && t2 >= 0.0f && t2 <= 1.0f)
    {
        out_t = t1;
        return true;
    }

    return false;
}


bool RayHitsBoundingBox(const BoundingObject& obs, glm::vec2 rayOrigin, glm::vec2 rayDir, float& out_t)
{
    float halfW = obs.width * 0.5f;
    float halfL = obs.length * 0.5f;
    float cosA = cos(obs.angleY);
    float sinA = sin(obs.angleY);

    auto LocalToWorld = [&](float lx, float lz) -> glm::vec2 {
        float x = cosA * lx - sinA * lz + obs.x;
        float z = sinA * lx + cosA * lz + obs.z;
        return glm::vec2(x, z);
    };

    glm::vec2 bl = LocalToWorld(-halfW, -halfL);
    glm::vec2 br = LocalToWorld( halfW, -halfL);
    glm::vec2 tr = LocalToWorld( halfW,  halfL);
    glm::vec2 tl = LocalToWorld(-halfW,  halfL);

    std::vector<std::pair<glm::vec2, glm::vec2>> edges = {
        {bl, br},
        {br, tr},
        {tr, tl},
        {tl, bl}
    };

    bool hit = false;
    float minT = std::numeric_limits<float>::max();

    for (const auto& edge : edges) {
        float t;
        if (RaySegmentIntersect(rayOrigin, rayDir, edge.first, edge.second, t)) {
            if (t < minT) {
                minT = t;
                hit = true;
            }
        }
    }

    if (hit)
        out_t = minT;

    return hit;
}


bool RayHitsSphere(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 center, float radius, float& out_t)
{
    glm::vec2 L = center - rayOrigin;

    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(rayDir, L);
    float c = glm::dot(L, L) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
        return false;

    float t = (-b - sqrt(discriminant)) / (2.0f * a);
    if (t >= 0.0f) {
        out_t = t;
        return true;
    }

    return false;
}



bool RayHitsTriangle(glm::vec2 rayOrigin, glm::vec2 rayDir,
                     glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float& out_t)
{
    glm::vec2 edge1 = v1 - v0;
    glm::vec2 edge2 = v2 - v0;

    glm::vec2 normal(-rayDir.y, rayDir.x); // perpendicular

    float det = glm::dot(edge1, normal);
    if (fabs(det) < 1e-6f) return false;

    float u = glm::dot(rayOrigin - v0, normal) / det;
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec2 q = rayOrigin - v0 - u * edge1;
    float v = glm::dot(rayDir, q) / glm::dot(edge2, rayDir);

    if (v >= 0.0f && u + v <= 1.0f)
    {
        out_t = glm::length(rayDir) * u; // aproximação simples da distância
        return true;
    }

    return false;
}



CollisionSides CheckBoxCollision(
    float posA_X, float posA_Z, float angleA, float halfWidthA, float halfLengthA,
    float posB_X, float posB_Z, float angleB, float widthB, float lengthB)
{
    using namespace glm;

    CollisionSides collision;

    // faces Box A
    vec2 forwardA(sin(angleA), cos(angleA));
    vec2 rightA(forwardA.y, -forwardA.x);
    vec2 centerA(posA_X, posA_Z);

    vec2 pointFront = centerA + forwardA * halfLengthA;
    vec2 pointBack  = centerA - forwardA * halfLengthA;
    vec2 pointRight = centerA + rightA   * halfWidthA;
    vec2 pointLeft  = centerA - rightA   * halfWidthA;

    // width/length em half Box B
    float halfWidthB = widthB * 0.5f;
    float halfLengthB = lengthB * 0.5f;

    // 4 cantos da Box B em coordenadas locais
    std::vector<vec2> cornersB = {
        vec2(-halfWidthB, -halfLengthB),
        vec2(+halfWidthB, -halfLengthB),
        vec2(+halfWidthB, +halfLengthB),
        vec2(-halfWidthB, +halfLengthB)
    };

    // Rotaciona/translada os cantos Box B
    float cosB = cos(angleB);
    float sinB = sin(angleB);

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (auto& corner : cornersB) {
        float rotatedX = cosB * corner.x - sinB * corner.y;
        float rotatedZ = sinB * corner.x + cosB * corner.y;

        float worldX = rotatedX + posB_X;
        float worldZ = rotatedZ + posB_Z;

        minX = std::min(minX, worldX);
        maxX = std::max(maxX, worldX);
        minZ = std::min(minZ, worldZ);
        maxZ = std::max(maxZ, worldZ);
    }

    // Verifica se os pontos de A estão dentro da AABB de B
    auto pointInside = [&](vec2 p) -> bool {
        return (p.x >= minX && p.x <= maxX && p.y >= minZ && p.y <= maxZ);
    };

    if (pointInside(pointFront))  collision.front = true;
    if (pointInside(pointBack))   collision.back = true;
    if (pointInside(pointLeft))   collision.left = true;
    if (pointInside(pointRight))  collision.right = true;

    return collision;
}




CollisionSides CheckWallCollision(
    float posX, float posZ, float angleY,
    float carHalfWidth, float carHalfLength,
    float mapWidth, float mapDepth)
{
    using namespace glm;

    vec2 forward(sin(angleY), cos(angleY));
    vec2 rightVec(forward.y, -forward.x);
    vec2 center(posX, posZ);

    // Posições das 4 faces do carro
    vec2 front  = center + forward * carHalfLength;
    vec2 back   = center - forward * carHalfLength;
    vec2 right  = center + rightVec * carHalfWidth;
    vec2 left   = center - rightVec * carHalfWidth;

    // Limites do mapa
    float wallMinX = -mapWidth / 2.0f;
    float wallMaxX =  mapWidth / 2.0f;
    float wallMinZ = -mapDepth / 2.0f;
    float wallMaxZ =  mapDepth / 2.0f;

    CollisionSides collision;

    // A frente do carro passou qualquer limite?
    if (front.x < wallMinX || front.x > wallMaxX ||
        front.y < wallMinZ || front.y > wallMaxZ)
    {
        collision.front = true;
    }

    // A traseira do carro passou qualquer limite?
    if (back.x < wallMinX || back.x > wallMaxX ||
        back.y < wallMinZ || back.y > wallMaxZ)
    {
        collision.back = true;
    }

    // A lateral direita do carro passou qualquer limite?
    if (right.x < wallMinX || right.x > wallMaxX ||
        right.y < wallMinZ || right.y > wallMaxZ)
    {
        collision.right = true;
    }

    // A lateral esquerda do carro passou qualquer limite?
    if (left.x < wallMinX || left.x > wallMaxX ||
        left.y < wallMinZ || left.y > wallMaxZ)
    {
        collision.left = true;
    }


    if (!g_Win && front.y >= (mapDepth / 2.0f - limiar_vitoria)) {
        g_Win = true;


        if (!g_TimerStopped) {
            g_FinalTime = glfwGetTime() - g_StartTime;
            g_TimerStopped = true;
            printf("GANHOU em %.2f segundos!\n", g_FinalTime);
        }
    }

    return collision;
}



CollisionSides CheckAllCollisions(
    float carX, float carZ, float carAngleY,
    float carHalfWidth, float carHalfLength,
    float mapWidth, float mapDepth,
    const std::vector<BoundingObject>& obstacles)
{
    // 1. Colisão com as paredes
    CollisionSides result = CheckWallCollision(
        carX, carZ, carAngleY,
        carHalfWidth, carHalfLength,
        mapWidth, mapDepth);

    // 2. Colisão com cada obstáculo
    for (const auto& obs : obstacles) {
        CollisionSides current = CheckBoxCollision(
            carX, carZ, carAngleY, carHalfWidth, carHalfLength,
            obs.x, obs.z, obs.angleY, obs.width, obs.length);

        // 3. Combina resultados
        result.front = result.front || current.front;
        result.back  = result.back  || current.back;
        result.left  = result.left  || current.left;
        result.right = result.right || current.right;
    }

    return result;
}

BoundingObject MakeBox(int id, float x, float z, float angleY, float width, float length, float height)
{
    return BoundingObject(
        id,
        x, z,
        angleY,
        width, length, height,
        1,                 // type = 1 (Box)
        0.0f,              // radius (não usado)
        glm::vec2(0.0f),   // v0
        glm::vec2(0.0f),   // v1
        glm::vec2(0.0f)    // v2
    );
}

BoundingObject MakeSphere(int id, float x, float z, float radius)
{
    return BoundingObject(
        id,
        x, z,
        0.0f,              // angleY (não usado para esfera)
        0.0f, 0.0f, 0.0f,        // width, length, height (não usados)
        2,                 // type = 2 (Sphere)
        radius,            // radius usado
        glm::vec2(0.0f),   // v0
        glm::vec2(0.0f),   // v1
        glm::vec2(0.0f)    // v2
    );
}

BoundingObject MakeTriangle(int id, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
{
    return BoundingObject(
        id,
        0.0f, 0.0f,        // x, z (opcional para triângulo)
        0.0f,              // angleY
        0.0f, 0.0f, 0.0f,        // width, length
        3,                 // type = 3 (Triangle)
        0.0f,              // radius (não usado)
        v0, v1, v2
    );
}


Mesh CreateCylinderMesh(float radius, float height, int segments)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float halfHeight = height / 2.0f;

    // Gera os vértices laterais (pares de top/bottom)
    for (int i = 0; i <= segments; ++i)
    {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // Topo
        vertices.insert(vertices.end(), { x, +halfHeight, z });
        // Base
        vertices.insert(vertices.end(), { x, -halfHeight, z });
    }

    // Adiciona vértices centrais das tampas
    vertices.insert(vertices.end(), { 0.0f, +halfHeight, 0.0f }); // centro do topo
    vertices.insert(vertices.end(), { 0.0f, -halfHeight, 0.0f }); // centro da base

    unsigned int top_center_index = (segments + 1) * 2;      // índice do centro superior
    unsigned int bottom_center_index = top_center_index + 1; // índice do centro inferior

    // Triângulos das laterais
    for (int i = 0; i < segments; ++i)
    {
        unsigned int top1 = i * 2;
        unsigned int bot1 = i * 2 + 1;
        unsigned int top2 = (i + 1) * 2;
        unsigned int bot2 = (i + 1) * 2 + 1;

        // Anti-horário
        indices.insert(indices.end(), { top1, top2, bot1 });
        indices.insert(indices.end(), { bot1, top2, bot2 });
    }

    // Triângulos da tampa superior
    for (int i = 0; i < segments; ++i)
    {
        unsigned int top1 = i * 2;
        unsigned int top2 = ((i + 1) % (segments + 1)) * 2;

        indices.insert(indices.end(), {
            top2, top1, top_center_index
        });
    }

    // Triângulos da tampa inferior
    for (int i = 0; i < segments; ++i)
    {
        unsigned int bot1 = i * 2 + 1;
        unsigned int bot2 = ((i + 1) % (segments + 1)) * 2 + 1;

        indices.insert(indices.end(), {
            bottom_center_index, bot1, bot2
        });
    }

    // Envia para a GPU
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // layout: posição (x, y, z) em location = 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
//---FONTE CHATGPT
std::vector<float> texcoords;
for (int i = 0; i <= segments; ++i)
{
    float u = (float)i / segments;
    texcoords.push_back(u); texcoords.push_back(1.0f); // topo
    texcoords.push_back(u); texcoords.push_back(0.0f); // base
}
// Centro topo e base
texcoords.push_back(0.5f); texcoords.push_back(1.0f); // centro topo
texcoords.push_back(0.5f); texcoords.push_back(0.0f); // centro base
glBindVertexArray(mesh.VAO); // <--- necessário para vincular corretamente no VAO
GLuint texcoord_vbo;
glGenBuffers(1, &texcoord_vbo);
glBindBuffer(GL_ARRAY_BUFFER, texcoord_vbo);
glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);
// Cores brancas para todos os vértices (RGBA = 1.0)
std::vector<float> colors(vertices.size() / 3 * 4, 1.0f); // 4 floats por vértice

GLuint color_vbo;
glGenBuffers(1, &color_vbo);
glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(1);
glBindVertexArray(0);

//---Fim


    glBindVertexArray(0);

    mesh.num_indices = indices.size();
    return mesh;
}
 // Função criada com auxilio de IA

void DrawCylinder(Mesh& mesh, GLint render_as_black_uniform)
{
    glUniform1i(render_as_black_uniform, GL_FALSE);
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



// Função que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Função que remove a matriz atualmente no topo da pilha e armazena a mesma na variável M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Função que desenha um cubo com arestas em preto, definido dentro da função BuildTriangles().
void DrawCube(GLint render_as_black_uniform)
{
    // Informamos para a placa de vídeo (GPU) que a variável booleana
    // "render_as_black" deve ser colocada como "false". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, false);

    // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
    // VAO como triângulos, formando as faces do cubo. Esta
    // renderização irá executar o Vertex Shader definido no arquivo
    // "shader_vertex.glsl", e o mesmo irá utilizar as matrizes
    // "model", "view" e "projection" definidas acima e já enviadas
    // para a placa de vídeo (GPU).
    //
    // Veja a definição de g_VirtualScene["cube_faces"] dentro da
    // função BuildTriangles(), e veja a documentação da função
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_faces"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
        g_VirtualScene["cube_faces"].num_indices,    //
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_faces"].first_index
    );


    // Informamos para a placa de vídeo (GPU) que a variável booleana
    // "render_as_black" deve ser colocada como "true". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, true);

    // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
    // VAO como linhas, formando as arestas pretas do cubo. Veja a
    // definição de g_VirtualScene["cube_edges"] dentro da função
    // BuildTriangles(), e veja a documentação da função
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_edges"].rendering_mode,
        g_VirtualScene["cube_edges"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_edges"].first_index
    );
}

// Constrói triângulos para futura renderização
GLuint BuildTriangles()
{
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
    // homogêneas.  Veja as seguintes referências:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
    // Vértices de um cubo
    //    X      Y     Z     W
        -0.5f,  0.0f,  0.5f, 1.0f, // posição do vértice 0
        -0.5f, -1.0f,  0.5f, 1.0f, // posição do vértice 1
         0.5f, -1.0f,  0.5f, 1.0f, // posição do vértice 2
         0.5f,  0.0f,  0.5f, 1.0f, // posição do vértice 3
        -0.5f,  0.0f, -0.5f, 1.0f, // posição do vértice 4
        -0.5f, -1.0f, -0.5f, 1.0f, // posição do vértice 5
         0.5f, -1.0f, -0.5f, 1.0f, // posição do vértice 6
         0.5f,  0.0f, -0.5f, 1.0f, // posição do vértice 7
    // Vértices para desenhar o eixo X
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 8
         1.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 9
    // Vértices para desenhar o eixo Y
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 10
         0.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 11
    // Vértices para desenhar o eixo Z
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 12
         0.0f,  0.0f,  1.0f, 1.0f, // posição do vértice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posição.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_model_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    GLfloat color_coefficients[] = {
    // Cores dos vértices do cubo
    //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 7
    // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 9
    // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 11
    // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir polígonos utilizando os vértices do array
    // model_coefficients.
    //
    // Para referência sobre os modos de renderização, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //

    //---FONTE: CHATGPT
    // Coordenadas de textura UV
    GLfloat texture_coords[] = {
        //     U      V
        0.0f, 1.0f, // 0
        0.0f, 0.0f, // 1
        1.0f, 0.0f, // 2
        1.0f, 1.0f, // 3
        0.0f, 1.0f, // 4
        0.0f, 0.0f, // 5
        1.0f, 0.0f, // 6
        1.0f, 1.0f, // 7
        0.0f, 0.0f, // eixo X (ignorado)
        0.0f, 0.0f,
        0.0f, 0.0f, // eixo Y
        0.0f, 0.0f,
        0.0f, 0.0f, // eixo Z
        0.0f, 0.0f,
    };

    GLuint VBO_texture_coords_id;
    glGenBuffers(1, &VBO_texture_coords_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coords_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
    location = 2; // location = 2 em shader_vertex.glsl
    number_of_dimensions = 2; // vec2
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //---FIM
    GLuint indices[] = {
    // Definimos os índices dos vértices que definem as FACES de um cubo
    // através de 12 triângulos que serão desenhados com o modo de renderização
    // GL_TRIANGLES.
        0, 1, 2, // triângulo 1
        7, 6, 5, // triângulo 2
        3, 2, 6, // triângulo 3
        4, 0, 3, // triângulo 4
        4, 5, 1, // triângulo 5
        1, 5, 6, // triângulo 6
        0, 2, 3, // triângulo 7
        7, 5, 4, // triângulo 8
        3, 6, 7, // triângulo 9
        4, 3, 7, // triângulo 10
        4, 1, 0, // triângulo 11
        1, 6, 2, // triângulo 12
    // Definimos os índices dos vértices que definem as ARESTAS de um cubo
    // através de 12 linhas que serão desenhadas com o modo de renderização
    // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
    // Z, que serão desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 36;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere às arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (void*)(36*sizeof(GLuint)); // Primeiro índice está em indices[36]
    cube_edges.num_indices    = 24; // Último índice está em indices[59]; total de 24 índices.
    cube_edges.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (void*)(60*sizeof(GLuint)); // Primeiro índice está em indices[60]
    axes.num_indices    = 6; // Último índice está em indices[65]; total de 6 índices.
    axes.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.
    g_VirtualScene["axes"] = axes;

    // Criamos um buffer OpenGL para armazenar os índices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;


std::vector<glm::vec3> bezier_control_points = {
    glm::vec3(-10.0f, 0.0f,  0.0f),
    glm::vec3(-5.0f,  0.0f,  6.0f),
    glm::vec3( 0.0f,  0.0f,  7.0f),
    glm::vec3( 5.0f,  0.0f,  6.0f),
    glm::vec3( 10.0f, 0.0f,  0.0f)
};

float bunny_time = 0.0f;
GLuint g_BunnyVAO;
int g_BunnyNumTriangles;
//GLuint g_BunnyTexture;
// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da câmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   += 0.01f*dy;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_TorsoPositionX += 0.01f*dx;
        g_TorsoPositionY -= 0.01f*dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ======================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        g_UseFreeCamera = !g_UseFreeCamera;
    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    //float delta = 3.141592 / 16; // 22.5 graus, em radianos.


    bool is_pressed = (action != GLFW_RELEASE);
    switch (key)
    {
        case GLFW_KEY_W: g_Input.W = is_pressed; break;
        case GLFW_KEY_A: g_Input.A = is_pressed; break;
        case GLFW_KEY_S: g_Input.S = is_pressed; break;
        case GLFW_KEY_D: g_Input.D = is_pressed; break;
    } // Para movimentação do carro, atualiza a struct global gInput para saber qual tecla está pressionada





    // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        float currentTime = (float)glfwGetTime();

        if (currentTime - lastShotTime >= shotCooldown)
        {
            // IMPORTANTE: use as variáveis globais de posição e rotação do carro
            FireCannon(g_TorsoPositionX, g_TorsoPositionZ, g_CarAngleY);

            lastShotTime = currentTime;
        }
    }



    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        g_UseFreeCamera = !g_UseFreeCamera;
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.


