#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cube.h"
#include "Lighting.h"
#include "Camera.h"
#include "Gizmo.h"

CubeObject objects[] = {
    {0, 0, 0, 0.7, 0.7, 0.7, 3, 3, 3, 0, 0, 0},
    {2, 3, 4, 0.15, 0.5, 0.2, 2, 2, 4, 0, 0, 0},
    {-3, -3, -3, 0.15, 0.5, 0.2, 30, 1, 30, 0, 0, 0},
};

LightSource lights[] = {
    {6, 6, 6, 2.7f, 2.7f, 2.7f, 0, 0},
    {0, 0, 0, 3.0f, 3.0f, 3.0f, 0.01f, 0.005f},
};

Camera* camera;
Gizmo gizmo;
int selectedObject = 0;
struct nk_glfw glfw = {0};
struct nk_context *ctx;

// UI state variables
static char text_buffer[256] = "";
static int text_len = 0;
static int slider_value = 50;
static int checkbox_value = 1;
static int counter = 0;

void handle_custom_input(GLFWwindow* window) {
    // Only process custom hotkeys when UI is NOT focused
    if (nk_window_is_any_hovered(ctx) || nk_item_is_any_active(ctx)) {
        return; // UI has focus, let Nuklear handle everything
    }
    
    // Object selection
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        selectedObject = 0;
        GIZMO_setTarget(&gizmo, &objects[0]);
        printf("Selected object 1\n");
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        selectedObject = 1;
        GIZMO_setTarget(&gizmo, &objects[1]);
        printf("Selected object 2\n");
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        selectedObject = 2;
        GIZMO_setTarget(&gizmo, &objects[2]);
        printf("Selected object 3\n");
    }
    
    // Gizmo mode
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        GIZMO_setMode(&gizmo, GIZMO_TRANSLATE);
        printf("Translate mode\n");
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        GIZMO_setMode(&gizmo, GIZMO_ROTATE);
        printf("Rotate mode\n");
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        GIZMO_setMode(&gizmo, GIZMO_SCALE);
        printf("Scale mode\n");
    }
}

void handle_gizmo_interaction(GLFWwindow* window) {
    // Only handle gizmo if UI is not being interacted with
    if (nk_window_is_any_hovered(ctx)) {
        return;
    }
    
    static int wasLeftPressed = 0;
    int isLeftPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // Left mouse button just pressed
    if (isLeftPressed && !wasLeftPressed) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        GizmoAxis clicked = GIZMO_checkAxisClick(&gizmo, mouseX, mouseY, width, height);
        
        if (clicked != AXIS_NONE) {
            GIZMO_startDrag(&gizmo, clicked);
            printf("Grabbed %s axis\n", 
                clicked == AXIS_X ? "X" : clicked == AXIS_Y ? "Y" : "Z");
        }
    }
    // Left mouse button released
    else if (!isLeftPressed && wasLeftPressed) {
        GIZMO_stopDrag(&gizmo);
    }
    
    wasLeftPressed = isLeftPressed;
}

int main() {
    camera = malloc(sizeof(Camera));
    CAMERA_init(camera);
    
    GIZMO_init(&gizmo, &objects[0]);
    
    if (!glfwInit()) return -1;
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Cglib Editor", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    
    glfwMakeContextCurrent(window);
    glewInit();
    
    // Initialize Nuklear with automatic callback installation
    // This lets Nuklear handle ALL input callbacks automatically
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
        glfwPollEvents();
        
        // Handle custom input ONLY when UI doesn't have focus
        handle_custom_input(window);
        
        // Camera movement (only when not interacting with gizmo)
        if (!GIZMO_isDragging(&gizmo)) {
            CAMERA_middleMouseLogic(camera, window);
        }
        CAMERA_movement(camera, window);
        // Update gizmo with mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        GIZMO_update(&gizmo, mouseX, mouseY);
        
        // Handle gizmo interaction
        handle_gizmo_interaction(window);
        
        glClearColor(0.7, 0.7, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Start Nuklear frame
        nk_glfw3_new_frame(&glfw);
        
        // ===== UI =====
        if (nk_begin(ctx, "Demo Panel", nk_rect(0, 0, 250, 720),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)) {
            
            // Some text labels
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Hello from Nuklear UI!", NK_TEXT_LEFT);
            nk_label(ctx, "This is a simple demo", NK_TEXT_CENTERED);
            
            // Buttons that print stuff
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_button_label(ctx, "Button 1")) {
                printf("Button 1 clicked!\n");
            }
            if (nk_button_label(ctx, "Button 2")) {
                printf("Button 2 pressed!\n");
            }
            
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Print Counter")) {
                counter++;
                printf("Counter: %d\n", counter);
            }
            
            // Text input field - Nuklear handles ALL keyboard input automatically!
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Text Input:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_edit_string(ctx, NK_EDIT_FIELD, text_buffer, &text_len, sizeof(text_buffer) - 1, nk_filter_default);
            
            // Button to print text
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Print Text")) {
                printf("Text: %s\n", text_buffer);
            }
            
            // Slider
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Slider Value:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_slider_int(ctx, 0, &slider_value, 100, 1);
            
            // Show slider value
            nk_layout_row_dynamic(ctx, 20, 1);
            char buffer[64];
            sprintf(buffer, "Value: %d", slider_value);
            nk_label(ctx, buffer, NK_TEXT_LEFT);
            
            // Checkbox
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_checkbox_label(ctx, "Enable Feature", &checkbox_value);
            
            // Show checkbox state
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, checkbox_value ? "Feature: ON" : "Feature: OFF", NK_TEXT_LEFT);
            
            // More random buttons
            nk_layout_row_dynamic(ctx, 30, 3);
            if (nk_button_label(ctx, "A")) printf("A\n");
            if (nk_button_label(ctx, "B")) printf("B\n");
            if (nk_button_label(ctx, "C")) printf("C\n");
        }
        nk_end(ctx);
        if (nk_begin(ctx, "FIles and stuff", nk_rect(250, 0, 1280-250, 100),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE)) {
            
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Yayy this is working!", NK_TEXT_LEFT);
            nk_label(ctx, "Adding here: Export to OpenVision", NK_TEXT_CENTERED);
            nk_label(ctx, "Adding here: Export to Cglib", NK_TEXT_CENTERED);
            nk_label(ctx, "Adding here: Export to Add scripts", NK_TEXT_CENTERED);
        }
        nk_end(ctx);
        // Render 3D scene
        glEnable(GL_DEPTH_TEST);
        CAMERA_perspective(camera, 1280, 720);
        LIGHT_apply(lights, sizeof(lights) / sizeof(LightSource));
        
        CUBE_drawArray(objects, sizeof(objects) / sizeof(CubeObject));
        
        GIZMO_draw(&gizmo);
        
        lights[1].x = camera->camX;
        lights[1].y = camera->camY;
        lights[1].z = camera->camZ;
        
        // Render Nuklear UI on top
        glDisable(GL_DEPTH_TEST);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        
        glfwSwapBuffers(window);
    }
    
    nk_glfw3_shutdown(&glfw);
    free(camera);
    glfwTerminate();
    return 0;
}