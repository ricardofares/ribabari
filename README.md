# SIMULADOR DE SISTEMA OPERACIONAL

## Instruções de compilação

Para compilar o programa é necessário 3 componentes, que podem ser instalados
em máquinas com Debian ou Ubuntu pelo seguinte comando:

*sudo apt install -y gcc make libncurses-dev*

Após isso, a compilação pode ser feita via o comando

*make all*

que irá gerar um binário chamado "main".

Ou se quiser compilar e rodar em seguida, o seguinte comando pode fazer isso:

*make run*

## Instruções de uso

Para mover no menu é usado "*j*" para ir para baixo e "*k*" para ir para cima.

Há duas opções no menu, a primeira é "*Process Create*", onde será pedido
o nome de um arquivo com o programa sintético, ocorrendo em seguido a
execução do programa sintético, com logs de memória e de execução ao lado.

É possível executar vários programas ao mesmo tempo, precisando somente inserir
outros processos via o comando "*Process Create*".

Para sair, é necessário somente selecionar "*Exit*" e apertar "*Enter*".
