# Análise Detalhada das Funções - Curvas Paramétricas

## Funções Utilitárias de Transformação

### `multMat3Vec(const float A[3][3], const float v[3], float out[3])`
**Parâmetros:**
- `A`: Matriz de transformação 3×3
- `v`: Vetor de entrada [x, y, 1] em coordenadas homogêneas
- `out`: Vetor resultado após a transformação

**O que faz:** Multiplica uma matriz 3×3 por um vetor 3D, implementando transformações geométricas 2D em coordenadas homogêneas. É a função base para todas as transformações do sistema.

**Propriedade específica:** Usa coordenadas homogêneas para unificar translação, rotação, escala e outras transformações em uma única operação matricial.

---

### `applyTransformToControl(const float T[3][3])`
**Parâmetros:**
- `T`: Matriz de transformação a ser aplicada

**O que faz:** Aplica uma transformação geométrica a TODOS os pontos de controle do polígono. Converte cada ponto (x,y) para coordenadas homogêneas [x,y,1], aplica a transformação e armazena o resultado de volta nos pontos de controle.

**Propriedade específica:** Modifica diretamente o polígono de controle, forçando a regeneração das curvas. Todas as transformações subsequentes preservam a geometria relativa dos pontos.

---

## Transformações Geométricas Específicas

### `translacao(float dx, float dy)`
**Parâmetros:**
- `dx`: Deslocamento horizontal em pixels
- `dy`: Deslocamento vertical em pixels

**O que faz:** Move todo o polígono de controle (e consequentemente as curvas) por uma distância específica. É uma transformação rígida que preserva ângulos, distâncias e formas.

**Propriedades específicas:**
- Preserva orientação e tamanho
- Não altera a forma das curvas, apenas sua posição
- Comutativa: T(a,b) seguido de T(c,d) = T(a+c, b+d)

---

### `escala_centro(float sx, float sy)`
**Parâmetros:**
- `sx`: Fator de escala horizontal (>1 aumenta, <1 diminui, <0 inverte)
- `sy`: Fator de escala vertical

**O que faz:** Redimensiona o polígono de controle em relação ao seu centróide (ponto médio). Calcula primeiro o centro geométrico, translada para origem, aplica escala, depois retorna à posição original.

**Propriedades específicas:**
- **Ponto invariante:** O centróide permanece fixo durante a escala
- **Escala uniforme:** sx = sy mantém proporções
- **Escala não-uniforme:** sx ≠ sy pode distorcer círculos em elipses
- **Escala negativa:** Inverte orientação (espelhamento + escala)
- **sx = sy = 1:** Transformação identidade

---

### `rotacao_centro(float ang_deg)`
**Parâmetros:**
- `ang_deg`: Ângulo de rotação em graus (positivo = anti-horário)

**O que faz:** Rotaciona todo o polígono em torno do seu centróide. Converte graus para radianos, calcula seno e cosseno, e aplica a matriz de rotação após centralizar na origem.

**Propriedades específicas:**
- **Ponto fixo:** O centróide não se move
- **Isometria:** Preserva distâncias e ângulos
- **Orientação:** Valores positivos rotacionam no sentido anti-horário
- **Periodicidade:** 360° retorna à posição original
- **Composição:** Rotações se somam algebricamente

---

### `shear(float shx, float shy)`
**Parâmetros:**
- `shx`: Fator de cisalhamento horizontal (afeta x baseado em y)
- `shy`: Fator de cisalhamento vertical (afeta y baseado em x)

**O que faz:** Aplica uma deformação de cisalhamento (shear) que "inclina" o polígono. Transforma retângulos em paralelogramos, preservando áreas mas alterando ângulos.

**Propriedades específicas:**
- **Preserva áreas:** A área total do polígono permanece constante
- **Altera ângulos:** Ângulos retos se tornam oblíquos
- **Linhas paralelas:** Permanecem paralelas após cisalhamento
- **shx > 0:** Inclina pontos superiores para direita
- **shy > 0:** Inclina pontos à direita para cima
- **Usado em:** Simulação de perspectiva, efeitos de inclinação

---

### `espelharX()` e `espelharY()`
**Parâmetros:** Nenhum

**O que fazem:**
- `espelharX()`: Reflexão em relação ao eixo X horizontal que passa pelo centróide (y → -y)
- `espelharY()`: Reflexão em relação ao eixo Y vertical que passa pelo centróide (x → -x)

**Propriedades específicas:**
- **Isometria:** Preserva distâncias e ângulos
- **Inverte orientação:** Muda sentido horário/anti-horário
- **Idempotente:** Aplicar duas vezes retorna ao original
- **Eixo de reflexão:** Passa sempre pelo centróide dos pontos

---

## Funções de Geração de Curvas

### `ptoCurva(float t, int j, float pp[3])`
**Parâmetros:**
- `t`: Parâmetro da curva (0 ≤ t ≤ 1)
- `j`: Índice do primeiro ponto de controle do segmento
- `pp`: Array onde será armazenado o ponto calculado [x, y, z]

**O que faz:** Calcula um único ponto na curva paramétrica para um valor específico de t. Seleciona 4 pontos de controle consecutivos (com wraparound circular) e aplica a fórmula da curva escolhida.

**Propriedades específicas por tipo de curva:**

**HERMITE:**
- **Interpolação:** Passa pelos pontos P₀ e P₁
- **Tangentes:** Calcula automaticamente usando diferenças centrais
- **Continuidade:** C¹ (tangente contínua entre segmentos)
- **Controle local:** Modificar um ponto afeta apenas segmentos adjacentes

**BEZIER:**
- **Não-interpolação:** Geralmente não passa pelos pontos de controle (exceto primeiro e último)
- **Envoltória convexa:** A curva fica sempre dentro do polígono formado pelos 4 pontos
- **Continuidade:** C⁰ entre segmentos (apenas posição contínua)
- **Controle intuitivo:** Pontos "puxam" a curva em sua direção

**BSPLINE:**
- **Suavidade máxima:** Continuidade C² (curvatura contínua)
- **Aproximação:** Raramente passa pelos pontos de controle
- **Estabilidade numérica:** Menos sensível a mudanças nos pontos
- **Suporte local:** Cada ponto de controle influencia apenas 4 segmentos

**CATMULL-ROM:**
- **Interpolação:** Sempre passa pelos pontos de controle
- **Continuidade:** C¹ (tangente contínua)
- **Tangentes automáticas:** Calculadas para suavidade ótima
- **Controle direto:** Ideal para passar por pontos específicos

---

### `geraCurva(int j)`
**Parâmetros:**
- `j`: Índice do segmento de curva a ser gerado

**O que faz:** Gera um segmento completo da curva, criando MAXVERTEXS pontos igualmente espaçados ao longo do parâmetro t (de 0 a 1). Armazena os pontos no array `ptsCurva`.

**Propriedades específicas:**
- **Discretização:** Converte curva contínua em pontos discretos para renderização
- **Resolução fixa:** Sempre usa MAXVERTEXS pontos por segmento
- **Uniformidade:** Espaçamento paramétrico uniforme (não necessariamente espacial)

---

## Funções de Interação

### `buscaPuntoClick(int x, int y)`
**Parâmetros:**
- `x, y`: Coordenadas do clique do mouse

**O que faz:** Encontra qual ponto de controle está mais próximo da posição clicada (dentro de um raio de 6 pixels). Retorna o índice do ponto encontrado ou -1 se nenhum ponto estiver próximo.

**Propriedades específicas:**
- **Tolerância:** Raio de 6 pixels para facilitar seleção
- **Primeiro encontrado:** Se múltiplos pontos estão próximos, retorna o primeiro
- **Coordenadas de tela:** Trabalha com coordenadas convertidas da janela

---

### `mouse(int button, int state, int x, int y)`
**Parâmetros:**
- `button`: Botão pressionado (GLUT_LEFT, GLUT_RIGHT, etc.)
- `state`: Estado (GLUT_DOWN, GLUT_UP)
- `x, y`: Posição do cursor

**O que faz:** Gerencia dois modos de interação:
1. **Modo criação** (!jaCurva): Cada clique esquerdo adiciona um novo ponto de controle
2. **Modo edição** (jaCurva): Seleciona pontos existentes para manipulação

**Propriedades específicas:**
- **Conversão de coordenadas:** Transforma coordenadas de janela para mundo
- **Estados mutuamente exclusivos:** Ou está criando ou editando pontos
- **Feedback visual:** Destaca ponto selecionado em vermelho

---

### `motion(int x, int y)`
**Parâmetros:**
- `x, y`: Nova posição do cursor durante movimento

**O que faz:** Atualiza a posição de um ponto selecionado conforme o mouse é arrastado, OU aplica transformações contínuas se um modo de transformação está ativo.

**Propriedades específicas:**
- **Atualização em tempo real:** Curvas são regeneradas a cada movimento
- **Modo dual:**
  - Manipulação direta de pontos
  - Aplicação contínua de transformações
- **Responsividade:** Mudanças visuais imediatas

---

## Funções de Inicialização e Controle

### `init()`
**Parâmetros:** Nenhum

**O que faz:** Reseta todo o sistema para estado inicial, zerando contadores de pontos e limpando arrays de pontos de controle e curvas.

### `processMenuCurvas(int option)`
**Parâmetros:**
- `option`: Tipo de curva (HERMITE, BEZIER, BSPLINE, CATMULLR)

**O que faz:** Altera o tipo de curva ativa copiando a matriz correspondente para a matriz de trabalho M[][]. Ativa a geração de curvas se há pontos suficientes (>3).

**Propriedade específica:** Normalização automática das matrizes B-Spline (/6) e Catmull-Rom (/2).

### `processMenuTransforma(int option)`
**Parâmetros:**
- `option`: Tipo de transformação (TRANSLACAO, ROTACAO, etc.)

**O que faz:** Aplica uma transformação específica com parâmetros pré-definidos e força regeneração das curvas. Cada chamada aplica um "passo" da transformação.

**Propriedades específicas:**
- **Parâmetros fixos:** Cada transformação usa valores pré-determinados para consistência
- **Aplicação incremental:** Chamadas múltiplas acumulam o efeito
- **Regeneração forçada:** jaCurva = 0 força recálculo das curvas
