# Documentação do Arquivo `teste.c`

## Visão Geral

Este programa em C utiliza a biblioteca OpenGL (GLUT) para desenhar e manipular curvas paramétricas, como Hermite, Bezier, B-Spline e Catmull-Rom. A aplicação permite que os usuários definam pontos de controle interativamente e apliquem várias transformações geométricas às curvas geradas.

## Funcionalidades

- **Múltiplos Tipos de Curva**: Suporte para curvas de Hermite, Bezier, B-Spline e Catmull-Rom.
- **Pontos de Controle Interativos**: Adicione pontos de controle com cliques do mouse.
- **Geração de Curva**: Renderiza curvas com base nos pontos de controle definidos.
- **Transformações Geométricas**:
  - Translação
  - Rotação
  - Escala
  - Cisalhamento
  - Reflexão (eixos X e Y)
- **Interface de Usuário Intuitiva**:
  - **Clique com o botão esquerdo**: Adiciona um novo ponto de controle.
  - **Clique com o botão direito**: Abre um menu para selecionar tipos de curva, transformações e outras opções.
  - **Arrastar o mouse**: Move um ponto de controle selecionado.

## Como Compilar e Executar

Para compilar o código, você precisará ter o compilador GCC e as bibliotecas do OpenGL (GLUT) instaladas.

```bash
gcc teste.c -o teste -lglut -lGL -lGLU -lm
./teste
```

## Como Usar

1. **Adicionar Pontos de Controle**: Clique com o botão esquerdo na janela para adicionar pontos de controle.
2. **Gerar uma Curva**:
   - Clique com o botão direito para abrir o menu.
   - Navegue até "Tipo de Curva" e selecione uma das opções (por exemplo, "Bezier").
3. **Manipular a Curva**:
   - Para mover um ponto de controle selecionado, clique e arraste-o com o mouse.
4. **Aplicar Transformações**:
   - Abra o menu com o botão direito.
   - Vá para "Transformacao" e escolha uma transformação (por exemplo, "Rotacao").

## Estrutura do Código

### Tipos de Dados

- `tipoPto`: Estrutura que representa um ponto no espaço 2D (com uma coordenada `z` não utilizada).
- `ptsCurva[]`: Array para armazenar os pontos da curva gerada.
- `ptsContrle[]`: Array para armazenar os pontos de controle definidos pelo usuário.

### Matrizes

- `M` (Matriz de Trabalho): Uma matriz 4x4 genérica que é usada para armazenar a matriz de base da curva atualmente selecionada (Hermite, Bezier, B-Spline ou Catmull-Rom). Ela é preenchida dinamicamente na função `processMenuCurvas` com os valores de `Mh`, `Mb`, `Ms` ou `Mc` dependendo da curva escolhida.

- `Mh` (Matriz de Hermite):
  ```c
  float Mh[4][4] =
  		{{2.0, -2.0, 1.0, 1.0},
  		 {-3.0, 3.0, -2.0, -1.0},
  		 {0.0, 0.0, 1.0, 0.0},
  		 {1.0, 0.0, 0.0, 0.0}};
  ```
  Esta é a matriz de base para as curvas de Hermite. As curvas de Hermite são definidas por dois pontos finais e dois vetores tangentes nesses pontos. A matriz `Mh` é usada para converter os pontos de controle e tangentes em coeficientes polinomiais para a equação da curva. As linhas representam os coeficientes para t^3, t^2, t e 1, respectivamente, quando multiplicadas por um vetor de pontos de controle e tangentes.

- `Mb` (Matriz de Bezier):
  ```c
  float Mb[4][4] =
  		{{-1.0, 3.0, -3.0, 1.0},
  		 {3.0, -6.0, 3.0, 0.0},
  		 {-3.0, 3.0, 0.0, 0.0},
  		 {1.0, 0.0, 0.0, 0.0}};
  ```
  Esta é a matriz de base para as curvas de Bezier cúbicas. As curvas de Bezier são definidas por quatro pontos de controle. A matriz `Mb` é a matriz de Bernstein cúbica na base de potência, usada para calcular os coeficientes polinomiais da curva a partir dos pontos de controle.

- `Ms` (Matriz de B-Spline):
  ```c
  float Ms[4][4] =
  		{{-1.0, 3.0, -3.0, 1.0},
  		 {3.0, -6.0, 3.0, 0.0},
  		 {-3.0, 0.0, 3.0, 0.0},
  		 {1.0, 4.0, 1.0, 0.0}};
  ```
  Esta é a matriz de base para as curvas B-Spline cúbicas uniformes. As curvas B-Spline são definidas por um conjunto de pontos de controle e oferecem mais suavidade e controle local do que as curvas de Bezier. A matriz `Ms` é usada para calcular os coeficientes polinomiais da curva. No código, os elementos desta matriz são divididos por 6.0f para normalização.

- `Mc` (Matriz de Catmull-Rom):
  ```c
  float Mc[4][4] =
  		{{-1.0, 3.0, -3.0, 1.0},
  		 {2.0, -5.0, 4.0, -1.0},
  		 {-1.0, 0.0, 1.0, 0.0},
  		 {0.0, 2.0, 0.0, 0.0}};
  ```
  Esta é a matriz de base para as curvas de Catmull-Rom. As curvas de Catmull-Rom são um tipo de spline que passa por todos os seus pontos de controle, sendo frequentemente usadas em interpolação. A matriz `Mc` é usada para calcular os coeficientes polinomiais da curva. No código, os elementos desta matriz são divididos por 2.0f para normalização.

- `MCor` (Matriz de Cores):
  ```c
  float MCor[9][3] =
  		{{1.0, 0.5, 0.0},
  		 {0.8, 0.7, 0.5},
  		 {0.5, 0.5, 0.5},
  		 {0.5, 1.0, 0.5},
  		 {0.5, 0.5, 1.0},
  		 {1.0, 0.5, 1.0},
  		 {0.0, 0.0, 1.0},
  		 {0.0, 1.0, 0.0},
  		 {1.0, 0.0, 0.0}};
  ```
  Esta matriz armazena uma coleção de 9 cores diferentes, onde cada linha representa uma cor no formato RGB (Vermelho, Verde, Azul). Essas cores são usadas para renderizar diferentes segmentos da curva, permitindo uma visualização mais clara de como a curva é construída a partir de múltiplos segmentos.

### Funções Principais

- **`main()`**: Esta é a função de entrada do programa. Ela inicializa a biblioteca GLUT, cria a janela da aplicação e configura as funções de "callback", que são as funções que serão chamadas em resposta a eventos como cliques do mouse, movimento do mouse e a necessidade de redesenhar a tela.

- **`Draw()`**: É a função de callback principal para renderização. Sempre que a janela precisa ser desenhada ou atualizada, esta função é chamada. Ela é responsável por desenhar tudo na tela, incluindo os eixos do sistema de coordenadas, os pontos de controle que o usuário definiu e a curva resultante.

- **`mouse()`**: Gerencia os eventos de clique do mouse. É usada para adicionar um novo ponto de controle quando o usuário clica com o botão esquerdo e para selecionar um ponto de controle existente.

- **`motion()`**: Gerencia os eventos de movimento do mouse enquanto um botão está pressionado. Sua principal função é permitir que o usuário "arraste" um ponto de controle selecionado para uma nova posição, atualizando a forma da curva em tempo real.

- **`processMenuCurvas()`**: Processa a seleção feita pelo usuário no menu de contexto (acessado com o botão direito do mouse) referente ao tipo de curva a ser desenhada (Hermite, Bezier, B-Spline ou Catmull-Rom).

- **`processMenuTransforma()`**: Processa a seleção do usuário no menu de contexto para aplicar uma transformação geométrica (translação, rotação, etc.) à curva.

- **`geraCurva()`**: Esta função é o coração da geração da curva. Com base nos pontos de controle definidos pelo usuário e no tipo de curva selecionado no menu, ela calcula e gera todos os pontos que compõem o traçado da curva.

- **`ptoCurva()`**: Calcula as coordenadas de um único ponto na curva para um valor específico do parâmetro `t` (que geralmente varia de 0 a 1). A função `geraCurva` chama `ptoCurva` repetidamente com valores crescentes de `t` para criar a sequência de pontos que forma a curva.

- **Funções de Transformação (`translacao`, `rotacao_centro`, etc.)**: Este conjunto de funções aplica as transformações geométricas (como translação, rotação em torno do centro, etc.) aos pontos de controle da curva. Quando os pontos de controle são transformados, a curva inteira é recalculada e redesenhada em sua nova posição, orientação ou escala.
