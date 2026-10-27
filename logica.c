#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logica.h"
#include "interface.h"


//Retorna uma carta com naipe e valor
Carta cria_carta(int valor, int naipe){
    Carta c;

    c.valor = valor;
    c.naipe = naipe;

    return c;
}
//Cria um baralho de cartas.
void cria_baralho(Carta baralho[], int n_baralhos){
    int i = 0;
    int b,n,v;
    for (b = 0; b < n_baralhos; b++){
        for (n = COPAS; n <= PAUS; n++){
            for (v = A; v <= K; v++){
                baralho[i] = cria_carta(v, n);
                i++;
            }
        }
    }
}

//Algoritmo Fisher-Yates para embaralhar o baralho.
void embaralha_cartas(Carta baralho[], int n_cartas){
    int i, j;
    Carta temp;

    for (i = n_cartas - 1; i > 0; i--){
        j = rand() % (i + 1);
        temp = baralho[i];
        baralho[i] = baralho[j];
        baralho[j] = temp;    
    }
}

// 1: GESTÃO DE PILHAS

// esvazia uma pilha de cartas 
// redefine o tamanho da pilha para 0
// auxiliar
void limpar_pilha(PilhaCartas *pilha){
    pilha->n_cartas = 0; // marca a pilha como vazia ao colocar o número de cartas a zero
}

// Adiciona uma carta ao topo da pilha
// coloca a nova carta na próxima posição livre e atualiza o topo da pilha
// auxiliar
void adicionar_carta(PilhaCartas *pilha, Carta c){
    pilha->cartas[pilha->n_cartas] = c; // guarda a carta na posição correspondente ao topo atual
    pilha->n_cartas++;
}

// remove a carta do topo da pilha
// faz o topo descer uma posição e devolve a carta que estava no topo
// auxiliar
Carta remover_carta(PilhaCartas *pilha){
    pilha->n_cartas--; // reduz o número de cartas, removendo a carta do topo
    return pilha->cartas[pilha->n_cartas]; // devolve a carta que ficou fora da pilha
}

// 2: INICIALIZAÇÃO DO JOGO

// procura um tipo de pilha na definição do jogo
// percorre a lista de tipos devolve a posição do tipo com o nome indicado
// auxiliar para a próxima função
int procurar_tipo(DefinicaoJogo *def, char tipo[]){ //char tipo[] é apenas um tipo, que é representado numa string
    int pos = -1;

    for (int i = 0; i < def->n_tipos; i++){
        if (strcmp(def->tipos[i].nome, tipo) == 0)
            pos = i;
    }

    return pos;
}

// copia informação de tipo para uma pilha do estado
// associa à pilha o seu tipo e as flags/regras definidas para esse tipo
// auxiliar
// é necessária porque os inits[] só guardam o nome do tipo, não as flags. Para saber as flags vamos buscar ao array tipos[], mas não sabes em que posição está, por isso é que chamamos a procurar_tipo

void copiar_tipo_pilha(EstadoJogo *estado, int indice){
    int pos;

    // PASSO 1 - copia o nome do tipo de inits[] para a pilha do estado (inits só tem o nome e não tem flags) 
    strcpy(estado->pilhas[indice].tipo,
           estado->definicao.inits[indice].tipo);

    // PASSO 2 - procura em tipos[] em que posição está esse nome (para poder ir buscar as flags)
    pos = procurar_tipo(&estado->definicao,
                        estado->definicao.inits[indice].tipo);

    // PASSO 3 - se encontrou , copia as flags para a pilha
    if (pos >= 0) strcpy(estado->pilhas[indice].flags, estado->definicao.tipos[pos].flags);
}

// cria todas as pilhas vazias do jogo
// inicializa o número de pilhas e prepara cada uma com o tipo correto e sem cartas
// auxiliar
void criar_pilhas_vazias(EstadoJogo *estado){
    int i;

    estado->n_pilhas = estado->definicao.n_inits; // define quantas pilhas existirão no estado inicial

    for (i = 0; i < estado->n_pilhas; i++){
        copiar_tipo_pilha(estado, i);
        limpar_pilha(&estado->pilhas[i].pilha);
    }
}

// distribui cartas pelos decks iniciais
// coloca, por ordem no baralho embaralhado, as cartas nas pilhas iniciais definidas
// auxiliar
void distribuir_cartas(EstadoJogo *estado, Carta baralho[]){
    int i, j, atual = 0;

    for (i = 0; i < estado->n_pilhas; i++){ 
        for (j = 0; j < estado->definicao.inits[i].n_cartas; j++){
            adicionar_carta(&estado->pilhas[i].pilha, baralho[atual]); // adiciona a carta atual do baralho ao topo da pilha i
            atual++;
        }
    }
}

// inicializa todo o estado do jogo
// copia a definição, cria e embaralha o baralho, cria pilhas e distribui as cartas
// função principal
void iniciar_estado(DefinicaoJogo *def, EstadoJogo *estado){
    Carta baralho[MAX_CARTAS_TOTAL];
    int total_cartas;

    estado->definicao = *def; // copia a definição do jogo para o estado atual

    total_cartas = estado->definicao.n_baralhos * CARTAS_POR_BARALHO;

    cria_baralho(baralho, estado->definicao.n_baralhos);
    embaralha_cartas(baralho, total_cartas);
    criar_pilhas_vazias(estado);
    distribuir_cartas(estado, baralho);
}

// 3: VALIDAÇÃO DE MOVIMENTOS

// verifica se o índice de uma pilha é válido
// confirma que o índice está dentro dos limites do array de pilhas do estado atual
// auxiliar
int indice_valido(EstadoJogo *estado, int indice){
    return indice >= 0 && indice < estado->n_pilhas; // devolve 1 se estiver entre 0 e n_pilhas-1, senão devolve 0
}

// verifica as condições básicas de validade de um movimento
// valida origem, destino, quantidade de cartas e existência de cartas suficientes na origem
// auxiliar
int verificacao_basica_movimento(EstadoJogo *estado, Movimento m){
    int ok = 1;

    if (!indice_valido(estado, m.origem)) ok = 0;
    if (!indice_valido(estado, m.destino)) ok = 0;
    if (ok && estado->pilhas[m.origem].pilha.n_cartas < m.n_cartas) ok = 0; 

    if (ok && flag_existe(estado->pilhas[m.destino].flags, '1'))
        if (estado->pilhas[m.destino].pilha.n_cartas + m.n_cartas > 1) ok = 0;

    return ok;
}

// verifica se uma flag existe dentro de uma string
// usado para confirmar se uma regra específica está ativa
// auxiliar
int flag_existe(char flags[], char flag){
    if (strchr(flags, flag) == NULL) return 0; //strchr retorna char *, um ponteiro para a primeira ocorrência de um determinado char na string. Se for NULL, é porque percorreu a string toda e não encontrou o char flag.
    return 1;
}

// verifica se os tipos de pilha de origem e destino são compatíveis com uma regra
// se uma das pilhas tiver tipo diferente que o tipo citado na regra, então não são compatíveis e a regra não deve ser aplicada.
// auxiliar
int regra_tipos_compativeis(RegraMovimento *r, PilhaJogo *origem, PilhaJogo *destino){
    int mesma_origem = strcmp(r->origem, origem->tipo) == 0; //strcmp retorna 0,1 ou -1
    int mesmo_destino = strcmp(r->destino, destino->tipo) == 0;

    return mesma_origem && mesmo_destino; //a função só retorna 1 se ambos tiverem o mesmo destino
}

// verifica se a regra permite a quantidade de cartas a mover
// por omissão só permite 1 carta, a menos que exista a flag '+'
// auxiliar
int pode_mover_varias(RegraMovimento *r, Movimento m){
    int ok = 1;

    if (m.n_cartas > 1 && !flag_existe(r->flags, '+'))
        ok = 0; //rejeita o movimento se tentar mover várias cartas sem a flag

    return ok;
}

// devolve a carta que está atualmente no topo de uma pilha
// auxiliar
Carta carta_topo(PilhaCartas *pilha){
    return pilha->cartas[pilha->n_cartas - 1]; // a carta de topo corresponde a última posição ocupada da pilha
}

// devolve a carta base do grupo que vai ser movido
// corresponde à primeira carta do bloco de n_cartas contado a partir do topo
// auxiliar
Carta carta_base_movimento(PilhaCartas *pilha, int n_cartas){
    int indice = pilha->n_cartas - n_cartas; // calcula o índice da primeira carta do grupo movido. É a última porque é a carta do topo a que podemos mover.
    return pilha->cartas[indice];
}

// verifica se dois valores são adjacentes
// auxiliar 
int valores_adjacentes(Carta a, Carta b){
    int diferenca = a.valor - b.valor;
    if (diferenca == 1 || diferenca == -1) return 1;
    return 0;
}

// verifica se a carta a é imediatamente inferior à carta b
// auxiliar
int valor_inferior(Carta a, Carta b){
    if (b.valor - a.valor == 1) return 1;
    return 0;
}

// verifica se a carta a é imediatamente superior à carta b
// auxiliar
int valor_superior(Carta a, Carta b){
    if (a.valor - b.valor == 1) return 1;
    return 0;
}

// 4: REGRAS DE SEQUÊNCIAS

// verifica se um grupo de cartas forma uma sequência decrescente
// percorre as cartas do grupo e confirma que cada carta vale exatamente 1 a mais do que a seguinte
// auxiliar
int sequencia_decrescente(PilhaCartas *pilha, int n_cartas){
    int i;
    int fim = pilha->n_cartas - 1;

    for (i= pilha->n_cartas - n_cartas; i < fim; i++){
    if (pilha->cartas[i].valor - pilha->cartas[i + 1].valor != 1)
        return 0;
}
    return 1;
}

// verifica se todas as cartas do grupo têm o mesmo naipe
// útil para regras que exigem sequências uniformes em termos de naipe
// auxiliar
int sequencia_mesmo_naipe(PilhaCartas *pilha, int n_cartas){
    int i;
    int fim = pilha->n_cartas - 1;

    for (i = pilha->n_cartas - n_cartas; i < fim;i++){
    if (pilha->cartas[i].naipe != pilha->cartas[i + 1].naipe)
        return 0;
}
    return 1;
}

// verifica se um grupo de cartas forma uma sequência crescente
// confirma que cada carta tem valor exatamente 1 abaixo da seguinte
// auxiliar
int sequencia_crescente(PilhaCartas *pilha, int n_cartas){
    int i;
    int fim = pilha->n_cartas - 1;

    for (i = pilha->n_cartas - n_cartas; i < fim; i++)
        if (pilha->cartas[i].valor - pilha->cartas[i + 1].valor != -1)
            return 0;
    return 1;
}

// verifica se os naipes alternam ao longo do grupo
// duas cartas consecutivas nunca podem ter o mesmo naipe
// auxiliar
int sequencia_naipes_alternados(PilhaCartas *pilha, int n_cartas){
    int i;
    int fim = pilha->n_cartas - 1;

    for (i = pilha->n_cartas - n_cartas; i < fim; i++)
        if (pilha->cartas[i].naipe == pilha->cartas[i + 1].naipe)
            return 0;
    return 1;
}

// verifica se duas cartas têm a mesma cor
// auxiliar
int mesma_cor(Carta a, Carta b){
    int cor_a = (a.naipe == COPAS || a.naipe == OUROS);
    int cor_b = (b.naipe == COPAS || b.naipe == OUROS);

    return cor_a == cor_b; // devolve 1 se ambas forem da mesma cor
}

// verifica se as cartas do grupo alternam de cor
// auxiliar
int cores_alternadas(PilhaCartas *pilha, int n_cartas){
    int i;
    int fim = pilha->n_cartas - 1;

    for (i = pilha->n_cartas - n_cartas; i < fim; i++)
        if (mesma_cor(pilha->cartas[i], pilha->cartas[i + 1]))
            return 0;
    return 1;
}

// 5: FLAGS

// verifica se existe um Ás num dos limites do grupo movido
// a regra fica satisfeita se a carta base ou a carta do topo for um Ás
// auxiliar
int tem_as_limite(PilhaCartas *pilha, int n_cartas){
    Carta base = carta_base_movimento(pilha, n_cartas);
    Carta topo = carta_topo(pilha);

    return (base.valor == A || topo.valor == A); // devolve verdadeiro se um dos extremos do grupo for Ás
}

// verifica se existe um Rei num dos limites do grupo movido
// a regra fica satisfeita se a carta base ou a carta do topo for um Rei
// auxiliar
int tem_rei_limite(PilhaCartas *pilha, int n_cartas){
    Carta base = carta_base_movimento(pilha, n_cartas);
    Carta topo = carta_topo(pilha);

    return (base.valor == K || topo.valor == K);
}


// verifica se a sequência respeita a ordem indicada pelas flags
// valida sequências crescentes ou decrescentes dependendo das regras ativas
// auxiliar para regras_grupo_movido
int verificar_ordem_sequencia(PilhaCartas *origem, RegraMovimento *r, int n_cartas){
    int ok = 1;
    if (flag_existe(r->flags, '[') && !sequencia_decrescente(origem, n_cartas)) ok = 0;
    if (flag_existe(r->flags, ']') && !sequencia_crescente(origem, n_cartas)) ok = 0;
    return ok;
}
// verifica se a sequência respeita as regras de naipes e cores
// confirma se todas as cartas têm o mesmo naipe, alternam naipes ou alternam cores
// auxiliar para verificar_sequencias
int verificar_tipo_sequencia(PilhaCartas *origem, RegraMovimento *r, int n_cartas){
    int ok = 1;
    if (flag_existe(r->flags, 'm') && !sequencia_mesmo_naipe(origem, n_cartas)) ok = 0;
    if (flag_existe(r->flags, 'x') && !sequencia_naipes_alternados(origem, n_cartas)) ok = 0;
    if (flag_existe(r->flags, 'd') && !cores_alternadas(origem, n_cartas)) ok = 0;
    return ok;
}

// verifica todas as regras de sequência aplicadas ao grupo movido
// junta a validação da ordem e do tipo da sequência
// auxiliar
int verificar_sequencias(PilhaCartas *origem, RegraMovimento *r, int n_cartas){
    int ok = verificar_ordem_sequencia(origem, r, n_cartas);
    if (ok) ok = verificar_tipo_sequencia(origem, r, n_cartas);
    return ok;
}

// verifica se os limites da sequência cumprem as flags especiais
// confirma a presença de Ás ou Rei nas extremidades do grupo movido
// auxiliar
int verificar_limites(PilhaCartas *origem, RegraMovimento *r, int n_cartas){
    int ok = 1;
    if ((flag_existe(r->flags, 'a') || flag_existe(r->flags, 'A')) && !tem_as_limite(origem, n_cartas))  ok = 0;
    if ((flag_existe(r->flags, 'k') || flag_existe(r->flags, 'K')) && !tem_rei_limite(origem, n_cartas)) ok = 0;
    return ok;
}

// valida todas as regras aplicadas ao grupo de cartas movido
// verifica sequências e limites especiais definidos nas flags
// auxiliar
int regras_grupo_movido(PilhaCartas *origem, RegraMovimento *r, int n_cartas){
    int ok = verificar_sequencias(origem, r, n_cartas);
    if (ok) ok = verificar_limites(origem, r, n_cartas);
    return ok;
}

// verifica se a regra exige que o destino esteja vazio
// auxiliar para regras_flags_validas
int destino_vazio_valido(PilhaCartas *destino, RegraMovimento *r){
    int ok = 1;
    if (flag_existe(r->flags, 'V') && destino->n_cartas != 0) ok = 0;
    return ok;
}

// verifica se o naipe da carta base é compatível com o topo do destino
// usado em regras que proíbem naipes iguais consecutivos
// auxiliar
int destino_naipe_valido(PilhaCartas *destino, RegraMovimento *r, Carta base){
    int ok = 1;
    if (flag_existe(r->flags, 'X') && destino->n_cartas > 0 && base.naipe == carta_topo(destino).naipe) ok = 0;
    return ok;
}

// verifica se as cores entre a carta base e o topo do destino respeitam as flags
// pode exigir cores iguais ou alternadas
// auxiliar
int destino_cor_valida(PilhaCartas *destino, RegraMovimento *r, Carta base){
    int ok = 1;
    if (flag_existe(r->flags, 'c') && destino->n_cartas > 0 && !mesma_cor(base, carta_topo(destino))) ok = 0;
    if (flag_existe(r->flags, 'D') && destino->n_cartas > 0 && mesma_cor(base, carta_topo(destino))) ok = 0;
    return ok;
}

// valida todas as flags relacionadas com a pilha de destino
// verifica estado vazio, compatibilidade de naipes e cores
// auxiliar
int verificar_flags_destino(PilhaCartas *destino, RegraMovimento *r, Carta base){
    int ok = destino_vazio_valido(destino, r);
    if (ok) ok = destino_naipe_valido(destino, r, base);
    if (ok) ok = destino_cor_valida(destino, r, base);
    return ok;
}

// verifica se a regra exige que o destino já tenha cartas
// usado em comparações de valor com o topo do destino
// auxiliar
int exige_destino_com_carta(RegraMovimento *r){
    int exige = 0;
    if (flag_existe(r->flags, '<')) exige = 1;
    if (flag_existe(r->flags, '>')) exige = 1;
    if (flag_existe(r->flags, '~')) exige = 1;
    return exige;
}

// compara o valor da carta base com a carta do topo do destino
// valida relações de inferioridade, superioridade ou adjacência
// auxiliar
int comparar_valor_destino(PilhaCartas *destino, RegraMovimento *r, Carta base){
    int ok = 1;
    if (flag_existe(r->flags, '<')) ok = valor_inferior(base, carta_topo(destino));
    if (ok && flag_existe(r->flags, '>')) ok = valor_superior(base, carta_topo(destino));
    if (ok && flag_existe(r->flags, '~')) ok = valores_adjacentes(base, carta_topo(destino));
    return ok;
}


// compara o valor da carta base com a carta do topo do destino
// valida relações de inferioridade, superioridade ou adjacência
// auxiliar
int verificar_flags_valor(PilhaCartas *destino, RegraMovimento *r, Carta base){
    int ok = 1;
    if (destino->n_cartas == 0 && exige_destino_com_carta(r)) ok = 0;
    if (ok) ok = comparar_valor_destino(destino, r, base);
    return ok;
}

// valida todas as flags associadas a uma regra de movimento
// verifica grupo movido, restrições do destino e relações de valor
// função principal de validação de flags
int regra_flags_validas(EstadoJogo *estado, RegraMovimento *r, Movimento m){

    PilhaCartas *origem  = &estado->pilhas[m.origem].pilha;
    PilhaCartas *destino = &estado->pilhas[m.destino].pilha;
    Carta base = carta_base_movimento(origem, m.n_cartas);

    // grupo movido: flags são  < > m x d a k
    int ok = regras_grupo_movido(origem, r, m.n_cartas);

    if (flag_existe(r->flags, '*')) return 1; //se existe a flag universal, então o movimento é válido, independente das próximas verificações.

    // flags destino: V X c D
    if (ok) ok = verificar_flags_destino(destino, r, base);

    // flags valor: < > =
    if (ok) ok = verificar_flags_valor(destino, r, base);

    return ok;
}


// 6: EXECUÇÃO DE MOVIMENTOS E VITÓRIA

// verifica se existe alguma regra válida que permita um determinado movimento m.
// percorre todas as regras de movimento num Estado (cada regra é uma linha dsl) e, se alguma regra for compatível com o tipo de movimento que queremos, então ok = 1.
// auxiliar
int existe_regra_valida(EstadoJogo *estado, Movimento m){ 
    int i;
    int ok = 0;
    PilhaJogo *origem = &estado->pilhas[m.origem];
    PilhaJogo *destino = &estado->pilhas[m.destino];

    for (i = 0; i < estado->definicao.n_movimentos; i++){ //percorre as regras de movimento
        if (!ok && regra_tipos_compativeis(&estado->definicao.movimentos[i], origem, destino) && pode_mover_varias(&estado->definicao.movimentos[i], m)){ 
            // esse ciclo so para quando encontra uma regra que já o valide.

            ok = regra_flags_validas(estado, &estado->definicao.movimentos[i], m);

            //Exemplo: tipos compativeis? sim -> quantidade de sequência permitida? sim -> então vai recorrer à tal flag, o mov só é valido se for compatível com essa flag final
    }
}

    return ok; // devolve 1 se encontrou alguma regra válida, senão 0
}

// verifica se um movimento é totalmente válido
// primeiro faz a validação básica e depois verifica se alguma regra do jogo o permite
// funcao principal -> utiliza as auxiliares acima
int movimento_e_valido(EstadoJogo *estado, Movimento m){
    int ok = verificacao_basica_movimento(estado, m); // se o ok já falhar as verificacoes basicas entao o movimento já deve ser invalido

    if (ok)
        ok = existe_regra_valida(estado, m); // só testa regras detalhadas se a validação básica tiver passado

    return ok; // devolve o resultado final
}

// para evitar ter de pedir ao jogador o numero de cartas que quer mover todas as jogadas, esta função determina automaticamente o número máximo de cartas que podem ser movidas
// começa por tentar mover 1 carta e vai aumentando até encontrar um movimento válido.
int descobrir_numero_cartas(EstadoJogo *estado, Movimento *m){ //essa funcao retorna 0 ou 1 so pra ser chamada depois no ciclo_jogo
    int n;
    int total = estado->pilhas[m->origem].pilha.n_cartas; // total de cartas disponíveis na origem

    for (n = 1; n <= total; n++){
        m->n_cartas = n;
        if (movimento_e_valido(estado, *m)) return 1; // encontrou uma quantidade válida
    }

    return 0; // nenhuma quantidade de cartas resulta num movimento válido

    // EXEMPLO: pilha de origem tem [AC, 2H, 3D, 4S, 5C] (5C no topo) e a regra exige flag 'a' (tem que ter ás no limite do grupo movido)
    //    n=1 -> move só 5C -> não tem Ás, invalida o mov
    //    n=2 -> move 4S+5C -> não tem Ás, invalida o mov
    //    ...
    //    n=5 -> move AC+2H+3D+4S+5C -> AC é a base do grupo -> tem ás, vai retornar 1
}


// move efetivamente cartas da origem para o destino
// retira as cartas e volta a colocá-las (de trás pra frente porque o ultimo elemento do array é a carta do topo e ela é a carta que é removida primeiro)
void mover_cartas(EstadoJogo *estado, Movimento m){
    Carta aux[MAX_CARTAS_TOTAL]; // array temporário usado para preservar a ordem das cartas movidas
    int i;

    for (i = m.n_cartas - 1; i >= 0; i--)
        aux[i] = remover_carta(&estado->pilhas[m.origem].pilha);

    for (i = 0; i < m.n_cartas; i++)
        adicionar_carta(&estado->pilhas[m.destino].pilha, aux[i]);


        //EXEMPLO: [4,5,7,8,K] -> queremos mover as 3 ultimas, nesta ordem: 7,8,K.
        //Vamos criar o array temporário de trás pra frente, removendo da pilha de origem: aux[2] = K, aux[1] = 8, aux[0] = 7
        // E depois vamos adicionar essas cartas em ordem normal, adicionando primeiro aux[0]... etc

}

// tenta executar um movimento pedido pelo jogador
void executar_movimento(EstadoJogo *estado, Movimento m){
    if (movimento_e_valido(estado, m)){
        mover_cartas(estado, m);
        printf("Movimento realizado.\n");
    } else{
        printf("Movimento invalido.\n");
    }
}

// verifica se uma regra de vitória foi cumprida
// soma todas as cartas das pilhas do tipo indicado e compara com o valor esperado
// auxiliar
int regra_vitoria_cumprida(EstadoJogo *estado, RegraVitoria *r){
    int i, contador = 0;

    for (i = 0; i < estado->n_pilhas; i++){
        if (strcmp(estado->pilhas[i].tipo, r->tipo) == 0) contador += estado->pilhas[i].pilha.n_cartas;
            }

    if (contador == r->n_cartas) return 1;
    else return 0;

    // EXEMPLO: FreeCell tem WIN TAB 0 e WIN CELL 0. Para a regra WIN TAB 0: percorre todas as pilhas à procura do tipo TAB e soma as cartas. Se o total for 0 (todas as TAB vazias), retorna 1. O jogo só está ganho quando AMBAS as regras forem satisfeitas TAB com 0 cartas E CELL com 0 cartas, o que significa que todas as 52 cartas foram movidas para as FUND.
}

// verifica se o jogo foi ganho
// o jogo só termina com vitória quando todas as regras de vitória forem satisfeitas
int jogo_ganho(EstadoJogo *estado){
    int i;
    int ganhou = 1;

    for (i = 0; i < estado->definicao.n_vitorias; i++)
        if (!regra_vitoria_cumprida(estado, &estado->definicao.vitorias[i]))
            ganhou = 0; // se uma única condição falhar, o jogo ainda não foi ganho

    return ganhou; // todas as condições foram cumpridas
}

// 7: AUTOMÁTICOS + LOOP DO JOGO

// funcao separada pra remover instruções do McCabe
void preparar_movimento_auto(Movimento *m, int origem, int destino){
    m->origem = origem;
    m->destino = destino;
    m->n_cartas = 1;
}


// FUNÇÕES EM CADEIA:

//1. Fixa uma origem e procura um destino automático
int tentar_destinos_auto(EstadoJogo *estado, int regra, int origem){
    int destino, feito = 0;
    Movimento m;

    for (destino = 0; destino < estado->n_pilhas; destino++){
        preparar_movimento_auto(&m, origem, destino);
        if (!feito && regra_tipos_compativeis(&estado->definicao.automaticos[regra], &estado->pilhas[origem], &estado->pilhas[destino]) && movimento_e_valido(estado, m)){
            mover_cartas(estado, m);
            printf("Auto: Pilha %d para %d.\n", origem + 1, destino + 1);
            feito = 1;
        }
    }
    return feito;
}

//2. Fixa uma regra e percorre todos as origens à procura de um destino
int tentar_origens_auto(EstadoJogo *estado, int regra){
    int origem;
    int feito = 0;

    for (origem = 0; origem < estado->n_pilhas; origem++)
        if (!feito) feito = tentar_destinos_auto(estado, regra, origem);
    return feito;
}

//3. Finalmente, procura um movimento automático percorrendo todas as regras.
int tentar_movimento_automatico(EstadoJogo *estado){
    int regra;
    int feito = 0;

    for (regra = 0; regra < estado->definicao.n_automaticos; regra++)
        if (!feito) feito = tentar_origens_auto(estado, regra);
    return feito;
}
//---------------------------------------------
// 8: SAVE E LOAD

// converte o valor (inteiro do enum) para o char correspondente. Inverso de str_para_valor.
// auxiliar
void valor_para_str(int valor, char str[]){ //tem que ser valor para string por causa do 10 que são "dois caracteres" e nao cabe num unico char.
    if (valor == A) strcpy(str, "A");
    else if (valor == J) strcpy(str, "J");
    else if (valor == Q) strcpy(str, "Q");
    else if (valor == K) strcpy(str, "K");
    else sprintf(str, "%d", valor + 1); //valor +1 por causa do enum, que começa em 0.
}

// dado um char valor da carta, converte-o para o inteiro do enum correspondente. Inverso de valor_para_str.
// auxiliar
int str_para_valor(char str[]){ //dada uma string (ou um caracter), devolve o número que está representado. Utilizamos a função atoi para passar de caractere para inteiro.
    if (strcmp(str, "A") == 0) return A;
    if (strcmp(str, "J") == 0) return J;
    if (strcmp(str, "Q") == 0) return Q;
    if (strcmp(str, "K") == 0) return K;
    return atoi(str) - 1; //-1 por causa do enum.
}


// converte o naipe (inteiro do enum) para o char correspondente. Inverso de char_para_naipe.
// auxiliar
char naipe_para_char(int naipe){
    if (naipe == COPAS)   return 'H';
    if (naipe == ESPADAS) return 'S';
    if (naipe == OUROS)   return 'D';
    return 'C';
}

// dado um char de naipe, devolve o inteiro do enum correspondente. Inverso de naipe_para_char.
// auxiliar
int char_para_naipe(char c){
    if (c == 'H') return COPAS;
    if (c == 'S') return ESPADAS;
    if (c == 'D') return OUROS;
    return PAUS;
}


// escreve uma carta no ficheiro, por exemplo "10H", "AS", "KC".
// auxiliar
void guardar_carta(FILE *f, Carta c){
    char val[3]; //10 + barra zero = array com 3 elementos 
    valor_para_str(c.valor, val);
    fprintf(f, "%s%c", val, naipe_para_char(c.naipe)); 
}

// guarda todas as cartas de uma pilha numa linha, separadas por espaços. Exemplo: "2C 7D 10H AS"
void guardar_pilha(FILE *f, PilhaJogo *p){
    int i;
    for (i = 0; i < p->pilha.n_cartas; i++){
        if (i > 0) fprintf(f, " "); // para separar as cartas com um espaço
        guardar_carta(f, p->pilha.cartas[i]);
    }
    fprintf(f, "\n");
}

// percorre todas as pilhas do estado e guarda cada uma numa linha do ficheiro.
void guardar_todas_pilhas(FILE *f, EstadoJogo *estado){
    int i;
    for (i = 0; i < estado->n_pilhas; i++)
        guardar_pilha(f, &estado->pilhas[i]);
}

// guarda o estado do jogo num ficheiro de texto
// devolve 1 se conseguiu guardar, 0 se o ficheiro não pôde ser criado.
int guardar_estado(EstadoJogo *estado, char nome_ficheiro[]){
    FILE *f = fopen(nome_ficheiro, "w"); //tem que ser w para criar (write / escrever) o ficheiro. Se fosse r, seria de ler.
    if (f == NULL) return 0;
    fprintf(f, "%s\n", estado->definicao.nome);

    guardar_todas_pilhas(f, estado);

    fclose(f);
    return 1;
}

// dada uma string de carta separada ("10H", "AS", por exemplo), preenche a struct Carta com o valor e naipe correspondentes
// devolve 1 se a carta é válida, 0 caso contrário.
int ler_carta_str(char cartinhaSeparada[], Carta *c){
    int cartaCaracteres = strlen(cartinhaSeparada); //um exemplo de carta separada seria, no meio de uma string, 10S, AC, KH, etc

    char val_str[3];
    char naipe_char;

    if (cartaCaracteres < 2) return 0; //uma carta separada sempre preicsa de pelo menos 2 caracteres (numero + naipe)

    strncpy(val_str, cartinhaSeparada, cartaCaracteres - 1); //tamanho - 1 exclui o barra zero

    val_str[cartaCaracteres - 1] = '\0'; //mete um barra zero para excluir o naipe

    naipe_char = cartinhaSeparada[cartaCaracteres - 1]; //o naipe é o ultimo caracter da cartinhaSeparada

    c->valor = str_para_valor(val_str); //o valor da carta que queremos ler vai ser convertido de str / caractere para inteiro
    c->naipe = char_para_naipe(naipe_char); //o naipe da carta que queremos ler vai ser convertido de char para inteiro

    return 1;
}

// dada uma linha de texto com cartas separadas por espaços (2C 7D 10H, por exemplo), reconstrói a pilha correspondente.
void ler_pilha_linha(char linha[], PilhaJogo *p){
    char cartinhaSeparada[4];
    int offset = 0, lidos;
    Carta c;

    p->pilha.n_cartas = 0;
    lidos = sscanf(linha + offset, "%3s", cartinhaSeparada); //sscanf retorna o número de caracteres lidos na string linha, neste caso, o numero de cartinhas separadas que foram lidas.
    //este lidos lê carta separada por carta separada até o loop acabar, e acaba quando não ler mais nada (ou seja quando o sscanf retorna 0)

    while (lidos == 1){
        if (ler_carta_str(cartinhaSeparada, &c)) adicionar_carta(&p->pilha, c);

        offset += strlen(cartinhaSeparada) + 1; // offset faz começar a ler a string a partir de um outro espaço. Lê separadamente as palavras.

        lidos = sscanf(linha + offset, "%3s", cartinhaSeparada);
    }
}


// lê o estado do jogo a partir de um ficheiro de texto
// devolve 1 se conseguiu carregar, 0 se o ficheiro não existe ou não pôde ser aberto.
int carregar_estado(EstadoJogo *estado, char nome_ficheiro[]){
    FILE *f = fopen(nome_ficheiro, "r"); //aqui é r para ler
    char linha[TAMANHO_LINHA];
    int i;

    if (f == NULL) return 0;
    fgets(linha, sizeof(linha), f); //pega a linha e não faz nada nela. Como cada linha termina com um newline char, o ponteiro vai passar pra linha de baixo que realmente interessa.

    for (i = 0; i < estado->n_pilhas; i++){
        if (fgets(linha, sizeof(linha), f) != NULL) ler_pilha_linha(linha, &estado->pilhas[i]); //se a linha nao tiver vazia, vamos ler a pilha e pô-la no jogo.
    }
    fclose(f);
    return 1;
}


// executa uma jogada do utilizador e depois aplica movimentos automáticos
// auxiliar do ciclo principal
void executar_jogada_utilizador(EstadoJogo *estado, Movimento m){
    executar_movimento(estado, m);
    tentar_movimento_automatico(estado);
}

// trata os comandos de save (origem == -2) e load (origem == -3)
// devolve 1 se o comando foi executado com sucesso, 0 caso contrário
int comando_especial(EstadoJogo *estado, Movimento m){
    int fazer = 1;

    if (m.origem == -2) fazer = guardar_estado(estado, "save.txt");
    else if (m.origem == -3) fazer = carregar_estado(estado, "save.txt");
    else fazer = 0;

    return fazer;
}

// trata do user input no caso de este for save ou load
void tratar_comando_especial(EstadoJogo *estado, Movimento m){
    if (comando_especial(estado, m)) printf("Comando realizado.\n");
    else printf("Comando invalido.\n");
}

// devolve 1 se o jogo deve parar (vitória), 0 caso contrário.
int deve_parar_jogo(EstadoJogo *estado){
    int parar = 0;

    if (jogo_ganho(estado)) parar = 1;

    return parar;
}

// se o movimento for undo (origem == -1) e existir estado anterior, restaura esse estado
// devolve 1 se o undo foi feito, 0 caso contrário.
int tratar_undo(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior, Movimento m){
    int tratado = 0;

    if (m.origem == -1 && *tem_estado_anterior){
        *estado = *estado_anterior;
        tratado = 1;
    }

    return tratado;
}

// tenta executar um movimento normal: se válido, guarda o estado anterior para undo e executa a jogada
// devolve 1 se o movimento foi executado, 0 caso contrário.
int tratar_movimento_normal(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior, Movimento m){
    int tratado = 0;

    if (descobrir_numero_cartas(estado, &m)){
        *estado_anterior = *estado; //atualiza o estado anterior pra poder fazer undo
        *tem_estado_anterior = 1;
        executar_jogada_utilizador(estado, m);
        tratado = 1;
    }

    return tratado;
}

// executa um único passo do ciclo principal do jogo
// imprime o estado, recebe jogada do utilizador e processa o movimento
// auxiliar
int passo_ciclo_jogo(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior){
    Movimento m;
    int ativo = 1;

    imprimir_estado(estado);
    if (deve_parar_jogo(estado)) ativo = 0;
    if (ativo) m = pedir_movimento();
    if (ativo && m.origem < -1) tratar_comando_especial(estado, m);
    else if (ativo && !tratar_undo(estado, estado_anterior, tem_estado_anterior, m))
        if (!tratar_movimento_normal(estado, estado_anterior, tem_estado_anterior, m))
            printf("Movimento invalido.\n");

    return ativo;
}

// ciclo principal do jogo
// repete a sequência de imprimir estado, pedir jogada e executar até o jogo terminar
void ciclo_jogo(EstadoJogo *estado){
    EstadoJogo estado_anterior;
    int tem_estado_anterior = 0;
    int ativo = 1;

    tentar_movimento_automatico(estado);
    while (ativo)
        ativo = passo_ciclo_jogo(estado, &estado_anterior, &tem_estado_anterior);
    if (jogo_ganho(estado)) printf("Ganhaste!\n");
}
