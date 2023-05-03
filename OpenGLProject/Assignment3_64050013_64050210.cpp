#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;

float pitch = 0.0f , yaw = -90.0f ;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

glm::vec3 lightColour = glm::vec3(1.0f,1.0f,1.0f);

void CreateTriangle()
{
    GLfloat vertices[] =
    {                               //aTexCoord
        -1.0f, -1.0f, 0.0f,         0.0f,0.0f,
        0.0f,   -1.0f,  1.0f,       0.5f,0.0f,
        1.0f, -1.0f, 0.0f,          1.0f,0.0f,
        0.0f, 1.0f, 0.0f,           0.5f,1.0f
    };

    unsigned int indices[] = //������ҧ����������
    {
        0,  3,  1,
        1,  3,  2,
        2,  3,  0,
        0, 1, 2,
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12); //9 ��� �ش , 3 ��� �ӹǹ index
    meshList.push_back(obj1);
}
void CreateOBJ()
{
    Mesh* obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/mount.obj");
    Mesh* obj2 = new Mesh();
    bool loaded2 = obj2->CreateMeshFromOBJ("Models/mustang.obj");
    Mesh* obj3 = new Mesh();
    bool loaded3 = obj3->CreateMeshFromOBJ("Models/ganapati.obj");

    if (loaded && loaded2 && loaded3)

    {
        meshList.push_back(obj1);
        meshList.push_back(obj2);
        meshList.push_back(obj3);

    }
    else
    {
        std::cout << "Failed to load Model" << std::endl;
    }
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void checkMouse() 
{
    float xoffset, yoffset;

    double xpos, ypos;
    glfwGetCursorPos(mainWindow.getWindow(), &xpos, &ypos);

    static float lastX = xpos;
    static float lastY = ypos;

    xoffset = xpos - lastX;
    yoffset = lastY - ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;

}
unsigned int loadTexture(const char *textureFile) {
    int width, height, nrChannels;
    unsigned char*data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}
int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth()/ (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-0.4f, 0.4f, -0.3f, 0.3f, 0.1f, 100.0f);

    glm::vec3 cameraPos = glm::vec3(2.0f, 5.0f, 10.0f);

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up,cameraDirection));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));


    float deltaTime, lastFrame = glfwGetTime();

    unsigned int tex1 = loadTexture("Textures/grass.jpg");
    unsigned int tex2 = loadTexture("Textures/dragon.jpg");
    unsigned int tex3 = loadTexture("Textures/rainbow.jpg");


   /* int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/island.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    stbi_image_free(data); */

    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Get + Handle user input events
        glfwPollEvents();
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraDirection *deltaTime *8.0f;
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraDirection * deltaTime *8.0f;
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= cameraRight * deltaTime *8.0f;
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += cameraRight * deltaTime * 8.0f;

        checkMouse();

        glm::vec3 direction;

        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        cameraDirection = glm::normalize(direction);

        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw here
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformProjection = shaderList[0].GetUniformLocation("projection");
        uniformView = shaderList[0].GetUniformLocation("view");

        glm::vec3 pyramidPositions[] =
        {
        glm::vec3(0.0f, 0.0f, -2.5f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
        };



        //Object
        
        glm::mat4 view (1.0f);
        /*
        glm::mat4 cameraPosMat(1.0f);

        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        glm::mat4 cameraRotateMat(1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0.0f);
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0.0f);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0.0f);

        view = cameraRotateMat * cameraPosMat; */

        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        //for (int i = 0; i < 10; i++)
       // {
            //model = glm::rotate(model, (float)glfwGetTime());
            glm::mat4 model( 1.0f );
            //model = glm::translate(model, pyramidPositions[i]);
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));
            //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

            glUniform3fv(shaderList[0].GetUniformLocation("lightColour"),1,(GLfloat*)&lightColour);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex2);
            meshList[1]->RenderMesh();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex3);
            meshList[2]->RenderMesh();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex1);
            meshList[0]->RenderMesh();
       // }
        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
