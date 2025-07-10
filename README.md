# Trabalho de Computação Gráfica

Este jogo de estilo corrida contra o tempo é o trabalho final de FCG/2025
Este trabalho utiliza várias instâncias de um mesmo objeto para renderizar o protagonista, os bloqueios e a arena. O usuário atua controlando o protagonista (um tanque), que causa transformações geométricas em suas rodas ao fazê-las girar com animação baseada no tempo ($\Delta t$). Para completar o objetivo (chegar no final e encontrar os coelhos feitos de malhas poligonais complexas!) permite-se observar a arena tanto através de uma câmera fixa no protagonista, quanto uma camera central da arena. Mas cuidado! as paredes se movimentam em curvas de Bézier para complicar seu objetivo! Para melhorar os efeitos visuais, o tanque tem uma textura militar, a arena e os obstáculos contam com uma textura de muro de pedra, e a roda conta com uma textura de roda de tanque de guerra! Para poder observar com qualidade os detalhes desse jogo, todos os objetos recebem um modelo de Iluminação Difusa e Blinn-Phong, e objetos como o protagonista recebem também os modelos de Interpolação de Phong e Gouraud.
Este trabalho utiliza várias instâncias de um mesmo objeto para renderizar o protagonista, os bloqueios e a arena. O usuário atua controlando o protagonista (um tanque), que causa transformações geométricas em suas rodas ao fazê-las girar com animação baseada no tempo ($\Delta t$). Para completar o objetivo (chegar no final e encontrar os coelhos feitos de malhas poligonais complexas!) permite-se observar a arena tanto através de uma câmera fixa no protagonista, quanto uma camera central da arena. Mas cuidado! as paredes se movimentam em curvas de Bézier para complicar seu objetivo! Para melhorar os efeitos visuais, o tanque tem uma textura militar, a arena e os obstáculos contam com uma textura de muro de pedra, e a roda conta com uma textura de roda de tanque de guerra! Para poder observar com qualidade os detalhes desse jogo, todos os objetos recebem um modelo de Iluminação Difusa e Blinn-Phong, e objetos como o protagonista recebem também os modelos de Interpolação de Phong e Gouraud.

---

## Contribuição dos Integrantes

- Bruno:
- Marcelo: Ficou responsável por implementar os [modelos de Iluminação Difusa e Blinn-Phong](issues/6), [Modelos de Interpolação de Phong e Gouraud](#7), [Câmera livre e câmera look-at](#3), [Malhas poligonais complexas](#1) e [Movimentação com curva Bézier cúbica](#9).
- Bruno:
- Marcelo: Ficou responsável por implementar os modelos de Iluminação Difusa e Blinn-Phong (issue #6), Modelos de Interpolação de Phong e Gouraud (#7), Câmera livre e câmera look-at (#3), Malhas poligonais complexas (#1) e Movimentação com curva Bézier cúbica (#9).

---

## Uso de Ferramentas de IA (ChatGPT, Copilot, etc.)

Fizemos uso do ChatGPT, que foi muito útil, especialmente em debug. Usei (marcelo) bastante para a parte das texturas e na importação do objeto de malha complexa (coelho).

---

## Desenvolvimento e Aplicação dos Conceitos de Computação Gráfica

Utilizamos vários conceitos trabalhados em aula, como

- Transformações Hierárquicas (LAB3)
- - Chaci, Rodas e Canhão
- Texturas (LAB5)
- - Arena, tanque, obstáculos, rodas, etc.
- Iluminação (LAB4)
- Modelo Local (LAB2)
- - Para aplicar texturas localmente

---

## Imagens da Aplicação

### Screenshot 1

![Screenshot 1](imagens/screenshot1.png)

### Screenshot 2

![Screenshot 2](imagens/screenshot2.png)

---

## Manual de Uso

- **W A S D:** movimentam o objeto principal
- **Tecla `F` | Tecla `T`:** troca a câmera
- **Tecla `Esc`:** fecha a aplicação
- **Tecla `Espaço`:** Atira

---

## Compilação e Execução

### Requisitos

- [ ] Code::Bloks
- [ ] Biblioteca OpenGL instalada
- [ ] GLFW
- [ ] Windows

### Passos
