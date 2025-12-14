#include"Camera.h"
float lastMouseX = 400, lastMouseY = 300;
int firstMouse = 1;

void CAMERA_init(Camera* camera)
{
    if (!camera) return;
    camera->speed = 0.1f;
    camera->camX = 0.0f;
    camera->camY = 1.7f;
    camera->camZ = 5.0f;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->camForwardX = 0.0f;
    camera->camForwardY = 0.0f;
    camera->camForwardZ = -1.0f;
    camera->fov = 80.0f;
}
void CAMERA_middleMouseLogic(Camera* cam, GLFWwindow* window) {
    static double lastMouseX = 0.0, lastMouseY = 0.0;
    static bool firstMiddleMouse = true;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (firstMiddleMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMiddleMouse = false;
        }
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;
        float sensitivity = 0.1f;
        cam->yaw   += deltaX * sensitivity;
        cam->pitch -= deltaY * sensitivity;
        if (cam->pitch > 89.0f) cam->pitch = 89.0f;
        if (cam->pitch < -89.0f) cam->pitch = -89.0f;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    } else {
        firstMiddleMouse = true;
    }
}

void CAMERA_perspective(Camera* camera, int windowX, int windowY)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera->fov, (float)windowX/(float)windowY, 0.1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        camera->camX, camera->camY, camera->camZ,
        camera->camX + camera->camForwardX, camera->camY + camera->camForwardY, camera->camZ + camera->camForwardZ,
        0,1,0
    );
}
void CAMERA_rotation(Camera* camera)
{
    camera->camForwardX = cosf(camera->yaw * M_PI/180) * cosf(camera->pitch * M_PI/180);
    camera->camForwardY = sinf(camera->pitch * M_PI/180);
    camera->camForwardZ = sinf(camera->yaw * M_PI/180) * cosf(camera->pitch * M_PI/180);
}
void CAMERA_mouse(Camera* camera, double xMouse, double yMouse)
{
    if (firstMouse) {lastMouseX = xMouse;lastMouseY = yMouse;firstMouse = 0;}

    float xoffset = xMouse - lastMouseX;
    float yoffset = lastMouseY - yMouse; 
    lastMouseX = xMouse;
    lastMouseY = yMouse;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->yaw   += xoffset;
    camera->pitch += yoffset;

    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    CAMERA_rotation(camera);

}
void CAMERA_movement(Camera* camera, GLFWwindow* window)
{

    float rightX = camera->camForwardZ;
    float rightY = 0;
    float rightZ = -camera->camForwardX;

    float len = sqrt(rightX*rightX + rightZ*rightZ);
    rightX /= len; rightZ /= len;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {camera->yaw -= 1;}
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {camera->yaw += 1;}

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {camera->pitch -= 0.7f;}
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {camera->pitch += 0.7f;}
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {camera->camY -= camera->speed;}
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {camera->camY +=camera->speed;}
    
    CAMERA_rotation(camera);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->camX += camera->camForwardX * camera->speed;
        camera->camY += camera->camForwardY * camera->speed;
        camera->camZ += camera->camForwardZ * camera->speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->camX -= camera->camForwardX * camera->speed;
        camera->camY -= camera->camForwardY * camera->speed;
        camera->camZ -= camera->camForwardZ * camera->speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->camX += rightX * camera->speed;
        camera->camZ += rightZ * camera->speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->camX -= rightX * camera->speed;
        camera->camZ -= rightZ * camera->speed;
    }
}