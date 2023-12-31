#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAPLINES 30
#define MAPCOLUMNS 60
#define MAX_INIMIGOS 15
#define MAX_OBJECTS 999
#define MAX_PROJETEIS 15
#define TURQUOISE (Color){0,159,150,255}
#define LIGHT_BLUE CLITERAL(Color){0,168,255,255}
#include "raymath.h"
#include <time.h>
#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

#define WindowWidth 900
#define WindowHeight 750
/* declaracao de variaveis */

typedef struct Circle_t
{
    Vector2 posCenter;
    float radius;
}Circle;

typedef struct Objetos_Estaticos_t
{
    Vector2 posObjeto;
    Rectangle objetoRec;
    Rectangle spriteSource;
    Texture2D spriteObjeto;
    int quantiaFrames;
    int frameIndex;
    float frameTime;
    int tipoObjeto;
    bool ativo;
} OBJETO_ESTATICO;
typedef struct Projetil_t
{
    Vector2 posProj;
    Circle bullet;
    float airTime;
    Vector2 dirProj;
    bool ativo;

}PROJETIL;

typedef struct Bomb_t
{
    OBJETO_ESTATICO obj;
    bool dandoDano;
}BOMB_EXPL;

typedef struct Inimigo_t
{
    Vector2 posInimigo;//Vector2 eh um struct com float x e y, util para armazenar posicao
    Vector2 dirInimigo;
    Rectangle inimigoRec;
    Color spriteColor;
    float flash_Duration;
    bool TomouDano;
    int tipoInimigo;
    float tempoChase;
    int moveDuration;
    int HP;
    float speed;
    bool ativo;
} INIMIGO;

typedef struct Player_t
{
    Vector2 posplayer;
    Vector2 dirPlayer;
    Vector2 dirShooting;
    Rectangle playerRec;
    Color spriteColor;
    float fire_Delay;
    float flash_Duration;
    bool TomouDano,vivo;
    int HP ;
    int dmg;
    int bombAmount;
    int projetilIndex;
    PROJETIL playerProj[MAX_PROJETEIS];
    BOMB_EXPL Bomb;



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
    int pontuacao;
    int InimigosNaFase;
    int bombasNaFase;
    int armadilhasNaFase;

} STATUS;

STATUS status_jogo_atual;
Texture2D texturaTitle,texturaParede;
Texture2D spriteBomba;
Texture2D spriteTrap,FundoMenus,FundoJogo;
Texture2D spritePortal,spriteBombExpl,FundoStats;
Texture2D sapoLoco,sapoRight,sapoLeft,sapoDmg01,sapoDmg02,sapoDmg03,sapoDmg04;
Sound pauseSound,selectionSound;
Sound unpauseSound,explosionSound;
Sound dmgSound,dmgSoundEnemy,deathSoundEnemy;
Sound titleTheme;
Font fonteTitle;
int moveDuration = 0;
bool jogoAtivo=true;
int invulnTime=0;
float tempo_i_chase =0;
float sizeMulti[5]={1,1,1,1,1};
float Escala = 0;
Vector2 mouse;
Vector2 virtualMouse = { 0 };
float sizeMulti2[5]={1,1,1,1,1};
float sizeMulti3[5]={1,1,1,1,1};
Rectangle RetangulosTitle[3] = {(Rectangle)
{.x=225,.y=450,.width=360,.height=60},
{.x=225,.y=550,.width=450,.height=60},
{.x=225,.y=650,.width=200,.height=60}};

Rectangle RetangulosPausa[5] = {(Rectangle)
{.x=300,.y=270,.width=300,.height=50},
{.x=275,.y=330,.width=350,.height=50},
{.x=270,.y=390,.width=390,.height=50},
{.x=255,.y=450,.width=390,.height=50},
{.x=340,.y=510,.width=220,.height=50}};

Rectangle RetangulosDeath[3] = {(Rectangle)
{.x=300,.y=315,.width=300,.height=40},
{.x=275,.y=375,.width=350,.height=40},
{.x=340,.y=435,.width=220,.height=40}};

Rectangle RetangulosConfigMenu[2] = {(Rectangle)
{.x=290,.y=423,.width=300,.height=40},
{.x=333,.y=483,.width=220,.height=40}};

Rectangle RetangulosDeSomMusica[10] ={(Rectangle)
{445, 366, 20, 35},
{462, 366, 20, 35},
{479, 366, 20, 35},
{496, 366, 20, 35},
{513, 366, 20, 35},
{530, 366, 20, 35},
{547, 366, 20, 35},
{564, 366, 20, 35},
{580, 366, 20, 35},
{597, 366, 20, 35}};

Rectangle RetangulosDeEfeitoSonoro[10] ={(Rectangle)
{445, 306, 20, 35},
{462, 306, 20, 35},
{479, 306, 20, 35},
{496, 306, 20, 35},
{513, 306, 20, 35},
{530, 306, 20, 35},
{547, 306, 20, 35},
{564, 306, 20, 35},
{580, 306, 20, 35},
{598, 306, 20, 35}};

Color cores[2] = {WHITE, BLACK};
bool PausadoConfig = false;
bool GameStart = false;
Texture2D sapo,sapoVermelho;
time_t tempo_inicio;
float tempo_atual;
float tempo_pausado;
bool Pausado = false;
float flash_Duration = 0.5;
bool TomouDano = false;
int SomMusica[10] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
float SomDaMusica = 0.20;
int SomEfeitosSonoros[10] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
float SomDosEfeitosSonoros = 1.0;

void ArmazenaPosicoes(STATUS *s)
{
    int posiMatrix,posjMatrix,checador,coords[2]={};
    posiMatrix=trunc((s->player.posplayer.y)/15);
    printf(" HAURR %d \n",posiMatrix);
    posjMatrix=trunc((s->player.posplayer.x)/15);
    printf("o char eh %c \n",s->CurrentLevelMatrix[posiMatrix][posjMatrix]);
    if(s->CurrentLevelMatrix[posiMatrix][posjMatrix]=='\0')
    {
        s->CurrentLevelMatrix[posiMatrix][posjMatrix]='J';
        printf("o char antes vazio agr eh %c\n",s->CurrentLevelMatrix[posiMatrix][posjMatrix]);


    }
    else
    {
        printf("hunga");
        for(int i=0; i<MAPLINES; i++)
        {
            if(s->CurrentLevelMatrix[i][posjMatrix]=='\0')
            {
                s->CurrentLevelMatrix[i][posjMatrix]='J';
                i=MAPLINES;
            }
            else
            {
                for(int j=0; j<MAPCOLUMNS; j++)
                {
                    if(s->CurrentLevelMatrix[i][j]=='\0')
                    {
                        s->CurrentLevelMatrix[i][j]='J';
                        i=MAPLINES;
                        j=MAPCOLUMNS;
                    }
                }
            }

        }
    }
    for(int l=0;l<MAX_INIMIGOS;l++)
    {
        if(s->Inimigos[l].ativo)
        {
            posiMatrix = trunc((s->Inimigos[l].posInimigo.y)/15);
            posjMatrix = trunc((s->Inimigos[l].posInimigo.x)/15);
            printf("y: %.2f x: %2.f \n",s->Inimigos[l].posInimigo.y,s->Inimigos[l].posInimigo.x);
            printf("y:%d x:%d\n",posiMatrix,posjMatrix);
            if(s->CurrentLevelMatrix[posiMatrix][posjMatrix]=='\0')
            {
                s->CurrentLevelMatrix[posiMatrix][posjMatrix]='I';
            }
            else
            {

                for(int i=0; i<MAPLINES; i++)
                {
                    if(s->CurrentLevelMatrix[i][posjMatrix]=='\0')
                    {
                        s->CurrentLevelMatrix[i][posjMatrix]='I';
                        i=MAPLINES;
                    }
                    else
                    {
                        for(int j=0; j<MAPCOLUMNS; j++)
                        {
                            if(s->CurrentLevelMatrix[i][j]=='\0')
                            {
                                s->CurrentLevelMatrix[i][j]='I';
                                printf("clear on attempt %d",j);

                                i=MAPLINES;
                                j=MAPCOLUMNS;
                            }
                        }
                    }
                }

            }


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
    fprintf(savegame,"%d\n%d\n%d\n%.1f\n%d",s->player.HP,s->player.bombAmount,s->mapaAtual,tempo_atual,s->pontuacao);
    fclose(savegame);

}
/* Verifica se o movimento eh possivel na direcao X atraves da colisao de um retangulo representando a posicao desejada do movimento
checa os indices em volta da posicao do jogador/inimigo para ver se possuem paredes
caso possuam, ele checa a colisao entre o retangulo da nova posicao e retangulos nas posicoes das paredes encontradas
caso haja colisao, retorna 0, n da pra mover */
int PodeMoverX(Vector2 dir, Vector2 pos, Vector2 newPos)
{
    int posiLow,posj;
    Rectangle newPosRec = (Rectangle){.x=newPos.x,.y=newPos.y,.width=30,.height=30};
   // DrawRectangleRec(newPosRec,ORANGE);
    posiLow=trunc(pos.y/15);
    posj=dir.x>0?trunc(pos.x/15)+2:trunc(pos.x/15)-1;

    for(int i=0;i<3;i++)
    {
        if(status_jogo_atual.CurrentLevelMatrix[posiLow+i][posj]=='#')
        {          //  DrawRectangleRec((Rectangle){.x=posj*15,.y=(posiLow+i)*15,.width=15,.height=15},RED);

            if(CheckCollisionRecs(newPosRec,(Rectangle){.x=posj*15,.y=(posiLow+i)*15,.width=15,.height=15}))
                return 0;

        }

    }

    return 1;


}
/* Verifica se o movimento eh possivel na direcao Y atraves da colisao de um retangulo representando a posicao desejada do movimento
checa os indices em volta da posicao do jogador/inimigo para ver se possuem paredes
caso possuam, ele checa a colisao entre o retangulo da nova posicao e retangulos nas posicoes das paredes encontradas
caso haja colisao, retorna 0, n da pra mover */
int PodeMoverY(Vector2 dir, Vector2 pos, Vector2 newPos)
{
    int posjLow,posiLow,posj,posi;
    Rectangle newPosRec = (Rectangle){.x=newPos.x,.y=newPos.y,.width=30,.height=30};
//    DrawRectangleRec(newPosRec,ORANGE);
    posjLow=trunc(pos.x/15);
    posi=dir.y>0?trunc((pos.y/15))+2:trunc(pos.y/15)-1;
    for(int i=0;i<3;i++)
    {
        if(status_jogo_atual.CurrentLevelMatrix[posi][posjLow+i]=='#')
        {
                           // DrawRectangleRec((Rectangle){.x=(posjLow+1)*15,.y=posi*15,.width=15,.height=15},RED);

            if(CheckCollisionRecs(newPosRec,(Rectangle){.x=(posjLow+i)*15,.y=posi*15,.width=15,.height=15}))
                return 0;

        }

    }

    return 1;


}

/* algoritmo de inimigo que persegue o jogador */
void inimigoPerseguePlayer(INIMIGO *Inim_ptr, PLAYER *p,float *tempo_i_chase)
 {
     Vector2 newPos;
     Vector2 not_Norm,Norm,atualDir;

     *tempo_i_chase -= GetFrameTime();
     if(*tempo_i_chase<=0)
     {
         not_Norm = Vector2Subtract(p->posplayer,Inim_ptr->posInimigo);
         Norm = Vector2Normalize(not_Norm);
         atualDir.x = Inim_ptr->dirInimigo.x;
         atualDir.y = Inim_ptr->dirInimigo.y;
         Inim_ptr->speed=0.25*(7+rand()%6);
         if(fabs(not_Norm.x)>30 )
         {
            Inim_ptr->dirInimigo.x = Norm.x;
         }

            if(fabs(not_Norm.y)>30)
         {

             Inim_ptr->dirInimigo.y = Norm.y;
         }

        *tempo_i_chase=0.1*(5+rand()%4);
     }

     newPos.x = Inim_ptr->dirInimigo.x*Inim_ptr->speed+Inim_ptr->posInimigo.x;
     newPos.y = Inim_ptr->posInimigo.y;

   if(PodeMoverX(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))
    {
        Inim_ptr->posInimigo.x+=Inim_ptr->dirInimigo.x*Inim_ptr->speed;

    }
    else
    {
        Inim_ptr->dirInimigo.x*=-1;
        *tempo_i_chase=0.1*(2+rand()%3);


    }
    newPos.y = Inim_ptr->dirInimigo.y*Inim_ptr->speed+Inim_ptr->posInimigo.y;
    newPos.x = Inim_ptr->posInimigo.x;
     if(PodeMoverY(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))
    {
        Inim_ptr->posInimigo.y+=Inim_ptr->dirInimigo.y*Inim_ptr->speed;

    }
    else
    {
        Inim_ptr->dirInimigo.y*=-1;
        *tempo_i_chase=0.1*(2+rand()%3);



    }

 }

/* verifica os inputs para definir a direcao do jogador */
void CriaProjetil(PLAYER *p)
{
       //if(p->projetilIndex>=MAX_PROJETEIS;)
        while(p->playerProj[p->projetilIndex].ativo)
        {
            p->projetilIndex+=1;
            if(p->projetilIndex>MAX_PROJETEIS-1)
            {
                p->projetilIndex=0;
            }
        }
        p->playerProj[p->projetilIndex].airTime=3;
        p->playerProj[p->projetilIndex].ativo=true;
        p->playerProj[p->projetilIndex].bullet.posCenter.x=p->dirShooting.x>0?p->posplayer.x+35:p->dirShooting.x<0?p->posplayer.x-5:p->posplayer.x+15;
        p->playerProj[p->projetilIndex].bullet.posCenter.y=p->dirShooting.y>0?p->posplayer.y+35:p->dirShooting.y<0?p->posplayer.y-5:p->posplayer.y+15;
        p->playerProj[p->projetilIndex].dirProj=p->dirShooting;


}
int PlayerAttackHandler(PLAYER *p)
{
    p->dirShooting=(Vector2){0,0};
    p->fire_Delay-=GetFrameTime();
    if(p->fire_Delay<=0)
    {
        if(IsKeyDown(KEY_UP))
        {
            p->dirShooting.y=-1;
            CriaProjetil(p);
            p->fire_Delay=0.42;
            return 1;
        }
        if(IsKeyDown(KEY_RIGHT))
        {
            p->dirShooting.x=1;
            CriaProjetil(p);
            p->fire_Delay=0.42;

            return 1;

        }
        if(IsKeyDown(KEY_DOWN))
        {
            p->dirShooting.y=1;
            CriaProjetil(p);
            p->fire_Delay=0.42;


            return 1;

        }
        if(IsKeyDown(KEY_LEFT))
        {
            p->dirShooting.x=-1;
            p->fire_Delay=0.42;

            CriaProjetil(p);
            return 1;
        }
    }
    return 0;



}
int PlayerMovementHandler(PLAYER *p)
{
    p->dirPlayer=(Vector2){0,0};

    if(IsKeyDown(KEY_W))
    {
      p->dirPlayer.y=-1;
    }
    if(IsKeyDown(KEY_A))
    {
      p->dirPlayer.x=-1;

    }
    if(IsKeyDown(KEY_D))
    {
      p->dirPlayer.x=1;
    }
    if(IsKeyDown(KEY_S))
    {
      p->dirPlayer.y=1;

    }
    if(p->dirPlayer.x!=0 || p->dirPlayer.y!=0)
    {
        return 1;
    }
    return 0;
}
/* caso haja input,ou seja, haja direcao diferente de 0, verifica se o movimento eh possivel e move o jogador */
void Mover(PLAYER *p)
{
    if(PlayerMovementHandler(p))
    {
        Vector2 newPos;
        newPos.x = p->dirPlayer.x*3+p->posplayer.x;
        newPos.y =p->posplayer.y;

        if(PodeMoverX(p->dirPlayer,p->posplayer,newPos))
        {
            p->posplayer.x+=p->dirPlayer.x*3;
        }
         newPos.x = p->posplayer.x;
         newPos.y = p->dirPlayer.y*3+p->posplayer.y;
        if(PodeMoverY(p->dirPlayer,p->posplayer,newPos))
        {
            p->posplayer.y+=p->dirPlayer.y*3;
        }
    }

}
/* gera uma direcao aleatoria diagonal */
void gerarDirecaoAleatoria(Vector2 *dir)
{
    int dx,dy;
    int a = 0,b=0;
    do
    {
        dx= -1 + rand() % 3;
        a++;

    }
    while(dx==0 && a<20);
    do
    {
        dy=-1 + rand()%3;
        b++;
    }
    while(dy==0 && b<20);
    dir->x=dx;
    dir->y=dy;
}
/* inimigo que se move aleatoriamente em diagonais */
int moveInimigo(INIMIGO *Inim_ptr,int *moveDuration)
{
    Vector2 newPos;
    if(*moveDuration<=0)
    {
        gerarDirecaoAleatoria(&Inim_ptr->dirInimigo);
        *moveDuration = 10 +rand()%100;
    }
    else
    {
        *moveDuration-=1;
    }
    newPos.x = Inim_ptr->dirInimigo.x + Inim_ptr->posInimigo.x;
    newPos.y = Inim_ptr->posInimigo.y;
    if(!PodeMoverX(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos)||!PodeMoverY(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))
    *moveDuration=0;
    else
    {
        Inim_ptr->posInimigo.x+=Inim_ptr->dirInimigo.x*1;

    }
    newPos.x=Inim_ptr->posInimigo.x;
    newPos.y=Inim_ptr->dirInimigo.y+Inim_ptr->posInimigo.y;

    if(!PodeMoverY(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos)||!PodeMoverY(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))

    *moveDuration=0;
     else
    {
        Inim_ptr->posInimigo.y+=Inim_ptr->dirInimigo.y*1;
    }
    //printf("%.5f %.5f \n",Inim_ptr->dirInimigo.y,newPos.y);

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
    if(mapaLevel==NULL||s->mapaAtual>99)
    {
        jogoAtivo = false;
        s->pontuacao+=(10000-(tempo_atual*(log(tempo_atual))));
        if(s->pontuacao<0)
        {
            s->pontuacao=0;
        }

    }
    if(type==1)
    {
        fclose(mapaLevel);
        mapaLevel=fopen("JogoSalvo.txt","r");
    }
    s->InimigosNaFase=0;
    s->bombasNaFase=0;
    s->armadilhasNaFase=0;
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
                s->player.Bomb.obj.ativo=false;
                s->player.Bomb.dandoDano=false;
                s->player.Bomb.obj.frameIndex=0;
                s->player.Bomb.obj.frameTime=0.25;
                s->player.Bomb.obj.quantiaFrames=22;
                s->player.Bomb.obj.spriteObjeto=spriteBombExpl;
                s->CurrentLevelMatrix[iMap][jMap]='\0';
                break;
                case 'I':
                if(s->InimigosNaFase<=MAX_INIMIGOS)
                {
                    s->Inimigos[s->InimigosNaFase].posInimigo.x = jMap*15;
                    s->Inimigos[s->InimigosNaFase].posInimigo.y = iMap*15;
                    s->Inimigos[s->InimigosNaFase].tipoInimigo = 1;
                    s->Inimigos[s->InimigosNaFase].HP = 5;
                    s->Inimigos[s->InimigosNaFase].tempoChase = 0.5;
                    s->Inimigos[s->InimigosNaFase].moveDuration= 0;
                    s->Inimigos[s->InimigosNaFase].ativo = true;
                    s->Inimigos[s->InimigosNaFase].TomouDano = false;
                    s->Inimigos[s->InimigosNaFase].speed = 2;
                    s->Inimigos[s->InimigosNaFase].spriteColor = WHITE;
                    s->Inimigos[s->InimigosNaFase].flash_Duration=0.3;
                    gerarDirecaoAleatoria(&s->Inimigos[s->InimigosNaFase].dirInimigo);
                    s->CurrentLevelMatrix[iMap][jMap]='\0';
                    s->InimigosNaFase+=1;

                }
                break;
                case 'B':
                s->Bombas[s->bombasNaFase].posObjeto.x=jMap*15;
                s->Bombas[s->bombasNaFase].posObjeto.y=iMap*15;
                s->Bombas[s->bombasNaFase].spriteObjeto=spriteBomba;
                s->Bombas[s->bombasNaFase].ativo=true;
                s->Bombas[s->bombasNaFase].tipoObjeto=0;
                s->bombasNaFase+=1;
                printf("%d",s->bombasNaFase);
                break;
                 case 'X':
                s->Armadilhas[s->armadilhasNaFase].posObjeto.x=jMap*15;
                s->Armadilhas[s->armadilhasNaFase].posObjeto.y=iMap*15;
                s->Armadilhas[s->armadilhasNaFase].ativo=true;
                s->Armadilhas[s->armadilhasNaFase].tipoObjeto=1;
                s->Armadilhas[s->armadilhasNaFase].spriteObjeto=spriteTrap;
                s->armadilhasNaFase+=1;
                break;
                 case 'P':
                s->Portal.posObjeto.x=jMap*15;
                s->Portal.posObjeto.y=iMap*15;
                s->Portal.quantiaFrames = 2;
                s->Portal.frameTime=0.5;
                s->Portal.ativo=false;
                s->Portal.spriteObjeto=spritePortal;
                break;
                case '#':
                break;

                default:
                s->CurrentLevelMatrix[iMap][jMap]='\0';
                break;
            }
        }

    }
    //desativa os inimigos excedentes ao numero de inimigos na fase
    for (int i = s->InimigosNaFase; i<MAX_INIMIGOS; i++)
    {
        s->Inimigos[i].ativo = false;
        s->Inimigos[s->InimigosNaFase].moveDuration= 0;


    }
    for(int i=0;i<MAX_PROJETEIS;i++)
    {
        s->player.playerProj[i].ativo=false;
        s->player.playerProj[i].airTime=3;
        s->player.playerProj[i].bullet=(Circle){.posCenter.x=0,.posCenter.y=0,.radius=7.5};

    }
    for(int i = s->armadilhasNaFase;i<MAX_OBJECTS;i++)
    {
        s->Armadilhas[i].ativo=false;
    }
     for(int i = s->bombasNaFase;i<MAX_OBJECTS;i++)
    {
        s->Bombas[i].ativo=false;
    }
    if(type==1)
    {
        fscanf(mapaLevel,"%d\n%d\n%d\n%f\n%d",&s->player.HP,&s->player.bombAmount,&s->mapaAtual,&s->tempo_restante,&s->pontuacao);
        printf("%.1f",s->tempo_restante);

    }
    //printf("%f", player->posplayer.x);
    fclose(mapaLevel);


}



/* renderiza os elementos que devem aparecer quando o jogo esta pausado */
void PausaRenderer()
{
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 175 });
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            DrawTextureEx(FundoMenus,(Vector2){175+(64*i),92+(64*j)}, 7, 0.8,WHITE);
        }
    }

    DrawTextEx(fonteTitle, "PAUSADO", (Vector2){305,170}, 60, 10, BLACK);
    DrawTextEx(fonteTitle, "PAUSADO", (Vector2){308,173}, 60, 10, WHITE);

    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){32,250}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){34,253}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){32,300}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){34,303}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){32,350}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){34,353}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){30,400}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){32,402}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){32,450}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){34,453}, 26, 0, CLITERAL(Color){0,220,220,255});
    DrawTextEx(fonteTitle, TextFormat("Pontuacao:%d ",status_jogo_atual.pontuacao), (Vector2){32,500}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Pontuacao:%d ",status_jogo_atual.pontuacao), (Vector2){34,503}, 26, 0, CLITERAL(Color){0,220,220,255});


    DrawTextEx(fonteTitle, "Novo Jogo - N", (Vector2){310,270}, 45*sizeMulti[0], 0, BLACK);
    DrawTextEx(fonteTitle, "Novo Jogo - N", (Vector2){313,273}, 45*sizeMulti[0], 0, CLITERAL(Color){0,220,220,255});


    DrawTextEx(fonteTitle, "Salvar Jogo - S", (Vector2){305,330}, 45*sizeMulti[1], 0, BLACK);
    DrawTextEx(fonteTitle, "Salvar Jogo - S", (Vector2){308,333}, 45*sizeMulti[1], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Carregar Jogo - C", (Vector2){280,390}, 45*sizeMulti[2], 0, BLACK);
    DrawTextEx(fonteTitle, "Carregar Jogo - C", (Vector2){283,393}, 45*sizeMulti[2], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Configuracoes - F", (Vector2){280,450}, 45*sizeMulti[3], 0, BLACK);
    DrawTextEx(fonteTitle, "Configuracoes - F", (Vector2){283,453}, 45*sizeMulti[3], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Sair - Q", (Vector2){380,510}, 45*sizeMulti[4], 0, BLACK);
    DrawTextEx(fonteTitle, "Sair - Q", (Vector2){383,513}, 45*sizeMulti[4], 0, CLITERAL(Color){0,220,220,255});

      for(int iPausa=0;iPausa<5;iPausa++)
    {
        float firstSize = sizeMulti[iPausa];
        DrawRectangle(RetangulosPausa[iPausa].x,RetangulosPausa[iPausa].y,RetangulosPausa[iPausa].width,RetangulosPausa[iPausa].height, BLANK);
        if(CheckCollisionPointRec(virtualMouse,RetangulosPausa[iPausa]))
        {
            sizeMulti[iPausa]=1.25;
        }
        else
        {
            sizeMulti[iPausa]=1;
        }
        if(firstSize-sizeMulti[iPausa]<0)
        {
            PlaySound(selectionSound);
        }
    }
}
/* A funcao JogoRenderer desenha os elementos do jogo na tela */
void RenderizaSapo(Vector2 pos,int scale)
{
    if(status_jogo_atual.player.bombAmount>=5 && status_jogo_atual.player.HP>=3)
    {
        DrawTextureEx(sapoLoco,pos,0,scale,status_jogo_atual.player.spriteColor);
    }
    else if(status_jogo_atual.player.HP>=4&&status_jogo_atual.player.bombAmount<5)
    {
        if((int)tempo_atual%2==0)
        {
          DrawTextureEx(sapoRight,pos,0,scale,status_jogo_atual.player.spriteColor);

        }
        else if((int)tempo_atual%3==0)
        {
            DrawTextureEx(sapo,pos,0,scale,status_jogo_atual.player.spriteColor);
        }
        else
        {
            DrawTextureEx(sapoLeft,pos,0,scale,status_jogo_atual.player.spriteColor);
        }

    }
    else if(status_jogo_atual.player.HP==3)
    {
         DrawTextureEx(sapoDmg01,pos,0,scale,status_jogo_atual.player.spriteColor);

    }
    else if(status_jogo_atual.player.HP==2)
    {
         DrawTextureEx(sapoDmg02,pos,0,scale,status_jogo_atual.player.spriteColor);

    }
     else if(status_jogo_atual.player.HP==1)
    {
         DrawTextureEx(sapoDmg04,pos,0,scale,status_jogo_atual.player.spriteColor);

    }


}
void JogoRenderer(STATUS *s)
{
    s->player.playerRec = (Rectangle){.x=s->player.posplayer.x,.y=s->player.posplayer.y,.width=30,.height=30};
    DrawRectangleRec(s->player.playerRec,BLANK);
    if(s->player.TomouDano)
    {
        s->player.flash_Duration-=GetFrameTime();
        s->player.spriteColor = RED;
        if(s->player.flash_Duration<=0)
        {
            s->player.TomouDano = false;
            s->player.spriteColor=WHITE;
            s->player.flash_Duration=0.3;
        }
    }
    RenderizaSapo((Vector2){.x=s->player.posplayer.x-15,.y=s->player.posplayer.y-15},1);
    for(int i=0; i<MAPLINES; i++)
    {
        for(int j=0; j<MAPCOLUMNS; j++)
        {
            if(s->CurrentLevelMatrix[i][j]=='#')
            {
                //DrawRectangle(j*15,i*15,15,15,BROWN);
                DrawTextureEx(texturaParede,(Vector2){j*15,i*15},0,1,WHITE);
            }
        }
    }
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<3;j++)
        {
            DrawTextureEx(FundoStats,(Vector2){128*i,450+(128*j)},0,1,WHITE);
        }


}

}
/* renderiza os elementos que aparecem quando o jogo esta despausado */

void UnpausedRenderer(STATUS *s)
{
    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){50,500}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){53,503}, 50, 0, CLITERAL(Color){0,220,220,255});
    DrawRectangle(317,512,3,206,BROWN);
    DrawRectangle(520,512,3,206,BROWN);


    DrawRectangle(317,512,206,3,BROWN);
    DrawRectangle(317,715,206,3,BROWN);

    DrawRectangle(320,515,200,200,CLITERAL(Color){ 0, 0, 0, 220 });
    RenderizaSapo((Vector2){325,500},3);

    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){50,570}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){53,573}, 50, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){50,640}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){53,643}, 50, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){600,500}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){603,503}, 50, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){600,570}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){603,573}, 50, 0, CLITERAL(Color){0,220,220,255});
    DrawTextEx(fonteTitle, TextFormat("Pontos:%d ",status_jogo_atual.pontuacao), (Vector2){600,640}, 50, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Pontos:%d ",status_jogo_atual.pontuacao), (Vector2){603,643}, 50, 0, CLITERAL(Color){0,220,220,255});

}
/* A funcao NovoJogo recebe um struct STATUS, criado ao inicializar o jogo, que armazena as principais informacos sobre o jogo, e confere a ele os valores padrao de um novo jogo,
tambem coloca a bool GameStart como true, para o jogo sair do menu principal e comecar */
void NovoJogo(STATUS *s)
{
    s->player.HP = 5;
    s->player.bombAmount=3;
    s->player.dmg = 1;
    s->player.vivo=true;
    s->mapaAtual=1;
    s->player.projetilIndex=0;
    s->player.fire_Delay=0;
    s->pontuacao=0;


    for(int i=0;i<MAX_PROJETEIS;i++)
    {
        s->player.playerProj[i].ativo=false;
        s->player.playerProj[i].airTime=3;
        s->player.playerProj[i].bullet=(Circle){.posCenter.x=0,.posCenter.y=0,.radius=7.5};


    }

    CarregaMapa(s,0);
    jogoAtivo=true;
    GameStart = true;
    s->tempo_restante=0;
    tempo_atual=s->tempo_restante;


}
/* A funcao Carregajogo recebe o ponteiro para o struct STATUS atual, e chama a funcao CarregaMapa com o tipo 1 para abrir o arquivo JogoSalvo.txt, le as informacoes relevantes para o jogo e as armazena no struct */
void CarregaJogo(STATUS *s)
{
    CarregaMapa(s,1);
    s->player.vivo=true;
    s->player.projetilIndex=0;
    s->player.fire_Delay=0;

    for(int i=0;i<MAX_PROJETEIS;i++)
    {
        s->player.playerProj[i].ativo=false;
        s->player.playerProj[i].airTime=3;
        s->player.playerProj[i].bullet=(Circle){.posCenter.x=0,.posCenter.y=0,.radius=7.5};
    }

    jogoAtivo=true;
    GameStart = true;
    tempo_atual=s->tempo_restante;

}
/* faz o jogador tomar dano, e caso HP menor que 0, mata o jogador */
void TakeDmg(PLAYER *p)
{
    p->HP-=1;
    p->TomouDano=true;
    PlaySound(dmgSound);
    if(p->HP<=0)
    p->vivo = false;


}

void MoveProjeteis(PROJETIL *projetil)
{
    projetil->bullet.posCenter.x+=projetil->dirProj.x*3.5;
    projetil->bullet.posCenter.y+=projetil->dirProj.y*3.5;

}
void LimpadorProjetil(PROJETIL *projetil)
{
    projetil->airTime-=GetFrameTime();
    int posi = trunc(projetil->bullet.posCenter.y/15);
    int posj=trunc(projetil->bullet.posCenter.x/15);
    if(projetil->airTime<=0)
    {
        projetil->ativo=false;
        projetil->airTime=3;
    }
    if(status_jogo_atual.CurrentLevelMatrix[posi][posj]=='#')
    {
        if(CheckCollisionCircleRec(projetil->bullet.posCenter,projetil->bullet.radius,(Rectangle){.x=posj*15,.y=posi*15,.width=15,.height=15}))
            {
                projetil->ativo=false;

            }
    }



}
void RenderizaProjeteis(PROJETIL projetil)
{
    DrawCircle(projetil.bullet.posCenter.x,projetil.bullet.posCenter.y,projetil.bullet.radius,LIGHT_BLUE);

}

void TakeDmgEnemy(INIMIGO *inim)
{
    inim->HP-=1;
    PlaySound(dmgSoundEnemy);
    inim->TomouDano=true;
    if(inim->HP<=0)
    {
        PlaySound(deathSoundEnemy);
        printf("dead!");
        inim->ativo=false;
        status_jogo_atual.pontuacao+=100;
        status_jogo_atual.InimigosNaFase-=1;

    }
}

int ColisaoProjeteis_Inimigos(PROJETIL *projetil,INIMIGO inimigos[MAX_INIMIGOS])
{
    for(int i=0;i<MAX_INIMIGOS;i++)
    {
        if(inimigos[i].ativo)
        {
            if(CheckCollisionCircleRec(projetil->bullet.posCenter,projetil->bullet.radius,inimigos[i].inimigoRec))
            {
                TakeDmgEnemy(&inimigos[i]);
                projetil->ativo=false;
                printf("zunga!");
                return i;
            }
        }
    }
    return 0;

}
void ChecadorProjetil(PROJETIL projeteis[MAX_PROJETEIS])
{
    for(int i=0;i<MAX_PROJETEIS;i++)
    {
        if(projeteis[i].ativo)
        {
            LimpadorProjetil(&projeteis[i]);
            MoveProjeteis(&projeteis[i]);
            RenderizaProjeteis(projeteis[i]);
            ColisaoProjeteis_Inimigos(&projeteis[i],status_jogo_atual.Inimigos);



        }
    }
}
void CollisionPlayerEnemy(PLAYER *p, INIMIGO *inim)
{
    if(CheckCollisionRecs(p->playerRec,inim->inimigoRec))
    {
        if(invulnTime==0)
        {
            TakeDmg(p);
            invulnTime=30;
        }
    }

}
void InimigoRenderer(INIMIGO *inim)
{
    if(inim->TomouDano)
        {
            inim->flash_Duration-=GetFrameTime();
            inim->spriteColor= RED;
            if(inim->flash_Duration<=0)
            {
                inim->TomouDano=false;
                inim->spriteColor=WHITE;
                inim->flash_Duration=0.3;
            }
        }
        inim->inimigoRec = (Rectangle){.x=inim->posInimigo.x,.y=inim->posInimigo.y,.width=30,.height=30};
         DrawRectangleRec(inim->inimigoRec,BLANK);
        DrawTextureEx(sapoVermelho,(Vector2){inim->posInimigo.x-15,inim->posInimigo.y-15},0,1,inim->spriteColor);

}
void ChecadorInimigos(INIMIGO inimigo[MAX_INIMIGOS])
{
    for(int i=0;i<MAX_INIMIGOS;i++)
    {
        if(inimigo[i].ativo)
        {
            InimigoRenderer(&inimigo[i]);
            CollisionPlayerEnemy(&status_jogo_atual.player,&inimigo[i]);
            if(i%2!=0)
            {
                moveInimigo(&inimigo[i],&inimigo[i].moveDuration);
            }
            else
            {
                inimigoPerseguePlayer(&inimigo[i],&status_jogo_atual.player,&inimigo[i].tempoChase);
            }

        }
    }
}
void ObjetoRenderer(OBJETO_ESTATICO *objeto)
{
    objeto->objetoRec = (Rectangle){.x=objeto->posObjeto.x,.y=objeto->posObjeto.y,.width=30,.height=30};
    DrawTextureEx(objeto->spriteObjeto,objeto->posObjeto,0,0.6,WHITE);
}
void ObjetoRendererPro(OBJETO_ESTATICO *objeto)
{
    objeto->objetoRec  =(Rectangle){.x=objeto->posObjeto.x,.y=objeto->posObjeto.y,.width=30,.height=30};
    Rectangle dest = objeto->objetoRec;
    dest.width = 64;
    dest.height = 64;
    objeto->frameTime-=GetFrameTime();
    objeto->spriteSource = (Rectangle){0,0,64,64};
    if(objeto->frameTime<=0.25)
    {
        objeto->spriteSource.x=64;
        if(objeto->frameTime<=0)
        {
            objeto->frameTime=0.5;
        }
    }
    DrawTexturePro(objeto->spriteObjeto,objeto->spriteSource,dest,(Vector2){dest.width/2,dest.height/2},0,WHITE);

}
void BombaColocadaRenderer(BOMB_EXPL *b)
{
    b->obj.objetoRec=(Rectangle){.x=b->obj.posObjeto.x,.y=b->obj.posObjeto.y,.width=64,.height=64};
    Rectangle dest = b->obj.objetoRec;
    b->obj.spriteSource = (Rectangle){0,0,64,64};
    b->obj.spriteSource.x=64*b->obj.frameIndex;
    b->obj.frameTime-=GetFrameTime();
    if(b->obj.frameTime<=0)
    {
        b->obj.frameIndex+=1;
        b->obj.frameTime=0.075;
    }
    if(b->obj.frameIndex==16)
    {
        PlaySound(explosionSound);
    }
    if(b->obj.frameIndex>=16 && b->obj.frameIndex<22)
    {
        b->dandoDano=true;
    }
    if(b->obj.frameIndex>=22)
    {
        b->dandoDano=false;
        b->obj.ativo=false;
        b->obj.frameTime=0.1;
        b->obj.frameIndex=0;
    }
    DrawTexturePro(b->obj.spriteObjeto,b->obj.spriteSource,dest,(Vector2){0,0},0,WHITE);

}
void ChecaColisaoBombaInim(INIMIGO *inim)
{
    if(CheckCollisionRecs(status_jogo_atual.player.Bomb.obj.objetoRec,inim->inimigoRec))
    {
        inim->HP=0;
        TakeDmgEnemy(inim);
    }

}
void TentaSpawnarBomba(BOMB_EXPL *b)
{
    printf("bomb sabugada");
    if(!b->obj.ativo && status_jogo_atual.player.bombAmount>0)
    {
        b->obj.posObjeto.x=status_jogo_atual.player.posplayer.x;
        b->obj.posObjeto.y=status_jogo_atual.player.posplayer.y;

        printf("bomb esbugalhada");
        status_jogo_atual.player.bombAmount-=1;
        b->dandoDano=false;
        b->obj.ativo=true;
        b->obj.frameTime=0.1;
        b->obj.frameIndex=0;

    }
}
void PlayerBombInput(PLAYER *p)
{
    if(IsKeyPressed(KEY_E))
    {
        printf("bomba coisasda");
        TentaSpawnarBomba(&p->Bomb);
    }
}
void BombaColocadaHandler(BOMB_EXPL *b)
{

    if(b->obj.ativo)
    {


        BombaColocadaRenderer(b);
        if(b->dandoDano)
        {
            for(int i=0;i<MAX_INIMIGOS;i++)
            {
                if(status_jogo_atual.Inimigos[i].ativo)
                {
                    ChecaColisaoBombaInim(&status_jogo_atual.Inimigos[i]);

                }
            }
        }
    }

}
void CollisionPlayerBomba(PLAYER *p,OBJETO_ESTATICO *bomba)
{
    if(CheckCollisionRecs(p->playerRec,bomba->objetoRec))
    {
        p->bombAmount+=1;
        bomba->ativo=false;
        status_jogo_atual.CurrentLevelMatrix[(int)bomba->posObjeto.y/15][(int)bomba->posObjeto.x/15] = '\0';


    }
}
void CollisionPlayerTrap(PLAYER *p, OBJETO_ESTATICO *trap)
{
    if(CheckCollisionRecs(p->playerRec,trap->objetoRec))
    {
        if(invulnTime==0)
        {
            TakeDmg(p);
            invulnTime=30;
        }

    }
}
void CollisionPlayerPortal(PLAYER *p, OBJETO_ESTATICO *portal)
{
    if(CheckCollisionRecs(p->playerRec,portal->objetoRec))
    {
        portal->ativo=false;
        status_jogo_atual.mapaAtual+=1;
        CarregaMapa(&status_jogo_atual,0);
    }
}
void ChecadorPortal(STATUS *s)
{

    if(s->Portal.ativo)
    {
        ObjetoRendererPro(&s->Portal);
        CollisionPlayerPortal(&s->player,&s->Portal);

    }
    else if(s->InimigosNaFase<=0)
    {
        s->Portal.ativo=true;

    }

}
void ChecadorObjeto(OBJETO_ESTATICO objeto[MAX_OBJECTS], int *quant_objetos,int tipo)
{
    if(tipo==0) // tipo 0 = bomba
    {
        for(int i=0; i<*quant_objetos; i++)
        {

            if(objeto[i].ativo)
            {
                ObjetoRenderer(&objeto[i]);
                CollisionPlayerBomba(&status_jogo_atual.player,&objeto[i]);

            }
        }

    }
   else if(tipo==1) //armadilha
    {
        for(int i=0; i<*quant_objetos; i++)
        {
            if(objeto[i].ativo)
            {
                ObjetoRenderer(&objeto[i]);
                CollisionPlayerTrap(&status_jogo_atual.player,&objeto[i]);

            }
        }

    }
    else if(tipo==2) //portal
    {
        for(int i=0; i<*quant_objetos; i++)
        {
            if(objeto[i].ativo)
            {

            }
        }
    }


}

/* renderiza as informacoes que devem ser mostrada na tela de game over */
void DeathScreenRenderer()
{
    DrawRectangle(0,0,900,700,CLITERAL(Color){200, 200, 180, 160});
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 190 });
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<6;j++)
        {
            DrawTextureEx(FundoMenus,(Vector2){175+(64*i),156+(64*j)},5.5,0.8,WHITE);
        }
    }

  //  DrawText("VOCE MORREU",280,120,50,BLACK);

    DrawTextEx(fonteTitle, "VOCE MORREU", (Vector2){270,212}, 60, 5, BLACK);
    DrawTextEx(fonteTitle, "VOCE MORREU", (Vector2){273,215}, 60, 5, WHITE);

    DrawTextEx(fonteTitle, "Novo Jogo - N", (Vector2){310,312}, 45*sizeMulti[0], 0, BLACK);
    DrawTextEx(fonteTitle, "Novo Jogo - N", (Vector2){313,315}, 45*sizeMulti[0], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Carregar Jogo - C", (Vector2){280,372}, 45*sizeMulti[1], 0, BLACK);
    DrawTextEx(fonteTitle, "Carregar Jogo - C", (Vector2){283,375}, 45*sizeMulti[1], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Sair - Q", (Vector2){380,432}, 45*sizeMulti[2], 0, BLACK);
    DrawTextEx(fonteTitle, "Sair - Q", (Vector2){383,435}, 45*sizeMulti[2], 0, CLITERAL(Color){0,220,220,255});

    for(int iDeath=0;iDeath<3;iDeath++)
    {
        float firstSize = sizeMulti[iDeath];
        DrawRectangle(RetangulosDeath[iDeath].x,RetangulosDeath[iDeath].y,RetangulosDeath[iDeath].width,RetangulosDeath[iDeath].height, BLANK);
        Vector2 mouse = GetMousePosition();
            virtualMouse.x = (mouse.x - (GetScreenWidth() - (900*Escala))*0.5f)/Escala;
            virtualMouse.y = (mouse.y - (GetScreenHeight() - (750*Escala))*0.5f)/Escala;
            virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)900, (float)750 });
        if(CheckCollisionPointRec(virtualMouse,RetangulosDeath[iDeath]))
        {
            sizeMulti[iDeath]=1.25;
        }
        else
        {
            sizeMulti[iDeath]=1;
        }
        if(firstSize-sizeMulti[iDeath]<0)
        {
            PlaySound(selectionSound);
        }
    }
}

void EndScreenRenderer()
{
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 128 });
    DrawText("Voce ganhou!",300,100,50,BLACK);
    DrawText(TextFormat("Pontuacao:%d",status_jogo_atual.pontuacao),300,150,50,BLACK);


}
void ControleMusica(int iMusica)
{

    for(int i = 0; i<=iMusica;i++)
    {
        SomMusica[i] = 1;
    }

    SomDaMusica = -0.05;
    for(int iMusica=0;iMusica<10;iMusica++)
    {
        if(SomMusica[iMusica] == 1)
        {
            SomDaMusica += 0.05;
        }
    }
    SetSoundVolume(titleTheme, SomDaMusica);
}

void ControleEfeitosSonoros(int iEfeitos)
{

    for(int i = 0; i<=iEfeitos;i++)
    {
        SomEfeitosSonoros[i] = 1;
    }
    SomDosEfeitosSonoros = -0.25;
    for(int iEfeitos=0;iEfeitos<10;iEfeitos++)
    {
        if(SomEfeitosSonoros[iEfeitos] == 1)
        {
            SomDosEfeitosSonoros += 0.25;
        }
    }
    SetSoundVolume(deathSoundEnemy,SomDosEfeitosSonoros);
    SetSoundVolume(dmgSoundEnemy,SomDosEfeitosSonoros);
    SetSoundVolume(dmgSound,SomDosEfeitosSonoros);
    SetSoundVolume(pauseSound,SomDosEfeitosSonoros);
    SetSoundVolume(selectionSound,SomDosEfeitosSonoros);
    SetSoundVolume(explosionSound,SomDosEfeitosSonoros);
    SetSoundVolume(unpauseSound,SomDosEfeitosSonoros);
}

/* A funcao Menu recebe um int type, 0 para o menu principal e 1 para o menu dentro do jogo, limitando as opcoes dependendo do contexto.
Dependendo da tecla apertada realiza outras funcoes de manipulacao do estado do jogo */
void MenuConfig()
{
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 128 });
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<7;j++)
        {
            DrawTextureEx(FundoMenus,(Vector2){175+(64*i),124+(64*j)},7,0.8,WHITE);
        }
    }

    DrawTextEx(fonteTitle, "PAUSADO", (Vector2){305,202}, 60, 10, BLACK);
    DrawTextEx(fonteTitle, "PAUSADO", (Vector2){308,205}, 60, 10, WHITE);

    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){32,250}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Tempo:%.1f s",tempo_atual), (Vector2){34,253}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){32,300}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Vida:%d",status_jogo_atual.player.HP), (Vector2){34,303}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){32,350}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount), (Vector2){34,353}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){30,400}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual), (Vector2){32,402}, 26, 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){32,450}, 26, 0, BLACK);
    DrawTextEx(fonteTitle, TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase), (Vector2){34,453}, 26, 0, CLITERAL(Color){0,220,220,255});


    DrawTextEx(fonteTitle, "SFX", (Vector2){340,302}, 45*sizeMulti[0], 0, BLACK);
    DrawTextEx(fonteTitle, "SFX", (Vector2){343,305}, 45*sizeMulti[0], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Musica", (Vector2){310,362}, 45*sizeMulti[1], 0, BLACK);
    DrawTextEx(fonteTitle, "Musica", (Vector2){313,365}, 45*sizeMulti[1], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Tela Cheia - F", (Vector2){310,422}, 45*sizeMulti[2], 0, BLACK);
    DrawTextEx(fonteTitle, "Tela Cheia - F", (Vector2){313,425}, 45*sizeMulti[2], 0, CLITERAL(Color){0,220,220,255});

    DrawTextEx(fonteTitle, "Voltar - Z", (Vector2){347,482}, 45*sizeMulti[3], 0, BLACK);
    DrawTextEx(fonteTitle, "Voltar - Z", (Vector2){350,485}, 45*sizeMulti[3], 0, CLITERAL(Color){0,220,220,255});


     for(int iConfig=0;iConfig<2;iConfig++)
        {
            float firstSize = sizeMulti[iConfig+2];
            DrawRectangle(RetangulosConfigMenu[iConfig].x,RetangulosConfigMenu[iConfig].y,RetangulosConfigMenu[iConfig].width,RetangulosConfigMenu[iConfig].height, BLANK);
            if(CheckCollisionPointRec(virtualMouse,RetangulosConfigMenu[iConfig]))
            {
                sizeMulti[iConfig+2]=1.25;
            }
            else
            {
                sizeMulti[iConfig+2]=1;
            }
            if(firstSize-sizeMulti[iConfig+2]<0)
            {
                PlaySound(selectionSound);
            }
        }
        for(int iSom=0;iSom<10;iSom++)
        {
            float firstSize = sizeMulti[iSom];
            DrawRectangle(RetangulosDeSomMusica[iSom].x+5, RetangulosDeSomMusica[iSom].y, RetangulosDeSomMusica[iSom].width-10, RetangulosDeSomMusica[iSom].height, cores[1]);
            DrawRectangle(RetangulosDeEfeitoSonoro[iSom].x+5, RetangulosDeEfeitoSonoro[iSom].y, RetangulosDeEfeitoSonoro[iSom].width-10, RetangulosDeEfeitoSonoro[iSom].height, cores[1]);

            if(CheckCollisionPointRec(virtualMouse,RetangulosDeSomMusica[iSom]))
            {
                SomMusica[iSom]=2;
            }

            else
                SomMusica[iSom]=0;


            if(CheckCollisionPointRec(virtualMouse,RetangulosDeEfeitoSonoro[iSom]))
            {
                SomEfeitosSonoros[iSom]=2;
            }

            else
                SomEfeitosSonoros[iSom]=0;
        }

            int m = (int)(SomDaMusica/0.05);
            int sfx = (int)(SomDosEfeitosSonoros/0.25);
            for(int i = 0; i<=m;i++)
            {
                DrawRectangle(RetangulosDeSomMusica[i].x+6.5, RetangulosDeSomMusica[i].y+1.5, RetangulosDeSomMusica[i].width-13, RetangulosDeSomMusica[i].height-3, cores[0]);
            }
            for(int k = 0; k<=sfx;k++)
            {
                DrawRectangle(RetangulosDeEfeitoSonoro[k].x+6.5, RetangulosDeEfeitoSonoro[k].y+1.5, RetangulosDeEfeitoSonoro[k].width-13, RetangulosDeEfeitoSonoro[k].height-3, cores[0]);
            }

}
void Fullscreen()
{
    if(!IsWindowFullscreen())
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
    }
    else
    {
        ToggleFullscreen();
        SetWindowSize(WindowWidth, WindowHeight);
    }

}
void Menu(int type)
{
    if(IsMouseButtonPressed(0))
    {
        if(type ==1)
        {
        if(sizeMulti[0]!=1)
        NovoJogo(&status_jogo_atual);
        if(sizeMulti[1]!=1)
        SalvaJogo(&status_jogo_atual);
        if(sizeMulti[2]!=1)
        CarregaJogo(&status_jogo_atual);
        if(sizeMulti[3]!=1)
        {
            MenuConfig(&status_jogo_atual);
            PausadoConfig = true;
        }
        if(sizeMulti[4]!=1)
        CloseWindow();
        }

        if(type == 2)
        {

            for(int iEfeitos = 0;iEfeitos<10;iEfeitos++)
            {
                if(SomEfeitosSonoros[iEfeitos]==2)
                {
                    ControleEfeitosSonoros(iEfeitos);
                }
            }

            for(int iMusica = 0;iMusica<10;iMusica++)
            {
                if(SomMusica[iMusica]==2)
                {
                    ControleMusica(iMusica);
                }
            }
            if(sizeMulti[2]!=1)
              Fullscreen();

            if(sizeMulti[3]!=1)
            {
                PausadoConfig = false;
                Pausado = true;
                PausaRenderer();
            }

            if(sizeMulti[4]!=1)
            CloseWindow();
        }


        if(type == 0)
        {
            if(sizeMulti[0]!=1)
            NovoJogo(&status_jogo_atual);
            if(sizeMulti[1]!=1)
            CarregaJogo(&status_jogo_atual);
            if(sizeMulti[2]!=1)
            CloseWindow();
        }
    }

    if(IsKeyPressed(KEY_N) && type!=2)
    {
        NovoJogo(&status_jogo_atual);
    }
    if(IsKeyPressed(KEY_S) && type!=2)
    {
        SalvaJogo(&status_jogo_atual);
    }
    if(IsKeyPressed(KEY_Q))
    {
        CloseWindow();
    }
    if(IsKeyPressed(KEY_C ) && type!=2)
    {
        CarregaJogo(&status_jogo_atual);
    }
    if(IsKeyPressed(KEY_F) && type==1)
    {
        MenuConfig(&status_jogo_atual);
        PausadoConfig = true;
    }
     if(IsKeyPressed(KEY_F) && type==2)
    {
        Fullscreen();
    }
    if(IsKeyPressed(KEY_Z) && type==2)
    {
        PausadoConfig = false;
        Pausado = true;
        PausaRenderer();
    }
}


/* A funcao TitleScreen renderiza o fundo e outros elementos da tela de titulo, e chama a fncao Menu com o type 0, limitando as opcoes disponiveis para
Novo Jogo, Carregar Jogo ou Sair */
void TitleScreen()
{
    DrawTexture(texturaTitle,0,0,WHITE);
    for(int iTitle=0;iTitle<3;iTitle++)
    {
        float firstSize = sizeMulti[iTitle];
        DrawRectangle(RetangulosTitle[iTitle].x,RetangulosTitle[iTitle].y,RetangulosTitle[iTitle].width,RetangulosTitle[iTitle].height,BLANK);
        if(CheckCollisionPointRec(virtualMouse,RetangulosTitle[iTitle]))
        {
            sizeMulti[iTitle]=1.25;
        }
        else
        {
            sizeMulti[iTitle]=1;
        }
        if(firstSize-sizeMulti[iTitle]<0)
        {
            PlaySound(selectionSound);
        }
    }
    DrawTextEx(fonteTitle,"Froggers frog frog",(Vector2){202,150},60,0,BLACK);
    DrawTextEx(fonteTitle,"Froggers frog frog",(Vector2){205,153},60,0,TURQUOISE);
    DrawTextEx(fonteTitle,"Novo Jogo - N",(Vector2){222,447},60*sizeMulti[0],0,BLACK);
    DrawTextEx(fonteTitle,"Novo Jogo - N",(Vector2){225,450},60*sizeMulti[0],0,TURQUOISE);
    DrawTextEx(fonteTitle,"Carregar Jogo - C",(Vector2){222,547},60*sizeMulti[1],0,BLACK);
    DrawTextEx(fonteTitle,"Carregar Jogo - C",(Vector2){225,550},60*sizeMulti[1],0,TURQUOISE);
    DrawTextEx(fonteTitle,"Sair - Q",(Vector2){222,647},60*sizeMulti[2],0,BLACK);
    DrawTextEx(fonteTitle,"Sair - Q",(Vector2){225,650},60*sizeMulti[2],0,TURQUOISE);
    Menu(0);

}
int cheatA =0;
int cheatB=0;
void Trapaca()
{
   if(IsKeyDown(KEY_M))
   {
       if(IsKeyPressed(KEY_A))
       {
           cheatA+=1;
       }
       if(IsKeyPressed(KEY_C))
       {
           cheatB+=1;

       }
       if(cheatA>2)
       {
           for(int i=0;i<MAX_INIMIGOS;i++)
           {
               status_jogo_atual.Inimigos[i].HP=0;
               TakeDmgEnemy(&status_jogo_atual.Inimigos[i]);
           }
           cheatA=0;
       }
       if(cheatB>1)
       {
           status_jogo_atual.player.HP+=5;
           cheatB=0;
       }

   }
   else
   {
       cheatA = 0;
       cheatB=0;
   }
}


int main()
{
    const int windowWidth=WindowWidth;
    const int windowHeight = WindowHeight;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(WindowWidth, WindowHeight, "trabalho");
    SetWindowMinSize(288, 240);

    int gameScreenWidth = 900;
    int gameScreenHeight = 750;

    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    srand(time(NULL));
    InitAudioDevice();
    SetMasterVolume(50);
    titleTheme=LoadSound("./sound/MoFTitle.mp3");
    dmgSound=LoadSound("./sound/takedmg.mp3");
    dmgSoundEnemy=LoadSound("./sound/dmgSoundEnemy.mp3");
    pauseSound = LoadSound("./sound/pause.mp3");
    deathSoundEnemy=LoadSound("./sound/deathSoundEnemy.mp3");
    selectionSound=LoadSound("./sound/selection.mp3");
    explosionSound=LoadSound("./sound/explosion.mp3");
    unpauseSound = LoadSound("./sound/unpause.mp3");
    texturaTitle = LoadTexture("TitleScreen.png");
    FundoMenus = LoadTexture("Grass_08-128x128.png");
    spriteBombExpl = LoadTexture("bombsheet.png");
    spritePortal=LoadTexture("Portal.png");
    spriteBomba=LoadTexture("bomb.png");
    FundoStats=LoadTexture("fundo.png");
    texturaParede=LoadTexture("paredinha.png");
    sapoVermelho=LoadTexture("sapoVermelho.png");
    spriteTrap=LoadTexture("trap.png");
    sapoDmg01=LoadTexture("sapoTodoCoitado01.png");
    sapoDmg02=LoadTexture("sapoTodoCoitado02.png");
    sapoDmg03=LoadTexture("sapoTodoCoitado02_2.png");
    sapoDmg04=LoadTexture("sapoTodoCoitado03.png");
    sapoLoco=LoadTexture("sapoLoco.png");
    sapoLeft=LoadTexture("sapoOlhandoPraEsquerda.png");
    sapoRight=LoadTexture("sapoOlhandoPraDireita.png");
    SetSoundVolume(deathSoundEnemy,SomDosEfeitosSonoros);
    SetSoundVolume(dmgSoundEnemy,SomDosEfeitosSonoros);
    SetSoundVolume(dmgSound,SomDosEfeitosSonoros);
    SetSoundVolume(pauseSound,SomDosEfeitosSonoros);
    SetSoundVolume(selectionSound,SomDosEfeitosSonoros);
    SetSoundVolume(explosionSound,SomDosEfeitosSonoros);
    SetSoundVolume(unpauseSound,SomDosEfeitosSonoros);
    SetSoundVolume(titleTheme,SomDaMusica); //SOUND VOLUME EH FLOAT ENTRE 0 E 1, SE BOTAR MAIS Q ISSO VAI ESTOURAR TEUS OUVIDO
    fonteTitle = LoadFontEx("SunnyspellsRegular-MV9ze.otf",75,NULL,0);
    sapo=LoadTexture("sapo.png");
    SetExitKey(KEY_Q);
    status_jogo_atual.player.TomouDano=false;
    status_jogo_atual.player.spriteColor=WHITE;
    status_jogo_atual.player.flash_Duration=0.3;
    status_jogo_atual.player.vivo = true;
    for(int i =0;i<MAX_INIMIGOS;i++)
     {
         status_jogo_atual.Inimigos[i].spriteColor=WHITE;
         status_jogo_atual.Inimigos[i].TomouDano=false;
         status_jogo_atual.Inimigos[i].flash_Duration=0.3;

     }
    while(!WindowShouldClose())
    {
        Escala = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
            Vector2 mouse = GetMousePosition();
            virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth*Escala))*0.5f)/Escala;
            virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight*Escala))*0.5f)/Escala;
            virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)gameScreenWidth, (float)gameScreenHeight });
        while(!GameStart)
        {
            Escala = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
            Vector2 mouse = GetMousePosition();
            virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth*Escala))*0.5f)/Escala;
            virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight*Escala))*0.5f)/Escala;
            virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)gameScreenWidth, (float)gameScreenHeight });
            if(!IsSoundPlaying(titleTheme))
            PlaySound(titleTheme);
            BeginTextureMode(target);

            //ClearBackground(WHITE);
            ClearBackground(CLITERAL(Color){200, 200, 180, 255});
            TitleScreen();
            EndTextureMode();
            tempo_inicio=time(NULL);
            BeginDrawing();
             ClearBackground(BLACK);     // Clear screen background

            // Draw render texture to screen, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*Escala))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*Escala))*0.5f,
                           (float)gameScreenWidth*Escala, (float)gameScreenHeight*Escala }, (Vector2){ 0, 0 }, 0.0f, WHITE);
            EndDrawing();

        }
        BeginTextureMode(target);
        if(status_jogo_atual.player.vivo&&jogoAtivo)
        {
            //DrawRectangle(0,0,900,700,CLITERAL(Color){200, 200, 180, 255});
            if(IsKeyPressed(KEY_ESCAPE))
            {
                if(Pausado)
                {
                    PlaySound(pauseSound);
                    ResumeSound(titleTheme);

                }
                if(!Pausado)
                {
                    PlaySound(unpauseSound);
                    PauseSound(titleTheme);

                }
                Pausado = !Pausado;
                PausadoConfig=false;

            }
            if(!Pausado)
            {
                Mover(&status_jogo_atual.player);
                PlayerAttackHandler(&status_jogo_atual.player);
                PlayerBombInput(&status_jogo_atual.player);
                BombaColocadaHandler(&status_jogo_atual.player.Bomb);
                ChecadorProjetil(status_jogo_atual.player.playerProj);
                ChecadorObjeto(status_jogo_atual.Bombas,&status_jogo_atual.bombasNaFase,0);
                ChecadorObjeto(status_jogo_atual.Armadilhas,&status_jogo_atual.armadilhasNaFase,1);
                ChecadorInimigos(status_jogo_atual.Inimigos);
                ChecadorPortal(&status_jogo_atual);
                tempo_atual+=GetFrameTime();
                Trapaca();
                if(invulnTime>0)
                {
                    invulnTime-=1;
                }

            }
            if(Pausado)
            {
                if(PausadoConfig)
                {
                    Menu(2);
                }
                else
                {
                  Menu(1);

                }
            }

        }
        if(status_jogo_atual.player.vivo==false||jogoAtivo==false)
        {
            Menu(0);
        }

        //ClearBackground(WHITE);
        ClearBackground(CLITERAL(Color){200, 200, 180, 255});
        JogoRenderer(&status_jogo_atual);
        if(!Pausado)UnpausedRenderer(&status_jogo_atual);
          if(Pausado)
        {
            if(PausadoConfig)
            {
                MenuConfig(&status_jogo_atual);
            }
            else
            {
               PausaRenderer();

            }
        }
        if(jogoAtivo==false)
        {
            EndScreenRenderer();
        }
        if(status_jogo_atual.player.vivo==false)
        {
            DeathScreenRenderer();
        }
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth*Escala))*0.5f)/Escala;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight*Escala))*0.5f)/Escala;
        virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)gameScreenWidth, (float)gameScreenHeight });
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
        (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*Escala))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*Escala))*0.5f,
        (float)gameScreenWidth*Escala, (float)gameScreenHeight*Escala }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        EndDrawing();

    }
    StopSound(titleTheme);
    CloseAudioDevice();
    UnloadRenderTexture(target);
    CloseWindow();


}
