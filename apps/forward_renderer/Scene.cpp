#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION

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
	std::cout << "# of attrib.vertices : " << attrib.vertices.size() / 3 << std::endl;
	std::cout << "# of attrib.normals : " << attrib.normals.size() / 3 << std::endl;
	std::cout << "# of attrib.texcoords : " << attrib.texcoords.size() / 2 << std::endl;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;

		std::string featureForIndex;
		std::pair<size_t, size_t> miMaxIndex = findMinMaxIndex(shapes[s].mesh.indices, featureForIndex);
		std::vector<glmlv::Vertex3f3f2f> vbo = std::vector<glmlv::Vertex3f3f2f>(miMaxIndex.second - miMaxIndex.first + 1);
		std::vector<uint32_t> ibo;
		int materialIndex = 0;

		std::cout << "Min : " << miMaxIndex.first << std::endl;
		std::cout << "Max : " << miMaxIndex.second << std::endl;
		std::cout << "Compute size : " << miMaxIndex.second - miMaxIndex.first + 1 << std::endl;
		std::cout << "Size : " << vbo.size() << std::endl;

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
				size_t iboIndex;
				if(featureForIndex == "vertex")
					iboIndex = idx.vertex_index - miMaxIndex.first;
				else if(featureForIndex == "normal")
					iboIndex = idx.normal_index - miMaxIndex.first;
				else if(featureForIndex == "tex")
					iboIndex = idx.texcoord_index - miMaxIndex.first;

				vbo[iboIndex] = glmlv::Vertex3f3f2f(position, normal, texCoord);
				//std::cout << "Vertex " << iboIndex << " : " << idx.vertex_index << " - " << idx.normal_index << " - " << idx.texcoord_index << std::endl;
				/*std::cout << "\t" << vbo[iboIndex].position.x << "; " << vbo[iboIndex].position.y << "; " << vbo[iboIndex].position.z << std::endl;
				std::cout << "\t" << vbo[iboIndex].normal.x << "; " << vbo[iboIndex].normal.y << "; " << vbo[iboIndex].normal.z << std::endl;
				std::cout << "\t" << vbo[iboIndex].texCoords.x << "; " << vbo[iboIndex].texCoords.y << std::endl;
				*/ibo.push_back(iboIndex);
			}
			index_offset += fv;

			// per-face material
			materialIndex = shapes[s].mesh.material_ids[f];
		}

		Mesh mesh = Mesh(vbo, ibo, program);
		if (materials[materialIndex].diffuse_texname != "")
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
	
	/*for (size_t i = 0; i < materials.size(); i++) {
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
	std::cout << "\n"<< std::endl;*/
	
}

std::pair<size_t, size_t> Scene::findMinMaxIndex(const std::vector<tinyobj::index_t>& indices, std::string& res)
{
	size_t size = 0;
	size_t tempSize = 0;
	std::pair<size_t, size_t> minMax;

	auto minIndex = std::min_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.texcoord_index < index2.texcoord_index;
	});

	auto maxIndex = std::max_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.texcoord_index < index2.texcoord_index;
	});
	tempSize = maxIndex->texcoord_index - minIndex->texcoord_index + 1;
	if (size < tempSize)
	{
		size = tempSize;
		minMax = std::make_pair(minIndex->texcoord_index, maxIndex->texcoord_index);
		res = "tex";
	}

	minIndex = std::min_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.vertex_index < index2.vertex_index;
	});
	maxIndex = std::max_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.vertex_index < index2.vertex_index;
	});
	if (size < tempSize)
	{
		size = maxIndex->vertex_index - minIndex->vertex_index + 1;
		minMax = std::make_pair(minIndex->vertex_index, maxIndex->vertex_index);
		res = "vertex";
	}

	minIndex = std::min_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.normal_index < index2.normal_index;
	});
	maxIndex = std::max_element(indices.begin(), indices.end(),
		[](auto index1, auto index2) {
		return index1.normal_index < index2.normal_index;
	});
	tempSize = maxIndex->normal_index - minIndex->normal_index + 1;
	if (size < tempSize)
	{
		size = tempSize;
		minMax = std::make_pair(minIndex->normal_index, maxIndex->normal_index);
		res = "normal";
	}

	return minMax;
}
