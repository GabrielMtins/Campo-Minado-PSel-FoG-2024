# Campo Minado PSel FoG 2024

Aqui nesse repositório estará os arquivos do campo minado construído para o
processo seletivo do FoG de 2024.

## Metas

### Realizadas
- Gerar uma distribuição aleatória a cada jogo.
- Cada quadrado possui um valor associado indicando quantas bombas existem em volta
ou se há uma bomba nele.
- Quando o usuário abre uma bomba, ele recebe uma mensagem de derrota e o jogo
é encerrado.
- Quando todos os quadrados (com exceção das bombas) são abertos, o usuário
recebe uma mensagem de vitória e o jogo é encerrado.
- Função de explodir (abrir quadrados adjacentes quando um quadrado que não
há bombas ao redor é aberto).
- O jogador nunca perde no primeiro quadrado que clica.
- Colocar bandeirinhas.

### Não Realizadas
- Construir uma interface para o usuário
- Montar três opções de dificuldade (iniciante, intermediário, avançado).
- Opção de reinício de partida.
- Contador de tempo.
- Salvar em arquivo os recordes dos melhores tempos em cada dificuldade
e permitir que o usuário veja o ranking.

### Metas que provavelmente não irei implementar
- Opção de dificuldade personalizada.
- Função de abrir quadrados em volta se todas as bombas ao redor
estão assinaladas.
