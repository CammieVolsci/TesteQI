#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>

//dimensões da tela
#define LINHAS 12
#define COLUNAS 12
#define ROW 600
#define COLUMN 800
#define VERTICES_ARVORE 11

//tamanho da tela
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;
float z_buffer[ROW][COLUMN];

//variáveis globais
SDL_Window *janela = NULL;
SDL_Surface *superficie_tela = NULL;
SDL_Renderer *renderizacao = NULL;
int pontuacao = 0;

//criando matrizes para as transformações
typedef double Matriz[3][3];
typedef double Matriz_vetor[3];

typedef double Matriz4[4][4];
typedef double Matriz4_vetor[4];

//estrutura de dados de vértices
typedef struct vertice Vertice;
struct vertice
{
    int x;
    int y;
    int z;
};

//estrutura de dados das faces
typedef struct face Face;
struct face
{
    Vertice v[VERTICES_ARVORE];
};

//vetor
typedef struct vetor2D Vetor2D;
struct vetor2D
{
    int x;
    int y;
    int z;
};

int criar_tela();       //inicia o SDL e cria a tela
void fechar_tela();     //fecha o SDL e fecha a tela
void teste_qi();        //função principal do programa
void desenhar_texto(const char* msg);
void desenhar_linha(Vertice v1, Vertice v2); //função que desenha uma linha
void desenhar_poligono(int total_poligonos, int max_vertices, Vertice v[][max_vertices]); //desenha >todos< os polígonos
void criar_matriz_identidade(Matriz m);                                 //cria a matriz identidade para usar depois
void criar_matriz(int n, int total_poligonos, Vertice v[][COLUNAS]);    //criando as matrizes para cada fase
void multiplicar_matrizes(Matriz m1, Matriz_vetor m2);                  //multiplica matriz 3x3 por uma matriz coluna
void translacao_homogenea(int lados, Vertice v[], Vetor2D vet);
void reflexao_homogenea(int lados, Vertice v[], Vetor2D vet);
void escala_homogenea(int lados, Vertice v[], Matriz_vetor vet);
void rotacao_homogenea(int lados, Vertice v[], double t);
void translacao_centro(int lados, Vertice v[], Vetor2D vet);
void fase1(int n, Vertice v[][n]);
void fase2(int n, Vertice v[][n]);
void fase3(int n, Vertice v[][n]);
void fase4(int n, Vertice v[][n]);
void fase5(int n, Vertice v[][n]);
Vetor2D max_vertice(int lados, Vertice v[]);
Vetor2D min_vertice(int lados, Vertice v[]);

// funções usadas para pintar poligono
int max(int x1, int x2, int x3);
int min(int x1, int x2, int x3);
int equacao_reta(Vetor2D v1, Vetor2D v2, int x, int y);
void pintar_poligono(Vertice v1, Vertice v2, Vertice v3);

// funções para desenhar isométrico
void criar_matriz_identidade4(Matriz4 m);
void multiplicar_matrizes4(Matriz4 m1, Matriz4 m2);
void rotacao_matriz(Vertice *v);
void criar_arvore(Face faces[]);
void criar_arvore_colorida(Face faces[]);
void desenhar_arvore(Face faces[]);
int equacao_reta_arvore(Vertice v1, Vertice v2, int x, int y);
float calcular_z_depth(Vertice v1, Vertice v2, Vertice v3, int x, int y);
float area_triangulo(Vertice v1, Vertice v2, Vertice v3);
void arvore_loop(Vertice v1, Vertice v2, Vertice v3, SDL_Color cor);
void pintar_arvore(Face faces[]);
void desenhar_linha_arvore(Vertice v1, Vertice v2, SDL_Color cor);

int main(int argc, char *args[])
{
    setlocale(LC_ALL, "Portuguese");
    printf("\nTrabalho 1 de implementação de Computação Gráfica\nTeste de QI -\n\n");
    printf("\nAperte as teclas (em ordem das figuras)\n'a' \t 'b'\n'c' \t 'd'\npara responder.\n\n");
    printf("\nO teste tem 5 perguntas, cada resposta certa vale 100 pontos.\nTotal máximo de 500 pontos.\n");
    teste_qi();

    printf("\n\nPontuação no teste de QI: %d pontos.\n", pontuacao);
    getchar();

    return 0;
}

void teste_qi()
{
    // inicializa Z-Buffer com maxima profundidade
    for(int i=0;i<ROW; i++)
        for(int j=0;j<COLUMN;j++)
            z_buffer[i][j] = 10;

    Vertice v[LINHAS][COLUNAS] = {0};
    Face arvore[2];

    int fase = 0;
    int ok = 1;

    if(!criar_tela())       //começa o SDL e cria a tela
        printf("\n\nFalha a inicializar!\n");
    else
    {
        int sair = 0;       //flag principal do loop
        SDL_Event evento;   //event handler
        while(!sair)                            //enquanto a aplicação estiver rodando
        {

            while(SDL_PollEvent(&evento)!=0)    //lida com eventos na fila
            {
                if(evento.type==SDL_QUIT)       //usuário pede para sair
                    sair = 1;
                else if(evento.type==SDL_KEYDOWN)
                {
                        switch(evento.key.keysym.sym)
                        {
                        case SDLK_a:
                            fase++;
                            pontuacao +=100;
                            ok = 1;
                            break;

                        case SDLK_b:
                        case SDLK_c:
                        case SDLK_d:
                            fase++;
                            ok = 1;
                            break;

                        case SDLK_ESCAPE:
                            sair = 1;
                            break;

                        default:
                            break;
                        }
                }
            }

            if(fase==0 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao, 0xFF, 0xFF, 0xFF, 0x00);
                SDL_RenderClear(renderizacao);
                criar_arvore(arvore);

                for (int i=0;i<2;i++)
                {
                    for (int j=0;j<VERTICES_ARVORE;j++)
                    {
                        arvore[i].v[j].x *= 5;
                        arvore[i].v[j].y *= 5;
                        arvore[i].v[j].z *= 5;

                        arvore[i].v[j].x += 150;
                        arvore[i].v[j].y += 300;

                        rotacao_matriz(&arvore[i].v[j]);
                    }
                }

                ok = 0;
            }

            if(fase==1 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao,0xFF,0xFF,0xFF,0x00);
                SDL_RenderClear(renderizacao);
                criar_matriz(fase, LINHAS, v);
                fase1(COLUNAS, v);
                ok = 0;
            }
            else if(fase==2 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao,0xFF,0xFF,0xFF,0x00);
                SDL_RenderClear(renderizacao);
                criar_matriz(fase, LINHAS, v);
                fase2(COLUNAS, v);
                ok = 0;
            }
            else if(fase==3 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao,0xFF,0xFF,0xFF,0x00);
                SDL_RenderClear(renderizacao);
                criar_matriz(fase, LINHAS, v);
                fase3(COLUNAS, v);
                ok = 0;
            }
            else if(fase==4 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao,0xFF,0xFF,0xFF,0x00);
                SDL_RenderClear(renderizacao);
                criar_matriz(fase, LINHAS, v);
                fase4(COLUNAS, v);
                ok = 0;
            }
            else if(fase==5 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao,0xFF,0xFF,0xFF,0x00);
                SDL_RenderClear(renderizacao);
                criar_matriz(fase, LINHAS, v);
                fase5(COLUNAS, v);
                ok = 0;
            }
            else if(fase==6 && ok==1)
            {
                SDL_SetRenderDrawColor(renderizacao, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderizacao);
                criar_arvore_colorida(arvore);

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < VERTICES_ARVORE; j++)
                    {
                        rotacao_matriz(&arvore[i].v[j]);
                    }
                }
                pintar_arvore(arvore);

                ok == 0;
            }
            else if(fase==7)
                sair = 1;

            if (fase == 0 || fase == 6 || fase == 7)
            {
                desenhar_arvore(arvore);
                pintar_arvore(arvore);
            }
            else
                desenhar_poligono(LINHAS, COLUNAS, v);

            SDL_RenderPresent(renderizacao);
        }
    }
    fechar_tela();

}

void criar_matriz(int n, int total_poligonos, Vertice v[][COLUNAS])
{
    int i , j;
    i = j = 0;
    FILE *arq;

    if(n==1)
    {
        arq = fopen("fase1.txt","r");
        if(arq==NULL)
        {
            printf("\nErro ao abrir o arquivo dos polígonos.\n");
            exit(1);
        }
        //lendo os polígonos de um arquivo e colocando direto na matriz
        //cada linha é um polígono e cada coluna é um vértice
        while((fscanf(arq,"%d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d;\n",
                      &v[i][0].x, &v[i][0].y, &v[i][1].x, &v[i][1].y,  &v[i][2].x,  &v[i][2].y,  &v[i][3].x,  &v[i][3].y,
                      &v[i][4].x, &v[i][4].y, &v[i][5].x, &v[i][5].y,  &v[i][6].x,  &v[i][6].y,  &v[i][7].x,  &v[i][7].y,
                      &v[i][8].x, &v[i][8].y, &v[i][9].x, &v[i][9].y,  &v[i][10].x, &v[i][10].y, &v[i][11].x, &v[i][11].y)!=EOF))
        {
            i++;
        }
    }

    if(n==2)
    {
        arq = fopen("fase2.txt","r");
        if(arq==NULL)
        {
            printf("\nErro ao abrir o arquivo dos polígonos.\n");
            exit(1);
        }
        //lendo os polígonos de um arquivo e colocando direto na matriz
        //cada linha é um polígono e cada coluna é um vértice
        while((fscanf(arq,"%d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d;\n",
                      &v[i][0].x, &v[i][0].y, &v[i][1].x, &v[i][1].y,  &v[i][2].x,  &v[i][2].y,  &v[i][3].x,  &v[i][3].y,
                      &v[i][4].x, &v[i][4].y, &v[i][5].x, &v[i][5].y,  &v[i][6].x,  &v[i][6].y,  &v[i][7].x,  &v[i][7].y,
                      &v[i][8].x, &v[i][8].y, &v[i][9].x, &v[i][9].y,  &v[i][10].x, &v[i][10].y, &v[i][11].x, &v[i][11].y)!=EOF))
        {
            i++;
        }
    }

    if(n==3)
    {
        arq = fopen("fase3.txt","r");
        if(arq==NULL)
        {
            printf("\nErro ao abrir o arquivo dos polígonos.\n");
            exit(1);
        }
        //lendo os polígonos de um arquivo e colocando direto na matriz
        //cada linha é um polígono e cada coluna é um vértice
        while((fscanf(arq,"%d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d;\n",
                      &v[i][0].x, &v[i][0].y, &v[i][1].x, &v[i][1].y,  &v[i][2].x,  &v[i][2].y,  &v[i][3].x,  &v[i][3].y,
                      &v[i][4].x, &v[i][4].y, &v[i][5].x, &v[i][5].y,  &v[i][6].x,  &v[i][6].y,  &v[i][7].x,  &v[i][7].y,
                      &v[i][8].x, &v[i][8].y, &v[i][9].x, &v[i][9].y,  &v[i][10].x, &v[i][10].y, &v[i][11].x, &v[i][11].y)!=EOF))
        {
            i++;
        }
    }

    if(n==4)
    {
        arq = fopen("fase4.txt","r");
        if(arq==NULL)
        {
            printf("\nErro ao abrir o arquivo dos polígonos.\n");
            exit(1);
        }
        //lendo os polígonos de um arquivo e colocando direto na matriz
        //cada linha é um polígono e cada coluna é um vértice
        while((fscanf(arq,"%d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d;\n",
                      &v[i][0].x, &v[i][0].y, &v[i][1].x, &v[i][1].y,  &v[i][2].x,  &v[i][2].y,  &v[i][3].x,  &v[i][3].y,
                      &v[i][4].x, &v[i][4].y, &v[i][5].x, &v[i][5].y,  &v[i][6].x,  &v[i][6].y,  &v[i][7].x,  &v[i][7].y,
                      &v[i][8].x, &v[i][8].y, &v[i][9].x, &v[i][9].y,  &v[i][10].x, &v[i][10].y, &v[i][11].x, &v[i][11].y)!=EOF))
        {
            i++;
        }
    }

    if(n==5)
    {
        arq = fopen("fase5.txt","r");
        if(arq==NULL)
        {
            printf("\nErro ao abrir o arquivo dos polígonos.\n");
            exit(1);
        }
        //lendo os polígonos de um arquivo e colocando direto na matriz
        //cada linha é um polígono e cada coluna é um vértice
        while((fscanf(arq,"%d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d; %d %d;\n",
                      &v[i][0].x, &v[i][0].y, &v[i][1].x, &v[i][1].y,  &v[i][2].x,  &v[i][2].y,  &v[i][3].x,  &v[i][3].y,
                      &v[i][4].x, &v[i][4].y, &v[i][5].x, &v[i][5].y,  &v[i][6].x,  &v[i][6].y,  &v[i][7].x,  &v[i][7].y,
                      &v[i][8].x, &v[i][8].y, &v[i][9].x, &v[i][9].y,  &v[i][10].x, &v[i][10].y, &v[i][11].x, &v[i][11].y)!=EOF))
        {
            i++;
        }
    }

    for(i=0;i<total_poligonos;i++)
        for(j=0;j<COLUNAS;j++)
            v[i][j].z = 1;

    fclose(arq);
}

int criar_tela()
{
    int funciona = 1; //flag para iniciar o SDL

    if(SDL_Init(SDL_INIT_VIDEO)<0) //inicializa o SDL e testa se funciona
    {
        printf("\n\nO SDL não pode inicializar! SDL_ERROR: %s\n", SDL_GetError());
        funciona = 0;
    }
    else
    {
        //cria a janela
        janela = SDL_CreateWindow("Teste de QI!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

        if(janela==NULL)
        {
            printf("\n\nA janela não pode ser criada! SLD_ERROR: %s\n", SDL_GetError());
            funciona = 0;
        }
        else //pega a superfície da tela
        {
            renderizacao = SDL_CreateRenderer(janela,-1,SDL_RENDERER_ACCELERATED); //cria renderizacao para janela

            if(renderizacao==NULL )
			{
				printf("\n\nNão pode ser renderizado! SDL Error: %s\n",SDL_GetError());
				funciona = 0;
			}
			else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderizacao, 0xFF, 0xFF, 0xFF, 0xFF );
                superficie_tela = SDL_GetWindowSurface(janela);
            }
        }
    }
    return funciona;

}

void fechar_tela()
{
    //TTF_Quit();
    SDL_DestroyRenderer(renderizacao);
    SDL_DestroyWindow(janela);
    janela = NULL;
    renderizacao = NULL;

    SDL_Quit();
}

void desenhar_linha(Vertice v1, Vertice v2)
{
    SDL_SetRenderDrawColor(renderizacao,0x00,0x00,0xFF,0xFF);
    SDL_RenderDrawLine(renderizacao,v1.x,v1.y,v2.x,v2.y);
}

void desenhar_poligono(int total_poligonos, int max_vertices, Vertice v[][max_vertices])
{
    //o total_poligonos são as linhas da matriz e max_vertices são as colunas

    for(int i=0;i<total_poligonos;i++)
    {
        for(int j=0;j<max_vertices;j++)
        {
            if((j+1<max_vertices) && (j!=0) && (v[i][j+1].x==0) && (v[i][j+1].y==0)) //checo se não estou na última coluna de vértices e se não é o primeiro vértice
            {                                                               //e se não é os zeros extras no arquivo da matriz
                desenhar_linha(v[i][j],v[i][0]);
                break;
            }
            else if(j+1==max_vertices)                       //desenha a linha entre o último vértice e o primeiro vértice
                desenhar_linha(v[i][j],v[i][0]);
            else                                    //desenha a linha entre dois vértices
                desenhar_linha(v[i][j],v[i][j+1]);
        }
    }
}

void criar_matriz_identidade(Matriz m)
{
    //crio a matriz identidade para servir de base para as matrizes de transformações
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(i==j)
                m[i][j] = 1;
            else
                m[i][j] = 0;
        }
    }
}

void multiplicar_matrizes(Matriz m1, Matriz_vetor m2)
{
    //função para multiplicar a matriz (de transformação) pela matriz coluna dos vértices do polígono
    Matriz_vetor temp;

    for(int i=0;i<3;i++)
            temp[i] = m1[i][0]*m2[0] + m1[i][1]*m2[1] + m1[i][2]*m2[2];

    for(int i=0;i<3;i++)
            m2[i] = temp[i];

}

void translacao_homogenea(int lados, Vertice v[], Vetor2D vet)
{
    Matriz m;
    criar_matriz_identidade(m);

    //aqui coloco os valores do vetor de translação na matriz
    m[0][2] = vet.x;
    m[1][2] = vet.y;

    for(int i=0;i<lados;i++)
    {
        Matriz_vetor vet2;

        if(i<(lados-1) && (i!=0))                   //para não transladar os vertices 00 dentro do vetor de vértices e criar vértices novos
             if((v[i].x==0) && (v[i].y==0) && (v[i+1].x==0) && (v[i+1].y==0))
                break;

        if((i+1)>=lados  && (v[i].x==0) && (v[i].y==0)) //porque os hexágonos e bandeirinhas não funcionaram
            continue;

        vet2[0] = v[i].x;
        vet2[1] = v[i].y;
        vet2[2] = v[i].z;

        multiplicar_matrizes(m,vet2);

        v[i].x = vet2[0];
        v[i].y = vet2[1];
        v[i].z = vet2[2];
    }
}

void reflexao_homogenea(int lados, Vertice v[], Vetor2D vet)
{
    Matriz m;
    Vetor2D v2max, v2min, v2centro;
    criar_matriz_identidade(m);

    //aqui coloco os valores de reflexão (-1 ou 1) na matriz
    m[0][0] = vet.x;
    m[1][1] = vet.y;

    v2max = max_vertice(lados, v);
    v2min = min_vertice(lados, v);

    //defininindo o ponto médio ("centro de massa") do quadrado ao redor do polígono
    //deixando negativo para traze-los a posição (0,0)
    v2centro.x = -(v2max.x + v2min.x)/2;
    v2centro.y = -(v2max.y + v2min.y)/2;
    v2centro.z = 1;

    //translado ao ponto (0,0)
    translacao_centro(lados, v, v2centro);

    for(int i=0;i<lados;i++)
    {
        Matriz_vetor vet2;

        vet2[0] = v[i].x;
        vet2[1] = v[i].y;
        vet2[2] = v[i].z;

        multiplicar_matrizes(m,vet2);

        v[i].x = vet2[0];
        v[i].y = vet2[1];
        v[i].z = vet2[2];
    }

    v2centro.x *= -1;
    v2centro.y *= -1;

    //translado de volto a posição original
    translacao_centro(lados, v, v2centro);
}

void escala_homogenea(int lados, Vertice v[], Matriz_vetor vet)
{
    Matriz m;
    criar_matriz_identidade(m);

    //aqui coloco os valores que quero fazer escala na diagonal da matriz
    m[0][0] = vet[0];
    m[1][1] = vet[1];

    for(int i=0;i<lados;i++)
    {
        Matriz_vetor vet2;

        //passo todos os valores de cada vértice para a matriz coluna que vai multiplicar pela matriz transformação
        vet2[0] = v[i].x;
        vet2[1] = v[i].y;
        vet2[2] = v[i].z;

        multiplicar_matrizes(m,vet2);

        //aqui coloco os valores resultado de volta nos vértices
        v[i].x = vet2[0];
        v[i].y = vet2[1];
        v[i].z = vet2[2];
    }

}

void rotacao_homogenea(int lados, Vertice v[], double t)
{
    Matriz m;
    Vetor2D v2max, v2min, v2centro;
    criar_matriz_identidade(m);

    //aqui efetivamente coloco a matriz de rotação na matriz identidade que tinha feito anteriormente
    //colocando os valores de sen e cos nos lugares certps
    m[0][0] = cos(t);
    m[0][1] = sin(t);
    m[1][0] = -sin(t);
    m[1][1] = cos(t);

    v2max = max_vertice(lados, v);
    v2min = min_vertice(lados, v);

    v2centro.x = -(v2max.x + v2min.x)/2;
    v2centro.y = -(v2max.y + v2min.y)/2;
    v2centro.z = 1;

    translacao_centro(lados, v, v2centro);

    for(int i=0;i<lados;i++)
    {
        Matriz_vetor vet;

        vet[0] = v[i].x;
        vet[1] = v[i].y;
        vet[2] = v[i].z;

        multiplicar_matrizes(m,vet);

        v[i].x = vet[0];
        v[i].y = vet[1];
        v[i].z = vet[2];
    }

    v2centro.x *= -1;
    v2centro.y *= -1;

    translacao_centro(lados, v, v2centro);

}

void translacao_centro(int lados, Vertice v[], Vetor2D vet)
{
    translacao_homogenea(lados, v, vet);
}

Vetor2D  max_vertice(int lados, Vertice v[])
{
    Vetor2D max;
    max.x = -10000;
    max.y = -10000;

    for(int i=0; i<lados; i++)
    {
        if(v[i].x >= max.x)
            max.x = v[i].x;

        if(v[i].y >= max.y)
            max.y = v[i].y;
    }
    return max;
}

Vetor2D min_vertice(int lados, Vertice v[])
{
    Vetor2D min;
    min.x = 10000;
    min.y = 10000;

    for(int i=0; i<lados; i++)
    {
        if(v[i].x == 0 && v[i].y == 0 && i!=0)
            break;

        if(v[i].x <= min.x)
            min.x = v[i].x;

        if(v[i].y <= min.y)
            min.y = v[i].y;
    }
    return min;
}

void fase1(int n, Vertice v[][n])
{
    Vetor2D vet;
    double grau_rad;
    Matriz_vetor m_vet;

    for(int i=0;i<LINHAS;i++)
    {
        vet.x = 100;
        vet.x += ((i%12)>7) ? (i%2)*300 : (i%3)*150;
        vet.y = 100;
        vet.y += (i%12>9) ? 560 : ((i%10)>7) ? 450 :((i%8)>5) ? 300 : ((i%6)>2) ? 150 : 0;

        translacao_homogenea(COLUNAS,v[i],vet);
    }

    //fazendo todas as reflexões da fase
    vet.x = 1;
    vet.y = -1;

    reflexao_homogenea(COLUNAS,v[1],vet);
    reflexao_homogenea(COLUNAS,v[2],vet);
    reflexao_homogenea(COLUNAS,v[11],vet);

    //fazendo os "losangos" usando quadrados
    vet.x = 1;
    vet.y = 1;
    grau_rad = M_PI_4;

    rotacao_homogenea(COLUNAS,v[0],grau_rad);
    rotacao_homogenea(COLUNAS,v[3],grau_rad);
    rotacao_homogenea(COLUNAS,v[6],grau_rad);

    //fazendo as rotações
    grau_rad =-M_PI_2;

    rotacao_homogenea(COLUNAS,v[4],grau_rad);
    rotacao_homogenea(COLUNAS,v[5],grau_rad);
    rotacao_homogenea(COLUNAS,v[10],grau_rad);

    //fazendo rotação da bandeirinha letra c
    grau_rad = M_PI * 0.75;

    rotacao_homogenea(COLUNAS,v[9],grau_rad);

    //fazendo as escalas
    m_vet[0] = 0.6;
    m_vet[1] = 1;
    m_vet[2] = 1;

    vet.x = 60;
    vet.y = 0;

    escala_homogenea(COLUNAS,v[0],m_vet);
    translacao_homogenea(COLUNAS,v[0],vet);
    escala_homogenea(COLUNAS,v[6],m_vet);
    translacao_homogenea(COLUNAS,v[6],vet);

    //fazendo a escala do losango deitado
    m_vet[0] = 1;
    m_vet[1] = 0.6;
    m_vet[2] = 1;

    vet.x = 0;
    vet.y = 120;

    escala_homogenea(COLUNAS,v[3],m_vet);
    translacao_homogenea(COLUNAS,v[3],vet);

}

void fase2(int n, Vertice v[][n])
{
    Vetor2D vet;
    double grau_rad;
    Matriz_vetor m_vet;

    for(int i=0;i<LINHAS;i++)
    {
        vet.x = 100;
        vet.x += ((i%12)>7) ? (i%2)*300 : (i%3)*150;
        vet.y = 100;
        vet.y += (i%12>9) ? 560 : ((i%10)>7) ? 450 :((i%8)>5) ? 300 : ((i%6)>2) ? 150 : 0;

        translacao_homogenea(COLUNAS, v[i], vet);
    }

    grau_rad = -M_PI_2;

    rotacao_homogenea(COLUNAS,v[0],grau_rad);
    rotacao_homogenea(COLUNAS,v[7],grau_rad);

    grau_rad = M_PI_2;

    rotacao_homogenea(COLUNAS,v[2],grau_rad);
    rotacao_homogenea(COLUNAS,v[9],grau_rad);

    grau_rad = M_PI;

    rotacao_homogenea(COLUNAS,v[3],grau_rad);
    rotacao_homogenea(COLUNAS,v[5],grau_rad);

    m_vet[0] = 0.5;
    m_vet[1] = 0.5;
    m_vet[2] = 1;

    vet.x = 240;
    vet.y = 350;

    escala_homogenea(COLUNAS,v[11],m_vet);
    translacao_homogenea(COLUNAS,v[11],vet);

}

void fase3(int n, Vertice v[][n])
{
    Vetor2D vet;
    double grau_rad;
    Matriz_vetor m_vet;

    for(int i=0;i<LINHAS;i++)
    {
        vet.x = 100;
        vet.x += ((i%12)>7) ? (i%2)*300 : (i%3)*150;
        vet.y = 100;
        vet.y += (i%12>9) ? 560 : ((i%10)>7) ? 450 :((i%8)>5) ? 300 : ((i%6)>2) ? 150 : 0;

        translacao_homogenea(COLUNAS, v[i], vet);
    }

    vet.x = 1;
    vet.y = -1;

    reflexao_homogenea(COLUNAS,v[10],vet);

    grau_rad = M_PI_2;

    rotacao_homogenea(COLUNAS,v[0],grau_rad);

    grau_rad = M_PI_4;

    rotacao_homogenea(COLUNAS,v[1],grau_rad);
    rotacao_homogenea(COLUNAS,v[4],grau_rad);
    rotacao_homogenea(COLUNAS,v[11],grau_rad);

    grau_rad = -M_PI_4;

    rotacao_homogenea(COLUNAS,v[7],grau_rad);

    grau_rad = -M_PI_2;

    rotacao_homogenea(COLUNAS,v[5],grau_rad);

    m_vet[0] = 0.6;
    m_vet[1] = 1;
    m_vet[2] = 1;

    vet.x = 60;
    vet.y = 0;

    escala_homogenea(COLUNAS,v[6],m_vet);
    translacao_homogenea(COLUNAS,v[6],vet);

    m_vet[0] = 1;
    m_vet[1] = 0.7;
    m_vet[2] = 1;

    vet.x = 0;
    vet.y = 180;

    escala_homogenea(COLUNAS,v[8],m_vet);
    translacao_homogenea(COLUNAS,v[8],vet);
}

void fase4(int n, Vertice v[][n])
{
    Vetor2D vet;

    for(int i=0;i<LINHAS;i++)
    {
        vet.x = 100;
        vet.x += ((i%12)>7) ? (i%2)*300 : (i%3)*150;
        vet.y = 100;
        vet.y += (i%12>9) ? 560 : ((i%10)>7) ? 450 :((i%8)>5) ? 300 : ((i%6)>2) ? 150 : 0;

        translacao_homogenea(COLUNAS, v[i], vet);
    }

    for(int i=0;i<LINHAS;i++)
    {
        for(int j=0;j<4;j++)
        {
            if(j>=2)
            {
                if(i==0 || i==4 || i==8 || i==9)
                    SDL_SetRenderDrawColor(renderizacao, 0xFF, 0x00, 0x00, 0xFF);
                else if(i==1 || i==5 || i==6 || i==10 || i==11)
                    SDL_SetRenderDrawColor(renderizacao, 0x00, 0xFF, 0x00, 0xFF);
                else
                    SDL_SetRenderDrawColor(renderizacao, 0x00, 0x00, 0xFF, 0xFF);

                pintar_poligono(v[i][5], v[i][j], v[i][j+1]);
            }
        }
    }

    for(int i=0; i<LINHAS; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(j <= 1)
            {
                if(i==1 || i==5 || i==6 || i==11)
                    SDL_SetRenderDrawColor(renderizacao, 0xFF, 0x00, 0x00, 0xFF);
                else if(i==2 || i==3 || i==7 || i==9)
                    SDL_SetRenderDrawColor(renderizacao, 0x00, 0xFF, 0x00, 0xFF);
                else
                    SDL_SetRenderDrawColor(renderizacao, 0x00, 0x00, 0xFF, 0xFF);

                pintar_poligono(v[i][5], v[i][j], v[i][j+1]);
            }
        }
    }
}

void fase5(int n, Vertice v[][n])
{
    Vetor2D vet;
    Matriz_vetor m_vet;

    for(int i=0;i<LINHAS;i++)
    {
        vet.x = 100;
        vet.x += ((i%12)>7) ? (i%2)*300 : (i%3)*150;
        vet.y = 100;
        vet.y += (i%12>9) ? 560 : ((i%10)>7) ? 450 :((i%8)>5) ? 300 : ((i%6)>2) ? 150 : 0;

        translacao_homogenea(COLUNAS, v[i], vet);
    }

    vet.x = 75;
    vet.y = 0;

    m_vet[0] = 0.5;
    m_vet[1] = 1;
    m_vet[2] = 1;

    escala_homogenea(COLUNAS,v[0],m_vet);
    translacao_homogenea(COLUNAS,v[0],vet);

    vet.x = 155;
    vet.y = 0;

    escala_homogenea(COLUNAS,v[7],m_vet);
    translacao_homogenea(COLUNAS,v[7],vet);

    vet.x = 225;
    vet.y = 0;

    escala_homogenea(COLUNAS,v[5],m_vet);
    translacao_homogenea(COLUNAS,v[5],vet);
    escala_homogenea(COLUNAS,v[9],m_vet);
    translacao_homogenea(COLUNAS,v[9],vet);

    m_vet[0] = 1;
    m_vet[1] = 0.5;
    m_vet[2] = 1;

    vet.x = 0;
    vet.y = 100;

    escala_homogenea(COLUNAS,v[1],m_vet);
    translacao_homogenea(COLUNAS,v[1],vet);

    vet.x = 0;
    vet.y = 175;

    escala_homogenea(COLUNAS,v[3],m_vet);
    translacao_homogenea(COLUNAS,v[3],vet);

    vet.x = 0;
    vet.y = 325;

    escala_homogenea(COLUNAS,v[8],m_vet);
    translacao_homogenea(COLUNAS,v[8],vet);

    vet.x = 0;
    vet.y = 380;

    escala_homogenea(COLUNAS,v[11],m_vet);
    translacao_homogenea(COLUNAS,v[11],vet);

    for(int i = 0; i < LINHAS; i++)
    {
        for(int j = 1; j < COLUNAS; j++)
        {
            if(i==0 || i==4 || i==8)
                SDL_SetRenderDrawColor(renderizacao, 0xFF, 0x00, 0x00, 0xFF);
            else if(i==1 || i==5 || i==6 || i==9)
                SDL_SetRenderDrawColor(renderizacao, 0x00, 0xFF, 0x00, 0xFF);
            else
                SDL_SetRenderDrawColor(renderizacao, 0x00, 0x00, 0xFF, 0xFF);

            pintar_poligono(v[i][0], v[i][j], v[i][j+1]);
        }
    }
}

void pintar_poligono(Vertice v1, Vertice v2, Vertice v3)
{
    Vetor2D vet1, vet2, vet3;

    //cria os 3 vetores baseando nos valores dos vértices do polígono
    vet1.x = v1.x;
    vet1.y = v1.y;
    vet2.x = v2.x;
    vet2.y = v2.y;
    vet3.x = v3.x;
    vet3.y = v3.y;

    if((vet2.x!=0) && (vet2.y!=0) && (vet3.x!=0) && (vet3.y!=0))
    {
        //cria um "retângulo" que limita o polígono, tanto no x quanto no y
        //assim, no loop, o i e j só precisam andar dentro desse "rentângulo" e não por toda a janela gráfica
        int xmin = min(v1.x,v2.x,v3.x);
        int xmax = max(v1.x,v2.x,v3.x);

        int ymin = min(v1.y,v2.y,v3.y);
        int ymax = max(v1.y,v2.y,v3.y);

        for(int i=xmin;i<=xmax;i+=1)
        {
            for(int j=ymin;j<=ymax;j+= 1)
            {
                  //o if checa para ver se o ponto (i,j) está ou não dentro do polígono
                  //se ele estiver, então para as três equações do triângulo formado por 2 lados do polígono
                  //+ 1 lado que é formado por ligar um vertice fixo(o v[0] de cada poligono) com o último vertice usado no argumento.
                  //O produto_vetorial deve retornar <= 0, pois os vertices estão indo numa ordem anti-horaria
                    if(equacao_reta(vet1,vet2,i,j) <= 0 && equacao_reta(vet2,vet3,i,j) <= 0
                    && equacao_reta(vet3,vet1, i,j) <= 0)
                        SDL_RenderDrawPoint(renderizacao, i, j);
            }
        }
    }
}

int max(int x1, int x2, int x3)
{

    if((x1>=x2) && (x1>=x3))
        return x1;
    else if((x2>=x3) && (x2>=x1))
        return x2;
    else
        return x3;
}

int min(int x1, int x2, int x3)
{

    if((x1<=x2) && (x1<=x3))
        return x1;
    else if((x2<=x3) && (x2<=x1))
        return x2;
    else
        return x3;
}

int equacao_reta(Vetor2D v1, Vetor2D v2, int x, int y)
{
    //o "produto vetorial" de R2 é usado para equação da reta que liga dois vértices
    //de um lado do poligono, é "basicamente" a equação de um dos lados do poligono
    int A = v1.y - v2.y;
    int B = v2.x - v1.x;
    int C = v1.x*v2.y - v1.y*v2.x;

    return A*x + B*y + C;
}

void criar_matriz_identidade4(Matriz4 m)
{
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            if (i==j)
                m[i][j] = 1;
            else
                m[i][j] = 0;
        }
    }

}

void multiplicar_matrizes4(Matriz4 m1, Matriz4 m2)
{
    Matriz4 temp;

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[i][j] = m1[i][0]*m2[0][j] + m1[i][1]*m2[1][j] + m1[i][2]*m2[2][j] + m1[i][3]*m2[3][j];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            m1[i][j] = temp[i][j];
}

void multiplicar_matriz4_vetor4(Matriz4 m, Matriz4_vetor v)
{
    Matriz4_vetor temp;

    for(int i=0;i<4;i++)
        temp[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2] + m[i][3]*v[3];

    for(int i=0;i<4;i++)
        v[i] = temp[i];
}

void rotacao_matriz(Vertice *v)
{
    Matriz4 m1;
    criar_matriz_identidade4(m1);

    Matriz4 m2;
    criar_matriz_identidade4(m2);

    m1[0][0] = cos(M_PI_4);
    m1[0][2] = -sin(M_PI_4);
    m1[2][0] = sin(M_PI_4);
    m1[2][2] = cos(M_PI_4);

    m2[1][1] = cos(35.26 * (M_PI/180));
    m2[1][2] = -sin(35.26 * (M_PI/180));
    m2[2][1] = sin(35.26 * (M_PI/180));
    m2[2][2] = cos(35.26 * (M_PI/180));

    Matriz4 m3;
    criar_matriz_identidade4(m3);
    m3[2][2] = 0;

    Matriz4_vetor ve;
    ve[0] = v->x;
    ve[1] = v->y;
    ve[2] = v->z;
    ve[3] = 1;

    multiplicar_matrizes4(m2,m1);
    multiplicar_matrizes4(m3,m2);
    multiplicar_matriz4_vetor4(m3,ve);

    v->x = ve[0];
    v->y = ve[1];
    v->z = ve[2];
}

void criar_arvore(Face faces[])
{
    int i = 0;
    FILE *arq;
    arq = fopen("arvore.txt","r");

    if(arq==NULL)
    {
        printf("\nErro ao abrir o arquivo dos polígonos.\n");
        exit(1);
    }

    while((fscanf(arq, "%d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d;\n", &faces[i].v[0].x, &faces[i].v[0].y, &faces[i].v[0].z, &faces[i].v[1].x, &faces[i].v[1].y, &faces[i].v[1].z, &faces[i].v[2].x, &faces[i].v[2].y, &faces[i].v[2].z, &faces[i].v[3].x, &faces[i].v[3].y, &faces[i].v[3].z, &faces[i].v[4].x, &faces[i].v[4].y, &faces[i].v[4].z, &faces[i].v[5].x, &faces[i].v[5].y, &faces[i].v[5].z, &faces[i].v[6].x, &faces[i].v[6].y, &faces[i].v[6].z, &faces[i].v[7].x, &faces[i].v[7].y, &faces[i].v[7].z, &faces[i].v[8].x, &faces[i].v[8].y, &faces[i].v[8].z, &faces[i].v[9].x, &faces[i].v[9].y, &faces[i].v[9].z, &faces[i].v[10].x, &faces[i].v[10].y, &faces[i].v[10].z) != EOF))
    {
        i++;
    }

    fclose(arq);
}

void criar_arvore_colorida(Face faces[])
{
    int i = 0;
    FILE *arq;
    arq = fopen("arvorecolorida.txt","r");

    if(arq==NULL)
    {
        printf("\nErro ao abrir o arquivo dos polígonos.\n");
        exit(1);
    }

    while((fscanf(arq, "%d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d; %d %d %d;\n", &faces[i].v[0].x, &faces[i].v[0].y, &faces[i].v[0].z, &faces[i].v[1].x, &faces[i].v[1].y, &faces[i].v[1].z, &faces[i].v[2].x, &faces[i].v[2].y, &faces[i].v[2].z, &faces[i].v[3].x, &faces[i].v[3].y, &faces[i].v[3].z, &faces[i].v[4].x, &faces[i].v[4].y, &faces[i].v[4].z, &faces[i].v[5].x, &faces[i].v[5].y, &faces[i].v[5].z, &faces[i].v[6].x, &faces[i].v[6].y, &faces[i].v[6].z, &faces[i].v[7].x, &faces[i].v[7].y, &faces[i].v[7].z, &faces[i].v[8].x, &faces[i].v[8].y, &faces[i].v[8].z, &faces[i].v[9].x, &faces[i].v[9].y, &faces[i].v[9].z, &faces[i].v[10].x, &faces[i].v[10].y, &faces[i].v[10].z) != EOF))
    {
        i++;
    }

    fclose(arq);
}

void desenhar_arvore(Face faces[])
{
    SDL_Color cor = {0, 0, 200, 0xFF};

    for(int i=0;i<2;i++)
    {
        for(int j=0;j<VERTICES_ARVORE;j++)
        {
            if (j+1==VERTICES_ARVORE)
                desenhar_linha_arvore(faces[i].v[j], faces[i].v[0], cor);
            else
                desenhar_linha_arvore(faces[i].v[j], faces[i].v[j+1], cor);
        }
    }

    for(int i=0;i<VERTICES_ARVORE;i++)
        desenhar_linha_arvore(faces[0].v[i], faces[1].v[i], cor);
}

void desenhar_linha_arvore(Vertice v1, Vertice v2, SDL_Color cor)
{
    SDL_SetRenderDrawColor(renderizacao,cor.r,cor.b,cor.g,cor.a);
    SDL_RenderDrawLine(renderizacao,v1.x,v1.y,v2.x,v2.y);
}

void pintar_arvore(Face faces[])
{

    SDL_Color cor2 = {0, 0, 200, 0xFF};
    arvore_loop(faces[1].v[10],faces[1].v[0],faces[1].v[1],cor2);
    arvore_loop(faces[1].v[10],faces[1].v[1],faces[1].v[8],cor2);
    arvore_loop(faces[1].v[10],faces[1].v[8],faces[1].v[9],cor2);
    arvore_loop(faces[1].v[1],faces[1].v[2],faces[1].v[3],cor2);
    arvore_loop(faces[1].v[1],faces[1].v[3],faces[1].v[6],cor2);
    arvore_loop(faces[1].v[1],faces[1].v[6],faces[1].v[7],cor2);
    arvore_loop(faces[1].v[1],faces[1].v[7],faces[1].v[8],cor2);
    arvore_loop(faces[1].v[3],faces[1].v[4],faces[1].v[5],cor2);
    arvore_loop(faces[1].v[3],faces[1].v[5],faces[1].v[6],cor2);

    SDL_Color cor3 = {0, 0, 150, 0xFF};
    arvore_loop(faces[0].v[10],faces[0].v[0],faces[1].v[0],cor3);
    arvore_loop(faces[0].v[10],faces[1].v[0],faces[1].v[10],cor3);
    arvore_loop(faces[0].v[0],faces[1].v[0],faces[1].v[1],cor3);
    arvore_loop(faces[0].v[0],faces[1].v[1],faces[0].v[1],cor3);
    arvore_loop(faces[0].v[1],faces[0].v[2],faces[1].v[2],cor3);
    arvore_loop(faces[0].v[1],faces[1].v[2],faces[1].v[1],cor3);
    arvore_loop(faces[0].v[3],faces[0].v[2],faces[1].v[2],cor3);
    arvore_loop(faces[0].v[3],faces[1].v[2],faces[1].v[3],cor3);
    arvore_loop(faces[0].v[3],faces[0].v[4],faces[1].v[4],cor3);
    arvore_loop(faces[0].v[3],faces[1].v[4],faces[1].v[3],cor3);
    arvore_loop(faces[0].v[5],faces[0].v[4],faces[1].v[4],cor3);
    arvore_loop(faces[0].v[5],faces[1].v[4],faces[1].v[5],cor3);
    arvore_loop(faces[0].v[6],faces[0].v[5],faces[1].v[5],cor3);
    arvore_loop(faces[0].v[6],faces[1].v[5],faces[1].v[6],cor3);
    arvore_loop(faces[0].v[7],faces[0].v[6],faces[1].v[6],cor3);
    arvore_loop(faces[0].v[7],faces[1].v[6],faces[1].v[7],cor3);
    arvore_loop(faces[0].v[8],faces[0].v[7],faces[1].v[7],cor3);
    arvore_loop(faces[0].v[8],faces[1].v[7],faces[1].v[8],cor3);
    arvore_loop(faces[0].v[9],faces[0].v[8],faces[1].v[8],cor3);
    arvore_loop(faces[0].v[9],faces[1].v[8],faces[1].v[9],cor3);
    arvore_loop(faces[0].v[10],faces[0].v[9],faces[1].v[9],cor3);
    arvore_loop(faces[0].v[10],faces[1].v[9],faces[1].v[10],cor3);

    SDL_Color cor = {0, 0, 100, 0xFF};
    arvore_loop(faces[0].v[10],faces[0].v[0],faces[0].v[1],cor);
    arvore_loop(faces[0].v[10],faces[0].v[1],faces[0].v[8],cor);
    arvore_loop(faces[0].v[10],faces[0].v[8],faces[0].v[9],cor);
    arvore_loop(faces[0].v[1],faces[0].v[2],faces[0].v[3],cor);
    arvore_loop(faces[0].v[1],faces[0].v[3],faces[0].v[6],cor);
    arvore_loop(faces[0].v[1],faces[0].v[6],faces[0].v[7],cor);
    arvore_loop(faces[0].v[1],faces[0].v[7],faces[0].v[8],cor);
    arvore_loop(faces[0].v[3],faces[0].v[4],faces[0].v[5],cor);
    arvore_loop(faces[0].v[3],faces[0].v[5],faces[0].v[6],cor);

}

void arvore_loop(Vertice v1, Vertice v2, Vertice v3, SDL_Color cor)
{
    float z_depth;

    int xmin = min(v1.x,v2.x,v3.x);
    int xmax = max(v1.x,v2.x,v3.x);

    int ymin = min(v1.y,v2.y,v3.y);
    int ymax = max(v1.y,v2.y,v3.y);

    for(int i=xmin;i<=xmax;i++)
    {
        for(int j=ymin;j<=ymax;j++)
        {
            if(equacao_reta_arvore(v1,v2,i,j) <= 0 && equacao_reta_arvore(v2,v3,i,j) <= 0
                && equacao_reta_arvore(v3,v1, i,j) <= 0)
            {
                z_depth = calcular_z_depth(v1,v2,v3,i,j);

                if(z_depth < z_buffer[i][j])
                {
                    z_buffer[i][j] = z_depth;
                    SDL_SetRenderDrawColor(renderizacao,cor.r,cor.b,cor.g,cor.a);
                    SDL_RenderDrawPoint(renderizacao, i, j);
                }
            }
        }
    }
}

float calcular_z_depth(Vertice v1, Vertice v2, Vertice v3, int x, int y)
{
    Vertice v4;
    v4.x = x;
    v4.y = y;
    v4.z = 0;

    float AreaABC = area_triangulo(v1,v2,v3);

    float u = area_triangulo(v3,v1,v4)/AreaABC;
    float v = area_triangulo(v1,v2,v4)/AreaABC;
    float w = area_triangulo(v2,v3,v4)/AreaABC;
    float z_depth = (u/v1.z) + (v/v2.z) + (w/v3.z);

    return 1/z_depth;
}

float area_triangulo(Vertice v1, Vertice v2, Vertice v3)
{
    Vetor2D A, B, C;

    A.x = v2.x - v1.x;
    A.y = v2.y - v1.y;
    A.z = v2.z - v1.z;

    B.x = v3.x - v1.x;
    B.y = v3.y - v1.y;
    B.z = v3.z - v1.z;

    C.x = A.y*B.z - A.z*B.y;
    C.y = A.z*B.x - A.x*B.z;
    C.z = A.x*B.y - A.y*B.x;

    float area = (sqrt(C.x*C.x + C.y*C.y + C.z*C.z)/2.0);

    return area;
}

int equacao_reta_arvore(Vertice v1, Vertice v2, int x, int y)
{
    //o "produto vetorial" de R2 é usado para equação da reta que liga dois vértices
    //de um lado do poligono, é "basicamente" a equação de um dos lados do poligono
    int A = v1.y - v2.y;
    int B = v2.x - v1.x;
    int C = v1.x*v2.y - v1.y*v2.x;

    return A*x + B*y + C;
}
