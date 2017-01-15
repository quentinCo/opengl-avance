#pragma once

#include <vector>
#include <memory>

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

class Application
{
public:
    Application(int argc, char** argv);
	~Application();

    int run();
private:
	// Window
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ static_cast<int>(m_nWindowWidth), static_cast<int>(m_nWindowHeight), "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

	// App path
    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

	// Objects with their buffers
	qc::Mesh cube;
	qc::Mesh sphere;

	// Shader program
    glmlv::GLProgram m_program;

	// Camera matrix
    GLuint u_modelViewProjMatrix = 0;
    GLuint u_modelViewMatrix = 0;
    GLuint u_normalMatrix = 0;

	qc::Camera camera;

	// Lights
	GLuint u_directionalLightDir = 0;
	GLuint u_directionalLightIntensity = 0;
	glm::vec3 directionalLightDir;
	float directionalLightIntensity;

	GLuint u_pointLightPosition = 0;
	GLuint u_pointLightIntensity = 0;
	glm::vec3 pointLightPosition;
	float pointLightIntensity;

    // Color
	GLuint u_Kd;

    // Texture
    GLuint u_activeTexture;
    bool activeTexture = true;
    GLuint u_KdSampler;

    GLuint m_sampler;


	// Functions
	void gui(float clearColor[3]);

    void initUniforms();
    void initSampler();
    void drawObject(const qc::Mesh& mesh);
	void setUniformsValues(const qc::Mesh& mesh);
    void bindTex(const qc::Mesh& mesh);
    void unBindTex();
};