#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAPLINES 30
#define MAPCOLUMNS 60
#define MAX_INIMIGOS 15


typedef struct Inimigo_t
{
    Vector2 posInimigo;//Vector2 eh um struct com float x e y, util para armazenar posicao
    int tipoInimigo;
    int HP;
    bool ativo;



}INIMIGO;

typedef struct Player_t
{
    Vector2 posplayer;
    float HP ;
    int moveSpeed;
    float dmg;
    int bombAmount;


}PLAYER;
/* CarregaMapa recebe o numero da fase desejada e carrega o arquivo correspondente ao nivel desejado.
A funcao usa sprintf para transformar o int levelnumber recebido em uma string, e 12 eh tamanho o suficiente para qualquer int
A funcao entao concatena strings para gerar uma string no formato "Level[levelNumber].txt" para poder abrir o arquivo de Level na pasta do jogo
O arquivo de level precisa comecar com Level, ter letra maiuscula, ter um int depois do Level e ser um arquivo txt
Esse arquivo precisa ter MAPLINES linhas e MAPCOLUMNS colunas para a funcao funcionar direito
A partir do arquivo determina a posicao inicial do jogador, inimigos e outros elementos do mapa, armazenando essas informacoes em uma matriz
com MAPLINES linhas e MAPCOLUMNS colunas
*/
void CarregaMapa(int levelNumber, char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS],PLAYER *player, INIMIGO Inimigos[],int *InimigosNaFase)
{
    char level_N_aux[12];
    char levelName[20] = "Level";
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
            if(CurrentLevelMatrix[iMap][jMap]=='J')
            {
                player->posplayer.x = iMap;
                player->posplayer.y = jMap;
            }
            if(CurrentLevelMatrix[iMap][jMap]=='I')
            {
                Inimigos[*InimigosNaFase].posInimigo.x = iMap;
                Inimigos[*InimigosNaFase].posInimigo.y = jMap;
                Inimigos[*InimigosNaFase].tipoInimigo = 1;
                Inimigos[*InimigosNaFase].HP = 5;
                Inimigos[*InimigosNaFase].ativo = true;
                *InimigosNaFase+=1;
            }
        }

    }
    //printf("%f", player->posplayer.x);
    fclose(mapaLevel);
    return;

}
int main()
{
    InitWindow(800,800, "trabalho");
    SetTargetFPS(30);
    int iMapRender,jMapRender;
    int InimigosNaFase = 0;
    char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS]; //definicao da matriz para passar para a funcao CaregaMapa
    PLAYER jogador; //criacao do jogador
    INIMIGO Inimigos[MAX_INIMIGOS];
    CarregaMapa(1,CurrentLevelMatrix,&jogador,Inimigos,&InimigosNaFase);
    printf("%d",InimigosNaFase);

    while(!WindowShouldClose())
    {
        BeginDrawing();

        EndDrawing();

    }

}
