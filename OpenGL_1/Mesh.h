#ifndef MESH_H
#define MESH_H

#include "INCLUDES.h"

#include "Material.h"

class Mesh {
	private:
		int vertexCount;
		GLuint obj, vertBuff, texBuff, normBuff;
		Material material;

	public:
		Mesh(){
			material.set_color(glm::vec3(1.f));
		}

		void load_mesh(const char *objName){
			// get setup to read OBJ
			std::vector<glm::vec2>				   tempTextures;
			std::vector<glm::vec3>    tempVertices,			   tempNormals;
			std::vector<glm::vec2>				   textures;
			std::vector<glm::vec3>    vertices,                normals;
			std::vector<unsigned int> vertIndices, texIndices, normIndices;

			// open OBJ file and begin reading the OBJ's contents
			char objFile[256];
			sprintf(objFile, "res/models/%s", objName);
			FILE *file = fopen(objFile, "r");

			while(true){
				char lineHeader[128];

				int res = fscanf(file, "%s", lineHeader);
				if(res == EOF)
					break;

				if(strcmp(lineHeader, "v") == 0){
					glm::vec3 vertex;

					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

					tempVertices.push_back(vertex);
				}else if(strcmp(lineHeader, "vt") == 0){
					glm::vec2 texture;

					fscanf(file, "%f %f\n", &texture.x, &texture.y);

					tempTextures.push_back(texture);
				}else if(strcmp(lineHeader, "vn") == 0){
					glm::vec3 normal;

					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

					tempNormals.push_back(normal);
				}else if(strcmp(lineHeader, "f") == 0){
					std::string vertex1, vertex2, vertex3;
					unsigned int vertexIndex[3], textureIndex[3], normalIndex[3];

					int matches = fscanf(file,
						   "%d/%d/%d %d/%d/%d %d/%d/%d\n",
						   &vertexIndex[0],
						   &textureIndex[0],
						   &normalIndex[0],
						   &vertexIndex[1],
						   &textureIndex[1],
						   &normalIndex[1],
						   &vertexIndex[2],
						   &textureIndex[2],
						   &normalIndex[2]
					);

					if(matches != 9)
						printf("File cannot be read correctly.\n");
			
					vertIndices.push_back(vertexIndex[0]);
					vertIndices.push_back(vertexIndex[1]);
					vertIndices.push_back(vertexIndex[2]);

					texIndices.push_back(textureIndex[0]);
					texIndices.push_back(textureIndex[1]);
					texIndices.push_back(textureIndex[2]);

					normIndices.push_back(normalIndex[0]);
					normIndices.push_back(normalIndex[1]);
					normIndices.push_back(normalIndex[2]);
				}else {
					char garbage[1024];
					fgets(garbage, 1024, file);
				}
			}

			fclose(file);

			for(unsigned int i = 0; i < vertIndices.size(); i++){
				unsigned int vertexIndex = vertIndices[i],
							 texIndex    = texIndices[i],
							 normalIndex = normIndices[i];

				glm::vec3 vertex  = tempVertices[vertexIndex - 1];
				glm::vec2 texture = tempTextures[texIndex    - 1];
				glm::vec3 normal  =  tempNormals[normalIndex - 1];

				vertices.push_back(vertex);
				textures.push_back(texture);
				 normals.push_back(normal);
			}

			vertexCount = vertices.size();
	
			glGenVertexArrays(1, &obj);
			glGenBuffers(1, &vertBuff);
			glGenBuffers(1, &texBuff);
			glGenBuffers(1, &normBuff);

			// create vertex array object
			glBindVertexArray(obj);
				// vertices
				glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
				glEnableVertexAttribArray(0);
				// texture coordinates
				glBindBuffer(GL_ARRAY_BUFFER, texBuff);
				glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				// normals
				glBindBuffer(GL_ARRAY_BUFFER, normBuff);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0],  GL_STATIC_DRAW);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
				glEnableVertexAttribArray(2);
			glBindVertexArray(0);
		}

		void load_texture(const char *textureFile){
			material.set_texture(textureFile);
		}

		Material get_material(){
			return material;
		}

		void set_color(glm::vec3 color){
			material.set_color(color);
		}

		void set_specular_intensity(float intensity){
			material.set_specular_intensity(intensity);
		}

		void set_specular_exponent(float exponent){
			material.set_specular_exponent(exponent);
		}

		void draw(){
			//glActiveTexture(GL_TEXTURE0);
			if(material.get_texture())
				glBindTexture(GL_TEXTURE_2D, material.get_texture());

			glBindVertexArray(obj);
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			glBindVertexArray(0);
		}
};

class Plane {
	private:
		GLuint VBO, VAO, EBO;
		Material material;

	public:
		Plane(){
			float vertices[] = {
				// positions		 normals			texture coords
				 1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
				-1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
				-1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

				// 1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
				 1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
				//-1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
			};
			unsigned int indices[] = {
				0, 1, 2, // first Triangle
				0, 3, 1  // second Triangle
			};

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
				
				// vertices
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				
				// textures
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glEnableVertexAttribArray(1);
				
				// normals
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
			glBindVertexArray(0); 
		}

		void applyTexture(const char *texture){
			material.set_texture(texture);
		}

		Material get_material(){
			return material;
		}

		void set_color(glm::vec3 color){
			material.set_color(color);
		}

		void set_specular_intensity(float intensity){
			material.set_specular_intensity(intensity);
		}

		void set_specular_exponent(float exponent){
			material.set_specular_exponent(exponent);
		}

		void draw(){
			if(material.get_texture()){
				//glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material.get_texture());
			}

			glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
};

#endif