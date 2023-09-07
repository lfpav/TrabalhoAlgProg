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
#include "raymath.h"
#include <time.h>
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
Texture2D texturaTitle;
Texture2D spriteBomba;
Texture2D spriteTrap;
Texture2D spritePortal,spriteBombExpl;
Sound pauseSound,selectionSound;
Sound unpauseSound,explosionSound;
Sound dmgSound,dmgSoundEnemy,deathSoundEnemy;
Sound titleTheme;
Font fonteTitle;
int moveDuration = 0;
bool jogoAtivo=true;
int invulnTime=0;
float tempo_i_chase =0;
float sizeMulti[4]={1,1,1,1};
Rectangle RetangulosTitle[3] = {(Rectangle){.x=225,.y=450,.width=360,.height=60},(Rectangle){.x=225,.y=550,.width=450,.height=60},{.x=225,.y=650,.width=200,.height=60}};
Rectangle RetangulosPausa[4] = {(Rectangle){.x=310,.y=200,.width=300,.height=40},(Rectangle){.x=290,.y=250,.width=300,.height=40},{.x=350,.y=300,.width=300,.height=40},{.x=290,.y=350,.width=300,.height=40}};
Rectangle RetangulosDeath[3] = {(Rectangle){.x=310,.y=200,.width=200,.height=40},{.x=350,.y=250,.width=300,.height=40},{.x=290,.y=300,.width=300,.height=40}};
bool GameStart = false;
Texture2D sapo;
time_t tempo_inicio;
float tempo_atual;
float tempo_pausado;
bool Pausado = false;
float flash_Duration = 0.5;
bool TomouDano = false;
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
     *tempo_i_chase += GetFrameTime();
     if(*tempo_i_chase>0.25)
     {
         not_Norm = Vector2Subtract(p->posplayer,Inim_ptr->posInimigo);
         Norm = Vector2Normalize(not_Norm);
         atualDir.x = Inim_ptr->dirInimigo.x;
         atualDir.y = Inim_ptr->dirInimigo.y;
         if(fabs(not_Norm.x)>30 )
         {
            Inim_ptr->dirInimigo.x = Norm.x;
         }

            if(fabs(not_Norm.y)>30)
         {

             Inim_ptr->dirInimigo.y = Norm.y;
         }

        *tempo_i_chase=0;
     }

     newPos.x = Inim_ptr->dirInimigo.x*2+Inim_ptr->posInimigo.x;
     newPos.y = Inim_ptr->posInimigo.y;

   if(PodeMoverX(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))
    {
        Inim_ptr->posInimigo.x+=Inim_ptr->dirInimigo.x*2;

    }
    newPos.y = Inim_ptr->dirInimigo.y*2+Inim_ptr->posInimigo.y;
    newPos.x = Inim_ptr->posInimigo.x;
     if(PodeMoverY(Inim_ptr->dirInimigo,Inim_ptr->posInimigo,newPos))
    {
        Inim_ptr->posInimigo.y+=Inim_ptr->dirInimigo.y*2;

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
            p->fire_Delay=0.5;
            return 1;
        }
        if(IsKeyDown(KEY_RIGHT))
        {
            p->dirShooting.x=1;
            CriaProjetil(p);
            p->fire_Delay=0.5;

            return 1;

        }
        if(IsKeyDown(KEY_DOWN))
        {
            p->dirShooting.y=1;
            CriaProjetil(p);
            p->fire_Delay=0.5;


            return 1;

        }
        if(IsKeyDown(KEY_LEFT))
        {
            p->dirShooting.x=-1;
            p->fire_Delay=0.5;

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
                    s->Inimigos[s->InimigosNaFase].tempoChase = 0;
                    s->Inimigos[s->InimigosNaFase].moveDuration= 0;
                    s->Inimigos[s->InimigosNaFase].ativo = true;
                    s->Inimigos[s->InimigosNaFase].TomouDano = false;
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
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 128 });
    DrawRectangleV((Vector2){200,50},(Vector2){500,630},GREEN);
    DrawText("PAUSED",350,100,50,BLACK);
    DrawText(TextFormat("Tempo:%.1f s",tempo_atual),0,25,30,BLACK);
    DrawText(TextFormat("Vida:%d",status_jogo_atual.player.HP),0,75,30,BLACK);
    DrawText(TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount),0,125,30,BLACK);
    DrawText(TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual),0,175,30,BLACK);
    DrawText(TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase),0,225,30,BLACK);
    DrawText("Novo Jogo - N",310,200,40*sizeMulti[0],BLACK);
    DrawText("Salvar Jogo - S",290,250,40*sizeMulti[1],BLACK);
    DrawText("Carregar Jogo - C",270,300,40*sizeMulti[2],BLACK);
    DrawText("Sair - Q",350,350,40*sizeMulti[3],BLACK);
      for(int iPausa=0;iPausa<4;iPausa++)
    {
        float firstSize = sizeMulti[iPausa];
        DrawRectangle(RetangulosPausa[iPausa].x,RetangulosPausa[iPausa].y,RetangulosPausa[iPausa].width,RetangulosPausa[iPausa].height,BLANK);
        if(CheckCollisionPointRec(GetMousePosition(),RetangulosPausa[iPausa]))
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
void JogoRenderer(STATUS *s)
{
    s->player.playerRec = (Rectangle){.x=s->player.posplayer.x,.y=s->player.posplayer.y,.width=30,.height=30};
    DrawRectangleRec(s->player.playerRec,GREEN);
    //DrawRectangleV(s->player.posplayer,(Vector2){30,30},GREEN);
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
    DrawTextureEx(sapo,(Vector2){s->player.posplayer.x-15,s->player.posplayer.y-15},0,1,s->player.spriteColor);
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

}
/* renderiza os elementos que aparecem quando o jogo esta despausado */
void UnpausedRenderer(STATUS *s)
{
    DrawText(TextFormat("Tempo:%.1f s",tempo_atual),0,500,50,BLACK);
    DrawText(TextFormat("Vida:%d",status_jogo_atual.player.HP),0,550,50,BLACK);
    DrawText(TextFormat("Bombas:%d",status_jogo_atual.player.bombAmount),0,600,50,BLACK);
    DrawText(TextFormat("Pontos:%d",status_jogo_atual.pontuacao),0,650,50,BLACK);
    DrawText(TextFormat("Mapa atual:%d",status_jogo_atual.mapaAtual),0,450,50,BLACK);
    DrawText(TextFormat("Inimigos:%d",status_jogo_atual.InimigosNaFase),0,700,50,BLACK);

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
        s->player.playerProj[i].airTime=4;
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
        s->player.playerProj[i].airTime=4;
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
    projetil->bullet.posCenter.x+=projetil->dirProj.x*3;
    projetil->bullet.posCenter.y+=projetil->dirProj.y*3;

}
void LimpadorProjetil(PROJETIL *projetil)
{
    projetil->airTime-=GetFrameTime();
    if(projetil->airTime<=0)
    {
        projetil->ativo=false;
        projetil->airTime=4;
    }



}
void RenderizaProjeteis(PROJETIL projetil)
{
    DrawCircle(projetil.bullet.posCenter.x,projetil.bullet.posCenter.y,projetil.bullet.radius,RED);

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
                inim->spriteColor=PURPLE;
                inim->flash_Duration=0.3;
            }
        }
        inim->inimigoRec = (Rectangle){.x=inim->posInimigo.x,.y=inim->posInimigo.y,.width=30,.height=30};
        DrawRectangleRec(inim->inimigoRec,inim->spriteColor);

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
    DrawTextureEx(objeto->spriteObjeto,objeto->posObjeto,0,0.5,WHITE);
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
        b->obj.frameTime=0.1;
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
    DrawRectangleV((Vector2){0,0}, (Vector2){900, 750}, CLITERAL(Color){ 0, 0, 0, 128 });
    DrawRectangleV((Vector2){200,50},(Vector2){500,630},GREEN);
    DrawText("VOCE MORREU",350,100,50,BLACK);
    DrawText("Novo Jogo - N",310,200,40*sizeMulti[0],BLACK);
    DrawText("Carregar Jogo - C",270,250,40*sizeMulti[1],BLACK);
    DrawText("Sair - Q",350,300,40*sizeMulti[2],BLACK);
    for(int iDeath=0;iDeath<3;iDeath++)
    {
        float firstSize = sizeMulti[iDeath];
        DrawRectangle(RetangulosDeath[iDeath].x,RetangulosDeath[iDeath].y,RetangulosDeath[iDeath].width,RetangulosDeath[iDeath].height,BLANK);
        if(CheckCollisionPointRec(GetMousePosition(),RetangulosDeath[iDeath]))
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

/* A funcao Menu recebe um int type, 0 para o menu principal e 1 para o menu dentro do jogo, limitando as opcoes dependendo do contexto.
Dependendo da tecla apertada realiza outras funcoes de manipulacao do estado do jogo */
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
        CloseWindow();

        }
        else
        {
        if(sizeMulti[0]!=1)
        NovoJogo(&status_jogo_atual);
        if(sizeMulti[1]!=1)
        CarregaJogo(&status_jogo_atual);
        if(sizeMulti[2]!=1)
        CloseWindow();

        }

    }
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
    for(int iTitle=0;iTitle<3;iTitle++)
    {
        float firstSize = sizeMulti[iTitle];
        DrawRectangle(RetangulosTitle[iTitle].x,RetangulosTitle[iTitle].y,RetangulosTitle[iTitle].width,RetangulosTitle[iTitle].height,BLANK);
        if(CheckCollisionPointRec(GetMousePosition(),RetangulosTitle[iTitle]))
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
int main()
{
    InitWindow(900,750, "trabalho");
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
    spriteBombExpl = LoadTexture("bombsheet.png");
    spritePortal=LoadTexture("Portal.png");
    spriteBomba=LoadTexture("bomb.png");
    spriteTrap=LoadTexture("trap.png");
    SetSoundVolume(deathSoundEnemy,0.8);
    SetSoundVolume(dmgSoundEnemy,0.8);
    SetSoundVolume(titleTheme,0.15); //SOUND VOLUME EH FLOAT ENTRE 0 E 1, SE BOTAR MAIS Q ISSO VAI ESTOURAR TEUS OUVIDO
    fonteTitle = LoadFontEx("SunnyspellsRegular-MV9ze.otf",75,NULL,0);
    sapo=LoadTexture("sapo.png");
    SetExitKey(KEY_Q);
    status_jogo_atual.player.TomouDano=false;
    status_jogo_atual.player.spriteColor=WHITE;
    status_jogo_atual.player.flash_Duration=0.3;
    status_jogo_atual.player.vivo = true;
    for(int i =0;i<MAX_INIMIGOS;i++)
     {
         status_jogo_atual.Inimigos[i].spriteColor=PURPLE;
         status_jogo_atual.Inimigos[i].TomouDano=false;
         status_jogo_atual.Inimigos[i].flash_Duration=0.3;

     }
    while(!WindowShouldClose())
    {
        while(!GameStart)
        {
            if(!IsSoundPlaying(titleTheme))
            PlaySound(titleTheme);
            BeginDrawing();
            ClearBackground(WHITE);
            TitleScreen();
            tempo_inicio=time(NULL);
            EndDrawing();

        }
        if(status_jogo_atual.player.vivo&&jogoAtivo)
        {
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
                if(invulnTime>0)
                {
                    invulnTime-=1;
                }

            }
            if(Pausado)
            {
                Menu(1);
            }

        }
        if(status_jogo_atual.player.vivo==false||jogoAtivo==false)
        {
            Menu(0);
        }

        BeginDrawing();
        ClearBackground(WHITE);
        JogoRenderer(&status_jogo_atual);
        if(!Pausado)UnpausedRenderer(&status_jogo_atual);
          if(Pausado)
        {
            PausaRenderer();
        }
        if(jogoAtivo==false)
        {
            EndScreenRenderer();
        }
        if(status_jogo_atual.player.vivo==false)
        {
            DeathScreenRenderer();
        }

        EndDrawing();

    }
    StopSound(titleTheme);
    CloseAudioDevice();
    CloseWindow();


}
