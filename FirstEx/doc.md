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
   - Para mover um ponto de controle, clique e arraste-o com o mouse.
4. **Aplicar Transformações**:
   - Abra o menu com o botão direito.
   - Vá para "Transformacao" e escolha uma transformação (por exemplo, "Rotacao").

## Estrutura do Código

### Tipos de Dados

- `tipoPto`: Estrutura que representa um ponto no espaço 2D (com uma coordenada `z` não utilizada).
- `ptsCurva[]`: Array para armazenar os pontos da curva gerada.
- `ptsContrle[]`: Array para armazenar os pontos de controle definidos pelo usuário.

### Matrizes

- `Mh`, `Mb`, `Ms`, `Mc`: Matrizes 4x4 que definem as propriedades das curvas de Hermite, Bezier, B-Spline e Catmull-Rom, respectivamente.

### Funções Principais

- **`main()`**: Inicializa o GLUT, cria a janela e configura os callbacks.
- **`Draw()`**: Função de callback principal para renderizar a cena, incluindo os eixos, pontos de controle e a curva.
- **`mouse()`**: Gerencia os eventos de clique do mouse para adicionar ou selecionar pontos de controle.
- **`motion()`**: Gerencia o movimento do mouse para arrastar pontos de controle.
- **`processMenuCurvas()`**: Processa a seleção do tipo de curva no menu.
- **`processMenuTransforma()`**: Aplica a transformação geométrica selecionada.
- **`geraCurva()`**: Gera os pontos da curva com base nos pontos de controle e no tipo de curva selecionado.
- **`ptoCurva()`**: Calcula um único ponto na curva para um determinado parâmetro `t`.
- Funções de Transformação (`translacao`, `rotacao_centro`, etc.): Aplicam transformações geométricas aos pontos de controle.