#include "engine/engine.h"
#include "engine/renderer2d.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 400
#define CELL_ROWS 40
#define CELL_COLS 80
#define CELL_MAX_PARTICLES 5

//extern int Engin

Renderer2D_Renderer renderer = Renderer2D_Renderer();

byte gridTracker[CELL_ROWS][CELL_COLS][CELL_MAX_PARTICLES] = {0};



void Engine_start(){
    Renderer2D_init(&renderer,WIN_WIDTH,WIN_HEIGHT);
    
}

void Engine_update(float deltaTime){
    
}

void Engine_draw(){
    Renderer2D_updateDrawSize(&renderer,WIN_WIDTH,WIN_HEIGHT);
    Renderer2D_setShader(&renderer,renderer.colorShader);
    glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
    Renderer2D_setColor(&renderer,Vec4f({1.0,1.0,0.9,1}));
    for(int i = 0; i <= CELL_ROWS;i++)
    {
        int yy = (i*WIN_HEIGHT)/CELL_ROWS;
        Renderer2D_drawRectangle(&renderer, 0, yy-1, WIN_WIDTH, 2);
    }
    for(int i = 0; i <= CELL_COLS;i++)
    {
        int xx = (i*WIN_WIDTH)/CELL_COLS;
        Renderer2D_drawRectangle(&renderer, xx-1, 0, 2,WIN_HEIGHT);
    }
    Renderer2D_setColor(&renderer,Vec4f({0.1,0.5,0.3,1}));
    Renderer2D_drawRectangle(&renderer, 1, 1, 100, 100);
    
}
//We must deal wif dis
void Engine_finish(){

}

