#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include "Cube.h"
#include <stdlib.h>
#include "Lighting.h"
#include "Camera.h"
#include <stdbool.h>

#define PI 3.14159265359f

// Gizmo types
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

CubeObject objects[] = {
    {0, 0, 0, 0.7, 0.7, 0.7, 3, 3, 3, 0, 0, 0},
    {2, 3, 4, 0.15, 0.5, 0.2, 2, 2, 4, 0, 0, 0},
    {-3, -3, -3, 0.15, 0.5, 0.2, 30, 1, 30, 0, 0, 0},
};

LightSource lights[] = {
    {6, 6, 6, 4, 4, 4, 0, 0},
    {0, 0, 0, 5.0f, 5.0f, 5.0f, 0.4f, 0.1f},
};

Camera* camera;
Gizmo gizmo;
int selectedObject = 0;

// Helper: Draw line
void drawLine(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b, float width) {
    glDisable(GL_LIGHTING);
    glLineWidth(width);
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
    glEnable(GL_LIGHTING);
}

// Helper: Draw cone (arrow head)
void drawCone(float x, float y, float z, float axisX, float axisY, float axisZ, float r, float g, float b, float size) {
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    
    int segments = 16;
    float radius = size * 0.08f;
    float height = size * 0.25f;
    
    // Get two perpendicular vectors to the axis
    float perpX, perpY, perpZ;
    float perp2X, perp2Y, perp2Z;
    
    // Find first perpendicular vector
    if (fabs(axisX) < 0.9f) {
        perpX = 0; perpY = -axisZ; perpZ = axisY;
    } else {
        perpX = -axisZ; perpY = 0; perpZ = axisX;
    }
    
    // Normalize first perpendicular
    float len = sqrt(perpX*perpX + perpY*perpY + perpZ*perpZ);
    if (len > 0.0001f) {
        perpX /= len; perpY /= len; perpZ /= len;
    }
    
    // Get second perpendicular via cross product
    perp2X = axisY * perpZ - axisZ * perpY;
    perp2Y = axisZ * perpX - axisX * perpZ;
    perp2Z = axisX * perpY - axisY * perpX;
    
    // Draw cone sides
    glBegin(GL_TRIANGLE_FAN);
    // Tip of the cone
    glVertex3f(x + axisX * height, y + axisY * height, z + axisZ * height);
    
    // Base circle
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float px = perpX * cos(angle) + perp2X * sin(angle);
        float py = perpY * cos(angle) + perp2Y * sin(angle);
        float pz = perpZ * cos(angle) + perp2Z * sin(angle);
        
        glVertex3f(x + px * radius, y + py * radius, z + pz * radius);
    }
    glEnd();
    
    // Draw cone base (cap)
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    for (int i = segments; i >= 0; i--) {
        float angle = (float)i / segments * 2.0f * PI;
        float px = perpX * cos(angle) + perp2X * sin(angle);
        float py = perpY * cos(angle) + perp2Y * sin(angle);
        float pz = perpZ * cos(angle) + perp2Z * sin(angle);
        
        glVertex3f(x + px * radius, y + py * radius, z + pz * radius);
    }
    glEnd();
    
    glEnable(GL_LIGHTING);
}

// Helper: Draw circle
void drawCircle(float x, float y, float z, float radius, int axis, float r, float g, float b, float width) {
    glDisable(GL_LIGHTING);
    glLineWidth(width);
    glColor3f(r, g, b);
    
    glBegin(GL_LINE_LOOP);
    int segments = 64;
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / segments * 2.0f * PI;
        float cx = cos(angle) * radius;
        float cy = sin(angle) * radius;
        
        if (axis == AXIS_X) glVertex3f(x, y + cx, z + cy);
        else if (axis == AXIS_Y) glVertex3f(x + cx, y, z + cy);
        else if (axis == AXIS_Z) glVertex3f(x + cx, y + cy, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// Helper: Draw cube
void drawCube(float x, float y, float z, float size, float r, float g, float b) {
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    
    float s = size;
    glBegin(GL_QUADS);
    glVertex3f(x-s, y-s, z-s); glVertex3f(x+s, y-s, z-s); glVertex3f(x+s, y+s, z-s); glVertex3f(x-s, y+s, z-s);
    glVertex3f(x-s, y-s, z+s); glVertex3f(x+s, y-s, z+s); glVertex3f(x+s, y+s, z+s); glVertex3f(x-s, y+s, z+s);
    glVertex3f(x-s, y-s, z-s); glVertex3f(x-s, y+s, z-s); glVertex3f(x-s, y+s, z+s); glVertex3f(x-s, y-s, z+s);
    glVertex3f(x+s, y-s, z-s); glVertex3f(x+s, y+s, z-s); glVertex3f(x+s, y+s, z+s); glVertex3f(x+s, y-s, z+s);
    glVertex3f(x-s, y-s, z-s); glVertex3f(x+s, y-s, z-s); glVertex3f(x+s, y-s, z+s); glVertex3f(x-s, y-s, z+s);
    glVertex3f(x-s, y+s, z-s); glVertex3f(x+s, y+s, z-s); glVertex3f(x+s, y+s, z+s); glVertex3f(x-s, y+s, z+s);
    glEnd();
    glEnable(GL_LIGHTING);
}

// Project 3D to 2D screen coordinates
void worldToScreen(float wx, float wy, float wz, int sw, int sh, float* sx, float* sy, float* depth) {
    float modelview[16], projection[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);
    
    float x = modelview[0]*wx + modelview[4]*wy + modelview[8]*wz + modelview[12];
    float y = modelview[1]*wx + modelview[5]*wy + modelview[9]*wz + modelview[13];
    float z = modelview[2]*wx + modelview[6]*wy + modelview[10]*wz + modelview[14];
    float w = modelview[3]*wx + modelview[7]*wy + modelview[11]*wz + modelview[15];
    
    float px = projection[0]*x + projection[4]*y + projection[8]*z + projection[12]*w;
    float py = projection[1]*x + projection[5]*y + projection[9]*z + projection[13]*w;
    float pz = projection[2]*x + projection[6]*y + projection[10]*z + projection[14]*w;
    float pw = projection[3]*x + projection[7]*y + projection[11]*z + projection[15]*w;
    
    if (pw != 0) {
        px /= pw;
        py /= pw;
        pz /= pw;
    }
    
    *sx = (px + 1.0f) * sw * 0.5f;
    *sy = (1.0f - py) * sh * 0.5f;
    *depth = pz;
}

bool isNearPoint(float mx, float my, float px, float py, float threshold) {
    float dx = mx - px;
    float dy = my - py;
    return sqrt(dx*dx + dy*dy) < threshold;
}

void getCameraVectors(float* rightX, float* rightY, float* rightZ, float* upX, float* upY, float* upZ) {
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    
    *rightX = modelview[0];
    *rightY = modelview[4];
    *rightZ = modelview[8];
    
    *upX = modelview[1];
    *upY = modelview[5];
    *upZ = modelview[9];
}

GizmoAxis checkAxisClick(double mouseX, double mouseY, int screenWidth, int screenHeight) {
    float x = gizmo.posX, y = gizmo.posY, z = gizmo.posZ;
    float s = gizmo.size;
    
    float sx, sy, depth;
    float threshold = 25.0f;
    
    if (gizmo.mode == GIZMO_ROTATE) {
        // For rotation mode, check points along the circles
        int segments = 32;
        float minDistX = 10000.0f, minDistY = 10000.0f, minDistZ = 10000.0f;
        
        // Check X circle (YZ plane)
        for (int i = 0; i < segments; i++) {
            float angle = (float)i / segments * 2.0f * PI;
            float cx = cos(angle) * s;
            float cy = sin(angle) * s;
            worldToScreen(x, y + cx, z + cy, screenWidth, screenHeight, &sx, &sy, &depth);
            if (depth < 1.0f) {
                float dist = sqrt((mouseX - sx) * (mouseX - sx) + (mouseY - sy) * (mouseY - sy));
                if (dist < minDistX) minDistX = dist;
            }
        }
        
        // Check Y circle (XZ plane)
        for (int i = 0; i < segments; i++) {
            float angle = (float)i / segments * 2.0f * PI;
            float cx = cos(angle) * s;
            float cy = sin(angle) * s;
            worldToScreen(x + cx, y, z + cy, screenWidth, screenHeight, &sx, &sy, &depth);
            if (depth < 1.0f) {
                float dist = sqrt((mouseX - sx) * (mouseX - sx) + (mouseY - sy) * (mouseY - sy));
                if (dist < minDistY) minDistY = dist;
            }
        }
        
        // Check Z circle (XY plane)
        for (int i = 0; i < segments; i++) {
            float angle = (float)i / segments * 2.0f * PI;
            float cx = cos(angle) * s;
            float cy = sin(angle) * s;
            worldToScreen(x + cx, y + cy, z, screenWidth, screenHeight, &sx, &sy, &depth);
            if (depth < 1.0f) {
                float dist = sqrt((mouseX - sx) * (mouseX - sx) + (mouseY - sy) * (mouseY - sy));
                if (dist < minDistZ) minDistZ = dist;
            }
        }
        
        // Return the closest circle if within threshold
        float minDist = minDistX;
        GizmoAxis result = AXIS_X;
        
        if (minDistY < minDist) {
            minDist = minDistY;
            result = AXIS_Y;
        }
        if (minDistZ < minDist) {
            minDist = minDistZ;
            result = AXIS_Z;
        }
        
        if (minDist < threshold) return result;
        return AXIS_NONE;
    }
    
    // For translate and scale modes
    worldToScreen(x + s, y, z, screenWidth, screenHeight, &sx, &sy, &depth);
    if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_X;
    
    worldToScreen(x, y + s, z, screenWidth, screenHeight, &sx, &sy, &depth);
    if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_Y;
    
    worldToScreen(x, y, z + s, screenWidth, screenHeight, &sx, &sy, &depth);
    if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_Z;
    
    for (float t = 0.2f; t < 1.0f; t += 0.1f) {
        worldToScreen(x + s*t, y, z, screenWidth, screenHeight, &sx, &sy, &depth);
        if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_X;
    }
    
    for (float t = 0.2f; t < 1.0f; t += 0.1f) {
        worldToScreen(x, y + s*t, z, screenWidth, screenHeight, &sx, &sy, &depth);
        if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_Y;
    }
    
    for (float t = 0.2f; t < 1.0f; t += 0.1f) {
        worldToScreen(x, y, z + s*t, screenWidth, screenHeight, &sx, &sy, &depth);
        if (depth < 1.0f && isNearPoint(mouseX, mouseY, sx, sy, threshold)) return AXIS_Z;
    }
    
    return AXIS_NONE;
}

void drawGizmo() {
    if (!gizmo.targetObject) return;
    
    float x = gizmo.posX, y = gizmo.posY, z = gizmo.posZ;
    float s = gizmo.size;
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    float xWidth = (gizmo.activeAxis == AXIS_X) ? 6.0f : 3.0f;
    float yWidth = (gizmo.activeAxis == AXIS_Y) ? 6.0f : 3.0f;
    float zWidth = (gizmo.activeAxis == AXIS_Z) ? 6.0f : 3.0f;
    
    float xBright = (gizmo.activeAxis == AXIS_X) ? 1.0f : 0.7f;
    float yBright = (gizmo.activeAxis == AXIS_Y) ? 1.0f : 0.7f;
    float zBright = (gizmo.activeAxis == AXIS_Z) ? 1.0f : 0.7f;
    
    if (gizmo.mode == GIZMO_TRANSLATE) {
        // X axis (red)
        drawLine(x, y, z, x + s * 0.75f, y, z, xBright, 0, 0, xWidth);
        drawCone(x + s * 0.75f, y, z, 1, 0, 0, xBright, 0, 0, s);
        
        // Y axis (green)
        drawLine(x, y, z, x, y + s * 0.75f, z, 0, yBright, 0, yWidth);
        drawCone(x, y + s * 0.75f, z, 0, 1, 0, 0, yBright, 0, s);
        
        // Z axis (blue)
        drawLine(x, y, z, x, y, z + s * 0.75f, 0, 0, zBright, zWidth);
        drawCone(x, y, z + s * 0.75f, 0, 0, 1, 0, 0, zBright, s);
        
    } else if (gizmo.mode == GIZMO_ROTATE) {
        // Draw thicker circles with better visibility
        drawCircle(x, y, z, s, AXIS_X, xBright, 0, 0, xWidth);
        drawCircle(x, y, z, s, AXIS_Y, 0, yBright, 0, yWidth);
        drawCircle(x, y, z, s, AXIS_Z, 0, 0, zBright, zWidth);
        
        // Draw small spheres at cardinal points for better selection feedback
        float sphereSize = s * 0.08f;
        
        // X circle cardinal points (YZ plane)
        drawCube(x, y + s, z, sphereSize, xBright, 0, 0);
        drawCube(x, y - s, z, sphereSize, xBright, 0, 0);
        drawCube(x, y, z + s, sphereSize, xBright, 0, 0);
        drawCube(x, y, z - s, sphereSize, xBright, 0, 0);
        
        // Y circle cardinal points (XZ plane)
        drawCube(x + s, y, z, sphereSize, 0, yBright, 0);
        drawCube(x - s, y, z, sphereSize, 0, yBright, 0);
        drawCube(x, y, z + s, sphereSize, 0, yBright, 0);
        drawCube(x, y, z - s, sphereSize, 0, yBright, 0);
        
        // Z circle cardinal points (XY plane)
        drawCube(x + s, y, z, sphereSize, 0, 0, zBright);
        drawCube(x - s, y, z, sphereSize, 0, 0, zBright);
        drawCube(x, y + s, z, sphereSize, 0, 0, zBright);
        drawCube(x, y - s, z, sphereSize, 0, 0, zBright);
        
    } else if (gizmo.mode == GIZMO_SCALE) {
        float cubeSize = s * 0.12f;
        
        drawLine(x, y, z, x + s, y, z, xBright, 0, 0, xWidth);
        drawCube(x + s, y, z, cubeSize, xBright, 0, 0);
        
        drawLine(x, y, z, x, y + s, z, 0, yBright, 0, yWidth);
        drawCube(x, y + s, z, cubeSize, 0, yBright, 0);
        
        drawLine(x, y, z, x, y, z + s, 0, 0, zBright, zWidth);
        drawCube(x, y, z + s, cubeSize, 0, 0, zBright);
        
        drawCube(x, y, z, cubeSize * 1.5f, 0.9f, 0.9f, 0.9f);
    }
}

void updateGizmo(double mouseX, double mouseY) {
    if (!gizmo.targetObject) return;
    
    gizmo.posX = gizmo.targetObject->x;
    gizmo.posY = gizmo.targetObject->y;
    gizmo.posZ = gizmo.targetObject->z;
    
    if (gizmo.isDragging && gizmo.activeAxis != AXIS_NONE) {
        float deltaX = (mouseX - gizmo.lastMouseX);
        float deltaY = (mouseY - gizmo.lastMouseY);
        
        float camRightX, camRightY, camRightZ;
        float camUpX, camUpY, camUpZ;
        getCameraVectors(&camRightX, &camRightY, &camRightZ, &camUpX, &camUpY, &camUpZ);
        
        if (gizmo.mode == GIZMO_TRANSLATE) {
            float sensitivity = 0.01f;
            
            if (gizmo.activeAxis == AXIS_X) {
                float screenDelta = deltaX * camRightX - deltaY * camUpX;
                gizmo.targetObject->x += screenDelta * sensitivity;
            } 
            else if (gizmo.activeAxis == AXIS_Y) {
                float screenDelta = -deltaY * camUpY + deltaX * camRightY;
                gizmo.targetObject->y += screenDelta * sensitivity;
            } 
            else if (gizmo.activeAxis == AXIS_Z) {
                float screenDelta = deltaX * camRightZ - deltaY * camUpZ;
                gizmo.targetObject->z += screenDelta * sensitivity;
            }
        } 
        else if (gizmo.mode == GIZMO_ROTATE) {
            float rotationSpeed = 0.02f;
            float angle = (deltaX + deltaY) * rotationSpeed;
            
            if (gizmo.activeAxis == AXIS_X) {
                gizmo.targetObject->rotX += angle;
            }
            else if (gizmo.activeAxis == AXIS_Y) {
                gizmo.targetObject->rotY += angle;
            }
            else if (gizmo.activeAxis == AXIS_Z) {
                gizmo.targetObject->rotZ += angle;
            }
        }
        else if (gizmo.mode == GIZMO_SCALE) {
            float scale = 1.0f + (deltaX - deltaY) * 0.01f;
            
            if (gizmo.activeAxis == AXIS_X) gizmo.targetObject->sx *= scale;
            else if (gizmo.activeAxis == AXIS_Y) gizmo.targetObject->sy *= scale;
            else if (gizmo.activeAxis == AXIS_Z) gizmo.targetObject->sz *= scale;
            
            if (gizmo.targetObject->sx < 0.1f) gizmo.targetObject->sx = 0.1f;
            if (gizmo.targetObject->sy < 0.1f) gizmo.targetObject->sy = 0.1f;
            if (gizmo.targetObject->sz < 0.1f) gizmo.targetObject->sz = 0.1f;
        }
    }
    
    gizmo.lastMouseX = mouseX;
    gizmo.lastMouseY = mouseY;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!gizmo.isDragging) {
        CAMERA_middleMouseLogic(camera, window);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        if (action == GLFW_PRESS) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            GizmoAxis clicked = checkAxisClick(mouseX, mouseY, width, height);
            
            if (clicked != AXIS_NONE) {
                gizmo.isDragging = true;
                gizmo.activeAxis = clicked;
                printf("Grabbed %s axis\n", 
                    clicked == AXIS_X ? "X" : clicked == AXIS_Y ? "Y" : "Z");
            }
        } else if (action == GLFW_RELEASE) {
            gizmo.isDragging = false;
            gizmo.activeAxis = AXIS_NONE;
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        //HERE I CHANGE OBJECT INDEX
        if (key == GLFW_KEY_1) {
            selectedObject = 0;
            gizmo.targetObject = &objects[0];
            printf("Selected object 1\n");
        }
        if (key == GLFW_KEY_2) {
            selectedObject = 1;
            gizmo.targetObject = &objects[1];
            printf("Selected object 2\n");
        }
        if (key == GLFW_KEY_3) {
            selectedObject = 2;
            gizmo.targetObject = &objects[2];
            printf("Selected object 3\n");
        }
        
        if (key == GLFW_KEY_W) {
            gizmo.mode = GIZMO_TRANSLATE;
            printf("Translate mode\n");
        }
        if (key == GLFW_KEY_E) {
            gizmo.mode = GIZMO_ROTATE;
            printf("Rotate mode\n");
        }
        if (key == GLFW_KEY_R) {
            gizmo.mode = GIZMO_SCALE;
            printf("Scale mode\n");
        }
    }
}

int main() {
    camera = malloc(sizeof(Camera));
    CAMERA_init(camera);
    
    gizmo.mode = GIZMO_TRANSLATE;
    gizmo.activeAxis = AXIS_NONE;
    gizmo.posX = gizmo.posY = gizmo.posZ = 0;
    gizmo.size = 1.5f;
    gizmo.isDragging = false;
    gizmo.targetObject = &objects[0];
    
    if (!glfwInit()) return -1;
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Cglib Editor", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    
    glfwMakeContextCurrent(window);
    glewInit();
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    glEnable(GL_DEPTH_TEST);
    LIGHT_enable();
    LIGHT_ambient(0.1f, 0.1f, 0.1f);
    
    printf("=== Controls ===\n");
    printf("1/2/3 - Select object\n");
    printf("W - Translate, E - Rotate, R - Scale\n");
    printf("Click gizmo axes to drag!\n");
    printf("WASD + Mouse - Camera control\n");
    printf("================\n");
    
    while (!glfwWindowShouldClose(window)) {
        CAMERA_movement(camera, window);
        
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        updateGizmo(mouseX, mouseY);
        
        glClearColor(0.7, 0.7, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        CAMERA_perspective(camera, 1280, 720);
        LIGHT_apply(lights, sizeof(lights) / sizeof(LightSource));
        
        CUBE_drawArray(objects, sizeof(objects) / sizeof(CubeObject));
        
        drawGizmo();
        
        lights[1].x = camera->camX;
        lights[1].y = camera->camY;
        lights[1].z = camera->camZ;
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    free(camera);
    glfwTerminate();
    return 0;
}