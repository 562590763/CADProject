#include "main.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Other includes
#include "Example.h"
#include "Draw.h"

Example *example = new Example();
Solid *solid = example->scene[0]->getSolid_list()[0];
Solid *tempSolid = solid;

int counter = 0;
static int sceneID = 0, preSceneID = sceneID, drawMode = 0, preDrawMode = drawMode;
bool drawLine = false, light = true, canRotate = true, preRotate = canRotate;
GLfloat ambient = 0.2f, diffuse = 0.9f, specular = 0.6f;
glm::vec3 clearColor(0.4f, 0.5f, 0.6f);
GLfloat lightPos[] = {camera.Position[0], camera.Position[1], camera.Position[2], 1.0f};

int main() {

    // Init GLFW
    InitGLFW();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Initialize Done." << std::endl;

     // Game loop
    while(!glfwWindowShouldClose(window)) {

        if(light) InitLight();
        else {
            glDisable(GL_COLOR_MATERIAL);
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
        }

        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        do_movement();

        // Clear the colorbuffer
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Perspective
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 1.0f, 1000.0f);
        // Reset the lookat matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.Position[0], camera.Position[1], camera.Position[2],
                  camera.GetCenter()[0], camera.GetCenter()[1], camera.GetCenter()[2],
                  camera.Up[0], camera.Up[1], camera.Up[2]);

        // Draw solids
        Display(tempSolid, drawLine);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(425, 280));
        ImGui::SetNextWindowPos(ImVec2(5, 5));
        ImGui::Begin("CADProject"); 
        {   
            ImGui::Text("This is a GUI that controls the scene:");
            ImGui::Text("(1)Scroll the mouse to zoom the scene.");
            ImGui::Text("(2)Use A,S,D,W to move the camera.");
            ImGui::Text("(3)Use SPACE to raise the camera and V to lower the camera.");

            // Mouse rotate
            // ImGui::Checkbox("Rotate", &canRotate); ImGui::SameLine();

            // Light
            ImGui::Checkbox("Light", &light);
            ImGui::SliderFloat("Ambient Light", &ambient, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse Light", &diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular Light", &specular, 0.0f, 1.0f);
            ImGui::ColorEdit3("Clear Color", (float *)&clearColor);

            // Scene
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Choose a scene:"); ImGui::SameLine();
            ImGui::RadioButton("Demo", &sceneID, 0); ImGui::SameLine();
            ImGui::RadioButton("Flag", &sceneID, 1); ImGui::SameLine();
            ImGui::RadioButton("Letter", &sceneID, 2); ImGui::SameLine();
            ImGui::RadioButton("Chamfer", &sceneID, 3);

            // Drawing mode
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Choose drawing mode:"); ImGui::SameLine();
            ImGui::RadioButton("Filling", &drawMode, 0); ImGui::SameLine();
            ImGui::RadioButton("Wireframe", &drawMode, 1);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InitLight() {
    GLfloat ambientLight[] = {ambient, ambient, ambient, 1.0f}; // ambient light
    GLfloat diffuseLight[] = {diffuse, diffuse, diffuse, 1.0f}; // diffuse light
    GLfloat specularLight[] = {specular, specular, specular, 1.0f}; // specular light

    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);
    glMateriali(GL_FRONT, GL_SHININESS, 50);
}

void InitGLFW() {

    // Init GLFW
    if(!glfwInit()) {
        printf("GLFW init error.");
    }
    // Set all the required options for GLFW
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window with graphics context
    window = glfwCreateWindow(WIDTH, HEIGHT, "CADProject", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // GLFW Options
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);

    // Print out some info about the graphics drivers
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

// Moves/alters the camera positions based on user input
void do_movement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(keys[GLFW_KEY_SPACE])
        camera.ProcessKeyboard(UP, deltaTime);
    if(keys[GLFW_KEY_V])
        camera.ProcessKeyboard(DOWN, deltaTime);

    // Use the mouse to rotate the camera
    if(preRotate != canRotate) {
        if(!canRotate) glfwSetCursorPosCallback(window, nullptr);
        else glfwSetCursorPosCallback(window, mouse_callback);
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
        preRotate = canRotate;
    }

    // Toggle drawing mode
    if(preDrawMode != drawMode) {
        if(drawMode == 0) drawLine = false;
        else drawLine = true;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
        preDrawMode = drawMode;
    }
    //if(keys[GLFW_KEY_Q]) {
    //    drawLine = false;
    //    camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    //}
    //if(keys[GLFW_KEY_E]) {
    //    drawLine = true;
    //    camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    //}

    // Switch scenes
    if(preSceneID != sceneID) {
        solid = example->scene[sceneID]->getSolid_list()[0];
        tempSolid = solid;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
        preSceneID = sceneID;
    }
    /*if(keys[GLFW_KEY_0]) {
        solid = example->scene[0]->getSolid_list()[0];
        tempSolid = solid;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    } else if(keys[GLFW_KEY_1]) {
        solid = example->scene[1]->getSolid_list()[0];
        tempSolid = solid;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    } else if(keys[GLFW_KEY_2]) {
        solid = example->scene[2]->getSolid_list()[0];
        tempSolid = solid;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    } else if(keys[GLFW_KEY_3]) {
        solid = example->scene[3]->getSolid_list()[0];
        tempSolid = solid;
        camera.ResetCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    }*/
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
