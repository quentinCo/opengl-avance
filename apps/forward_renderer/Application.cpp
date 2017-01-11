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
        glm::mat4 viewMatrix = viewController.getViewMatrix();
        drawObject(&m_cubeVAO, cube, modelMatrix, viewMatrix);

        modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 2, -6));
        drawObject(&m_sphereVAO, sphere, modelMatrix, viewMatrix);

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
            viewController.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    viewController(m_GLFWHandle.window())
{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
   
    // Here we load and compile shaders from the library
    //-----------------------------------------------------------------SHADER----------------------------------------------------SHADER------------
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    
    glEnable(GL_DEPTH_TEST);

    m_program.use();

	//Camera
	float aspectScreenRatio = static_cast<float>(m_nWindowWidth) / m_nWindowHeight;
	projMatrix = glm::perspective(glm::radians(70.f), aspectScreenRatio, 0.1f, 100.f);

	//Init Uniforms Variables
    initUniforms();

    //Cube
    cube = glmlv::makeCube();
    initVboIbo(&m_cubeVBO, &m_cubeIBO, cube);
    initVao(&m_cubeVAO, &m_cubeVBO, &m_cubeIBO);
    //Sphere
    sphere = glmlv::makeSphere(16);
    initVboIbo(&m_sphereVBO, &m_sphereIBO, sphere);
    initVao(&m_sphereVAO, &m_sphereVBO, &m_sphereIBO);
	// TO_TEST_LIGHT
	//DirectionalLight
	glm::vec3 directionalLightDir = glm::vec3(1, 1, 0);
	glm::vec3 directionalLightIntensity = glm::vec3(0.25);

	//PointLight
	glm::vec3 pointLightPosition = glm::vec3(0);
	glm::vec3 pointLightIntensity = glm::vec3(0.5);
}

Application::~Application()
{
    if (m_cubeVBO) glDeleteBuffers(1, &m_cubeVBO);
    if (m_cubeIBO) glDeleteBuffers(1, &m_cubeIBO);
    if (m_cubeVAO) glDeleteBuffers(1, &m_cubeVAO);

    if (m_sphereVBO) glDeleteBuffers(1, &m_sphereVBO);
    if (m_sphereIBO) glDeleteBuffers(1, &m_sphereIBO);
    if (m_sphereVAO) glDeleteBuffers(1, &m_sphereVAO);

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
	// TO_TEST_LIGHT
	// -> surmment changer vec3 -> vec4, puis re vec3 pour les uniformes
	ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(diffuseColor));
	ImGui::SliderFloat3("Directional Light Intensity", glm::value_ptr(directionalLightIntensity), 0, 1.f);
	ImGui::SliderFloat3("Point Light Intensity", glm::value_ptr(pointLightIntensity), 0, 1.f);

	ImGui::End();
}

void Application::initUniforms()
{
	// Camera
    u_modelViewProjMatrix = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    u_modelViewMatrix = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    u_normalMatrix = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
	// TO_TEST_LIGHT
	// Lights
	u_directionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
	u_directionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");
	u_pointLightPosition = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
	u_pointLightIntensity = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");
	u_Kd = glGetUniformLocation(m_program.glId(), "uKd");
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

    if (positionAttrLocation > 0)
    {
        glEnableVertexAttribArray(positionAttrLocation);
        glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    }

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

void Application::drawObject(GLuint* vao, const glmlv::SimpleGeometry& object, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix)
{
	setUniformsValues(viewMatrix * modelMatrix);

    glBindVertexArray(*vao);

    glDrawElements(GL_TRIANGLES, object.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);   
}

void Application::setUniformsValues(const glm::mat4& modelViewMatrix)
{
	glUniformMatrix4fv(u_modelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix * modelViewMatrix));
	glUniformMatrix4fv(u_modelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix4fv(u_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(modelViewMatrix))));

	glUniform3fv(u_directionalLightDir, 1, glm::value_ptr(directionalLightDir));
	glUniform3fv(u_directionalLightIntensity, 1, glm::value_ptr(directionalLightIntensity));
	glUniform3fv(u_pointLightPosition, 1, glm::value_ptr(pointLightPosition));
	glUniform3fv(u_pointLightIntensity, 1, glm::value_ptr(pointLightIntensity));
	glUniform3fv(u_Kd, 1, glm::value_ptr(diffuseColor));
}