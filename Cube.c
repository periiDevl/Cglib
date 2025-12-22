#include "Cube.h"
#include <math.h>

void CUBE_setPosition(CubeObject *cube, int x, int y, int z)
{
    cube->x = x;
    cube->y = y;
    cube->z = z;
}

void CUBE_setColor(CubeObject *cube, int r, int g, int b)
{
    cube->r = r;
    cube->g = g;
    cube->b = b;
}

void CUBE_draw()
{
    glBegin(GL_QUADS);
    glNormal3f(0,0,1); glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
    glNormal3f(0,0,-1); glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
    glNormal3f(-1,0,0); glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1); glVertex3f(-1,1,1); glVertex3f(-1,1,-1);
    glNormal3f(1,0,0); glVertex3f(1,-1,-1); glVertex3f(1,1,-1); glVertex3f(1,1,1); glVertex3f(1,-1,1);
    glNormal3f(0,1,0); glVertex3f(-1,1,-1); glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(1,1,-1);
    glNormal3f(0,-1,0); glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1); glVertex3f(1,-1,1); glVertex3f(-1,-1,1);
    glEnd();
}

void CUBE_drawArray(CubeObject obj[], int size)
{
    for (int i = 0; i < size; i++)
    {
        glPushMatrix();
        
        // Apply transformations in order: Translate -> Rotate -> Scale
        glTranslatef(obj[i].x, obj[i].y, obj[i].z);
        
        // Apply rotations (convert radians to degrees)
        glRotatef(obj[i].rotX * 180.0f / 3.14159265359f, 1, 0, 0);
        glRotatef(obj[i].rotY * 180.0f / 3.14159265359f, 0, 1, 0);
        glRotatef(obj[i].rotZ * 180.0f / 3.14159265359f, 0, 0, 1);
        
        glScalef(obj[i].sx/2, obj[i].sy/2, obj[i].sz/2);
        
        glColor3f(obj[i].r, obj[i].g, obj[i].b);
        CUBE_draw();
        
        glPopMatrix();
    }
}