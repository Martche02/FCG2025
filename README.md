# Trabalho de Computação Gráfica

Este jogo de estilo corrida contra o tempo é o trabalho final de FCG/2025
Este trabalho utiliza várias instâncias de um mesmo objeto para renderizar o protagonista, os bloqueios e a arena. O usuário atua controlando o protagonista (um tanque), que causa transformações geométricas em suas rodas ao fazê-las girar com animação baseada no tempo ($\Delta t$). Para completar o objetivo (chegar no final e encontrar os coelhos feitos de malhas poligonais complexas!) permite-se observar a arena tanto através de uma câmera fixa no protagonista, quanto uma camera central da arena. Mas cuidado! as paredes se movimentam em curvas de Bézier para complicar seu objetivo!

---

## Contribuição dos Integrantes

- Fulano: Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer non mi nec elit pretium dignissim. Responsável principalmente por X, Y e Z.
- Sicrano: Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec eget ipsum quis risus blandit fermentum. Atuou majoritariamente em A, B e C.

---

## Uso de Ferramentas de IA (ChatGPT, Copilot, etc.)

Utilizamos a ferramenta **ChatGPT** durante o desenvolvimento do trabalho, especialmente nas etapas de pesquisa e depuração de código. Por exemplo, buscamos explicações sobre conceitos de rasterização e OpenGL, além de exemplos de código para atalhos de teclado.

Achamos a ferramenta útil principalmente para esclarecer dúvidas conceituais rapidamente e fornecer exemplos de estruturas comuns. No entanto, em partes mais específicas do projeto, como integração com a lógica da nossa aplicação ou depuração de erros gráficos específicos, a ferramenta não foi tão eficaz e exigiu aprofundamento manual.

---

## Desenvolvimento e Aplicação dos Conceitos de Computação Gráfica

Durante o desenvolvimento da aplicação, foram utilizados diversos conceitos estudados ao longo da disciplina. Entre eles:

- **Modelagem com primitivas geométricas:** Lorem ipsum dolor sit amet, consectetur adipiscing elit.
- **Transformações geométricas (translação, rotação, escala):** Lorem ipsum dolor sit amet.
- **Pipelines gráficos e shaders (quando aplicável):** Lorem ipsum dolor sit amet.
- **Interação com o usuário via teclado e mouse:** Lorem ipsum dolor sit amet.

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla fermentum libero sed est convallis luctus. Vivamus id fringilla nulla.

---

## Imagens da Aplicação

### Screenshot 1

![Screenshot 1](imagens/screenshot1.png)

### Screenshot 2

![Screenshot 2](imagens/screenshot2.png)

---

## Manual de Uso

- **Setas do teclado:** movimentam o objeto principal
- **Tecla `R`:** reinicia a simulação
- **Tecla `Esc`:** fecha a aplicação
- **Mouse:** Lorem ipsum dolor sit amet

Outros comandos podem ser adicionados conforme necessidade da aplicação.

---

## Compilação e Execução

### Requisitos

- [ ] Compilador compatível com C++17 ou superior
- [ ] Biblioteca OpenGL instalada
- [ ] FreeGLUT / GLFW / SDL2 (especifique qual você usou)
- [ ] Sistema operacional (Linux / Windows / Mac)

### Passos

```bash
git clone https://github.com/seu-usuario/seu-repositorio.git
cd seu-repositorio
make
./nome_da_aplicacao
```
