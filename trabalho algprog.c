#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAPLINES 30
#define MAPCOLUMNS 60
#define MAX_INIMIGOS 15
#define MAX_OBJECTS 999
#define TURQUOISE (Color){0,159,150,255}
#include "raymath.h"
#include <time.h>
/* declaracao de variaveis */



typedef struct Objetos_Estaticos_t
{
    Vector2 posObjeto;
    int tipoObjeto;
    bool ativo;
} OBJETO_ESTATICO;

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
    int HP ;
    int dmg;
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



} STATUS;

STATUS status_jogo_atual;
Texture2D texturaTitle;
char titleText[] = "haur";
Font fonteTitle;
int tamanhotitle;
bool GameStart = false;
Texture2D sapo;


/* A funcao SalvaJogo recebe o ponteiro para o o struct STATUS atual, e armazena as informacoes relevantes em um arquivo com uma estrutura semelhante aos arquivos de mapa,
mas com informacoes adicionais nas linhas posteriores */
void SalvaJogo(STATUS *s)
{
    FILE* savegame;
    savegame = fopen("JogoSalvo.txt","w");
    for(int i=0;i<MAPLINES;i++)
    {
        for(int j=0;j<MAPCOLUMNS+1;j++)
        {   if(j<MAPCOLUMNS)
            fprintf(savegame,"%c",s->CurrentLevelMatrix[i][j]);
            if(j==MAPCOLUMNS)
            fprintf(savegame,"%c",'\n');

        }
    }
    fprintf(savegame,"%d\n%d\n%.2f\n%d",s->player.HP,s->player.bombAmount,s->tempo_restante,s->mapaAtual);
    fclose(savegame);



}
/* A funcao Carregajogo recebe o ponteiro para o struct STATUS atual, abre o arquivo JogoSalvo.txt, le as informacoes relevantes para o jogo e as armazena no struct */
void CarregaJogo(STATUS *s)
{
    FILE* savegame;
    savegame = fopen("JogoSalvo.txt","r");
    for(int i=0;i<MAPLINES;i++)
    {
        for(int j=0;j<MAPCOLUMNS+1;j++)
        {
            s->CurrentLevelMatrix[i][j]=fgetc(savegame);
        }

    }
    fscanf(savegame,"%d\n%d\n%.2f\n%d",&s->player.HP,&s->player.bombAmount,&s->tempo_restante,&s->mapaAtual);
    fclose(savegame);
    GameStart = true;

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

void JogoRenderer(STATUS *s)
{
    DrawRectangleV(Vector2Scale(s->player.posplayer,15),(Vector2){30,30},GREEN);
    DrawTextureEx(sapo,Vector2Scale(s->player.posplayer,30),0,1,WHITE);
    for(int i=0;i<MAPLINES;i++)
    {
        for(int j=0;j<MAPCOLUMNS;j++)
        {
            if(s->CurrentLevelMatrix[i][j]=='#')
            {
                DrawRectangle(j*15,i*15,15,15,BROWN);
            }
        }
    }
    DrawRectangle(0,450,900,300,RED);
    //DrawTextEx()

}
/* CarregaMapa recebe o numero da fase desejada e carrega o arquivo correspondente ao nivel desejado.
A funcao usa Textformat junta o numero do level com mapa e .txt para carregar o mapa, substituindo a necessidade de diversas concatenacoes de strings e sprintf
O arquivo de level precisa comecar com mapa, ter letra minuscula, ter um int depois do mapa e ser um arquivo txt
Esse arquivo precisa ter MAPLINES linhas e MAPCOLUMNS colunas para a funcao funcionar direito
A partir do arquivo determina a posicao inicial do jogador, inimigos e outros elementos do mapa, armazenando essas informacoes em uma matriz
com MAPLINES linhas e MAPCOLUMNS colunas
*/
void CarregaMapa(int levelNumber, char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS],PLAYER *player, INIMIGO Inimigos[],int *InimigosNaFase)
{
    char *levelName = TextFormat("mapa%d.txt",levelNumber);
    int contadorInimigos = 0;
    puts(levelName);
    //teste !!!!!!!!
    FILE* mapaLevel = fopen(levelName,"r");
    *InimigosNaFase=0;
    for(int iMap=0; iMap<MAPLINES; iMap++)
    {
        for(int jMap=0; jMap<MAPCOLUMNS+1; jMap++)
        {

            CurrentLevelMatrix[iMap][jMap] = fgetc(mapaLevel);

            switch(CurrentLevelMatrix[iMap][jMap])
            {
            case 'J':
                player->posplayer.x = jMap;
                player->posplayer.y = iMap;
                break;
            case 'I':
                if(*InimigosNaFase<=MAX_INIMIGOS)
                {
                    Inimigos[*InimigosNaFase].posInimigo.x = jMap;
                    Inimigos[*InimigosNaFase].posInimigo.y = iMap;
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


/* A funcao NovoJogo recebe um struct STATUS, criado ao inicializar o jogo, que armazena as principais informacos sobre o jogo, e confere a ele os valores padrao de um novo jogo,
tambem coloca a bool GameStart como true, para o jogo sair do menu principal e comecar */

void NovoJogo(STATUS *s)
{
    s->player.HP = 5;
    s->player.bombAmount=3;
    s->player.dmg = 1;
    s->mapaAtual=1;
    CarregaMapa(s->mapaAtual,s->CurrentLevelMatrix,&s->player,s->Inimigos,&s->InimigosNaFase);
    GameStart = true;
    return;

}


/* A funcao Menu recebe um int type, 0 para o menu principal e 1 para o menu dentro do jogo, limitando as opcoes dependendo do contexto.
Dependendo da tecla apertada realiza outras funcoes de manipulacao do estado do jogo */
void Menu(int type)
{
    if(IsKeyPressed(KEY_N))
    {
        NovoJogo(&status_jogo_atual);
    }
    if(IsKeyPressed(KEY_S)&&type==1)
    {
        SalvaJogo(&status_jogo_atual);
    }
    if(IsKeyPressed(KEY_Q))
    {
        CloseWindow();
    }
    if(IsKeyPressed(KEY_C))
    {
        CarregaJogo(&status_jogo_atual);
    }



}
/* A funcao TitleScreen renderiza o fundo e outros elementos da tela de titulo, e chama a fncao Menu com o type 0, limitando as opcoes disponiveis para
Novo Jogo, Carregar Jogo ou Sair */
void TitleScreen()
{
    DrawTexture(texturaTitle,0,0,WHITE);
    DrawTextEx(fonteTitle,"Froggers frog frog",(Vector2){202,150},60,0,BLACK);
    DrawTextEx(fonteTitle,"Froggers frog frog",(Vector2){205,153},60,0,TURQUOISE);
    DrawTextEx(fonteTitle,"Novo Jogo - N",(Vector2){222,447},60,0,BLACK);
    DrawTextEx(fonteTitle,"Novo Jogo - N",(Vector2){225,450},60,0,TURQUOISE);
    DrawTextEx(fonteTitle,"Carregar Jogo - C",(Vector2){222,547},60,0,BLACK);
    DrawTextEx(fonteTitle,"Carregar Jogo - C",(Vector2){225,550},60,0,TURQUOISE);
    DrawTextEx(fonteTitle,"Sair - Q",(Vector2){222,647},60,0,BLACK);
    DrawTextEx(fonteTitle,"Sair - Q",(Vector2){225,650},60,0,TURQUOISE);
    Menu(0);






}
int main()
{
    InitWindow(900,750, "trabalho");
    SetTargetFPS(30);
    texturaTitle = LoadTexture("TitleScreen.png");
    fonteTitle = LoadFontEx("SunnyspellsRegular-MV9ze.otf",60,NULL,0);
    sapo=LoadTexture("sapo.png");
    //CarregaMapa(1,CurrentLevelMatrix,&jogador,Inimigos,&InimigosNaFase);
    printf("%d",status_jogo_atual.InimigosNaFase);
    printf(" \n %d",tamanhotitle);
    time_t tempo_inicio =time(NULL);
    int tempoAtual;

    while(!WindowShouldClose())
    {
         while(!GameStart)
         {
            BeginDrawing();
            ClearBackground(WHITE);
            TitleScreen();
            EndDrawing();

         }
         Menu(1);

        BeginDrawing();
        ClearBackground(WHITE);
        tempoAtual = time(NULL)-tempo_inicio;
        JogoRenderer(&status_jogo_atual);
        DrawText(TextFormat("Tempo:%d s",tempoAtual),450,450,50,BLACK);
        DrawText(TextFormat("Vida:%d",status_jogo_atual.player.HP),450,550,50,BLACK);
        DrawText(TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount),450,500,50,BLACK);

        EndDrawing();


    }

}
