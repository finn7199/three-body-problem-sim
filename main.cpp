#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "simulation_config.h"
#include "Shader.h"
#include "Simulation.h"
#include "Camera.h"

#include <glm/gtx/norm.hpp>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(std::vector<std::string> faces);

// Camera
Camera camera(glm::vec3(0.0f, 300.0f, 650.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float accumulator = 0.0f;

Simulation* simulation = nullptr;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Three Body Simulation", NULL, NULL);
    if (window == NULL) { return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to initialize GLAD" << std::endl; return -1; }

    simulation = new Simulation();
    simulation->SetIntegrator(IntegratorMethod::RK4);
    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Build and compile shaders
    Shader sphereShader("sphere.vs", "sphere.frag");
    Shader skyboxShader("skybox.vs", "skybox.frag");
    Shader blurShader("quad.vs", "blur.frag");
    Shader bloomFinalShader("quad.vs", "bloom_final.frag");

    std::vector<std::string> faces{
    "skybox/px.jpg",  // Right
    "skybox/nx.jpg",  // Left
    "skybox/py.jpg",  // Top
    "skybox/ny.jpg",  // Bottom
    "skybox/pz.jpg",  // Front
    "skybox/nz.jpg"   // Back
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // --- Skybox setup ---
    float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // --- IBL SETUP USING 6 JPEG FILES ---
   // This process creates our irradiance map from a standard cubemap.
   // 1. Create a framebuffer for our off-screen rendering
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // 2. Create an empty cubemap to store the final irradiance map
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3. Set up the projection and view matrices for rendering to the cubemap faces
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // 4. Use the "irradiance_convolution" shader to process the skybox
    Shader irradianceShader("cubemap.vs", "irradiance_convolution.frag");
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // original skybox cubemap

    glViewport(0, 0, 32, 32); // Render to a small 32x32 cubemap for the blur
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --- Bloom Framebuffer Setup ---
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
    }

    // --- Screen Quad Setup ---
    float quadVertices[] = {
    -1.0f,  1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,  1.0f,  1.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Light source position
    glm::vec3 lightPos(0.0f, 0.0f, 100.0f);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- ENERGY LOGGING SETUP ---
    std::ofstream energyLog("energy_log.csv");
    energyLog << "Time,TotalEnergy\n"; // Write the CSV header
    double totalSimTime = 0.0;
    int physicsSteps = 0;
    // --- END OF LOGGING SETUP --
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Physics Update
        accumulator += deltaTime;
        const int MAX_STEPS_PER_FRAME = 5;
        int steps_taken = 0;
        while (accumulator >= FIXED_TIMESTEP && steps_taken < MAX_STEPS_PER_FRAME) {
            simulation->update(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
            steps_taken++;

            // --- LOGGING LOGIC ---
            totalSimTime += FIXED_TIMESTEP;
            physicsSteps++;
            // Log the energy every 10 physics steps to keep the file size reasonable
            if (physicsSteps % 10 == 0)
            {
                double totalEnergy = simulation->CalculateTotalEnergy();
                energyLog << totalSimTime << "," << totalEnergy << "\n";
            }
            // --- END OF LOGGING LOGIC ---
        }

        // --- RENDER SCENE TO HDR FRAMEBUFFER ---
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = simulation->bodies[0].position;
        glm::vec3 lightColor = glm::vec3(1.0f);

        // Render space objects
        sphereShader.use();
        sphereShader.setMat4("projection", projection);
        sphereShader.setMat4("view", view);
        // Set uniforms that are the same for all objects
        sphereShader.setVec3("lightPos", lightPos);
        sphereShader.setVec3("lightColor", lightColor);
        sphereShader.setVec3("viewPos", camera.Position);
        // --- IBL ---
        sphereShader.setInt("irradianceMap", 1); // Tell shader to use texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        for (const auto& body : simulation->bodies) {
            // Set the uniforms that are unique to this body
            sphereShader.setBool("isEmissive", body.isEmissive);
            sphereShader.setVec3("objectColor", body.color);
            sphereShader.setFloat("shininess", 10.0f); // Could also be a property per-body

            body.draw(sphereShader);
        }

        // --- RENDER PARTICLE TRAILS ---
        //glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        for (const auto& body : simulation->bodies) {
            body.RenderTrail(view, projection);
        }
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       // glEnable(GL_DEPTH_TEST);

        // --- BLUR BRIGHT FRAGMENTS (BLOOM PASS) ---
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        blurShader.setInt("image", 0);
        for (unsigned int i = 0; i < amount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setBool("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            horizontal = !horizontal;
            if (first_iteration) first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // --- RENDER FINAL IMAGE ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // a. Draw Skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setInt("skybox", 0);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        // b. Draw Bloomed Scene on top
        bloomFinalShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        bloomFinalShader.setInt("scene", 0);
        bloomFinalShader.setInt("bloomBlur", 1);
        bloomFinalShader.setFloat("exposure", 0.75f);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    energyLog.close();
    delete simulation;
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        simulation->LoadScenario(Scenario::FIGURE_EIGHT);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        simulation->LoadScenario(Scenario::SOLAR_SYSTEM);

    //Integrator selection
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        simulation->SetIntegrator(IntegratorMethod::EULER);
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        simulation->SetIntegrator(IntegratorMethod::LEAPFROG);
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        simulation->SetIntegrator(IntegratorMethod::RK4);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (camera.Zoom >= 1.0f && camera.Zoom <= 90.0f)
        camera.Zoom -= yoffset;
    if (camera.Zoom <= 1.0f)
        camera.Zoom = 1.0f;
    if (camera.Zoom >= 90.0f)
        camera.Zoom = 90.0f;
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}