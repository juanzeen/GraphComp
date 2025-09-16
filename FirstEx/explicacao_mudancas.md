# Explicação Detalhada das Mudanças

Este documento detalha as alterações realizadas para corrigir o arquivo `codigoErrado.c` e gerar o `curves.c`, explicando os valores alterados e a razão por trás das mudanças.

## 1. Definição de Macros para Espelhamento

Foram adicionadas as seguintes macros:

```c
#define ESPELHARX 5
#define ESPELHARY 6
```

**Por quê?** Essas macros definem constantes para os novos itens de menu de espelhamento, tornando o código mais legível e fácil de manter. Em vez de usar números "mágicos" (5 e 6) no código, usamos nomes descritivos.

## 2. Correção das Matrizes de Base das Curvas

As matrizes de base para Hermite, Bézier e B-spline foram corrigidas.

### Matriz de Hermite

**Antes (`codigoErrado.c`):**
```c
{{1.0, 0.0, 0.0, 0.0},
 {0.0, 1.0, 0.0, 0.0},
 {0.0, 0.0, 1.0, 0.0},
 {0.0, 0.0, 0.0, 1.0}}
```
**Depois (`curves.c`):**
```c
{{2.0, -2.0, 1.0, 1.0},
 {-3.0, 3.0, -2.0, -1.0},
 {0.0, 0.0, 1.0, 0.0},
 {1.0, 0.0, 0.0, 0.0}}
```
**Por quê?** A matriz em `codigoErrado.c` era uma matriz identidade, que não produz uma curva de Hermite. A matriz correta em `curves.c` contém os coeficientes que misturam os pontos de controle (P0, P1) e as tangentes (T0, T1) para gerar a curva cúbica de Hermite.

### Matriz de Bézier

**Antes (`codigoErrado.c`):**
```c
{{1.0, 0.0, 0.0, 0.0},
 {0.0, 1.0, 0.0, 0.0},
 {0.0, 0.0, 1.0, 0.0},
 {0.0, 0.0, 0.0, 1.0}}
```
**Depois (`curves.c`):**
```c
{{-1.0, 3.0, -3.0, 1.0},
 {3.0, -6.0, 3.0, 0.0},
 {-3.0, 3.0, 0.0, 0.0},
 {1.0, 0.0, 0.0, 0.0}}
```
**Por quê?** Assim como na de Hermite, a matriz em `codigoErrado.c` era uma identidade. A matriz correta em `curves.c` corresponde à base de Bernstein para curvas de Bézier cúbicas.

### Matriz de B-spline

**Antes (`codigoErrado.c`):**
```c
{{1.0, 0.0, 0.0, 0.0},
 {0.0, 1.0, 0.0, 0.0},
 {0.0, 0.0, 1.0, 0.0},
 {0.0, 0.0, 0.0, 1.0}}
```
**Depois (`curves.c`):**
```c
{{-1.0, 3.0, -3.0, 1.0},
 {3.0, -6.0, 3.0, 0.0},
 {-3.0, 0.0, 3.0, 0.0},
 {1.0, 4.0, 1.0, 0.0}}
```
**Por quê?** A matriz em `codigoErrado.c` era uma identidade. A matriz correta em `curves.c` é a matriz de base para B-splines cúbicas uniformes. Além disso, no `curves.c`, o resultado da multiplicação pela matriz é dividido por 6.0 para normalização.

## 3. Implementação de Funções de Transformação Geométrica

O `curves.c` introduz um conjunto de funções para aplicar transformações 2D nos pontos de controle, utilizando matrizes 3x3 e coordenadas homogêneas.

*   `multMat3Vec`: Multiplica uma matriz 3x3 por um vetor 3x1.
*   `applyTransformToControl`: Aplica uma matriz de transformação a todos os pontos de controle.
*   `translacao(dx, dy)`: Translada os pontos de controle.
*   `escala_centro(sx, sy)`: Escala os pontos de controle em torno do centroide do polígono de controle.
*   `rotacao_centro(ang_deg)`: Rotaciona os pontos de controle em torno do centroide.
*   `shear(shx, shy)`: Aplica um cisalhamento.
*   `espelharX()` e `espelharY()`: Espelham os pontos de controle em torno do eixo X ou Y que passa pelo centroide.

**Por quê?** O `codigoErrado.c` não tinha nenhuma funcionalidade de transformação. Essas funções foram adicionadas para permitir a manipulação interativa das curvas no espaço 2D, uma funcionalidade essencial em computação gráfica.

## 4. Cálculo de Tangentes na Curva de Hermite

A função `geraCurva` foi modificada para calcular as tangentes da curva de Hermite de uma maneira mais robusta.

**Antes (`codigoErrado.c`):**
O código calculava as tangentes de forma incorreta, usando diferenças simples entre pontos adjacentes.

**Depois (`curves.c`):**
```c
// t0 = (P1 - P_{-1})/2, t1 = (P2 - P0)/2
ptsCont[2].v[0] = (ptsCont[1].v[0] - px_minus) * 0.5f;
ptsCont[2].v[1] = (ptsCont[1].v[1] - py_minus) * 0.5f;
ptsCont[3].v[0] = (px_plus2 - ptsCont[0].v[0]) * 0.5f;
ptsCont[3].v[1] = (py_plus2 - ptsCont[0].v[1]) * 0.5f;
```
**Por quê?** O método implementado em `curves.c` usa diferenças centrais para calcular as tangentes nos pontos de controle. Isso resulta em tangentes mais suaves e, consequentemente, uma curva com continuidade C1 (continuidade de tangentes) na junção dos segmentos, similar ao método de Catmull-Rom.

## 5. Atualizações na Interatividade e Desenho

*   **Menus:** O menu foi atualizado para incluir as opções de espelhamento.
*   **`mouseHandle`:** Foi adicionada uma verificação `if (ptoSelect >= 0 && ptoSelect < nPtsCtrole)` para garantir que o ponto selecionado pelo mouse é válido antes de tentar modificar suas coordenadas. Isso previne erros de acesso a posições inválidas da memória.
*   **`motion`:** A função `motion` foi atualizada para chamar as funções de transformação quando o mouse é arrastado, permitindo a manipulação interativa das curvas.
*   **`display`:** A função `display` foi atualizada para chamar as funções de transformação e redesenhar a cena.

**Por quê?** Essas mudanças melhoram a usabilidade e a robustez do programa, fornecendo mais funcionalidades e prevenindo crashes.
