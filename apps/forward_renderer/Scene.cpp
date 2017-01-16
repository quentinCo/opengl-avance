#include <tiny_obj_loader.h>

#include "Scene.hpp"

using namespace qc;

void Scene::addMeshFromObjFile(const PathFile directory, const std::string& nameFile, const glmlv::GLProgram& program)
{
	std::string inputfile = (directory / "cornell_box.obj").string();
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
		std::cout << "shape[" << s << "].name =" << shapes[s].name.c_str() << std::endl;
		std::cout << "Size of shape[" << s << "].indices: " << shapes[s].mesh.indices.size() << std::endl;
		std::cout << "Size of shape[" << s << "].material_ids: " << shapes[s].mesh.material_ids.size() << std::endl;
		size_t index_offset = 0;

		std::cout << "shapes[" << s << "].mesh.num_face_vertices.size() : " << shapes[s].mesh.num_face_vertices.size() << std::endl;
		std::vector<tinyobj::index_t>::iterator minIndex = std::min_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2) {
			return index1.vertex_index < index2.vertex_index;
		});

		std::vector<tinyobj::index_t>::iterator maxIndex = std::max_element(shapes[s].mesh.indices.begin(), shapes[s].mesh.indices.end(),
			[](auto index1, auto index2) {
			return index1.vertex_index < index2.vertex_index;
		});

		std::cout << "index min: " << minIndex->vertex_index << std::endl;
		std::cout << "index max: " << maxIndex->vertex_index << std::endl;
		std::cout << "diff: " << maxIndex->vertex_index - minIndex->vertex_index + 1 << std::endl;
		std::vector<glmlv::Vertex3f3f2f> vbo = std::vector<glmlv::Vertex3f3f2f>(maxIndex->vertex_index - minIndex->vertex_index + 1);
		std::vector<uint32_t> ibo;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				//sstd::cout << "ibo index = " << idx.vertex_index - min->vertex_index << std::endl;
				glm::vec3 position = (attrib.vertices.size() > 0) ? glm::vec3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]) : glm::vec3(0);
				glm::vec3 normal = (attrib.normals.size() > 0) ? glm::vec3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]) : glm::vec3(1);
				glm::vec2 texCoord = (attrib.texcoords.size() > 0) ? glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]) : glm::vec2(0);
				
				size_t localIndex = idx.vertex_index - minIndex->vertex_index;
				vbo[localIndex] = glmlv::Vertex3f3f2f(position, normal, texCoord);
				ibo.push_back(localIndex);
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
		meshes.push_back(Mesh(vbo, ibo, program));
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