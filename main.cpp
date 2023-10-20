#include "engine/engine.h"
#include "engine/renderer2d.h"
#include <iostream>
#include <cmath>

#define ROOM_WIDTH 800
#define ROOM_HEIGHT 400
#define CELL_ROWS 40
#define CELL_COLS 80
#define CELL_MAX_PARTICLES 5
#define PART_NBR 200

//extern int Engin

struct Particle
{
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;
    float m;
    float p;
};

Renderer2D_Renderer renderer = Renderer2D_Renderer();

byte gridTracker[CELL_ROWS][CELL_COLS][CELL_MAX_PARTICLES] = {0};

Particle PARTS[PART_NBR];

byte PREV_ROW[PART_NBR] = {0};
byte PREV_COL[PART_NBR] = {0};

float waterMass = 1;
float waterRad = 0.2;
float PIXELS_PER_METER = 50;
float g = 9.82;
float waterTension = 50;
float dropletSize = 2;
float dropletReach = 0.2;

void Spring_Collide(int ind1,int ind2,float tension,float size,float reach)
{
    float dx = PARTS[ind2].x - PARTS[ind1].x;
    float dy = PARTS[ind2].y - PARTS[ind1].y;
    /*if (fabsf(dy)<0.1)
    {
        dy = 0.1;
    }*/
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
    float f = delta*tension;
    if (f>0)
    {
        f*=0.1;
    }
    float cosv = dx/dist;
    float sinv = dy/dist;
    float fx = f*cosv;
    float fy = f*sinv;
    PARTS[ind1].ax += fx/PARTS[ind1].m;
    PARTS[ind2].ax -= fx/PARTS[ind2].m;
    PARTS[ind1].ay += fy/PARTS[ind1].m;
    PARTS[ind2].ay -= fy/PARTS[ind2].m;
    PARTS[ind1].p += fabsf(f);
    PARTS[ind2].p += fabsf(f);
}

void Spring_UpdateMass(int ind1, int ind2,float size)
{
    float dx = PARTS[ind2].x - PARTS[ind1].x;
    float dy = PARTS[ind2].y - PARTS[ind1].y;
    float dist2 = dx*dx + dy*dy;
    if (dist2 >= size*size)
    {
        return;
    }
    float k = (PARTS[ind1].p / PARTS[ind2].p);
    float m = PARTS[ind1].m + PARTS[ind2].m;

    float dm = (m/(1 + k) - PARTS[ind2].m )/10;
    PARTS[ind1].m -= dm;
    PARTS[ind2].m += dm;
}

void Spring_CollideWall(int i)
{
    if (PARTS[i].x < dropletSize + dropletReach && PARTS[i].x > dropletSize)
        {
            PARTS[i].ax += (dropletSize - PARTS[i].x)*waterTension;
        }
        
        if (PARTS[i].x > (float)ROOM_WIDTH/PIXELS_PER_METER - dropletSize - dropletReach && PARTS[i].x < (float)ROOM_WIDTH/PIXELS_PER_METER - dropletSize)
        {
            PARTS[i].ax -= (PARTS[i].x-((float)ROOM_WIDTH/PIXELS_PER_METER - dropletSize))*waterTension;
        }
        if (PARTS[i].y < dropletSize + dropletReach)
        {
            PARTS[i].ay += (dropletSize - PARTS[i].y)*waterTension;
        }
        if (PARTS[i].y > (float)ROOM_HEIGHT/PIXELS_PER_METER - dropletSize - dropletReach)
        {
            PARTS[i].ay -= (PARTS[i].y-((float)ROOM_HEIGHT/PIXELS_PER_METER - dropletSize))*waterTension;
        }
}

void Engine_start(){
    Renderer2D_init(&renderer,ROOM_WIDTH,ROOM_HEIGHT);
    for(int i = 0; i < PART_NBR;i++)
    {
        
        PARTS[i].x = ROOM_WIDTH*((float)i)/PART_NBR/PIXELS_PER_METER;
        PARTS[i].y = ROOM_HEIGHT*((float)rand())/RAND_MAX/PIXELS_PER_METER;
        PARTS[i].vx = 0;
        PARTS[i].vy = 0;
        PARTS[i].ax = 0;
        PARTS[i].ay = 0;
        PARTS[i].m = 1.0;
        PARTS[i].p = 0.0;
    }
}

void Engine_update(float deltaTime){
    //Clear Acceleration
    deltaTime = 1.0/60;
    for(int i = 0; i < PART_NBR;i++)
    {
        PARTS[i].ax = 0;
        PARTS[i].ay = g;
        PARTS[i].p = g * PARTS[i].m;
    }
    if(Engine_keys[ENGINE_KEY_R].downed)
    {
        for(int i = 0; i < PART_NBR;i++)
        {
            PARTS[i].x = ROOM_WIDTH*((float)i)/PART_NBR/PIXELS_PER_METER;
            PARTS[i].y = ROOM_HEIGHT*((float)rand())/RAND_MAX/PIXELS_PER_METER;
            PARTS[i].vx = 0;
            PARTS[i].vy = 0;
            PARTS[i].ax = 0;
            PARTS[i].ay = 0;
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
            Spring_Collide(i,j,waterTension,dropletSize,dropletReach);
        }
        Spring_CollideWall(i);
    }
    for(int i = 0; i < PART_NBR;i++)
    {
        for(int j = i + 1; j < PART_NBR;j++)
        {
            Spring_UpdateMass(i,j,dropletSize);
        }
    }



    //Update Velocities
    for(int i = 0; i < PART_NBR;i++)
    {
        PARTS[i].vx += PARTS[i].ax*deltaTime;
        PARTS[i].vy += PARTS[i].ay*deltaTime;
    }
    //Update Position
    float bounce = 0.5;
    for(int i = 0; i < PART_NBR;i++)
    {
        PARTS[i].x += (PARTS[i].vx + PARTS[i].ax*deltaTime/2)*deltaTime;
        PARTS[i].y += (PARTS[i].vy + PARTS[i].ay*deltaTime/2)*deltaTime;

        if (PARTS[i].x + waterRad >= ROOM_WIDTH/(float)PIXELS_PER_METER)
        {
            PARTS[i].x = ROOM_WIDTH/(float)PIXELS_PER_METER - waterRad;
            PARTS[i].vx = fminf(PARTS[i].vx,-bounce*PARTS[i].vx);
        }
        if (PARTS[i].x - waterRad <= 0)
        {
            PARTS[i].x = waterRad;
            PARTS[i].vx = fmaxf(PARTS[i].vx,-bounce*PARTS[i].vx);
        }
        if (PARTS[i].y + waterRad >= ROOM_HEIGHT/(float)PIXELS_PER_METER)
        {
            PARTS[i].y = ROOM_HEIGHT/(float)PIXELS_PER_METER - waterRad;
            PARTS[i].vy = fminf(PARTS[i].vy,-bounce*PARTS[i].vy);
        }
        if (PARTS[i].y - waterRad <= 0)
        {
            PARTS[i].y = waterRad;
            PARTS[i].vy = fmaxf(PARTS[i].vy,-bounce*PARTS[i].vy);
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
        float xx = (PARTS[i].x-waterRad) * PIXELS_PER_METER;
        float yy = (PARTS[i].y-waterRad) * PIXELS_PER_METER;
        float dd = 2*waterRad*PIXELS_PER_METER;
        Renderer2D_drawRectangle(&renderer, xx, yy, dd, dd);
    }
    
    
}
//We must deal wif dis
void Engine_finish(){

}

