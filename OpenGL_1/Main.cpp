#include "INCLUDES.h"

#include "Engine.h"

int main(){
	// initialize SFML-OpenGL window
	char window_title[128] = "OpenGL";
	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = 4;
	contextSettings.depthBits = 32;
	sf::RenderWindow window(
		//sf::VideoMode(800, 600),
		sf::VideoMode(1024, 768),
		window_title,
		sf::Style::Resize | sf::Style::Close,
		contextSettings
	);

	// initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, window.getSize().x, window.getSize().y);

	Engine engine(window);
	engine.run(window);

	return EXIT_SUCCESS;
}