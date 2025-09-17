# Análise Matemática do Código de Curvas Paramétricas

## Estruturas de Dados Principais

```c
typedef struct spts {
    float v[3];
} tipoPto;

tipoPto ptsCurva[MAXVERTEXS];    // Pontos da curva gerada
tipoPto ptsContrle[MAXVERTEXS];  // Pontos de controle definidos pelo usuário
```

## Matrizes de Base das Curvas

O código implementa 4 tipos de curvas paramétricas cúbicas, cada uma definida por uma matriz 4×4:

### 1. Matriz de Hermite (Mh)
```c
float Mh[4][4] = {
    { 2.0, -2.0,  1.0,  1.0},
    {-3.0,  3.0, -2.0, -1.0},
    { 0.0,  0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0,  0.0}
};
```
**Base matemática**: Curvas de Hermite usam 4 parâmetros: P₀, P₁ (pontos inicial e final) e T₀, T₁ (tangentes nos pontos).
- **Equação**: P(t) = P₀H₀(t) + P₁H₁(t) + T₀H₂(t) + T₁H₃(t)
- **Polinômios de Hermite**:
  - H₀(t) = 2t³ - 3t² + 1
  - H₁(t) = -2t³ + 3t²
  - H₂(t) = t³ - 2t² + t
  - H₃(t) = t³ - t²

### 2. Matriz de Bézier (Mb)
```c
float Mb[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 3.0, -6.0,  3.0,  0.0},
    {-3.0,  3.0,  0.0,  0.0},
    { 1.0,  0.0,  0.0,  0.0}
};
```
**Base matemática**: Usa 4 pontos de controle com polinômios de Bernstein.
- **Equação**: P(t) = ∑ᵢ₌₀³ Pᵢ * C(3,i) * (1-t)³⁻ⁱ * tⁱ
- **Propriedades**: A curva sempre passa pelos pontos inicial e final, mas não pelos pontos intermediários.

### 3. Matriz B-Spline (Ms)
```c
float Ms[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 3.0, -6.0,  3.0,  0.0},
    {-3.0,  0.0,  3.0,  0.0},
    { 1.0,  4.0,  1.0,  0.0}
};
```
**Base matemática**: B-splines cúbicas uniformes (divididas por 6 para normalização).
- **Propriedades**: Continuidade C² (suave), não passa pelos pontos de controle (exceto casos especiais).

### 4. Matriz Catmull-Rom (Mc)
```c
float Mc[4][4] = {
    {-1.0,  3.0, -3.0,  1.0},
    { 2.0, -5.0,  4.0, -1.0},
    {-1.0,  0.0,  1.0,  0.0},
    { 0.0,  2.0,  0.0,  0.0}
};
```
**Base matemática**: Splines de interpolação (divididas por 2), passam pelos pontos de controle.

## Funções de Transformação Geométrica

Todas as transformações usam coordenadas homogêneas 2D (matrizes 3×3):

### 1. Translação
```c
void translacao(float dx, float dy) {
    float T[3][3] = {{1, 0, dx}, {0, 1, dy}, {0, 0, 1}};
    applyTransformToControl(T);
}
```
**Matriz de transformação**:
```
[1  0  dx]
[0  1  dy]
[0  0  1 ]
```
**Efeito**: Move todos os pontos por (dx, dy).

### 2. Escala no Centro
```c
void escala_centro(float sx, float sy)
```
**Processo**: T₂ × S × T₁, onde:
- T₁: translação para origem (-cx, -cy)
- S: escala (sx, sy)
- T₂: translação de volta (cx, cy)

**Matrizes**:
```
T₁ = [1  0  -cx]    S = [sx  0   0]    T₂ = [1  0  cx]
     [0  1  -cy]        [0  sy   0]         [0  1  cy]
     [0  0   1 ]        [0  0    1]         [0  0   1]
```

### 3. Rotação no Centro
```c
void rotacao_centro(float ang_deg)
```
**Processo**: T₂ × R × T₁
```
R = [cos(θ)  -sin(θ)  0]
    [sin(θ)   cos(θ)  0]
    [0        0       1]
```

### 4. Cisalhamento (Shear)
```c
void shear(float shx, float shy)
```
**Matriz de cisalhamento**:
```
Sh = [1    shx  0]
     [shy  1    0]
     [0    0    1]
```

### 5. Reflexões
- **espelharX()**: Reflexão em relação ao eixo X (y → -y)
- **espelharY()**: Reflexão em relação ao eixo Y (x → -x)

## Geração de Curvas

### Função `ptoCurva(float t, int j, float pp[3])`

Esta é a função central que calcula um ponto na curva:

1. **Seleção de pontos de controle**: Seleciona 4 pontos consecutivos começando do índice `j`
2. **Tratamento especial para Hermite**: Calcula tangentes usando diferenças centrais:
   - T₀ = (P₁ - P₋₁)/2
   - T₁ = (P₂ - P₀)/2
3. **Cálculo do ponto**: Para cada coordenada (x, y, z):
   ```
   P(t) = ∑ᵢ₌₀³ [t³M₀ᵢ + t²M₁ᵢ + tM₂ᵢ + M₃ᵢ] × Pᵢ
   ```

### Função `geraCurva(int j)`

Gera uma curva completa variando o parâmetro t de 0 a 1:
```c
dt = 1.0f / (MAXVERTEXS - 1);
for(nPtsCurva = 0; nPtsCurva < MAXVERTEXS; nPtsCurva++) {
    t = (float)(nPtsCurva) * dt;
    ptoCurva(t, j, ptsCurva[nPtsCurva].v);
}
```

## Formação do Polígono de Controle

### Como é formado:
1. **Entrada de pontos**: O usuário clica com o mouse esquerdo para definir pontos de controle
2. **Armazenamento**: Cada clique adiciona um ponto ao array `ptsContrle[]`
3. **Visualização**: O polígono é desenhado conectando todos os pontos em ordem (modo `GL_LINE_LOOP` para fechar)

### Processo no código:
```c
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN) {
        x = x - windW;  // Converte para coordenadas do mundo
        y = windH - y;

        if (!jaCurva) {  // Ainda coletando pontos
            ptsContrle[nPtsCtrole].v[0] = (float)x;
            ptsContrle[nPtsCtrole].v[1] = (float)y;
            nPtsCtrole++;
        }
    }
}
```

### Características do polígono:
- **Fechado**: Usa `GL_LINE_LOOP` para conectar automaticamente o último ao primeiro ponto
- **Interativo**: Após definir a curva, pode-se arrastar pontos individuais
- **Visual**: Pontos verdes, linhas pretas

## Curvas Fechadas

O código gera curvas fechadas processando segmentos consecutivos:
```c
while (j < nPtsCtrole) {
    geraCurva(j);    // Gera segmento j
    // Desenha o segmento com cor específica
    j++;
}
```

Cada segmento usa 4 pontos de controle com índices circulares (`% nPtsCtrole`), garantindo continuidade.

## Interação e Funcionalidades

- **Menu de curvas**: Permite alternar entre Hermite, Bézier, B-Spline e Catmull-Rom
- **Menu de transformações**: Aplica transformações geométricas aos pontos de controle
- **Manipulação direta**: Arrastar pontos de controle para modificar a curva em tempo real
- **Cores diferentes**: Cada segmento da curva fechada tem cor distinta para visualização
