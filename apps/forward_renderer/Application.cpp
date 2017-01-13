#include "Application.hpp"

// classics
#include <iostream>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

// imgui
#include <imgui.h>

// glmlv
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/Image2DRGBA.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));
        drawObject(&m_cubeVAO, &m_texCube, cube, modelMatrix, diffuseCubeColor);

        modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 2, -6));
        drawObject(&m_sphereVAO, &m_texSphere, sphere, modelMatrix, diffuseSphereColor);

        // GUI
        ImGui_ImplGlfwGL3_NewFrame();
		gui(clearColor);

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            camera.updateViewController(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" },
	camera(m_GLFWHandle.window(), glm::radians(70.f), (static_cast<float>(m_nWindowWidth) / m_nWindowHeight))
{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
   
    // Here we load and compile shaders from the library
    //-----------------------------------------------------------------SHADER----------------------------------------------------SHADER------------
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();
    glEnable(GL_DEPTH_TEST);
	
	//Init Uniforms Variables
    initUniforms();

    //Cube
    cube = glmlv::makeCube();
    initVboIbo(&m_cubeVBO, &m_cubeIBO, cube);
    initVao(&m_cubeVAO, &m_cubeVBO, &m_cubeIBO);
    initTexBuffer(&m_texCube, "red_panda_2.jpg");
    //Sphere
    sphere = glmlv::makeSphere(16);
    initVboIbo(&m_sphereVBO, &m_sphereIBO, sphere);
    initVao(&m_sphereVAO, &m_sphereVBO, &m_sphereIBO);
    initTexBuffer(&m_texSphere, "red_panda_1.jpg");

	//DirectionalLight
	directionalLightDir = glm::vec3(1, 1, 0);
	directionalLightIntensity = 25;

	//PointLight
	pointLightPosition = glm::vec3(-5, 0, 0);
	pointLightIntensity = 5;

    //Sampler
    initSampler();
}

Application::~Application()
{
    if (m_cubeVBO) glDeleteBuffers(1, &m_cubeVBO);
    if (m_cubeIBO) glDeleteBuffers(1, &m_cubeIBO);
    if (m_cubeVAO) glDeleteBuffers(1, &m_cubeVAO);

    if (m_sphereVBO) glDeleteBuffers(1, &m_sphereVBO);
    if (m_sphereIBO) glDeleteBuffers(1, &m_sphereIBO);
    if (m_sphereVAO) glDeleteBuffers(1, &m_sphereVAO);

    if (m_texCube) glDeleteTextures(1, &m_texCube);
    if (m_texSphere) glDeleteTextures(1, &m_texSphere);
    if (m_sampler) glDeleteSamplers(1, &m_sampler);

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}

// PRIVATE FUNCTIONS
void Application::gui(float clearColor[3])
{
	ImGui::Begin("GUI");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
	if (ImGui::ColorEdit3("clearColor", clearColor)) {
		glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
	}

    // Diffuse Color
	ImGui::ColorEdit3("Diffuse Cube Color", glm::value_ptr(diffuseCubeColor));    
    ImGui::ColorEdit3("Diffuse Sphere Color", glm::value_ptr(diffuseSphereColor));

    // Light intensity
	ImGui::SliderFloat("Directional Light Intensity", &directionalLightIntensity, 0, 100.f);
	ImGui::SliderFloat("Point Light Intensity", &pointLightIntensity, 0, 100.f);

    // Texture
    std::string activeTextureButton = activeTexture? "Without Textures": "With Textures";
    if (ImGui::Button(activeTextureButton.c_str()))
    {
        activeTexture = !activeTexture;
        unBindTex(); // TO MOVE
    }

	ImGui::End();
}

void Application::initUniforms()
{
	// Camera
    u_modelViewProjMatrix = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    u_modelViewMatrix = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    u_normalMatrix = glGetUniformLocation(m_program.glId(), "uNormalMatrix");

	// Lights
	u_directionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
	u_directionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");
	u_pointLightPosition = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
	u_pointLightIntensity = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");
	u_Kd = glGetUniformLocation(m_program.glId(), "uKd");

    u_activeTexture = glGetUniformLocation(m_program.glId(), "uActiveTexture");
    u_KdSampler = glGetUniformLocation(m_program.glId(), "uKdSampler");
}

void Application::initSampler()
{
    glGenSamplers(1, &m_sampler);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Application::initTexBuffer(GLuint* m_texObject, const std::string& nameFile)
{
    auto imageTex = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / nameFile);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, m_texObject);
    glBindTexture(GL_TEXTURE_2D, *m_texObject);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, imageTex.width(), imageTex.height());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageTex.width(), imageTex.height(), GL_RGBA, GL_UNSIGNED_BYTE, imageTex.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::initVao(GLuint* vao, GLuint* vbo, GLuint* ibo)
{
    glGenVertexArrays(1, vao);

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = glGetAttribLocation(m_program.glId(), "aPosition");
    const GLint normalAttrLocation = glGetAttribLocation(m_program.glId(), "aNormal");
    const GLint textAttrLocation = glGetAttribLocation(m_program.glId(), "aTexCoords");

    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    if (normalAttrLocation > 0)
    {
        glEnableVertexAttribArray(normalAttrLocation);
        glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    }

    if (textAttrLocation > 0)
    {
        glEnableVertexAttribArray(textAttrLocation);
        glVertexAttribPointer(textAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Application::initVboIbo(GLuint* vbo, GLuint* ibo, const glmlv::SimpleGeometry& object)
{
    // VBO
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferStorage(GL_ARRAY_BUFFER, object.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), object.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // IBO
    glGenBuffers(1, ibo);
    glBindBuffer(GL_ARRAY_BUFFER, *ibo);
    glBufferStorage(GL_ARRAY_BUFFER, object.indexBuffer.size() * sizeof(uint32_t), object.indexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Application::drawObject(GLuint* vao, GLuint* m_texObject, const glmlv::SimpleGeometry& object, const glm::mat4& modelMatrix, const glm::vec3& diffuseColor)
{
	setUniformsValues(modelMatrix, diffuseColor);
    if(activeTexture) bindTex(m_texObject);
    glBindVertexArray(*vao);
    glDrawElements(GL_TRIANGLES, object.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);   
}

void Application::setUniformsValues(const glm::mat4& modelMatrix, const glm::vec3& diffuseColor)
{
    // Camera
	glUniformMatrix4fv(u_modelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeModelViewProjMatrix(modelMatrix)));
	glUniformMatrix4fv(u_modelViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeModelViewMatrix(modelMatrix)));
	glUniformMatrix4fv(u_normalMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeNormalMatrix(modelMatrix)));

    // Lights
	glUniform3fv(u_directionalLightDir, 1, glm::value_ptr(directionalLightDir));
    glm::vec3 directionalLightIntensityVec3 = glm::vec3(directionalLightIntensity);
	glUniform3fv(u_directionalLightIntensity, 1, glm::value_ptr(directionalLightIntensityVec3));
	
    glUniform3fv(u_pointLightPosition, 1, glm::value_ptr(pointLightPosition));
    glm::vec3 pointLightIntensityVec3 = glm::vec3(pointLightIntensity);
	glUniform3fv(u_pointLightIntensity, 1, glm::value_ptr(pointLightIntensityVec3));

    // Color
	glUniform3fv(u_Kd, 1, glm::value_ptr(diffuseColor));

    // "Texture"
    glUniform1i(u_activeTexture, activeTexture);
}

void Application::bindTex(GLuint* m_texObject)
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(u_KdSampler, 0);
    glBindSampler(0, m_sampler);
    glBindTexture(GL_TEXTURE_2D, *m_texObject);
}

void Application::unBindTex()
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(u_KdSampler, 0);
    glBindSampler(0, 0); // MEH
    glBindTexture(GL_TEXTURE_2D, 0);
}



