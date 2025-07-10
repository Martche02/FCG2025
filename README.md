# Trabalho de Computação Gráfica

Este jogo de estilo corrida contra o tempo é o trabalho final de FCG/2025
Este trabalho utiliza várias instâncias de um mesmo objeto para renderizar o protagonista, os bloqueios e a arena.
O usuário atua controlando o protagonista (um tanque), que causa transformações geométricas em suas rodas ao fazê-las girar com animação baseada no tempo ($\Delta t$).
Para completar o objetivo (chegar no final e encontrar os coelhos feitos de malhas poligonais complexas!) permite-se observar a arena tanto através de uma câmera fixa no protagonista, quanto uma camera central da arena.
Mas cuidado! as paredes se movimentam em curvas de Bézier para complicar seu objetivo! Para melhorar os efeitos visuais, o tanque tem uma textura militar, a arena e os obstáculos contam com uma textura de muro de pedra, e a roda conta com uma textura de roda de tanque de guerra!
Para poder observar com qualidade os detalhes desse jogo, todos os objetos recebem um modelo de Iluminação Difusa e Blinn-Phong, e objetos como o protagonista recebem também os modelos de Interpolação de Phong e Gouraud.

---

## Contribuição dos Integrantes

- Bruno: Ficou responsável por implementar:
- - [jogabilidade e estrutura da arena](../../issues/11)
- - [Animações baseadas no tempo $\Delta t$](../../issues/10)
- - [Testes de Intersecção](../../issues/5)
- - [Transformação das Rodas](../../issues/2)
- - [Instância de objetos](../../issues/4)
- Marcelo: Ficou responsável por implementar:
- - [modelos de Iluminação Difusa e Blinn-Phong](../../issues/6)
- - [Modelos de Interpolação de Phong e Gouraud](../../issues/7)
- - [Câmera livre e câmera look-at](../../issues/3)
- - [Malhas poligonais complexas](../../issues/1)
- - [Movimentação com curva Bézier cúbica](../../issues/9).
- Ambos ficaram responsáveis pelas [texturas](../../issues/8)

---

## Uso de Ferramentas de IA

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

![Protagonista](imagens/protagonista.png)

### Screenshot 2

![Tela Inicial](imagens/inicial.png)

---

## Manual de Uso

- **W A S D:** movimentam o objeto principal
- **Tecla `F` | Tecla `T`:** troca a câmera
- **Tecla `Esc`:** fecha a aplicação
- **Tecla `Espaço`:** Atira

---

## Compilação e Execução

Execute `./bin/Release/main.exe`
