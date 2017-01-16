#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#include <memory>

#include "Scene.hpp"

using namespace qc;

void Scene::addMeshFromObjFile(const PathFile directory, const std::string& nameFile, const glmlv::GLProgram& program)
{
	std::string inputfile = (directory / nameFile).string();
	std::string dirObj = directory.string() + "\\";
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
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;

		std::vector<tinyobj::index_t>::iterator minIndex = std::min_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2) {
			return index1.vertex_index < index2.vertex_index;
		});

		std::vector<tinyobj::index_t>::iterator maxIndex = std::max_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2) {
			return index1.vertex_index < index2.vertex_index;
		});

		std::vector<glmlv::Vertex3f3f2f> vbo = std::vector<glmlv::Vertex3f3f2f>(maxIndex->vertex_index - minIndex->vertex_index + 1);
		std::vector<uint32_t> ibo;
		int materialIndex = 0;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				
				glm::vec3 position = (attrib.vertices.size() > 0) ? glm::vec3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]) : glm::vec3(0);
				glm::vec3 normal = (attrib.normals.size() > 0) ? glm::vec3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]) : glm::vec3(1);
				glm::vec2 texCoord = (attrib.texcoords.size() > 0) ? glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]) : glm::vec2(0);
				// BON POINT MAIS PAS BONNE VALEUR
				size_t iboIndex = idx.vertex_index - minIndex->vertex_index;
				vbo[iboIndex] = glmlv::Vertex3f3f2f(position, normal, texCoord);
				ibo.push_back(iboIndex);
			}
			index_offset += fv;

			// per-face material
			materialIndex = shapes[s].mesh.material_ids[f];
		}

		Mesh mesh = Mesh(vbo, ibo, program);
		if (materials[materialIndex].diffuse_texname != "" /*&& s == 2*/)
		{
			PathFile diffusePath = (directory / materials[materialIndex].diffuse_texname);
			/*std::cout << "Mesh : " << shapes[s].name << std::endl;
			std::cout << "\tTexture : " << materials[materialIndex].diffuse_texname << std::endl;
			std::cout << "\tPath : " << diffusePath.string() << std::endl;*/
			mesh.setDiffuseTexture(diffusePath);
		}
		glm::vec3 diffuseColor = glm::vec3(materials[materialIndex].diffuse[0], materials[materialIndex].diffuse[1], materials[materialIndex].diffuse[2]);
		if (diffuseColor == glm::vec3(0))
			diffuseColor = glm::vec3(1);

		mesh.setDiffuseColor(diffuseColor);

		meshes.push_back(mesh);
	}
	
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
		for (; it != itEnd; it++) 
			std::cout << "  material." << it->first.c_str() <<" = " << it->second.c_str()<< std::endl;
	}
	std::cout << "\n"<< std::endl;
	
}