#include "engine/engine.h"
#include "engine/renderer2d.h"
#include <iostream>
#include <cmath>

#define ROOM_WIDTH 800
#define ROOM_HEIGHT 400
#define CELL_ROWS 40
#define CELL_COLS 80
#define CELL_MAX_PARTICLES 5
#define PART_NBR 100

//extern int Engin


Renderer2D_Renderer renderer = Renderer2D_Renderer();

byte gridTracker[CELL_ROWS][CELL_COLS][CELL_MAX_PARTICLES] = {0};

float X[PART_NBR] = {0.0f};
float Y[PART_NBR] = {0.0f};
float VX[PART_NBR] = {0.0f};
float VY[PART_NBR] = {0.0f};
float AX[PART_NBR] = {0.0f};
float AY[PART_NBR] = {0.0f};
byte PREV_ROW[PART_NBR] = {0};
byte PREV_COL[PART_NBR] = {0};

float waterMass = 1;
float waterRad = 0.1;
float PIXELS_PER_METER = 50;
float g = 9.82;
float waterTension = 50;
float dropletSize = 2;
float dropletReach = 0.2;

void Collide_Droplet(int ind1,int ind2,float tension,float size,float reach)
{
    float dx = X[ind2] - X[ind1];
    float dy = Y[ind2] - Y[ind1];
    float dist2 = dx*dx + dy*dy;
    if (dist2 >= size*size*(1+reach)*(1+reach))
    {
        return;
    }
    float dist = sqrt(dist2);
    float delta = (dist-size);
    if (fabsf(dist) < 0.001)
    {
        return;
    }
    float a = delta*tension;
    float cosv = dx/dist;
    float sinv = dy/dist;
    float ax = a*cosv;
    float ay = a*sinv;
    AX[ind1] += ax;
    AX[ind2] -= ax;
    AY[ind1] += ay;
    AY[ind2] -= ay;
}

void Engine_start(){
    Renderer2D_init(&renderer,ROOM_WIDTH,ROOM_HEIGHT);
    for(int i = 0; i < PART_NBR;i++)
    {
        X[i] = ROOM_WIDTH*((float)i)/PART_NBR/PIXELS_PER_METER;
        Y[i] = ROOM_HEIGHT*((float)rand())/RAND_MAX/PIXELS_PER_METER;
        VX[i] = 0;
        VY[i] = 0;
    }
}

void Engine_update(float deltaTime){
    //Clear Acceleration
    deltaTime = 1.0/60;
    for(int i = 0; i < PART_NBR;i++)
    {
        AX[i] = 0;
        AY[i] = 9.82;
    }
    if(Engine_keys[ENGINE_KEY_R].downed)
    {
        for(int i = 0; i < PART_NBR;i++)
        {
            X[i] = ROOM_WIDTH*((float)i)/PART_NBR/PIXELS_PER_METER;
            Y[i] = ROOM_HEIGHT*((float)rand())/RAND_MAX/PIXELS_PER_METER;
            VX[i] = 0;
            VY[i] = 0;
        }
    }



    //Colide version 1
    if(Engine_keys[ENGINE_KEY_UP].down)
    {
        waterTension = fminf(100,waterTension*1.01);
        std::cout << " WaterTension:";
        std::cout << waterTension;
    }
    if(Engine_keys[ENGINE_KEY_DOWN].down)
    {
        waterTension = fmaxf(0.01,waterTension/1.01);
        std::cout << " WaterTension:";
        std::cout << waterTension;
    }
    if(Engine_keys[ENGINE_KEY_RIGHT].down)
    {
        dropletSize = fminf(10,dropletSize*1.01);
        std::cout << " DropletSize:";
        std::cout << dropletSize;
    }
    if(Engine_keys[ENGINE_KEY_LEFT].down)
    {
        dropletSize = fmaxf(0.01,dropletSize/1.01);
        std::cout << " DropletSize:";
        std::cout << dropletSize;
    }
    if(Engine_keys[ENGINE_KEY_W].down)
    {
        dropletReach = fminf(2,dropletReach+0.01);
        std::cout << " DropletReach:";
        std::cout << dropletReach;
    }
    if(Engine_keys[ENGINE_KEY_S].down)
    {
        dropletReach = fmaxf(0,dropletReach-0.01);
        std::cout << " DropletReach:";
        std::cout << dropletReach;
    }

    

    for(int i = 0; i < PART_NBR;i++)
    {
        for(int j = i + 1; j < PART_NBR;j++)
        {
            Collide_Droplet(i,j,waterTension,dropletSize,dropletReach);
        }
    }


    //Update Velocities
    for(int i = 0; i < PART_NBR;i++)
    {
        VX[i] += AX[i]*deltaTime;
        VY[i] += AY[i]*deltaTime;
    }
    //Update Position
    float bounce = 0.5;
    for(int i = 0; i < PART_NBR;i++)
    {
        X[i] += (VX[i] + AX[i]*deltaTime/2)*deltaTime;
        Y[i] += (VY[i] + AY[i]*deltaTime/2)*deltaTime;

        if (X[i] + waterRad >= ROOM_WIDTH/(float)PIXELS_PER_METER)
        {
            X[i] = ROOM_WIDTH/(float)PIXELS_PER_METER - waterRad;
            VX[i] = fminf(VX[i],-bounce*VX[i]);
        }
        if (X[i] - waterRad <= 0)
        {
            X[i] = waterRad;
            VX[i] = fmaxf(VX[i],-bounce*VX[i]);
        }
        if (Y[i] + waterRad >= ROOM_HEIGHT/(float)PIXELS_PER_METER)
        {
            Y[i] = ROOM_HEIGHT/(float)PIXELS_PER_METER - waterRad;
            VY[i] = fminf(VY[i],-bounce*VY[i]);
        }
        if (Y[i] - waterRad <= 0)
        {
            Y[i] = waterRad;
            VY[i] = fmaxf(VY[i],-bounce*VY[i]);
        }
    }

    
}

void Engine_draw(){
    //Y[4] = ROOM_HEIGHT*((float)rand())/RAND_MAX/PIXELS_PER_METER;
    //
    //std::cout << " y:"+Y[4]+"vy"+VY[4];
    Renderer2D_updateDrawSize(&renderer,ROOM_WIDTH,ROOM_HEIGHT);
    Renderer2D_setShader(&renderer,renderer.colorShader);
    glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
    Renderer2D_setColor(&renderer,Vec4f({1.0,1.0,0.9,1}));
    for(int i = 0; i <= CELL_ROWS;i++)
    {
        int yy = (i*ROOM_HEIGHT)/CELL_ROWS;
        Renderer2D_drawRectangle(&renderer, 0, yy-1, ROOM_WIDTH, 2);
    }
    for(int i = 0; i <= CELL_COLS;i++)
    {
        int xx = (i*ROOM_WIDTH)/CELL_COLS;
        Renderer2D_drawRectangle(&renderer, xx-1, 0, 2,ROOM_HEIGHT);
    }
    Renderer2D_setColor(&renderer,Vec4f({0.1,0.5,0.3,0.7}));
    for (int i = 0; i < PART_NBR;i++)
    {
        float xx = (X[i]-waterRad) * PIXELS_PER_METER;
        float yy = (Y[i]-waterRad) * PIXELS_PER_METER;
        float dd = 2*waterRad*PIXELS_PER_METER;
        Renderer2D_drawRectangle(&renderer, xx, yy, dd, dd);
    }
    
    
}
//We must deal wif dis
void Engine_finish(){

}

