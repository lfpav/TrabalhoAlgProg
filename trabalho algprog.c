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
    Vector2 dirInimigo;
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
    int tempo_restante;
    int mapaAtual;
    int InimigosNaFase;

} STATUS;

STATUS status_jogo_atual;
Texture2D texturaTitle;
Font fonteTitle;
bool GameStart = false;
Texture2D sapo;
time_t tempo_inicio;
int tempo_atual;
void ArmazenaPosicoes(STATUS *s)
{
    int posiMatrix,posjMatrix;
    posiMatrix=round(s->player.posplayer.y)/15;
    posjMatrix=round(s->player.posplayer.x)/15;
    s->CurrentLevelMatrix[posiMatrix][posjMatrix]='J';
    for(int l=0;l<MAX_INIMIGOS;l++)
    {
        if(s->Inimigos[l].ativo)
        {
            posiMatrix = round(s->Inimigos[l].posInimigo.y)/15;
            posjMatrix = round(s->Inimigos[l].posInimigo.x)/15;
            s->CurrentLevelMatrix[posiMatrix][posjMatrix]='I';

        }


    }


}

/* A funcao SalvaJogo recebe o ponteiro para o o struct STATUS atual, e armazena as informacoes relevantes em um arquivo com uma estrutura semelhante aos arquivos de mapa,
mas com informacoes adicionais nas linhas posteriores */
void SalvaJogo(STATUS *s)
{
    FILE* savegame;
    ArmazenaPosicoes(s);
    savegame = fopen("JogoSalvo.txt","w");
    for(int i=0; i<MAPLINES; i++)
    {
        for(int j=0; j<MAPCOLUMNS+1; j++)
        {
            if(j<MAPCOLUMNS)
                fprintf(savegame,"%c",s->CurrentLevelMatrix[i][j]);
            if(j==MAPCOLUMNS)
                fprintf(savegame,"%c",'\n');

        }
    }
    fprintf(savegame,"%d\n%d\n%d\n%d",s->player.HP,s->player.bombAmount,tempo_atual,s->mapaAtual);
    fclose(savegame);

}







void PausaJogo()
{
    //tecla esc

}
void DespausaJogo()
{

}
int PodeMover(Vector2 *dir)
{
    if(dir->x<30||dir->x>=840||dir->y<30||dir->y>390)
    return 0;
    else
    return 1;


}
int PodeMoverInimigo()
{

}
void Mover()
{

}
void gerarDirecaoAleatoria(Vector2 *dir)
{
    int dx,dy;
    do
    {
        dx= -1 + rand() % 3;
    }
    while(dx==0);
    do
    {
        dy=-1 + rand()%3;
    }
    while(dy==0);
    dir->x=dx;
    dir->y=dy;
}
int moveInimigo(INIMIGO *Inim_ptr,int *moveDuration)
{
    Vector2 newPos;
    if(*moveDuration==0)
    {
        gerarDirecaoAleatoria(&Inim_ptr->dirInimigo);
        *moveDuration = 10 +rand()%60;
    }
    else
    {
        *moveDuration-=1;
    }
    newPos = Inim_ptr->dirInimigo;
    newPos=Vector2Add(newPos,Inim_ptr->posInimigo);
    if(!PodeMover(&newPos))
    *moveDuration=0;
    else
    {
        Inim_ptr->posInimigo.x+=Inim_ptr->dirInimigo.x*2;
        Inim_ptr->posInimigo.y+=Inim_ptr->dirInimigo.y*2;
    }
    //printf("%.5f %.5f \n",Inim_ptr->dirInimigo.y,newPos.y);

}

/* A funcao JogoRenderer desenha os elementos do jogo na tela */
void JogoRenderer(STATUS *s)
{
    DrawRectangleV(s->player.posplayer,(Vector2){30,30},GREEN);
    DrawTextureEx(sapo,Vector2Scale(s->player.posplayer,3),0,1,WHITE);
    for(int i=0; i<MAPLINES; i++)
    {
        for(int j=0; j<MAPCOLUMNS; j++)
        {
            if(s->CurrentLevelMatrix[i][j]=='#')
            {
                DrawRectangle(j*15,i*15,15,15,BROWN);
            }
        }
    }
    DrawRectangle(0,450,900,300,RED);
    for(int k=0; k<s->InimigosNaFase; k++)
    {
        DrawRectangleV(s->Inimigos[k].posInimigo,(Vector2){30,30},RED);
    }
    //DrawTextEx()

}
/* CarregaMapa recebe o numero da fase desejada e o tipo de carregamento carrega o arquivo correspondente ao nivel desejado.
Caso seja tipo 0, a funcao carrega o mapa normalmente.
Caso seja tipo 1, a funcao carrega o mapa baseado no arquivo JogoSalvo.txt
A funcao usa Textformat junta o numero do level com mapa e .txt para carregar o mapa, substituindo a necessidade de diversas concatenacoes de strings e sprintf
O arquivo de level precisa comecar com mapa, ter letra minuscula, ter um int depois do mapa e ser um arquivo txt
Esse arquivo precisa ter MAPLINES linhas e MAPCOLUMNS colunas para a funcao funcionar direito
A partir do arquivo determina a posicao inicial do jogador, inimigos e outros elementos do mapa.
Apos obter as informacoes, apaga as posicoes dos atores moveis da matriz, elas serao salvas posteriormente.
*/
//void CarregaMapa(int levelNumber, char CurrentLevelMatrix[MAPLINES][MAPCOLUMNS],PLAYER *player, INIMIGO Inimigos[],int *InimigosNaFase)
void CarregaMapa(STATUS *s,int type)
{
    char*levelName = TextFormat("mapa%d.txt",s->mapaAtual);
    int contadorInimigos = 0;
    //teste !!!!!!!!
    FILE* mapaLevel = fopen(levelName,"r");
    if(type==1)
    {
        fclose(mapaLevel);
        mapaLevel=fopen("JogoSalvo.txt","r");
    }
    s->InimigosNaFase=0;
    for(int iMap=0; iMap<MAPLINES; iMap++)
    {
        for(int jMap=0; jMap<MAPCOLUMNS+1; jMap++)
        {
            s->CurrentLevelMatrix[iMap][jMap] = fgetc(mapaLevel);
            switch(s->CurrentLevelMatrix[iMap][jMap])
            {
                case 'J':
                s->player.posplayer.x = jMap*15;
                s->player.posplayer.y = iMap*15;
                s->CurrentLevelMatrix[iMap][jMap]='\0';
                break;
                case 'I':
                if(s->InimigosNaFase<=MAX_INIMIGOS)
                {
                    s->Inimigos[s->InimigosNaFase].posInimigo.x = jMap*15;
                    s->Inimigos[s->InimigosNaFase].posInimigo.y = iMap*15;
                    s->Inimigos[s->InimigosNaFase].tipoInimigo = 1;
                    s->Inimigos[s->InimigosNaFase].HP = 5;
                    s->Inimigos[s->InimigosNaFase].ativo = true;
                    gerarDirecaoAleatoria(&s->Inimigos[s->InimigosNaFase].dirInimigo);
                    s->CurrentLevelMatrix[iMap][jMap]='\0';
                    s->InimigosNaFase+=1;

                }
                break;
                default:
                break;
            }
        }

    }
    //desativa os inimigos excedentes ao numero de inimigos na fase
    for (int i = s->InimigosNaFase+1; i<MAX_INIMIGOS; i++)
    {
        s->Inimigos[i].ativo = false;

    }
    if(type==1)
    {
        fscanf(mapaLevel,"%d\n%d\n%d\n%d\n%d",&s->player.HP,&s->player.bombAmount,&s->tempo_restante,&s->mapaAtual);

    }
    //printf("%f", player->posplayer.x);
    fclose(mapaLevel);


}


/* A funcao NovoJogo recebe um struct STATUS, criado ao inicializar o jogo, que armazena as principais informacos sobre o jogo, e confere a ele os valores padrao de um novo jogo,
tambem coloca a bool GameStart como true, para o jogo sair do menu principal e comecar */

void NovoJogo(STATUS *s)
{
    s->player.HP = 5;
    s->player.bombAmount=3;
    s->player.dmg = 1;
    s->mapaAtual=1;
    CarregaMapa(s,0);
    GameStart = true;
    s->tempo_restante=0;


}
/* A funcao Carregajogo recebe o ponteiro para o struct STATUS atual, e chama a funcao CarregaMapa com o tipo 1 para abrir o arquivo JogoSalvo.txt, le as informacoes relevantes para o jogo e as armazena no struct */
void CarregaJogo(STATUS *s)
{
    CarregaMapa(s,1);
    GameStart = true;
    tempo_inicio = time(NULL);

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
    srand(time(NULL));
    texturaTitle = LoadTexture("TitleScreen.png");
    fonteTitle = LoadFontEx("SunnyspellsRegular-MV9ze.otf",60,NULL,0);
    sapo=LoadTexture("sapo.png");
    printf("%d",status_jogo_atual.InimigosNaFase);
    int moveDuration =0;


    while(!WindowShouldClose())
    {
        while(!GameStart)
        {
            BeginDrawing();
            ClearBackground(WHITE);
            TitleScreen();
            tempo_inicio=time(NULL);
            EndDrawing();

        }
        Menu(1);

        BeginDrawing();
        ClearBackground(WHITE);
        tempo_atual = time(NULL)-tempo_inicio+status_jogo_atual.tempo_restante;
        JogoRenderer(&status_jogo_atual);
        moveInimigo(&status_jogo_atual.Inimigos[0],&moveDuration);
        DrawText(TextFormat("Tempo:%d s",tempo_atual),0,500,50,BLACK);
        DrawText(TextFormat("Vida:%d",status_jogo_atual.player.HP),0,550,50,BLACK);
        DrawText(TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount),0,600,50,BLACK);
        DrawText(TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual),0,450,50,BLACK);
        DrawText(TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase),0,700,50,BLACK);


        EndDrawing();


    }

}
