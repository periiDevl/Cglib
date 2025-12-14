#ifndef GIZMO_H
#define GIZMO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Gizmo.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "Cube.h"

typedef enum { GIZMO_TRANSLATE, GIZMO_ROTATE, GIZMO_SCALE } GizmoMode;
typedef enum { AXIS_NONE = 0, AXIS_X, AXIS_Y, AXIS_Z } GizmoAxis;

typedef struct {
    GizmoMode mode;
    GizmoAxis activeAxis;
    float posX, posY, posZ;
    float size;
    bool isDragging;
    double lastMouseX, lastMouseY;
    float dragPlaneX, dragPlaneY, dragPlaneZ;
    CubeObject* targetObject;
} Gizmo;

void GIZMO_init(Gizmo* gizmo, CubeObject* targetObject);

void GIZMO_setMode(Gizmo* gizmo, GizmoMode mode);

void GIZMO_setTarget(Gizmo* gizmo, CubeObject* targetObject);

void GIZMO_update(Gizmo* gizmo, double mouseX, double mouseY);

void GIZMO_draw(Gizmo* gizmo);

GizmoAxis GIZMO_checkAxisClick(Gizmo* gizmo, double mouseX, double mouseY, int screenWidth, int screenHeight);

void GIZMO_startDrag(Gizmo* gizmo, GizmoAxis axis);

void GIZMO_stopDrag(Gizmo* gizmo);

bool GIZMO_isDragging(Gizmo* gizmo);

#endif // GIZMO_H