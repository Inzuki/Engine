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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

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
		ForwardAmbient forward_ambient_shader;
		ForwardDirectional forward_directional_shader;
		ForwardPoint forward_point_shader;
		ForwardSpot forward_spot_shader;

		// meshes
		Mesh stall, cube;
		Plane plane;

		// lights
		glm::vec3 ambient_light;
		std::vector<DirectionalLight> directional_lights;
		std::vector<PointLight> point_lights;
		std::vector<SpotLight> spot_lights;

		// shadows
		ShadowMap shadow_shader;
		DebugShader debug_shader;
		DepthShader depth_shader;
		GLuint frameBuffer, depthTexture;



		// display the game world
		void render(Shader shader){
			glActiveTexture(GL_TEXTURE0);
			
			// stall
			transform = glm::mat4();
			transform = glm::translate(transform, glm::vec3(8.f, -3.5f, -5.f));
			transform = glm::rotate(transform, glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
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
			//view_mat = glm::lookAt(glm::vec3(-10.f, 10.f, 10.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
			
			// projection matrix
			projection_mat = glm::mat4();
			projection_mat = glm::perspective(glm::radians((float)fov), aspect_ratio, zNear, zFar);
			//projection_mat = glm::ortho<float>(-30, 30, -30, 30, -30, 30);
			
			// update matrices in lighting shaders
			forward_ambient_shader.update_matrices(view_mat, projection_mat);
			forward_directional_shader.update_matrices(view_mat, projection_mat);
			forward_point_shader.update_matrices(view_mat, projection_mat);
			forward_spot_shader.update_matrices(view_mat, projection_mat);
		}

		// load resources (lights, models, sounds, ...)
		void load(){
			init_shadow();
			// directional lights
			//directional_lights.push_back(*new DirectionalLight(*new Light(glm::vec3(0.1f, 0.2f, 1.f), 0.3f), glm::vec3(1.f)));
			directional_lights.push_back(*new DirectionalLight(*new Light(glm::vec3(1.f), 0.8f), glm::vec3(-1.f, 1.f, 1.f)));

			// point lights
			//point_lights.push_back(*new PointLight(*new Light(glm::vec3(0.f, 1.f, 0.f), 0.3f), *new Attenuation(0.f, 0.f, 1.f), glm::vec3(10.f, -3.f, 10.f)));
			//point_lights.push_back(*new PointLight(*new Light(glm::vec3(1.f), 1.f), *new Attenuation(0.5f, 0.01f, 0.0001f), glm::vec3(10.f, 50.f, 10.f))); // sun

			// spot lights
			/*spot_lights.push_back(*new SpotLight(
							  *new PointLight(*new Light(glm::vec3(0.f, 1.f, 1.f), 0.4f), *new Attenuation(0.f, 0.f, 0.01f), glm::vec3(-10.f, -1.f, -10.f)),
						  glm::vec3(0.f, 0.f, 1.f), 0.7f));*/

			// setup values in ambient shader
			forward_ambient_shader.bind();
			forward_ambient_shader.setUniform("ambientIntensity", ambient_light);

			// load stall
			stall.load_mesh("stall.obj");
			stall.load_texture("stallTexture.png");

			// load plane
			plane.applyTexture("bricky.png");
			plane.set_specular_intensity(1.f);
			plane.set_specular_exponent(16.f);
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

		void init_shadow(){
			glGenFramebuffers(1, &frameBuffer);

			glGenTextures(1, &depthTexture);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
			glEnable(GL_TEXTURE_2D);

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
				
				// calculate shadows
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
					glViewport(0, 0, 1024, 1024);
					glClear(GL_DEPTH_BUFFER_BIT);

					glm::mat4 depth_view = glm::lookAt(glm::vec3(-10.f, 7.f, 10.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
					float val = 25.f, far_plane = 50.f;
					glm::mat4 depth_projection = glm::ortho(-val, val, -val, val, -val, far_plane);

					depth_shader.update_matrices(depth_view, depth_projection);
					render(depth_shader);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// render scene as normal
				input(window);
				update();

				glViewport(0, 0, window.getSize().x, window.getSize().y);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
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
					for(unsigned int i = 0; i < directional_lights.size(); i++){
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, depthTexture);
						forward_directional_shader.setUniform("depth_view", depth_view);
						forward_directional_shader.setUniform("depth_projection", depth_projection);
						forward_directional_shader.set_directional_light(directional_lights.at(i));
						render(forward_directional_shader);
					}

					/*// point lights
					forward_point_shader.bind();
					for(unsigned int i = 0; i < point_lights.size(); i++){
						forward_point_shader.set_point_light(point_lights.at(i));
						render(forward_point_shader);
					}

					// spot lights
					forward_spot_shader.bind();
					for(unsigned int i = 0; i < spot_lights.size(); i++){
						forward_spot_shader.set_spot_light(spot_lights.at(i));
						render(forward_spot_shader);
					}*/
				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);

				// debugging
				glViewport(0, 0, 256, 256);
				debug_shader.bind();
				debug_shader.setUniformf("near_plane", -val);
				debug_shader.setUniformf("far_plane", far_plane);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				renderQuad();

				window.display();
			}
		}
};

#endif