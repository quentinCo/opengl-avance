#pragma once

#include <vector>
#include <memory>

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>

#include "Camera.hpp"
#include "Scene.hpp"

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
	qc::Scene scene;

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

	GLuint u_pointLightPosition = 0;
	GLuint u_pointLightIntensity = 0;

    // Color
	GLuint u_Kd;

    // Texture
    GLuint u_activeTexture;
	bool activeTexture = false;//true;
   /* GLuint u_KdSampler;

    GLuint m_sampler;*/

	GLuint u_wireframe;
	bool wireFrame = false;

	// Functions
	void gui(float clearColor[3]);

    void initUniforms();
//    void initSampler();
    void drawScene();
	void setUniformsMeshValues(const qc::Mesh& mesh);
	void setUniformsLightDirValue(const qc::Light& dirLight);
	void setUniformsLightPointLight(const qc::Light& pointLight);
/*	void bindTex(const qc::Mesh& mesh);
    void unBindTex();*/
};