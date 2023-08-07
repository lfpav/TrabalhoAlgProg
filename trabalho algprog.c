#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAPLINES 30
#define MAPCOLUMNS 60
#define MAX_INIMIGOS 15
#define MAX_OBJECTS 999


typedef struct Objetos_Estaticos_t
{
    Vector2 posObjeto;
    int tipoObjeto;
    bool ativo;
}OBJETO_ESTATICO;

typedef struct Inimigo_t
{
    Vector2 posInimigo;//Vector2 eh um struct com float x e y, util para armazenar posicao
    int tipoInimigo;
    int HP;
    bool ativo;



} INIMIGO;

typedef struct Player_t
{
    Vector2 posplayer;
    float HP ;
    float dmg;
    int bombAmount;


} PLAYER;
typedef struct Status_Jogo_t
{
    PLAYER player;
    INIMIGO Inimigos[MAX_INIMIGOS];
    char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS];
    OBJETO_ESTATICO Bombas[MAX_OBJECTS];
    OBJETO_ESTATICO Armadilhas[MAX_OBJECTS];
    OBJETO_ESTATICO Portal;
    float tempo_restante;
    int mapaAtual;
    int InimigosNaFase;



}STATUS;
/* CarregaMapa recebe o numero da fase desejada e carrega o arquivo correspondente ao nivel desejado.
A funcao usa sprintf para transformar o int levelnumber recebido em uma string, e 12 eh tamanho o suficiente para qualquer int
A funcao entao concatena strings para gerar uma string no formato "mapa[levelNumber].txt" para poder abrir o arquivo de Level na pasta do jogo
O arquivo de level precisa comecar com mapa, ter letra minuscula, ter um int depois do mapa e ser um arquivo txt
Esse arquivo precisa ter MAPLINES linhas e MAPCOLUMNS colunas para a funcao funcionar direito
A partir do arquivo determina a posicao inicial do jogador, inimigos e outros elementos do mapa, armazenando essas informacoes em uma matriz
com MAPLINES linhas e MAPCOLUMNS colunas
*/

void SalvaJogo()
{

}

void CarregaJogo()
{

}



void PausaJogo()
{
    //tecla esc

}
void DespausaJogo()
{

}
int PodeMover(PLAYER *player)
{


}
int PodeMoverInimigo()
{

}
void Mover()
{

}
void CarregaMapa(int levelNumber, char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS],PLAYER *player, INIMIGO Inimigos[],int *InimigosNaFase)
{
    char level_N_aux[12];
    char levelName[20] = "mapa";
    int contadorInimigos = 0;
    sprintf(level_N_aux,"%d",levelNumber);
    strcat(level_N_aux,".txt");
    strcat(levelName,level_N_aux);
    puts(levelName);
    FILE* mapaLevel = fopen(levelName,"r");
    *InimigosNaFase=0;
    for(int iMap=0; iMap<MAPLINES; iMap++)
    {
        for(int jMap=0; jMap<MAPCOLUMNS; jMap++)
        {
            CurrentLevelMatrix[iMap][jMap] = fgetc(mapaLevel);
            switch(CurrentLevelMatrix[iMap][jMap])
            {
            case 'J':
                player->posplayer.x = iMap;
                player->posplayer.y = jMap;
                break;
            case 'I':
                if(*InimigosNaFase<=MAX_INIMIGOS)
                {
                    Inimigos[*InimigosNaFase].posInimigo.x = iMap;
                    Inimigos[*InimigosNaFase].posInimigo.y = jMap;
                    Inimigos[*InimigosNaFase].tipoInimigo = 1;
                    Inimigos[*InimigosNaFase].HP = 5;
                    Inimigos[*InimigosNaFase].ativo = true;
                    *InimigosNaFase+=1;
                    break;
                }
            default:
                break;

            }
        }

    }
    //desativa os inimigos excedentes ao numero de inimigos na fase
    for (int i = *InimigosNaFase+1; i<MAX_INIMIGOS; i++)
    {
        Inimigos[i].ativo = false;

    }
    //printf("%f", player->posplayer.x);
    fclose(mapaLevel);
    return;

}

void NovoJogo(STATUS *s)
{
  s->player.HP = 5;
  s->player.bombAmount=3;
  s->player.dmg = 1;
  s->mapaAtual=1;
  CarregaMapa(s->mapaAtual,s->CurrentLevelMatrix,&s->player,s->Inimigos,&s->InimigosNaFase);
  return;

}
int main()
{
    InitWindow(800,800, "trabalho");
    SetTargetFPS(30);
    int iMapRender,jMapRender;
    STATUS status_jogo_atual;
    NovoJogo(&status_jogo_atual);
    //CarregaMapa(1,CurrentLevelMatrix,&jogador,Inimigos,&InimigosNaFase);
    printf("%d",status_jogo_atual.InimigosNaFase);

    while(!WindowShouldClose())
    {
        BeginDrawing();

        EndDrawing();

    }

}
