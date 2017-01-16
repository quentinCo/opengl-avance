#define TINYOBJLOADER_IMPLEMENTATION

#include "Application.hpp"

// classics
#include <iostream>
#include <algorithm>

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

#include <tiny_obj_loader.h>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
		drawScene();

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
	glmlv::SimpleGeometry simpleCube = glmlv::makeCube();
	scene.addMesh(qc::Mesh(simpleCube, m_program, glm::vec3(0, 0, -5), (m_AssetsRootPath / m_AppName / "textures" / "red_panda_2.jpg")));
	//Sphere
	glmlv::SimpleGeometry simpleSphere = glmlv::makeSphere(16);
	scene.addMesh(qc::Mesh(simpleSphere, m_program, glm::vec3(0, 2, -6), (m_AssetsRootPath / m_AppName / "textures" / "red_panda_1.jpg")));

	
	//DirectionalLight
	scene.addDirectionalLight(qc::Light(glm::vec3(1, 1, 0), 25));

	//PointLight
	scene.addPointLight(qc::Light(glm::vec3(0, 0, 5), 50));

    //Sampler
    initSampler();

	tinyTest();
}

Application::~Application()
{
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
	std::vector<qc::Mesh>& meshes = scene.getMeshes();
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		std::string guiName = "Diffuse Color - Mesh " + std::to_string(i);
		ImGui::ColorEdit3(guiName.c_str(), glm::value_ptr(meshes[i].getDiffuseColor()));
	}

    // Light intensity
	std::vector<qc::Light>& dirLights = scene.getDirectionalLights();
	for (size_t i = 0; i < dirLights.size(); ++i)
	{
		std::string guiName = "Directional Light " + std::to_string(i) + " Intensity";
		ImGui::SliderFloat(guiName.c_str(), &(dirLights[i].getLightIntensity()), 0, 100.f);
	}
		
	std::vector<qc::Light>& pointLights = scene.getPointLights();
	for (size_t i = 0; i < dirLights.size(); ++i)
	{
		std::string guiName = "Point Light " + std::to_string(i) + " Intensity";
		ImGui::SliderFloat(guiName.c_str(), &(pointLights[i].getLightIntensity()), 0, 100.f);
	}
		

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

void Application::drawScene()
{
	const std::vector<qc::Mesh>& meshes = scene.getMeshes();
	const std::vector<qc::Light>& dirLights = scene.getDirectionalLights();
	const std::vector<qc::Light>& pointLights = scene.getPointLights();

	for (const auto& it : dirLights)
		setUniformsLightDirValue(it);

	for (const auto& it : pointLights)
		setUniformsLightPointLight(it);

	for (const auto& it : meshes)
	{
		setUniformsMeshValues(it);
		if (activeTexture) bindTex(it);
		it.drawMesh();
	}
}
void Application::setUniformsMeshValues(const qc::Mesh& mesh)
{
	const glm::mat4 modelMatrix = mesh.getModelMatrix();
	// Camera
	glUniformMatrix4fv(u_modelViewProjMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeModelViewProjMatrix(modelMatrix)));
	glUniformMatrix4fv(u_modelViewMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeModelViewMatrix(modelMatrix)));
	glUniformMatrix4fv(u_normalMatrix, 1, GL_FALSE, glm::value_ptr(camera.computeNormalMatrix(modelMatrix)));
	
	// Color
	glUniform3fv(u_Kd, 1, glm::value_ptr(mesh.getDiffuseColor()));

	// "Texture"
	glUniform1i(u_activeTexture, activeTexture);
}

void Application::setUniformsLightDirValue(const qc::Light& dirLight)
{
	glUniform3fv(u_directionalLightDir, 1, glm::value_ptr(dirLight.getLightPosition()));
	glm::vec3 directionalLightIntensityVec3 = glm::vec3(dirLight.getLightIntensity());
	glUniform3fv(u_directionalLightIntensity, 1, glm::value_ptr(directionalLightIntensityVec3));
}

void Application::setUniformsLightPointLight(const qc::Light& pointLight)
{
	glUniform3fv(u_pointLightPosition, 1, glm::value_ptr(pointLight.getLightPosition()));
	glm::vec3 pointLightIntensityVec3 = glm::vec3(pointLight.getLightIntensity());
	glUniform3fv(u_pointLightIntensity, 1, glm::value_ptr(pointLightIntensityVec3));
}

void Application::bindTex(const qc::Mesh& mesh)
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(u_KdSampler, 0);
    glBindSampler(0, m_sampler);
    glBindTexture(GL_TEXTURE_2D, mesh.getTexture()->getTexPointer());
}

void Application::unBindTex()
{
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(u_KdSampler, 0);
    glBindSampler(0, 0); // MEH
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Application::tinyTest()
{
	std::experimental::filesystem::path dir = (m_AssetsRootPath / m_AppName / "obj" / "Cornell_Box");
	std::string inputfile = (dir / "cornell_box.obj").string();
	std::string dirObj = dir.string() + "\\";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), dirObj.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;
	std::cout << "# of attrib : " << attrib.vertices.size() << std::endl;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		std::cout << "shape["<< s << "].name ="  << shapes[s].name.c_str()<< std::endl;
		std::cout << "Size of shape[" << s << "].indices: " << shapes[s].mesh.indices.size()<< std::endl;
		std::cout << "Size of shape[" << s << "].material_ids: " << shapes[s].mesh.material_ids.size()<< std::endl;
		size_t index_offset = 0;
		std::cout << "shapes[" << s << "].mesh.num_face_vertices.size() : " << shapes[s].mesh.num_face_vertices.size() << std::endl;
		std::vector<tinyobj::index_t>::iterator min = std::min_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2){
			return index1.vertex_index < index2.vertex_index;
		});

		std::vector<tinyobj::index_t>::iterator max = std::max_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2) {
			return index1.vertex_index < index2.vertex_index;
		});

		std::cout << "index min: " << min->vertex_index << std::endl;
		std::cout << "index max: " << max->vertex_index << std::endl;
		std::cout << "diff: " << max->vertex_index - min->vertex_index + 1<< std::endl;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				std::cout << "ibo index = " << idx.vertex_index - min->vertex_index << std::endl;
				if (attrib.vertices.size() > 0)
				{
					float vx = attrib.vertices[3 * idx.vertex_index + 0];
					float vy = attrib.vertices[3 * idx.vertex_index + 1];
					float vz = attrib.vertices[3 * idx.vertex_index + 2];
					std::cout << "\tVertice : [" << vx << "; " << vy << "; " << vz << "]" << std::endl;
				}
				if (attrib.normals.size() > 0)
				{
					float nx = attrib.normals[3 * idx.normal_index + 0];
					float ny = attrib.normals[3 * idx.normal_index + 1];
					float nz = attrib.normals[3 * idx.normal_index + 2];
					std::cout << "\tNormal : [" << nx << "; " << ny << "; " << nz << "]" << std::endl;
				}
				if (attrib.texcoords.size() > 0)
				{
					float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					std::cout << "\tTexture : [" << tx << "; " << ty << "]" << std::endl;
				}
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];

		}
	}
/*
	for (size_t i = 0; i < materials.size(); i++) {
		std::cout << "material" << i << "].name = " << materials[i].name.c_str() << std::endl;
		std::cout << "  material.Ka = (" << materials[i].ambient[0] << ", " << materials[i].ambient[1] <<", "<< materials[i].ambient[2]<< ")"<< std::endl;
		std::cout << "  material.Kd = (" << materials[i].diffuse[0] << ", " << materials[i].diffuse[1] << ", " << materials[i].diffuse[2] << ")"<< std::endl;
		std::cout << "  material.Ks = (" << materials[i].specular[0] << ", " << materials[i].specular[1] << ", " << materials[i].specular[2] << ")"<< std::endl;
		std::cout << "  material.Tr = (" << materials[i].transmittance[0] << ", " << materials[i].transmittance[1] << ", " << materials[i].transmittance[2] << ")"<< std::endl;
		std::cout << "  material.Ke = (" << materials[i].emission[0] << ", " << materials[i].emission[1] << ", " << materials[i].emission[2] << ")"<< std::endl;
		std::cout << "  material.Ns = " << materials[i].shininess << std::endl;
		std::cout << "  material.Ni = " << materials[i].ior<< std::endl;
		std::cout << "  material.dissolve = " << materials[i].dissolve<< std::endl;
		std::cout << "  material.illum = " << materials[i].illum<< std::endl;
		std::cout << "  material.map_Ka = " << materials[i].ambient_texname.c_str()<< std::endl;
		std::cout << "  material.map_Kd = " << materials[i].diffuse_texname.c_str()<< std::endl;
		std::cout << "  material.map_Ks = " << materials[i].specular_texname.c_str()<< std::endl;
		std::cout << "  material.map_Ns = " << materials[i].specular_highlight_texname.c_str()<< std::endl;
		std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
		for (; it != itEnd; it++) {
			std::cout << "  material." << it->first.c_str() <<" = " << it->second.c_str()<< std::endl;
		}
		std::cout << "\n"<< std::endl;
	}*/
}

