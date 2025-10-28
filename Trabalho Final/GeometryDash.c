// Geometry dash

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define ALTURA 600
#define LARGURA 1200
#define ALTURA_BARREIRA 325
#define TAM_BLOCO 60
#define VEL_PULO 20
#define CHAO 540
#define POSICAO_INICIAL 0

typedef enum
{
    MENU = 0,
    PLAY,
    LEADERBOARD,
    QUIT,
    INPUT_NOME
} TELA;

typedef struct jogador
{
    Rectangle area;
    float velocidadeY;
} JOGADOR;

typedef struct obstaculo
{
    Rectangle area;
} OBSTACULO;

typedef struct bloco
{
    Rectangle area;
} BLOCO;

typedef struct fim
{
    Rectangle area;
} FIM;

typedef struct ranking
{
    char nome[11];
    int score;
} RANKING;

// Leitura do mapa.txt
void leMapa(BLOCO *blocos, int *nrBlocos, OBSTACULO *obstacles, int *nrObstaculos, int *posicaoInicialX, int *posicaoInicialY, FIM *fins, int *nrFins)
{
    FILE *arq;

    arq = fopen("mapa.txt", "r");

    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return -1;
    }

    char caractere;
    int x = 0, y = 0;

    while (!feof(arq))
    {
        fread(&caractere, sizeof(char), 1, arq);

        if (caractere == 'B')
        {
            BLOCO bloco = {{x * TAM_BLOCO, 600 - (60 * y), TAM_BLOCO, TAM_BLOCO}};
            // Adicionando mais blocos no vetor
            // Ponteiros para transformar a variavel nrBlocos em numero novamente
            blocos[*nrBlocos] = bloco;
            (*nrBlocos)++;
        }

        if (caractere == 'O')
        {
            OBSTACULO obstaculo = {{x * TAM_BLOCO, 600 - (60 * y), TAM_BLOCO, TAM_BLOCO}};
            // Adicionando mais obstaculos no vetor
            // Ponteiros para transformar a variavel nrObstaculos em numero novamente
            obstacles[*nrObstaculos] = obstaculo;
            (*nrObstaculos)++;
        }

        if (caractere == 'P')
        {
            *posicaoInicialX = x * TAM_BLOCO;
            *posicaoInicialY = 600 - (60 * y);
        }

        if (caractere == 'F')
        {
            FIM fim = {{x * TAM_BLOCO, 600 - (60 * y), TAM_BLOCO, TAM_BLOCO}};
            fins[*nrFins] = fim;
            (*nrFins)++;
        }

        x++;

        // Se o caractere for uma nova linha, reseta a coordenada x
        if (caractere == '\n')
        {
            x = 0;
            y++;
        }
    }

    fclose(arq);
}

// funcao para ler o arquivo
void leScore(RANKING *ranking)
{
    FILE *arq;

    arq = fopen("top_scores.bin", "rb");

    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return -1;
    }

    for (int i = 0; i < 5; i++)
    {
        fread(ranking[i].nome, sizeof(char), 10, arq);
        fread(&(ranking[i].score), sizeof(int), 1, arq);
        printf("Exemplo %d - Valores lidos: %s %d\n", i, ranking[i].nome, ranking[i].score);
    }

    fclose(arq);
}

// Salva o ranking inteiro em um arquivo
void escreveRanking(RANKING *ranking)
{
    FILE *arq;

    arq = fopen("top_scores.bin", "wb");

    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return -1;
    }

    for (int i = 0; i < 5; i++)
    {
        fwrite(ranking[i].nome, sizeof(char), 10, arq);
        fwrite(&(ranking[i].score), sizeof(int), 1, arq);
    }

    fclose(arq);
}

// funcao para gravar o arquivo
void populaScore()
{
    RANKING ranking[5] =
        {
            {"Maria", 0},
            {"Joao", 1},
            {"Ana", 2},
            {"Lucas", 3},
            {"Pedro", 4},
        };

    escreveRanking(ranking);
}

void renderizarTelaInicial()
{
    // Titulo
    DrawText("GEOMETRIA DASH", 350, 150, 55, LIGHTGRAY);

    // Jogo
    DrawText("(P) Play", 400, 300, 40, LIGHTGRAY);

    // Leaderboard
    DrawText("(L) Leaderboard", 400, 350, 40, LIGHTGRAY);

    // Sair
    DrawText("(Q) Quit", 400, 400, 40, LIGHTGRAY);
}

int main()
{
    TELA telaAtual = MENU;
    int estaPulando = 0;
    int tentativas = 0;
    int i;
    int nrObstaculos = 0, nrBlocos = 0, nrFins = 0;
    int posicaoInicialX = 0, posicaoInicialY = 0;
    char nomeJogador[11] = "          \0"; // inicializa a string vazia
    int contadorLetras = 0;

    FILE *arq;

    arq = fopen("top_scores.bin", "r");
    if (arq == NULL)
    {
        populaScore();
    }
    else
    {
        fclose(arq);
    }

    // Limite de blocos a serem lidos
    BLOCO bloco[3000];
    // Limite de obstaculos a serem lidos
    OBSTACULO obstacle[3000];
    // Limite de fins a serem lidos
    FIM fim[100];

    JOGADOR player;

    RANKING ranking[5];

    leScore(ranking);

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(LARGURA, ALTURA, "Geometry Dash");

    InitAudioDevice();
    Wave pulo_wave = LoadWave("Pulo.wav");
    Sound som_pulo = LoadSoundFromWave(pulo_wave);

    Music music = LoadMusicStream("green.mp3");
    Music music2 = LoadMusicStream("menuTema.mp3");

    // camera
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    camera.offset = (Vector2){0, 0};
    camera.target = (Vector2){0, 0};

    // carrega imagem
    Image fundo = LoadImage("fundo.png"); // Load image in CPU memory (RAM)
    ImageResize(&fundo, 1200, 600);
    Texture2D texture = LoadTextureFromImage(fundo);

    float escala = 1.8;
    int contadorFrame = 0, frameAtual = 0;

    // Carrega a imagem do frame 1
    Image frame1 = LoadImage("Frame 1.png");
    ImageResize(&frame1, 27 * escala, 40 * escala);
    Texture2D frame1Textura = LoadTextureFromImage(frame1);

    // Carrega a imagem do frame 1
    Image frame2 = LoadImage("Frame 2.png");
    ImageResize(&frame2, 27 * escala, 40 * escala);
    Texture2D frame2Textura = LoadTextureFromImage(frame2);

    // Carrega a imagem do chao
    Image chao = LoadImage("Chao.png");
    ImageResize(&chao, TAM_BLOCO, TAM_BLOCO);
    Texture2D chaoTextura = LoadTextureFromImage(chao);

    int contadorFrameObstaculo = 0, frameAtualObstaculo = 0;

    // Carrega a imagem do frame 1 do obstaculo
    Image obsFrame1 = LoadImage("Obstaculo1.png");
    ImageResize(&obsFrame1, 60, 60);
    Texture2D obsFrame1Textura = LoadTextureFromImage(obsFrame1);

    // Carrega a imagem do frame 2 do obstaculo
    Image obsFrame2 = LoadImage("Obstaculo2.png");
    ImageResize(&obsFrame2, 60, 60);
    Texture2D obsFrame2Textura = LoadTextureFromImage(obsFrame2);

    // Carrega a imagem do menu
    Image menu = LoadImage("Menu.png");
    // ImageResize(&menu, 1200, 600);
    Texture2D menuTextura = LoadTextureFromImage(menu);

    // Carrega a imagem da tela de score
    Image telaScore = LoadImage("leaderboard.png");
    Texture2D scoreTextura = LoadTextureFromImage(telaScore);

    // Carrega a imagem da tela para digitar o nome
    Image telaNome = LoadImage("digitaNome.png");
    Texture2D nomeTextura = LoadTextureFromImage(telaNome);

    PlayMusicStream(music2);

    SetTargetFPS(60); // Setar o FPS para 60
    //--------------------------------------------------------------------------------------

    // loop do jogo principal
    while (!WindowShouldClose()) // Enquanto não fecha a janela, continua executando
    {

        // Desenho
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);

        UpdateMusicStream(music);
        UpdateMusicStream(music2);

        switch (telaAtual)
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case MENU:
            // renderizarTelaInicial();
            BeginMode2D(camera);

            DrawTexture(menuTextura, 0, 0, WHITE);

            // Interacoes
            if (IsKeyPressed(KEY_P))
            {
                telaAtual = PLAY;
                StopMusicStream(music2);
                PlayMusicStream(music);
                tentativas = 0;

                // inicializa
                leMapa(bloco, &nrBlocos, obstacle, &nrObstaculos, &posicaoInicialX, &posicaoInicialY, fim, &nrFins);

                player.area.x = posicaoInicialX;
                player.area.y = posicaoInicialY;
                player.area.width = TAM_BLOCO;
                player.area.height = TAM_BLOCO;
                player.velocidadeY = 0;
            }

            if (IsKeyPressed(KEY_L))
            {
                telaAtual = LEADERBOARD;
                StopMusicStream(music2);
            }

            if (IsKeyPressed(KEY_Q))
            {
                telaAtual = QUIT;
            }

            if (IsKeyPressed(KEY_M))
            {
                telaAtual = MENU;
            }
            break;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case PLAY:
            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

            // Movimentacao do jogador para frente
            player.area.x += 9;

            // gravidade
            player.velocidadeY -= 1.8;

            // Acompanha o jogador com a camera
            int posicaoXCamera = player.area.x - 100;
            ;
            if (posicaoXCamera < 0)
            {
                posicaoXCamera = 0;
            }

            // camera.offset = (Vector2){0 - player.area.x, 0};
            camera.target = (Vector2){posicaoXCamera, 0};

            // Pulo, quando tentar pular mais de uma vez vai ser impedido
            if ((IsKeyDown(KEY_SPACE)) && !estaPulando)
            {
                estaPulando = 1;
                player.velocidadeY = 25;
                PlaySound(som_pulo);
            }

            // Altera a posicao dele de acordo com a velocidade
            player.area.y += player.velocidadeY;

            // COLISAO OBSTACULO
            for (i = 0; i < nrObstaculos; i++)
            {
                if (CheckCollisionRecs(player.area, obstacle[i].area))
                {
                    player.area.x = posicaoInicialX;
                    player.area.y = posicaoInicialY;
                    camera.target = (Vector2){0, 0};
                    tentativas += 1;
                    printf("%d", tentativas);
                }
            }

            // COLISAO BLOCO
            for (i = 0; i < nrBlocos; i++)
            {
                if (CheckCollisionRecs(player.area, bloco[i].area))
                {
                    if (player.area.y >= (bloco[i].area.y + bloco[i].area.height / 2))
                    {
                        // Se ele cai de cima, reposiciona o player em cima do bloco
                        player.area.y = bloco[i].area.y + bloco[i].area.height;
                        estaPulando = 0;
                        // Quando colidir com um bloco a velocidade para de crescer
                        player.velocidadeY = 0;
                    }
                    else
                    {
                        player.area.x = posicaoInicialX;
                        player.area.y = posicaoInicialY;
                        camera.target = (Vector2){0, 0};
                        tentativas += 1;
                        printf("%d", tentativas);
                    }
                }
            }

            // COLISAO FIM
            for (i = 0; i < nrFins; i++)
            {
                if (CheckCollisionRecs(player.area, fim[i].area))
                {
                    telaAtual = INPUT_NOME;
                    // reseta a camera para cima
                    camera.target.x = 0;
                    camera.target.y = 0;
                    StopMusicStream(music);
                }
            }

            DrawText("Tela de Jogo", 20, 20, 40, LIGHTGRAY);

            // player.area.y invertida
            int posicaoCalculada = ALTURA - player.area.y;

            // carrega o fundo
            for (i = 0; i < 20; i++)
            {
                // repeticao do fundo para nao acabar
                DrawTexture(texture, i * 1200 + camera.target.x * 0.9, 0, WHITE);
            }

            // DrawRectangle(player.area.x, posicaoCalculada, player.area.width, player.area.height, RED);

            for (i = 0; i < nrObstaculos; i++)
            {
                // ? = operador ternario, se condicao for verdadeira retorna valor 1 se nao valor 2
                // condicao ? valor1 : valor2
                DrawTexture(frameAtualObstaculo == 1 ? obsFrame1Textura : obsFrame2Textura, obstacle[i].area.x, ALTURA - obstacle[i].area.y, WHITE);
            }

            contadorFrameObstaculo++;

            // animacao do kirby
            if (contadorFrameObstaculo > 20)
            {
                contadorFrameObstaculo = 0;
                if (frameAtualObstaculo == 1)
                {
                    frameAtualObstaculo = 0;
                }
                else if (frameAtualObstaculo == 0)
                {
                    frameAtualObstaculo = 1;
                }
            }

            for (i = 0; i < nrBlocos; i++)
            {
                DrawTexture(chaoTextura, bloco[i].area.x, ALTURA - bloco[i].area.y, WHITE);
            }

            if (frameAtual == 1)
            {
                DrawTexture(frame1Textura, player.area.x, ALTURA - player.area.y, WHITE);
            }
            else if (frameAtual == 0)
            {
                DrawTexture(frame2Textura, player.area.x, ALTURA - player.area.y, WHITE);
            }

            char textoTentativa[30];
            sprintf(textoTentativa, "Tentativa %d", tentativas);
            DrawText(textoTentativa, 800, 200, 100, PINK);

            contadorFrame++;

            // animacao do kirby
            if (contadorFrame > 5)
            {
                contadorFrame = 0;
                if (frameAtual == 1)
                {
                    frameAtual = 0;
                }
                else if (frameAtual == 0)
                {
                    frameAtual = 1;
                }
            }

            break;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case LEADERBOARD:

            // DrawText("Score jogadores", 20, 20, 40, LIGHTGRAY);
            ClearBackground(PINK);

            DrawTexture(scoreTextura, 0, 0, WHITE);

            char texto[5][30];
            int textoY = 200;

            for (i = 0; i < 5; i++)
            {
                sprintf(texto[i], "%d - %s", ranking[i].score, ranking[i].nome);
                DrawText(texto[i], 450, textoY, 40, BLUE);
                textoY += 50;
            }

            if (IsKeyPressed(KEY_M))
            {
                telaAtual = MENU;
                PlayMusicStream(music2);
            }

            break;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case INPUT_NOME:
            ClearBackground(SKYBLUE);
            DrawTexture(nomeTextura, 0, 0, WHITE);

            // if (contadorLetras > 0)
            // {
            //     DrawText("Pressione enter para confirmar", 275, 450, 40, BLACK);
            // }

            int tecla = GetCharPressed();

            while (tecla > 0)
            {

                if ((tecla >= 32) && (tecla <= 125) && (contadorLetras < 10))
                {
                    nomeJogador[contadorLetras] = (char)tecla;
                    nomeJogador[contadorLetras + 1] = '\0'; // Adicionar \0 para sinalizar que eh uma string
                    contadorLetras++;
                }

                tecla = GetCharPressed(); // Verifica a proxima letra da fila
            }

            // Tecla para apagar letra
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                contadorLetras--;
                if (contadorLetras < 0)
                    contadorLetras = 0;
                nomeJogador[contadorLetras] = '\0';
            }

            DrawText(nomeJogador, 400, 295, 70, BLACK);

            // Quando pressionar enter, salva o nome e o numero de tentativas na estrutura
            if (IsKeyPressed(KEY_ENTER) && contadorLetras > 0)
            {
                RANKING rankingPlayer;
                strcpy(rankingPlayer.nome, nomeJogador);
                rankingPlayer.score = tentativas;
                int playerFoiAdicionado = 0;

                RANKING rankingAtual[5];
                leScore(rankingAtual);

                // Iterar a struct do ranking
                RANKING rankingAtualizado[5];
                for (i = 0; i < 5; i++)
                {
                    if (playerFoiAdicionado)
                    {
                        rankingAtualizado[i].score = rankingAtual[i - 1].score;
                        strcpy(rankingAtualizado[i].nome, rankingAtual[i - 1].nome);
                        continue;
                    }

                    if (rankingAtual[i].score < rankingPlayer.score)
                    {
                        rankingAtualizado[i].score = rankingAtual[i].score;
                        strcpy(rankingAtualizado[i].nome, rankingAtual[i].nome);

                        // Pega a iteracao que esta e volta para a proxima iteracao
                        continue;
                    }
                    else
                    {
                        rankingAtualizado[i].score = rankingPlayer.score;
                        strcpy(rankingAtualizado[i].nome, rankingPlayer.nome);
                        playerFoiAdicionado = 1;
                        continue;
                    }
                }

                // Chama a funcao passando o ranking atualizado
                escreveRanking(rankingAtualizado);

                // joga a tela atual para a leaderboard
                telaAtual = LEADERBOARD;

                // Chama a funcao pra atualizar o ranking
                leScore(ranking);
            }
            break;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case QUIT:
            CloseWindow();
            break;
        }

        EndMode2D();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadTexture(texture);

    //--------------------------------------------------------------------------------------
    CloseWindow(); // Fecha a janela
    //--------------------------------------------------------------------------------------

    return 0;
}
