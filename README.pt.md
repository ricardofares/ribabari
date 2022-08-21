## :fountain_pen: Sobre

**Ribabari** é um simulador de sistema operacional focado no gerenciamento dos recursos do hardware entre os diferentes processos. Nosso simulador de sistema operacional fornece gerenciamento para os seguintes recursos do hardware, **CPU**, **Memória** , **E/S** e **Disco**.

## 🎓 Um projeto de universidade
O projeto de um simulador de sistema operacional, neste caso o **Ribabari**, é um requisito da disciplina de **Sistemas Operacionais**.

## :book: Introdução
Nosso simulador fornece um terminal interativo para os usuários executarem seus processos e verem a execução dos processos entre diferentes visualizaçoes, sendo cada visualização para um diferente recurso do hardware.

#### :question: **Você não entendeu o que é uma visualização?**
Uma visualização é basicamente uma janela no terminal do simulador que mostra por baixo dos panos as etapas tomadas pelo sistema operacional ao gerenciar o recurso de hardware desta visualização. Em cada visualização, um diferente recurso de hardware esta sendo gerenciado (por exemplo, existem visualizações mostrando o sistema operacional gerenciando o disco e os semáfaros).

#### :question: **Como eu posso executar meu próprio processo?**
Opa, espera um minuto, nas seções seguintes iremos mostrar em detalhes como escrever um **arquivo de programa sintético** :spiral_notepad: o qual especifica as instruções do processo, o tamanho do segmento, os semáfores a serem usados e algumas outras informações.

## :hammer_and_wrench: Compilação
Primeiro de tudo, é uma boa maneira iniciarmos compilando nosso simulador primeiro. Portanto, sugerimos seguir os próximos passos.

#### 1. Instalando o gcc.
Instale o *gcc*, o compilador **C**, executando a seguinte linha.
<p align="center"><i>sudo apt install gcc</i></p>

:bell: **Nota**: Se você já possui o *gcc* instalado, então você pode pular para o próximo passo.

#### 2. Instalando o make.
Instalar o *make* facilita a compilação de nosso simulator possibiltando compilar o projeto inteiro usando somente um comando.

<p align="center"><i>sudo apt install make</i></p>

:bell: **Nota**: Se você já possui o *make* instalado, então você pode pular para o próximo passo.

#### 3. Instalando o ncurses.
Instale o *ncurses* que é a biblioteca usada por nosso simulador para criar as interfaces do terminal.

<p align="center"><i>sudo apt install libncurses-dev</i></p>

:bell: **Nota**: Se você já possui o *ncurses* instalado, então você está **pronto**. Portanto, pode-se ir para a próxima seção.

## :rocket: Executando
Se você já seguiu os passos fornecidos na seção anterior, então você está **pronto** para executar nosso simulador de sistema operacional, para isso execute a seguinte linha **(no mesmo diretório em que está o arquivo Makefile)**.

<p align="center"><i>make run</i></p>

Após isso, veja nosso simulador um pouco e, teste algum de nossos programas sintéticos fornecidos.

## :mag_right: Opções do Menu do Terminal
Se esta é a primeira vez que você está executando nosso simulador, então é essencial que você leia esta seção. O menu do terminal forece três opções, sendo elas:

#### 1. Process Create
<p align="center"><i>Permite que o usuário crie um processo e execute suas instruções. Pressionando-o, uma janela é aberta no centro da janela principal pedindo o caminho do arquivo do programa sintético.</i></p>

#### 2. Toggle Resource Acquisition View
<p align="center"><i>Mostra/esconde a visualização de aquisição de recursos.</i></p>

#### 3. Exit
<p align="center"><i>Fecha o simulador de sistema operacional.</i></p>

## :clapper: Como posso exeutar meu próprio processo?
Todos os processos executados pelo simulador de sistema operacional são inseridos a partir de um *arquivo especial* chamado **arquivo de programa sintético**. Este arquivo tem um formato específico que especifica os metadados do processo e as instruções que a *CPU* irá executar.

### :scroll: Arquivo de Programa Sintetico
O formato do arquivo é dado a seguir

```vim
nome do programa
id do segmento
prioridade inicial do processo
tamanho do segmento
lista de semáforos (cada nome de semáfore separado por um espaço em branco)
<linha em branco>
sequência de instruçoes separadas por linha
```

Cada metadado do arquivo será detalhado a seguir.

- **nome do programa**: O nome do programa. Não há necessidade de ser único.
- **id do segmento**: O id do segmento de memória que irá ser alocado ao processo. O id do segmento **DEVE SER** único. Caso contrário, comportamentos inesperados podem ocorrer ao longo da simulação.
- **prioridade inicial do processo**: Indica em qual fila do escalonador o processo deve começar, sendo *0* para **a prioridade mais alta** e *1* para a **prioridade mais baixa**.
- **tamanho do segmento**: Indica o tamanho do segmento que irá ser alocado ao processo.
- **lista de semáforos**: A lista de semáfores a qual especifica os semáforos que podem ser usados pelo processo durante a simulação.

Um exemplo de tal arquivo de programa sintético é

```vim
process1
50
0
24
s

exec 20
exec 80
P(s)
read 110
print 50
exec 40
write 150
V(s)
exec 30
```

:bell: **Nota 1**: O arquivo de programa sintético dado acima especifica um processo com nome *process1*, com identificador de segmento *50*, tendo uma prioridade inicial na cpu definida para *0 (**prioridade mais alta**)*, com tamanho de segmento definido para *24 kbytes* e *s* como o semáforo a ser usado pelo processo.

:bell: **Nota 2**: O arquivo de programa sintético não precisa ter qualquer extensão de arquivo ou qualquer nome especial. Portanto, você pode dar qualquer nome que deseja ao arquivo de programa sintético.

:question: **Espere um minuto, quais são as instruções disponíveis?** É claro, nós já sabíamos que você perguntaria isto. Por favor, vá para a próxima subseção onde nós apresentaremos as instruções disponíveis.

#### :desktop_computer: Instruções Disponíveis
As *instruções disponíveis* listadas abaixo são todas as instruções que podem ser inseridas na parte de sequência de instruções do arquivo de programa sintético. Portanto, as instruções disponíveis são.

- **exec k**: Indica que o programa irá executar (algo) por **k** unidades de tempo.
- **read k**: Indica que o programa irá ler (algo) do disco na trilha **k**.
- **write k**: Indica que o programa irá escrever (algo) no disco na trilha **k**.
- **print t**: Indica que o programa irá imprimir (algo) por **k** unidades de tempo.
- **P(s)**: Indica que o programa irá acessar a região crítica limitada pelo semáforo **s**.
- **V(s)**: Indica que o programa irá liberar a região crítica limitada pelo semáforo **s**.

Após isso, divirta-se!
