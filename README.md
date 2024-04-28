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
- Montar três opções de dificuldade (iniciante, intermediário, avançado).
- Função de abrir quadrados em volta se todas as bombas ao redor
estão assinaladas.
- Opção de reinício de partida. (Não sei se devo criar uma distribuição nova ou não).
- Construir uma interface para o usuário.
- Contador de tempo.

### Metas que provavelmente não irei implementar
- Opção de dificuldade personalizada.
- Salvar em arquivo os recordes dos melhores tempos em cada dificuldade
e permitir que o usuário veja o ranking.

## Compilando

### Linux
Para compilar para linux, instale os seguintes pacotes pelo seu package manager:
```
libsdl2-dev
libsdl2-image-dev
libsdl2-ttf-dev
```

No Debian, o sistema que estou usando, basta instalar com o seguinte comando:
```
$ sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

Após isso, basta digitar
```
$ make
```

### Windows
Para compilar para windows, abra o arquivo Makefile\_win32 e configure-o devidamente.
Baixe as bibliotecas SDL2, SDL2-image e SDL2-ttf e escreva os diretórios desses devidamente
no Makefile\_win32.
Com isso feito, abra seu terminal/prompt de comando com o programa make já configurado
e digite:
```
$ make -f Makefile_win32
```
Observação: a compilação foi testada com o compilador mingw. Mais especificamente,
o compilador mingw instalado no Debian.
