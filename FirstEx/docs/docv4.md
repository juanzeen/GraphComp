# Guia Completo: Curvas Paramétricas e GLUT

## 1. CONFIGURAÇÃO E USO DO GLUT

### Requisitos Básicos do GLUT

**Headers necessários:**
```c
#include "GL/glut.h"  // Interface GLUT
#include <math.h>     // Funções trigonométricas
#include <stdio.h>    // I/O padrão
#include <stdlib.h>   // Funções utilitárias
```

**Variáveis de controle essenciais:**
```c
GLenum doubleBuffer;  // Controla double buffering
int windW, windH;     // Dimensões da janela (metade da tela)
```

**Inicialização obrigatória:**
```c
// 1. Inicializar GLUT
glutInit(&argc, argv);

// 2. Configurar tipo de display
type = GLUT_RGB;  // Modo de cor RGB
type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
glutInitDisplayMode(type);

// 3. Criar janela
glutInitWindowSize(600, 500);
glutCreateWindow("Título da Janela");

// 4. Registrar callbacks obrigatórias
glutReshapeFunc(Reshape);    // Redimensionamento
glutDisplayFunc(Draw);       // Desenho
glutKeyboardFunc(Key);       // Teclado
glutMouseFunc(mouse);        // Mouse
glutMotionFunc(motion);      // Movimento do mouse

// 5. Iniciar loop principal
glutMainLoop();  // NUNCA retorna!
```

**Sistema de coordenadas:**
```c
// Configuração da projeção ortográfica 2D
gluOrtho2D(-windW, windW, -windH, windH);
```
- **Centro**: (0,0) no meio da tela
- **Limites**: -windW a +windW (horizontal), -windH a +windH (vertical)
- **Conversão mouse**: `x = x - windW; y = windH - y;`

**Callbacks essenciais funcionam assim:**
- **glutDisplayFunc**: Chamada sempre que a tela precisa ser redesenhada
- **glutReshapeFunc**: Chamada quando janela é redimensionada
- **glutMouseFunc**: Chamada em cliques do mouse (DOWN/UP)
- **glutMotionFunc**: Chamada durante movimento com botão pressionado
- **glutKeyboardFunc**: Chamada em teclas normais (ESC, letras, números)

---

## 2. ESTRUTURAS DE DADOS PRINCIPAIS

```c
typedef struct spts {
    float v[3];  // [x, y, z] - coordenadas 3D (z sempre 0 em 2D)
} tipoPto;

tipoPto ptsCurva[MAXVERTEXS];    // Pontos da curva calculada
tipoPto ptsContrle[MAXVERTEXS];  // Pontos de controle do usuário

// Variáveis de estado
int nPtsCtrole;    // Número de pontos de controle definidos
int nPtsCurva;     // Número de pontos na curva gerada
int jaCurva;       // Flag: 0=coletando pontos, 1=curva pronta
int tipoCurva;     // Tipo ativo (HERMITE, BEZIER, BSPLINE, CATMULLR)
```

---

## 3. MATEMÁTICA DAS CURVAS PARAMÉTRICAS

### Conceito Fundamental

Todas as curvas seguem a forma geral:
**P(t) = T × M × G**

Onde:
- **T** = [t³, t², t, 1] (vetor de potências do parâmetro)
- **M** = Matriz de base 4×4 (específica do tipo de curva)
- **G** = Vetor de geometria 4×1 (pontos ou tangentes)
- **t** ∈ [0,1] (parâmetro que varia ao longo da curva)

### As 4 Matrizes de Base

#### 1. Matriz de Hermite (Mh)
```c
float Mh[4][4] = {
    { 2.0, -2.0,  1.0,  1.0},
    {-3.0,  3.0, -2.0, -1.0},
    { 0.0,  0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0,  0.0}
};
```

**Como funciona:**
- **Geometria**: G = [P₀, P₁, T₀, T₁] (2 pontos + 2 tangentes)
- **Cálculo das tangentes**: T₀ = (P₁ - P₋₁)/2, T₁ = (P₂ - P₀)/2
- **Resultado**: Curva que PASSA pelos pontos P₀ e P₁ com tangentes especificadas

**Propriedades:**
- ✅ **Interpolação**: Passa pelos pontos de controle
- ✅ **Continuidade C¹**: Tangente contínua entre segmentos
- ✅ **Controle de tangente**: Controle direto da inclinação
- ❌ **Não-local**: Mover um ponto afeta tangentes de vizinhos

#### 2. Matriz de Bézier (Mb)
```c
float Mb[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 3.0, -6.0,  3.0,  0.0},
    {-3.0,  3.0,  0.0,  0.0},
    { 1.0,  0.0,  0.0,  0.0}
};
```

**Como funciona:**
- **Geometria**: G = [P₀, P₁, P₂, P₃] (4 pontos de controle)
- **Base**: Polinômios de Bernstein B₃,ᵢ(t)
- **Resultado**: Curva que passa apenas por P₀ (t=0) e P₃ (t=1)

**Propriedades:**
- ✅ **Envoltória convexa**: Curva sempre dentro do polígono formado pelos 4 pontos
- ✅ **Controle intuitivo**: Pontos "puxam" a curva em sua direção
- ✅ **Estabilidade**: Numericamente estável
- ❌ **Não-interpolação**: Não passa pelos pontos intermediários
- ❌ **Descontinuidade**: Apenas C⁰ entre segmentos

#### 3. Matriz B-Spline (Ms ÷ 6)
```c
float Ms[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 3.0, -6.0,  3.0,  0.0},
    {-3.0,  0.0,  3.0,  0.0},
    { 1.0,  4.0,  1.0,  0.0}
};  // Dividida por 6 no código
```

**Como funciona:**
- **Geometria**: G = [P₀, P₁, P₂, P₃] (4 pontos de controle)
- **Base**: Splines cúbicas uniformes
- **Normalização**: Matriz dividida por 6 para manter propriedades

**Propriedades:**
- ✅ **Máxima suavidade**: Continuidade C² (curvatura contínua)
- ✅ **Suporte local**: Cada ponto afeta apenas 4 segmentos
- ✅ **Estabilidade numérica**: Menos sensível a perturbações
- ❌ **Não-interpolação**: Raramente passa pelos pontos de controle
- ⚠️ **Aproximação**: Fica "próximo" aos pontos, mas não passa por eles

#### 4. Matriz Catmull-Rom (Mc ÷ 2)
```c
float Mc[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 2.0, -5.0,  4.0, -1.0},
    {-1.0,  0.0,  1.0,  0.0},
    { 0.0,  2.0,  0.0,  0.0}
};  // Dividida por 2 no código
```

**Como funciona:**
- **Geometria**: G = [P₀, P₁, P₂, P₃] (4 pontos consecutivos)
- **Resultado**: Curva que passa por P₁ e P₂, usando P₀ e P₃ para calcular tangentes
- **Tangentes automáticas**: Calculadas para otimizar suavidade

**Propriedades:**
- ✅ **Interpolação**: SEMPRE passa pelos pontos de controle
- ✅ **Continuidade C¹**: Tangente contínua
- ✅ **Tangentes automáticas**: Não precisa especificar inclinações
- ✅ **Ideal para keyframes**: Perfeita para animação através de pontos específicos

---

## 4. PROCESSO DE GERAÇÃO DE CURVAS

### Função Central: `ptoCurva(float t, int j, float pp[3])`

**Algoritmo detalhado:**

1. **Seleção de 4 pontos consecutivos:**
```c
for (i = 0; i < 4; i++) {
    ji = (j + i) % nPtsCtrole;  // Índice circular
    ptsCont[i] = ptsContrle[ji];
}
```

2. **Tratamento especial para Hermite:**
```c
if (tipoCurva == HERMITE) {
    // Calcula tangentes por diferenças centrais
    px_minus = ptsContrle[(j + nPtsCtrole - 1) % nPtsCtrole].v[0];
    px_plus2 = ptsContrle[(j + 2) % nPtsCtrole].v[0];
    // T₀ = (P₁ - P₋₁)/2
    ptsCont[2].v[0] = (ptsCont[1].v[0] - px_minus) * 0.5f;
    // T₁ = (P₂ - P₀)/2
    ptsCont[3].v[0] = (px_plus2 - ptsCont[0].v[0]) * 0.5f;
}
```

3. **Aplicação da fórmula matricial:**
```c
for (i = 0; i < 4; i++) {
    // Calcula [t³, t², t, 1] × M[:,i]
    cc = t³×M[0][i] + t²×M[1][i] + t×M[2][i] + M[3][i];
    // Soma ponderada: P(t) = Σ cc × Pi
    pp[0] += cc * ptsCont[i].v[0];  // coordenada x
    pp[1] += cc * ptsCont[i].v[1];  // coordenada y
}
```

### Geração de Curva Completa: `geraCurva(int j)`

**Processo:**
1. **Discretização**: Divide t ∈ [0,1] em MAXVERTEXS pontos
2. **Cálculo**: Para cada t, chama `ptoCurva()`
3. **Armazenamento**: Guarda pontos em `ptsCurva[]`

```c
dt = 1.0f / (MAXVERTEXS - 1);
for(int i = 0; i < MAXVERTEXS; i++) {
    t = i * dt;
    ptoCurva(t, j, ptsCurva[i].v);
}
```

### Curvas Fechadas

O sistema gera curvas fechadas processando N segmentos (onde N = número de pontos de controle):

```c
for (j = 0; j < nPtsCtrole; j++) {
    geraCurva(j);  // Segmento j usa pontos j, j+1, j+2, j+3 (mod N)
    // Desenha segmento com cor específica
    Poligono(nPtsCurva, ptsCurva, cor[j][0], cor[j][1], cor[j][2]);
}
```

**Propriedades das curvas fechadas:**
- **Hermite**: C¹ (tangente contínua no fechamento)
- **Bézier**: C⁰ (apenas posição contínua)
- **B-Spline**: C² (curvatura contínua - máxima suavidade)
- **Catmull-Rom**: C¹ com interpolação de todos os pontos

---

## 5. TRANSFORMAÇÕES GEOMÉTRICAS DETALHADAS

### Sistema de Coordenadas Homogêneas

Todas as transformações usam matrizes 3×3 para 2D:
```
[x']   [m00  m01  m02] [x]
[y'] = [m10  m11  m12] [y]
[1 ]   [ 0    0    1 ] [1]
```

### Funções de Transformação

#### `translacao(float dx, float dy)`
**Parâmetros:**
- `dx`: Deslocamento horizontal em pixels
- `dy`: Deslocamento vertical em pixels

**Matriz:**
```
[1  0  dx]
[0  1  dy]
[0  0   1]
```

**O que faz:** Move todo o conjunto de pontos de controle por (dx, dy). É uma transformação rígida que preserva todas as propriedades geométricas.

**Propriedades específicas:**
- ✅ Preserva distâncias, ângulos e áreas
- ✅ Comutativa: T(a,b) + T(c,d) = T(a+c, b+d)
- ✅ Inversa: T(-dx, -dy)

---

#### `escala_centro(float sx, float sy)`
**Parâmetros:**
- `sx`: Fator de escala horizontal (>1 aumenta, <1 diminui, <0 inverte)
- `sy`: Fator de escala vertical

**Algoritmo:**
1. Calcula centróide: `(cx, cy) = média dos pontos`
2. Aplica: `T₂ × S × T₁` onde:
   - T₁: translação para origem `(-cx, -cy)`
   - S: escala `(sx, sy)`
   - T₂: translação de volta `(cx, cy)`

**Propriedades específicas:**
- **Ponto invariante**: Centróide permanece fixo
- **Escala uniforme** (sx = sy): Preserva formas
- **Escala não-uniforme** (sx ≠ sy): Distorce círculos em elipses
- **Escala negativa**: Inverte orientação + redimensiona
- **Área multiplicada por**: |sx × sy|

---

#### `rotacao_centro(float ang_deg)`
**Parâmetros:**
- `ang_deg`: Ângulo em graus (+ = anti-horário, - = horário)

**Matriz de rotação:**
```
[cos(θ)  -sin(θ)  0]
[sin(θ)   cos(θ)  0]
[0        0       1]
```

**Propriedades específicas:**
- ✅ **Isometria**: Preserva distâncias e ângulos
- ✅ **Ponto fixo**: Centróide não se move
- ✅ **Periodicidade**: 360° retorna ao original
- ✅ **Composição**: Ângulos se somam

---

#### `shear(float shx, float shy)`
**Parâmetros:**
- `shx`: Cisalhamento horizontal (x' = x + shx×y)
- `shy`: Cisalhamento vertical (y' = y + shy×x)

**Matriz:**
```
[1    shx  0]
[shy  1    0]
[0    0    1]
```

**Propriedades específicas:**
- ✅ **Preserva áreas**: Área total inalterada
- ❌ **Altera ângulos**: Retângulos viram paralelogramos
- ✅ **Linhas paralelas**: Permanecem paralelas
- **Aplicações**: Efeitos de perspectiva, inclinação, itálico

---

#### `espelharX()` e `espelharY()`
**Matrizes:**
```
EspelharX:          EspelharY:
[1   0  0]          [-1  0  0]
[0  -1  0]          [ 0  1  0]
[0   0  1]          [ 0  0  1]
```

**Propriedades específicas:**
- ✅ **Isometria**: Preserva distâncias e ângulos
- ❌ **Inverte orientação**: Muda sentido horário ↔ anti-horário
- ✅ **Idempotente**: Aplicar duas vezes = identidade
- ✅ **Eixo de reflexão**: Sempre passa pelo centróide

---

## 6. FORMAÇÃO E MANIPULAÇÃO DO POLÍGONO DE CONTROLE

### Processo de Criação

1. **Coleta inicial** (jaCurva = 0):
```c
if (!jaCurva) {
    // Cada clique esquerdo adiciona um ponto
    ptsContrle[nPtsCtrole].v[0] = (float)x;
    ptsContrle[nPtsCtrole].v[1] = (float)y;
    nPtsCtrole++;
}
```

2. **Visualização**:
   - **Pontos**: Círculos verdes (5px)
   - **Polígono**: Linhas pretas conectando pontos em ordem
   - **Fechamento**: `GL_LINE_LOOP` conecta último ao primeiro

3. **Ativação de curvas**: Automática quando nPtsCtrole > 3

### Manipulação Interativa

**Seleção de pontos:**
```c
int buscaPuntoClick(int x, int y) {
    for (i = 0; i < nPtsCtrole; i++) {
        distancia = sqrt((px-x)² + (py-y)²);
        if (distancia < 6.0) return i;  // Tolerância de 6 pixels
    }
    return -1;  // Nenhum ponto próximo
}
```

**Arrastamento:**
- Mouse pressionado → seleciona ponto mais próximo
- Mouse movendo → atualiza posição do ponto selecionado
- Curvas são regeneradas em tempo real

### Propriedades do Polígono

- **Fechado**: Sempre conecta o último ponto ao primeiro
- **Ordem importante**: Sequência dos pontos determina forma da curva
- **Mínimo 4 pontos**: Necessário para curvas cúbicas
- **Máximo MAXVERTEXS**: Limitado por constante (30 no código)

---

## 7. INTERFACE E MENUS GLUT

### Sistema de Menus

```c
void createGLUTMenus() {
    // Submenu de curvas
    submenu1 = glutCreateMenu(processMenuCurvas);
    glutAddMenuEntry("Hermite", HERMITE);
    glutAddMenuEntry("Bezier", BEZIER);
    glutAddMenuEntry("B-Spline", BSPLINE);
    glutAddMenuEntry("CatmullR", CATMULLR);

    // Submenu de transformações
    submenu2 = glutCreateMenu(processMenuTransforma);
    glutAddMenuEntry("Translacao", TRANSLACAO);
    glutAddMenuEntry("Rotacao", ROTACAO);
    glutAddMenuEntry("Escala", SCALA);
    // ... outras opções

    // Menu principal
    menu = glutCreateMenu(processMenuEvents);
    glutAddSubMenu("Tipo de Curva", submenu1);
    glutAddSubMenu("Transformacao", submenu2);
    glutAddMenuEntry("Limpar tudo...", 2);

    glutAttachMenu(GLUT_RIGHT_BUTTON);  // Botão direito abre menu
}
```

### Controles de Entrada

- **Clique esquerdo**: Adiciona ponto de controle OU seleciona ponto existente
- **Arrastar**: Move ponto selecionado OU aplica transformação contínua
- **Clique direito**: Abre menu contextual
- **ESC**: Sai do programa

### Estados do Sistema

1. **Coleta de pontos** (jaCurva = 0):
   - Aceita novos pontos de controle
   - Desenha apenas polígono de controle

2. **Curvas ativas** (jaCurva = 1):
   - Não aceita novos pontos
   - Permite manipulação de pontos existentes
   - Gera e desenha curvas automaticamente

3. **Modo transformação** (tipoTransforma > 0):
   - Movimentos do mouse aplicam transformações contínuas
   - Curvas são regeneradas constantemente

---

## 8. GARANTINDO O FUNCIONAMENTO

### Checklist de Configuração

✅ **Bibliotecas instaladas**:
- OpenGL (gl.h)
- GLU (glu.h)
- GLUT (glut.h)

✅ **Linking correto**:
```bash
gcc curves.c -lglut -lGL -lGLU -lm -o curves
```

✅ **Variáveis críticas inicializadas**:
- `windW`, `windH` definidas em Reshape()
- `doubleBuffer` configurada em Args()
- Arrays `ptsCurva`, `ptsContrle` zerados

✅ **Callbacks registradas**:
- Display, Reshape, Mouse, Motion, Keyboard

✅ **Sistema de coordenadas**:
- Projeção ortográfica 2D configurada
- Conversão mouse→mundo implementada

✅ **Estados válidos**:
- Mínimo 4 pontos para ativar curvas
- Matriz M[][] atualizada ao trocar tipo de curva

### Possíveis Problemas

❌ **Tela preta**: Verificar se Display() está sendo chamada
❌ **Coordenadas erradas**: Verificar conversão `x-windW`, `windH-y`
❌ **Menu não aparece**: Verificar se `createGLUTMenus()` foi chamada
❌ **Transformações não funcionam**: Verificar se há pontos de controle
❌ **Curvas distorcidas**: Verificar normalização das matrizes (÷6 para B-Spline, ÷2 para Catmull-Rom)
