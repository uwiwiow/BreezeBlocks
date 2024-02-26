#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Define screen dimensions
#define SCREEN_WIDTH    600
#define SCREEN_HEIGHT   800
#define BAR             120

#define NUM_ROWS 5
#define NUM_COLUMNS 10
#define BLOCK_WIDTH 50
#define BLOCK_HEIGHT 20

struct Bloque {
    SDL_Rect rect; // Rectángulo que representa el bloque
    int visible;   // Entero que indica si el bloque debe ser renderizado (0 para no visible, 1 para visible)
};

void drawHeart(SDL_Renderer* renderer, int x, int y, int size) {
    Uint8 r = 255, g = 0, b = 0, a = 255;

    Sint16 ptx[] = {x - (size/2),   x,              x + (size/2),   x};
    Sint16 pty[] = {y,              y - (size / 2), y,              y + (size/2)};
    filledPolygonRGBA(renderer, ptx, pty, 4, r, g, b, a);

    filledCircleRGBA(renderer, x - (size/4), y - (size/4), size/2.815, r, g, b, a);
    filledCircleRGBA(renderer, x + (size/4), y - (size/4), size/2.815, r, g, b, a);


}

bool colisionRectRect(SDL_Rect ball, SDL_Rect bar) {
    return ball.x < bar.x + bar.w &&
           ball.x + ball.w > bar.x &&
           ball.y < bar.y + bar.h &&
           ball.y + ball.h > bar.y;
}

void colisionRectBlock(SDL_Rect ball, struct Bloque* block, bool* x, bool* y) {
    if (block->visible) {

        int dx = ball.x - block->rect.x;
        int dy = ball.y - block->rect.y;
        int intersectX = ball.w / 2 + block->rect.w / 2 - abs(dx);
        int intersectY = ball.h / 2 + block->rect.h / 2 - abs(dy);

        if (intersectX > 0 && intersectY > 0) {
            if (intersectX > intersectY) {
                if (dy > 0) {
                    block->visible = false;
                    *y = true;
                } else {
                    block->visible = false;
                    *y = false;
                }
            } else {
                if (dx > 0) {
                    block->visible = false;
                    *x = true;
                } else {
                    block->visible = false;
                    *x = false;
                }
            }
        }

    }
}

void moverRectangulo(double angulo, double velocidad, double deltaTime, SDL_Rect* rect, bool* x, bool* y) {
    // Convertir el ángulo a radianes
    double radianes = angulo * M_PI / 180.0;

    // Calcular el cambio en x e y basado en el ángulo, la velocidad y el deltaTime
    double cambioX = velocidad * cos(radianes) * deltaTime;
    double cambioY = velocidad * sin(radianes) * deltaTime;

    if (*x) {
        rect->x += cambioX;
    }
    else {
        rect->x -= cambioX;
    }

    if (*y) {
        rect->y += cambioY;
    }
    else {
        rect->y -= cambioY;
    }
}


void barAngle(SDL_Rect ball, SDL_Rect bar, bool* x, bool* y, double* angle) {
    *y = false;
    int diff = (ball.x + 10) - (bar.x + (BAR / 2));

    if (diff >= 0) {
        *x = true;
        *angle = 90 - diff;
    }
    else {
        *x = false;
        *angle = 90 + diff;
    }

}

int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    // iniciar semilla de numeros pseudoaleatorios
    srand(time(NULL));

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
#endif

    int FPS = 60;

    const int DELAY_TIME = 1000 / FPS;

    Uint64 frameStart;
    Uint64 frameTime;

    // Create window
    SDL_Window *window = SDL_CreateWindow("BreezeBlocks",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {

            // Event loop exit flag
            bool quit = false;
            bool mousemode = false;

            int lifes = 3;

            // bar
            SDL_Rect bar;
            bar.w = BAR;
            bar.h = 20;
            bar.x = (SCREEN_WIDTH / 2) - (BAR / 2);
            bar.y = SCREEN_HEIGHT-100;

            // ball
            SDL_Rect ball;
            ball.w = 20;
            ball.h = 20;
            ball.x = (SCREEN_WIDTH / 2) - 10;
            ball.y = SCREEN_HEIGHT/2;

            double angle = 91;

            bool x = true;
            bool y = true;

            // blocks

            // Crear una matriz de bloques
            struct Bloque bloques[NUM_ROWS][NUM_COLUMNS];

            // Definir la disposición de los bloques en la pantalla
            for (int i = 0; i < NUM_ROWS; ++i) {
                for (int j = 0; j < NUM_COLUMNS; ++j) {
                    struct Bloque bloque;
                    bloque.rect.x = (((SCREEN_WIDTH - (BLOCK_WIDTH*NUM_COLUMNS))/NUM_COLUMNS) / 2) + j * BLOCK_WIDTH + (((SCREEN_WIDTH - (BLOCK_WIDTH*NUM_COLUMNS))/NUM_COLUMNS)*j);
                    bloque.rect.y = (((SCREEN_WIDTH - (BLOCK_WIDTH*NUM_COLUMNS))/NUM_COLUMNS) / 2) + i * BLOCK_HEIGHT+ (((SCREEN_WIDTH - (BLOCK_WIDTH*NUM_COLUMNS))/NUM_COLUMNS)*i);
                    bloque.rect.w = BLOCK_WIDTH;
                    bloque.rect.h = BLOCK_HEIGHT;
                    bloque.visible = 1;
                    bloques[i][j] = bloque;
                }
            }

            // Event loop
            while(!quit) {
                frameStart = SDL_GetTicks();
                SDL_Event e;

                while (SDL_PollEvent(&e)) {

                    // User requests quit
                    if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                        quit = true;
                    }
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_m){
                        mousemode = !mousemode;
                    }

                    if (mousemode) {
                        bar.x = e.motion.x - (BAR/2);
                    }
                    else {
                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_LEFT:
                                case SDLK_a: {
                                    bar.x -= 15;
                                } break;
                                case SDLK_RIGHT:
                                case SDLK_d: {
                                    bar.x += 15;
                                } break;
                            }
                        }
                    }

                }

                if ((ball.x+20) >= SCREEN_WIDTH) {
                    x = false;
                }
                if ((ball.x) <= 0) {
                    x = true;
                }
                if ((ball.y) >= SCREEN_HEIGHT) {
                    lifes--;
                    if (lifes == 0){
                        // TODO reset
                    } else {
                        ball.x = (SCREEN_WIDTH / 2) - 10;
                        ball.y = SCREEN_HEIGHT/2;

                        angle = 91;

                        x = true;
                        y = true;
                    }
                }
                if ((ball.y) <= 0) {
                    y = true;
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
                SDL_RenderClear(renderer);

                // Renderizar los bloques
                for (int i = 0; i < NUM_ROWS; ++i) {
                    for (int j = 0; j < NUM_COLUMNS; ++j) {
                        colisionRectBlock(ball, &bloques[i][j], &x, &y);
                        if (bloques[i][j].visible) {
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &bloques[i][j].rect);
                        }
                    }
                }

                for (int i = 0; i < lifes; ++i) {
                    drawHeart(renderer, 20 + (30*i), SCREEN_HEIGHT-20, 20);
                }

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &bar);

                moverRectangulo(angle, 0.5, DELAY_TIME, &ball, &x, &y);

                if (colisionRectRect(ball, bar)) {
                    y = false;
                    barAngle(ball, bar, &x, &y, &angle);
                }

                filledCircleRGBA(renderer, ball.x+10, ball.y+10, 10, 255, 255, 255, 255);

                // Update screen
                SDL_RenderPresent(renderer);

                frameTime = SDL_GetTicks64() - frameStart;

                // Esperar el tiempo restante hasta alcanzar el frame rate deseado
                if (frameTime < DELAY_TIME) {
                    SDL_Delay(DELAY_TIME - frameTime);
                }
            }

            // Destroy renderer
            SDL_DestroyRenderer(renderer);
        }

        // Destroy window
        SDL_DestroyWindow(window);
    }

    // Quit SDL
    SDL_Quit();


    return 0;
}