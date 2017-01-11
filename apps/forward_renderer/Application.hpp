#pragma once

#include <vector>

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/ViewController.hpp>

class Application
{
public:
    Application(int argc, char** argv);
	~Application();

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ static_cast<int>(m_nWindowWidth), static_cast<int>(m_nWindowHeight), "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;

	glmlv::SimpleGeometry cube;
    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;

    glmlv::SimpleGeometry sphere;
    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;

    glmlv::GLProgram m_program;

    GLuint u_modelViewProjMatrix;
    GLuint u_modelViewMatrix;
    GLuint u_normalMatrix;

    glm::mat4 projMatrix;

    glmlv::ViewController viewController;

    //perso
    void initUniform();
    void initVao(GLuint* vao, GLuint* vbo, GLuint* ibo);
    void initVboIbo(GLuint* vbo, GLuint* ibo, const glmlv::SimpleGeometry& object);
    void drawObject(GLuint* vao, const glmlv::SimpleGeometry& object, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix);
};