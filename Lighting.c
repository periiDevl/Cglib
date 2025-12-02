#include"Lighting.h"

void LIGHT_enable()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
}

void LIGHT_apply(LightSource lights[], int size)
{
    for (int i = 0; i < size; i++) {

        GLenum id = GL_LIGHT0 + i;
        glEnable(id);

        GLfloat pos[] = { lights[i].x, lights[i].y, lights[i].z, 1.0f };
        GLfloat color[] = { lights[i].r, lights[i].g, lights[i].b, 1.0f };

        glLightfv(id, GL_POSITION, pos);
        glLightfv(id, GL_DIFFUSE,  color);
        glLightfv(id, GL_SPECULAR, color);
    }

}
void LIGHT_ambient(float r, float g, float b)
{
    GLfloat ambientLight[]  = {r, g, b, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

}