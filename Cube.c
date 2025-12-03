#include"Cube.h"

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
        glTranslatef(obj[i].x, obj[i].y, obj[i].z);
        glScalef(obj[i].sizex, obj[i].sizey, obj[i].sizez); // scale per axis
        glColor3f(obj[i].r, obj[i].g, obj[i].b);
        CUBE_draw();
        glPopMatrix();
    }
}
