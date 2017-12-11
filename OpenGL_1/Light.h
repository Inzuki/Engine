#ifndef LIGHT_H
#define LIGHT_H

#include "INCLUDES.h"

class Attenuation {
	private:
		float constant, linear, exponent;

	public:
		Attenuation(){
			constant = 0.f;
			linear = 0.f;
			exponent = 0.f;
		}

		Attenuation(float constant, float linear, float exponent){
			this->constant = constant;
			this->linear = linear;
			this->exponent = exponent;
		}

		void set_constant(float constant){
			this->constant = constant;
		}
		void set_linear(float linear){
			this->linear = linear;
		}
		void set_exponent(float exponent){
			this->exponent = exponent;
		}

		float get_constant(){
			return constant;
		}

		float get_linear(){
			return linear;
		}

		float get_exponent(){
			return exponent;
		}
};

class Light {
	private:
		glm::vec3 color;
		float intensity;

	public:
		Light(){
			color = glm::vec3(1.f);
		}

		Light(glm::vec3 color, float intensity){
			this->color = color;
			this->intensity = intensity;
		}

		void set_color(glm::vec3 color){
			this->color = color;
		}

		void set_intensity(float intensity){
			this->intensity = intensity;
		}

		glm::vec3 get_color(){
			return color;
		}

		float get_intensity(){
			return intensity;
		}
};

class DirectionalLight {
	private:
		Light light;
		glm::vec3 direction;

	public:
		DirectionalLight(){}

		DirectionalLight(Light light, glm::vec3 direction){
			this->light = light;
			this->direction = direction;
			//this->light.set_shadow_info(*new ShadowInfo(glm::ortho(-40, 40, -40, 40, -40, 40)));
		}

		void set_light(Light light){
			this->light = light;
		}

		Light get_light(){
			return light;
		}

		void set_direction(glm::vec3 direction){
			this->direction = direction;
		}

		glm::vec3 get_direction(){
			return glm::normalize(direction);
		}
};

class PointLight {
	private:
		static const int COLOR_DEPTH = 256;
		Light light;
		glm::vec3 position;
		Attenuation attenuation;
		float range;

	public:
		PointLight(){
			position = glm::vec3(0.f);
			range = 10.f;
		}

		PointLight(Light light, Attenuation attenuation, glm::vec3 position){
			this->light = light;
			this->attenuation = attenuation;
			this->position = position;
			
			calculate_range();
		}

		void set_light(Light light){
			this->light = light;
		}

		void set_position(glm::vec3 position){
			this->position = position;
		}

		void set_attenuation(Attenuation attenuation){
			this->attenuation = attenuation;
			calculate_range();
		}

		void calculate_range(){
			float a = attenuation.get_exponent();
			float b = attenuation.get_linear();
			float c = attenuation.get_constant() - COLOR_DEPTH * light.get_intensity() * glm::max(light.get_color().x, glm::max(light.get_color().y, light.get_color().z));

			this->range = (float)((-b + sqrt(b * b - 4 * a * c)) / (2 * a));
		}

		Light get_light(){
			return light;
		}

		glm::vec3 get_position(){
			return position;
		}

		Attenuation get_attenuation(){
			return attenuation;
		}

		float get_range(){
			return range;
		}
};

class SpotLight {
	private:
		PointLight point_light;
		glm::vec3 direction;
		float cut_off;

	public:
		SpotLight(){}

		SpotLight(PointLight point_light, glm::vec3 direction, float cut_off){
			this->point_light = point_light;
			this->direction = direction;
			this->cut_off = cut_off;
		}

		void set_position(glm::vec3 position){
			point_light.set_position(position);
		}

		void set_point_light(PointLight point_light){
			this->point_light = point_light;
		}
		
		void set_direction(glm::vec3 direction){
			this->direction = direction;
		}
		
		void set_cut_off(float cut_off){
			this->cut_off = cut_off;
		}

		PointLight get_point_light(){
			return point_light;
		}

		glm::vec3 get_direction(){
			return glm::normalize(direction);
		}

		float get_cut_off(){
			return cut_off;
		}
};

#endif