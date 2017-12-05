#ifndef ENGINE_H
#define ENGINE_H

#include "INCLUDES.h"

#include "Camera.h"
#include "ForwardAmbient.h"
#include "ForwardDirectional.h"
#include "ForwardPoint.h"
#include "ForwardSpot.h"
#include "Light.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Time.h"

class Engine {
	private:
		bool running;

		// window settings
		int fov;
		float aspect_ratio, zNear, zFar;

		// camera
		Camera cam;

		// deltatime
		// this is important in ensuring the game runs the same every frame for any machine
		sf::Clock clk;
		float time;
		float deltaTime, lastFrame;

		// matrices
		glm::mat4 transform, view_mat, projection_mat;

		// shaders
		// Shader shader; // shader that every shader class will derive from
		ForwardAmbient forward_ambient_shader;
		ForwardDirectional forward_directional_shader;
		ForwardPoint forward_point_shader;
		ForwardSpot forward_spot_shader;

		// meshes
		Mesh stall, cube;
		Plane plane;

		// lights
			glm::vec3 ambient_light;

			// directional light
			DirectionalLight directional_light, directional_light2;

			// point lights
			PointLight point_light1;

			// spot lights
			SpotLight spot_light1;

		// display the game world
		void render(Shader shader){
			glActiveTexture(GL_TEXTURE0);
			
			// stall
			transform = glm::mat4();
			transform = glm::translate(transform, glm::vec3(0.f, -3.5f, 0.f));
			transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
			shader.update_uniforms(transform, stall.get_material(), cam.get_position());
			stall.draw();

			// plane (floor)
			transform = glm::mat4();
			transform = glm::translate(transform, glm::vec3(0.f, -3.5f, 0.f));
			transform = glm::scale(transform, glm::vec3(30.f));
			shader.update_uniforms(transform, plane.get_material(), cam.get_position());
			plane.draw();
		}

		// update variables (mainly updating the view and projection matrices in the shaders)
		void update(){
			// update time
			time = clk.getElapsedTime().asSeconds();

			// disallow the camera from moving on the y-axis (so it essentially can't fly)
			//cam.anchor_camera();

			// view matrix
			view_mat = glm::mat4();
			cam.compute_viewMatrix(view_mat);
			
			// projection matrix
			projection_mat = glm::mat4();
			projection_mat = glm::perspective(glm::radians((float)fov), aspect_ratio, zNear, zFar);
			
			// update matrices in lighting shaders
			forward_ambient_shader.update_matrices(view_mat, projection_mat);
			forward_directional_shader.update_matrices(view_mat, projection_mat);
			forward_point_shader.update_matrices(view_mat, projection_mat);
			forward_spot_shader.update_matrices(view_mat, projection_mat);
		}

		// load resources (lights, models, sounds, ...)
		void load(){
			// directional lights
			directional_light = *new DirectionalLight(*new Light(glm::vec3(1.f, 0.f, 0.f), 0.2f), glm::vec3(0.25f, 1.f, 0.25f));
			directional_light2 = *new DirectionalLight(*new Light(glm::vec3(0.f, 0.f, 1.f), 0.2f), glm::vec3(-0.25f, 1.f, -0.25f));

			// point lights
			point_light1 = *new PointLight(*new Light(glm::vec3(0.f, 1.f, 0.f), 0.3f), *new Attenuation(0.f, 0.f, 1.f), glm::vec3(10.f, -3.f, 10.f), 100.f);

			// spot lights
			spot_light1 = *new SpotLight(
							  *new PointLight(*new Light(glm::vec3(0.f, 1.f, 1.f), 0.9f), *new Attenuation(0.f, 0.f, 0.01f), glm::vec3(-10.f, -1.f, -10.f), 100.f),
						  glm::vec3(0.f, 1.f, 1.f), 0.7f);

			// setup values in ambient shader
			forward_ambient_shader.bind();
			forward_ambient_shader.setUniform("ambientIntensity", ambient_light);

			// load stall
			stall.load_mesh("stall.obj");
			stall.load_texture("stallTexture.png");
			stall.set_specular_intensity(2.f);
			stall.set_specular_exponent(32.f);

			// load plane
			plane.applyTexture("wood.png");
			plane.set_specular_intensity(1.f);
			plane.set_specular_exponent(8.f);
		}

		// handle keyboard input such as:
		// when the window is closed, movement, actions, ...
		void input(sf::RenderWindow &window){
			// hide the cursor when the game is in focus
			window.setMouseCursorVisible(!cam.is_locked());

			sf::Event event;
			while(window.pollEvent(event)){
				// close the window
				if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
					running = false;

				if(event.type == sf::Event::KeyReleased){
					// lock the window if Q is pressed
					if(event.key.code == sf::Keyboard::Q)
						cam.set_locked_camera(!cam.is_locked());
				}

				// when the window is resized, fix the viewport
				if(event.type == sf::Event::Resized)
					glViewport(0, 0, window.getSize().x, window.getSize().y);
			}

			// handle movement
			cam.handle_movement(window, deltaTime, view_mat);
		}

		// TODO: improve timestamp stability
		void calculate_timestamp(){
			float currentFrame = clk.getElapsedTime().asSeconds();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}

		// refresh the screen
		void clear_screen(){
			// TODO: stencil buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

	public:
		// default constructor (setup engine properties)
		Engine(sf::RenderWindow &window){
			// openGL settings
			glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
			glFrontFace(GL_CCW);
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_CLAMP);

			fov = 45;
			aspect_ratio = (float)(window.getSize().x / window.getSize().y);
			zNear = 0.1f;
			zFar = 1000.f;

			// timestamp settings
			clk.restart();
			time = clk.getElapsedTime().asSeconds();
			deltaTime = 0.f;
			lastFrame = 0.f;

			// load resources
			ambient_light = glm::vec3(0.2f);
			load();

			// start the game
			running = true;
		}

		// displays the main game
		void run(sf::RenderWindow &window){
			while(running){
				calculate_timestamp();

				clear_screen();

				input(window);
				update();
				
				// ambient light (default lighting)
				forward_ambient_shader.bind();
				render(forward_ambient_shader);
				
				// calculate the rest of the lights (directional, point, spot).
				// this is forward rendering;
				// this lets us avoid having to set up constants in both the engine and shaders
				// to allow us to create as many lights in the scene as we want dynamically
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_EQUAL);
					// directional lights
					forward_directional_shader.bind();
					forward_directional_shader.set_directional_light(directional_light);
					render(forward_directional_shader);

					forward_directional_shader.set_directional_light(directional_light2);
					render(forward_directional_shader);

					// point lights
					forward_point_shader.bind();
					forward_point_shader.set_point_light(point_light1);
					render(forward_point_shader);

					// spot lights
					forward_spot_shader.bind();
					forward_spot_shader.set_spot_light(spot_light1);
					render(forward_spot_shader);
				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);

				window.display();
			}
		}
};

#endif