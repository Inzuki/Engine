#ifndef CAMERA_H
#define CAMERA_H

#include "INCLUDES.h"

class Camera {
	private:
		bool locked, first_mouse;
		float last_x, last_y;

		float speed, actual_speed, mouse_sensitivity;

		float pitch, yaw;
		float last_pitch, last_yaw;
		glm::vec3 position, right, front, up;

	public:
		Camera(){
			locked = true;
			first_mouse = false;
			speed = 3.f;
			actual_speed = speed * 4.f;
			mouse_sensitivity = 0.01f;
			pitch = 0.f;
			yaw = 0.f;
			position = glm::vec3(0.f, 0.f, 3.f);
			right = glm::vec3(0.f, 0.f, 0.f);
			front = glm::vec3(0.f, 0.f, -1.f);
			up = glm::vec3(0.f, 1.f, 3.f);
		}

		// toggle the cursor locked.
		// useful so the mouse can be moved around instead of fixed to the center of the screen
		void set_locked_camera(bool locked){
			this->locked = locked;

			if(!locked){
				last_yaw = yaw;
				last_pitch = pitch;
			}
		}

		// check if the cursor is locked
		bool is_locked(){
			return locked;
		}

		// get the camera's position
		glm::vec3 get_position(){
			return position;
		}

		// get the camera's forward vector
		glm::vec3 get_forward(){
			return front;
		}

		// set the player's speed
		void set_speed(float speed){
			this->speed = speed;
		}

		// set the mouse sensitivity (how fast the camera can move around)
		void set_mouse_sensitivity(float mouse_sensitivity){
			this->mouse_sensitivity = mouse_sensitivity;
		}

		// compute the view matrix for the shader
		void compute_viewMatrix(glm::mat4 &viewMat){
			viewMat = glm::lookAt(position, position + front, up);
		}

		// anchor the camera so the y-axis never moves
		void anchor_camera(){
			position.y = 1.15f;
		}

		// handle all keyboard and mouse movements
		void handle_movement(sf::RenderWindow &window, float deltaTime, glm::mat4 &viewMat){
			// mouse movement
			double mouse_pos_x = sf::Mouse::getPosition().x,
				   mouse_pos_y = sf::Mouse::getPosition().y;

			// check if the locked boolean is set.
			// if it is, lock the cursor to the center of the screen.
			// this ensures the cursor doesn't get stuck in the corner or edges
			// of the screen and stopping the camera from moving around freely.
			if(locked){
				sf::Mouse::setPosition(sf::Vector2i(window.getPosition().x + window.getSize().x / 2, window.getPosition().y + window.getSize().y / 2));

				yaw   += mouse_sensitivity * float(window.getPosition().x + window.getSize().x / 2.f - mouse_pos_x);
				pitch += mouse_sensitivity * float(window.getPosition().y + window.getSize().y / 2.f - mouse_pos_y);
			}else {
				yaw = last_yaw;
				pitch = last_pitch;
			}

			// prevent the camera from turning upside down
			if(pitch > glm::radians(89.f))
				pitch = glm::radians(89.f);
			if(pitch < glm::radians(-89.f))
				pitch = glm::radians(-89.f);

			front = glm::vec3(
				cos(pitch) * sin(yaw),
				sin(pitch),
				cos(pitch) * cos(yaw)
			);

			right = glm::vec3(
				sin(yaw - 3.1415f / 2.f),
				0.f,
				cos(yaw - 3.1415f / 2.f)
			);

			// keyboard movement
			// forward
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				position += actual_speed * deltaTime * front;
			// left
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
				position.x -= actual_speed * deltaTime * right.x;
				position.z -= actual_speed * deltaTime * right.z;
			}
			// backward
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				position -= actual_speed * deltaTime * front;
			// right
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
				position.x += actual_speed * deltaTime * right.x;
				position.z += actual_speed * deltaTime * right.z;
			}
			
			up = glm::cross(right, front);
		}
};

#endif