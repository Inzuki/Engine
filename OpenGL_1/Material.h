#ifndef MATERIAL_H
#define MATERIAL_H

#include "INCLUDES.h"

#include "Texture.h"

class Material {
	private:
		glm::vec3 color;
		GLuint texture;
		float specular_intensity, specular_exponent;

	public:
		Material(){
			color = glm::vec3(1.f);
			specular_intensity = 2.f;
			specular_exponent = 32.f;
		}

		void set_color(glm::vec3 color){
			this->color = color;
		}

		void set_texture(const char *texture){
			this->texture = loadTexture(texture);
		}

		void set_specular_intensity(float specular_intensity){
			this->specular_intensity = specular_intensity;
		}

		void set_specular_exponent(float specular_exponent){
			this->specular_exponent = specular_exponent;
		}

		glm::vec3 get_color(){
			return color;
		}

		GLuint get_texture(){
			return texture;
		}

		float get_specular_intensity(){
			return specular_intensity;
		}

		float get_specular_exponent(){
			return specular_exponent;
		}
};

#endif