#include"Camera.h"
void CAMERA_init(Camera* camera) {
    glm_vec3_copy((vec3){0.0f, 2.0f, 8.0f}, camera->cameraPos);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->cameraFront);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->cameraUp);

    camera->lastX = 400.0f;
    camera->lastY = 300.0f;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->firstMouse = 1;
    camera->speed = 2.5f;
    camera->fov = 45.0f;
}
void CAMERA_mouseRotation(Camera* camera,GLFWwindow* window, double xpos, double ypos)
{
    if (camera->firstMouse) {
        camera->lastX = xpos;
        camera->lastY = ypos;
        camera->firstMouse = 0;
    }
    float xoffset = xpos - camera->lastX;
    float yoffset = camera->lastY - ypos;
    camera->lastX = xpos;
    camera->lastY = ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    camera->yaw += xoffset;
    camera->pitch += yoffset;
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    camera->cameraFront[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    camera->cameraFront[1] = sin(glm_rad(camera->pitch));
    camera->cameraFront[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_vec3_normalize(camera->cameraFront);
}
void CAMERA_wasd(Camera* camera, GLFWwindow* window, double deltaTime)
{
    float cameraSpeed = camera->speed * deltaTime;
    vec3 temp;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        glm_vec3_scale(camera->cameraFront, cameraSpeed, temp);
        glm_vec3_add(camera->cameraPos, temp, camera->cameraPos);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        glm_vec3_scale(camera->cameraFront, cameraSpeed, temp);
        glm_vec3_sub(camera->cameraPos, temp, camera->cameraPos);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        vec3 crossVec;
        glm_vec3_cross(camera->cameraFront, camera->cameraUp, crossVec);
        glm_vec3_normalize(crossVec);
        glm_vec3_scale(crossVec, cameraSpeed, temp);
        glm_vec3_sub(camera->cameraPos, temp, camera->cameraPos);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        vec3 crossVec;
        glm_vec3_cross(camera->cameraFront, camera->cameraUp, crossVec);
        glm_vec3_normalize(crossVec);
        glm_vec3_scale(crossVec, cameraSpeed, temp);
        glm_vec3_add(camera->cameraPos, temp, camera->cameraPos);
    }
}
void CAMERA_perspective(Camera* camera, int screenX, int screenY,mat4 view, mat4 projection)
{
    vec3 target = {
        camera->cameraPos[0] + camera->cameraFront[0], 
        camera->cameraPos[1] + camera->cameraFront[1], 
        camera->cameraPos[2] + camera->cameraFront[2]
    };
    glm_lookat(camera->cameraPos, target, camera->cameraUp, view);
    glm_perspective(glm_rad(camera->fov), (float)screenX/(float)screenY, 0.1f, 100.0f, projection);

}