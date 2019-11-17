#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "player.h"
#include "enum.h"

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (NICK_SIZE + 100)
#define MAX_CLIENTS 2

void insereJogador(Jogador *jogadores, char *nick, int id, short *numJogadores)
{
    //TODO adicionar o jogador ao vetor de jogadores
    Jogador e = iniciaJogador(nick, id);
    jogadores[id] = e;
    (*numJogadores)++;
    return;
}

void enviaInimigo(Jogador *jogadores, int totalJogadores, char *estado)
{
    if (totalJogadores == MAX_CLIENTS)
    {
        //TODO envia dados do inimigo ao cliente
        Jogador_inimigo inimigo;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int id_cliente_inimigo = (i == 0 ? 1 : 0);
            inimigo.pos = jogadores[i].pos;
            inimigo.saude = jogadores[i].saude;
            strcpy(inimigo.nick, jogadores[i].nick);
            sendMsgToClient((Jogador_inimigo *)&inimigo, sizeof(Jogador_inimigo), id_cliente_inimigo);
        }

        *estado = JOGANDO;
    }
}

int main()
{
    //Vetor que armazena os jogadores no jogo
    Jogador *jogadores = (Jogador *)malloc(MAX_CLIENTS*sizeof(Jogador));

    //Matriz que armazena os clientes presente no jogo (Provavelmente desnecessaria)
    char clientes[MAX_CLIENTS][NICK_SIZE];
    char estado_jogo = PREJOGO;
    //String auxiliar
    char str_buffer[BUFFER_SIZE];

    //Inicia servidor
    serverInit(MAX_CLIENTS);

    puts("Servidor está online.");

    short numJogadores = 0;
    while (1)
    {
        int id = acceptConnection();
        if (id != NO_CONNECTION)
        {
            if (numJogadores < 2)
            {
                recvMsgFromClient(clientes[id], id, WAIT_FOR_IT);
                printf("%s conectou ao jogo\n", clientes[id]);
                insereJogador(jogadores, clientes[id], id, &numJogadores);
                imprimeJogador(jogadores[id]);
                sendMsgToClient((Jogador *)&jogadores[id], sizeof(Jogador), id);
            }
        }

        char *msg = malloc(350 * sizeof(char));
        struct msg_ret_t msg_ret = recvMsg(msg);

        if (msg_ret.status == DISCONNECT_MSG)
        {
            numJogadores = numJogadores - 1;
            printf("Jogador %s do id %d desconectou.\nPosicao %d esta livre\n", jogadores[msg_ret.client_id].nick,
                   msg_ret.client_id, msg_ret.client_id);
        }

        enviaInimigo(jogadores, numJogadores, &estado_jogo);
        
        while(estado_jogo == JOGANDO)
        {
            //o jogo comeca aqui
            puts("Entrei no jogo");
            break;
        }

        //TODO resto do servidor
    }
    free(jogadores);
    return 0;
}