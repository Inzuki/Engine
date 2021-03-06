﻿/*#include "INCLUDES.h"

#include "OBJ.h"
#include "Lamp.h"
#include "Camera.h"
#include "OLDASS.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"
#include "AssimpLoader.h"

int ID = -1;

struct Player {
	int id;
	char name[1024];
	glm::vec3 pos, dir, right;
	glm::mat4 model;
	bool moving[4];
};

void renderText(const std::string &str, FT_Face face, float x, float y, float sx, float sy);

int main(){
	// initialize OpenGL window
	sf::RenderWindow window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(64));
	glViewport(0, 0, window.getSize().x, window.getSize().y);
    window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	sf::Clock clk;
	char fps[64];

	// initialize GLEW
	glewExperimental = true; glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// load shaders
	GLuint lampShader     = loadShaders("vertex_lamp.txt", "fragment_lamp.txt");
	GLuint lightingShader = loadShaders("vertex.txt",      "fragment.txt");
	GLuint skyboxShader   = loadShaders("skybox.vert",     "skybox.frag");

	// lamps
	std::vector<Lamp> lamps;
	lamps.push_back(Lamp(glm::vec3(1.2f, 1.0f, 2.0f)));

	// load models
	OBJ player_mdl("player.obj", "res/textures/player.png");
	OBJ stall("stall.obj", "res/textures/stallTexture.png");
	OBJ cube("cube.obj", nullptr);

	// load skyboxes
	std::vector<const GLchar*> faces;
	faces.push_back("right.jpg");
	faces.push_back("left.jpg");
	faces.push_back("top.jpg");
	faces.push_back("bottom.jpg");
	faces.push_back("back.jpg");
	faces.push_back("front.jpg");
	CubeMap skybox1(faces);

	// load terrain
	Terrain terrain1("height.jpg", "res/textures/sandgrass.jpg", window);

	// light properties
	glUseProgram(lightingShader);

	for(int i = 0; i < lamps.size(); i++){
		char strang[128], streng[128];
		sprintf(strang, "light[%i].ambient", i);
		sprintf(streng, "light[%i].diffuse", i);
		glUniform3f(glGetUniformLocation(lightingShader, strang), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader, streng), 0.5f, 0.5f, 0.5f);
	}
	glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);
    glUniform1f(glGetUniformLocation(lightingShader, "material.shininess"), 32.0f);

	glm::mat4 boxModel;
	glm::vec3 boxPos(1.f, 5.f, 1.f);
	boxModel = glm::translate(boxModel, boxPos);

	#pragma region server
	// get name
	printf("Please enter a name: "); printf("Inzuki\n");
	char moniker[1024]; sprintf(moniker, "Inzuki");
	//gets(moniker);

	// get server IP
	printf("Please enter a server IP: "); printf("127.0.0.1\n");
	//char someIP[128];
	//gets(someIP);

	// connect to server
	const unsigned short port = 50001;
	//std::string serverIP = someIP;          // have the user enter an IP
	//std::string serverIP = "134.129.55.96"; // my IP
	std::string serverIP = "127.0.0.1";     // localhost
	sf::IpAddress server = serverIP;

	printf("Connecting to %s...\n", serverIP.c_str());
	
	// tell the server the client is attempting to connect
	char sendMsg[1024];
	sprintf(sendMsg, "+%s", moniker);
	
	sf::UdpSocket socket;
	//socket.send(sendMsg, sizeof(sendMsg), server, port);

	// receive from the server if the client successfully connected
	char buffer[1024];
	std::size_t received;
	sf::IpAddress sender;
	unsigned short senderPort;
	//socket.receive(buffer, sizeof(buffer), received, sender, senderPort);

	// if the client successfully connected, the server should send back a success signal
	if(buffer[0] == 'y'){
		char buff[1024];
		for(int i = 0; i < sizeof(buffer); i++)
			buff[i] = buffer[i + 1];

		ID = atoi(buff);

		printf("Connected to %s successfully (given ID %i).\n", serverIP.c_str(), ID);
	}
	printf("Other connected players:\n");

	// insert this player into the list
	std::vector<Player> players;
	Player tempPlayer;
	tempPlayer.id = ID;
	sprintf(tempPlayer.name, "Inzuki");
	players.push_back(tempPlayer);

	socket.setBlocking(false);
	#pragma endregion

	// hold chat messages
	std::vector<std::string> chatMsgs;

	FT_Library ft_lib{nullptr};
	if(FT_Init_FreeType(&ft_lib) != 0)
		throw std::runtime_error("FreeType init failed.");

	FT_Face face{nullptr};
	if(FT_New_Face(ft_lib, "res/fonts/trebuc.ttf", 0, &face) != 0)
		throw std::runtime_error("FreeType init failed.");

	// stuff
	bool running = true;
	bool moving[4]; moving[0] = false; moving[1] = false; moving[2] = false; moving[3] = false;
	glm::vec3 lastDir = getDir();
	sf::Clock dirChangeClk;

	// text shader n stuff
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint vbo{0}, vao{0}, texture{0}, sampler{0};
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glGenTextures(1, &texture);
	glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint textShader     = loadShaders("text.vert",       "text.frag");
	glUseProgram(textShader); glBindAttribLocation(textShader, 0, "in_Position");

	int chatMin = 0, chatMax = 0;
	char typingText[1024]; memset(typingText, 0, 1024);

	// anims
	core::Model mdl("skinning.vert", "skinning.frag");
	core::ModelLoader mdlLoad;
	mdlLoad.loadModel("res/models/boblampclean.md5mesh", &mdl);
	mdl.init();

	GLuint skinShader = loadShaders("skinning_uh.vert", "skinning.frag");
	Skinning skin(skinShader);
	Mesh mash;
	mash.loadMesh("res/models/boblampclean.md5mesh");

	while(running){
		float sx = 2.f / window.getSize().x,
			  sy = 2.f / window.getSize().y;

		// send the new direction of the player when they move
		if(moving[0] || moving[1] || moving[2] || moving[3]){
			if(lastDir != getDir() && dirChangeClk.getElapsedTime().asSeconds() > 0.25f){
				sprintf(sendMsg, "d%i,%f,%f,%f,%f,%f,%f", ID, getDir().x, getDir().z, getRight().x, getRight().z, getPos().x, getPos().z);
				socket.send(sendMsg, sizeof(sendMsg), server, port);

				lastDir = getDir();
				dirChangeClk.restart();
			}
		}

		#pragma region receiveFromServer
		// receive from the server
		if(socket.receive(buffer, sizeof(buffer), received, sender, senderPort) == sf::Socket::Done){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			switch(buffer[0]){
				// when a new player connects
				case 'a':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					tempPlayer.pos   = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.dir   = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.right = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.model = glm::mat4();
					tempPlayer.moving[0] = false; tempPlayer.moving[1] = false; tempPlayer.moving[2] = false; tempPlayer.moving[3] = false;
					players.push_back(tempPlayer);

					printf("%s has connected.\n", tempPlayer.name);
				}break;
				// inform the player of other connected players
				case 'A':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					printf("- %s\n", tempPlayer.name);
					tempPlayer.pos = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.dir = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.right = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.model = glm::mat4();
					tempPlayer.moving[0] = false; tempPlayer.moving[1] = false; tempPlayer.moving[2] = false; tempPlayer.moving[3] = false;
					players.push_back(tempPlayer);
				}break;
				// when a player disconnects
				case 'r':{
					printf("%s has disconnected.\n", players[atoi(buff)].name);

					for(int i = 0; i < players.size(); i++)
						if(players[i].id == atoi(buff))
							players.erase(players.begin() + i);
				}break;
				// when a player begins moving
				case 'm':{
					int id, key; float dirX, dirZ;
					sscanf(buff, "%i,%i,%f,%f", &id, &key, &dirX, &dirZ);

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							players[i].moving[key] = true;

							if(key == 0 || key == 2)
								players[i].dir   = glm::vec3(dirX, 0.f, dirZ);
							else
								players[i].right = glm::vec3(dirX, 0.f, dirZ);
						}
					}
				}break;
				// when a player stops moving
				case 's':{
					int id, key; float x, z;
					sscanf(buff, "%i,%i,%f,%f", &id, &key, &x, &z);

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							players[i].moving[key] = false;

							players[i].model = glm::translate(players[i].model,
															  glm::vec3(x - players[i].pos.x,
																		0.f,
																		z - players[i].pos.z));
							players[i].pos = glm::vec3(x, 0.f, z);
						}
					}
				}break;
				// when a player changes direction while moving
				case 'd':{
					int id; float x, z, dirX, dirZ, rX, rZ;
					sscanf(buff, "%i,%f,%f,%f,%f,%f,%f", &id, &dirX, &dirZ, &rX, &rZ, &x, &z);

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							players[i].dir   = glm::vec3(dirX, 0.f, dirZ);
							players[i].right = glm::vec3(rX,   0.f, rZ);

							players[i].model = glm::translate(players[i].model,
															  glm::vec3(x - players[i].pos.x,
																		0.f,
																		z - players[i].pos.z));
							players[i].pos = glm::vec3(x, 0.f, z);
						}
					}
				}break;
				// when a chat message is received
				case 't':{
					int id; char text[1024];
					sscanf(buff, "%i,%s", &id, &text);

					for(int i = 0; i < 1024; i++)
						if(text[i] == '☺')
							text[i] = ' ';

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							char message[1024];

							sprintf(message, "%s: %s", players[i].name, text);

							chatMsgs.push_back(message);

							if(chatMsgs.size() >= 10){
								chatMin++;
								chatMax++;
							}else
								chatMax++;
						}
					}
				}break;
			}
		}
		#pragma endregion

		// set the player's height to the terrain's height at that spot
		moveY(terrain1.getHeight(getPos().x, getPos().z) + 5.f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime     = clk.getElapsedTime().asSeconds(),
			  deltaTime       = float(currentTime - lastTime);
		// update window title to display framerate
		sprintf(fps, "[OpenGL] - FPS: %i", (int)round(1.f / deltaTime));
		window.setTitle(fps);

		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			// close the window and tell the server the client disconnected
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				//sprintf(sendMsg, "-%i", ID);
				//socket.send(sendMsg, sizeof(sendMsg), server, port);

				running = false;
				//break;
			}

			if(event.type == sf::Event::KeyPressed){
				// when the player moves
				if(event.key.code == sf::Keyboard::W && !moving[0]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 0, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[0] = true;
				}
				if(event.key.code == sf::Keyboard::A && !moving[1]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 1, getRight().x, getRight().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[1] = true;
				}
				if(event.key.code == sf::Keyboard::S && !moving[2]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 2, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[2] = true;
				}
				if(event.key.code == sf::Keyboard::D && !moving[3]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 3, getRight().x, getRight().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[3] = true;
				}
			}

			if(event.type == sf::Event::TextEntered){
				if(typing()){
					if(event.text.unicode > 31 && event.text.unicode < 128)
						sprintf(typingText, "%s%c", typingText, static_cast<char>(event.text.unicode));
				}
			}

			if(event.type == sf::Event::KeyReleased){
				// bring up text box to send messages
				if(event.key.code == sf::Keyboard::Return){
					if(typing()){
						for(int i = 0; i < 1024; i++)
							if(typingText[i] == ' ')
								typingText[i] = '☺';

						sprintf(sendMsg, "t%i,%s", ID, typingText);
						// sprintf(sendMsg, "=HELLO HI STUFF");
						socket.send(sendMsg, sizeof(sendMsg), server, port);
						memset(typingText, 0, 1024);
						setTyping(false);
					}else
						setTyping(true);
				}

				// lock/unlock the cursor
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();

				// when the player stops moving
				if(event.key.code == sf::Keyboard::W){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 0, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[0] = false;
				}
				if(event.key.code == sf::Keyboard::A){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 1, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[1] = false;
				}
				if(event.key.code == sf::Keyboard::S){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 2, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[2] = false;
				}
				if(event.key.code == sf::Keyboard::D){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 3, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[3] = false;
				}
			}

			// when the window is resized, fix the viewport
			if(event.type == sf::Event::Resized){
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				terrain1.updateRes(window);
			}

			// mouse picking information
			if(event.type == sf::Event::MouseButtonPressed){
				glm::vec3 v1, v2;
				get3DRay(&v1, &v2, window);

				// set a restriction on checking the area around the player
				// so it doesn't check over the entire map every call
				float range = 20.f;

				int x_min = 0, z_min = 0, x_max = 0, z_max = 0;

				(int(getPos().x) - range < 0) ? x_min = 0 : x_min = int(getPos().x) - range;
				(int(getPos().z) - range < 0) ? z_min = 0 : z_min = int(getPos().z) - range;

				(int(getPos().x) + range > 399) ? x_max = 399 : x_max = int(getPos().x) + range;
				(int(getPos().z) + range > 399) ? z_max = 399 : z_max = int(getPos().z) + range;

				// set the position of the box
				for(int z = z_min; z < z_max; z++){
					for(int x = x_min; x < x_max; x++){
						if(coll(glm::vec3(x, terrain1.getHeight(x, z), z), 2, v1, v2)){
							// set the box to the position the cursor is
							// pointing at, rather than translating it by that much
							boxModel = glm::translate(
								boxModel, glm::vec3(
									float(x) - boxPos.x,
									terrain1.getHeight(x, z) + 1.f - boxPos.y,
									float(z) - boxPos.z
								)
							);
							// set the position to the new position
							boxPos = glm::vec3(float(x), terrain1.getHeight(x, z) + 1.f, float(z));

							break; // quit running the for loop when the spot is found
						}
					}
				}
			}
		}
		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);
		
		// clear buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();
		glm::mat4 model;

		// skybox
		glDepthMask(GL_FALSE);
			glUseProgram(skyboxShader);
			glm::mat4 VP2 = getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(VP2));

			skybox1.draw(skyboxShader);
		glDepthMask(GL_TRUE);
		
		// draw terrain
		terrain1.draw(VP, lamps);

		// setup light properties
		glUseProgram(lightingShader);
		for(int i = 0; i < lamps.size(); i++){
			char strang[128];
			sprintf(strang, "light[%i].position", i);
			glUniform3f(glGetUniformLocation(lightingShader, strang),
				lamps[i].getLampPos().x,
				lamps[i].getLampPos().y,
				lamps[i].getLampPos().z
			);
		}

		// draw players
		for(int i = 0; i < players.size(); i++){
			if(players[i].id != ID){
				if(players[i].moving[0]){ // forward
					players[i].model = glm::translate(players[i].model,
													  glm::vec3(25.f * players[i].dir.x * deltaTime,
																0.f,
																25.f * players[i].dir.z * deltaTime));
					players[i].pos.x += 25.f * players[i].dir.x * deltaTime;
					players[i].pos.z += 25.f * players[i].dir.z * deltaTime;
				}
				if(players[i].moving[1]){ // left
					players[i].model = glm::translate(players[i].model,
													  glm::vec3(-(25.f * players[i].right.x * deltaTime),
																0.f,
																-(25.f * players[i].right.z * deltaTime)));
					players[i].pos.x -= 25.f * players[i].right.x * deltaTime;
					players[i].pos.z -= 25.f * players[i].right.z * deltaTime;
				}
				if(players[i].moving[2]){ // back
					players[i].model = glm::translate(players[i].model,
													  glm::vec3(-(25.f * players[i].dir.x * deltaTime),
																0.f,
																-(25.f * players[i].dir.z * deltaTime)));
					players[i].pos.x -= 25.f * players[i].dir.x * deltaTime;
					players[i].pos.z -= 25.f * players[i].dir.z * deltaTime;
				}
				if(players[i].moving[3]){ // right
					players[i].model = glm::translate(players[i].model,
													  glm::vec3(25.f * players[i].right.x * deltaTime,
																0.f,
																25.f * players[i].right.z * deltaTime));
					players[i].pos.x += 25.f * players[i].right.x * deltaTime;
					players[i].pos.z += 25.f * players[i].right.z * deltaTime;
				}

				cube.draw(players[i].model, VP, lightingShader);
			}
		}
		
		// draw stall
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(25.f, 0.f, 25.f));
		stall.draw(model, VP, lightingShader);

		// draw cube
		cube.draw(boxModel, VP, lightingShader);

		// draw guard
		glUseProgram(skinShader);

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.09f));
		std::vector<glm::mat4> transforms;

		mash.boneTransform(clk.getElapsedTime().asSeconds(), transforms);
		mash.render(model, VP, skinShader);

		for(unsigned int i = 0; i < transforms.size(); i++)
			skin.setBoneTrans(i, transforms[i]);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(25.f, -10.f, 0.f));
		model = glm::scale(model, glm::vec3(0.09f));
		mdl.tick(clk.getElapsedTime().asSeconds());
		mdl.render(deltaTime, VP, model);

		// draw lamps
		glUseProgram(lampShader);
		GLuint matrixLoc = glGetUniformLocation(lampShader, "VP"), modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-25.f, 60.f, -25.f));
		for(int i = 0; i < lamps.size(); i++)
			lamps[i].draw(model, modelLoc, matrixLoc, VP);

		// text
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindSampler(0, sampler);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glUseProgram(textShader);
		glUniform4f(glGetUniformLocation(textShader, "color"), 1.f, 1.f, 1.f, 0.7f);
		glUniform1i(glGetUniformLocation(textShader, "tex"),   0);
		FT_Set_Pixel_Sizes(face, 0, 18);

		// display chat messages
		for(int i = chatMin; i < chatMax; i++){
			int scroller = 0;
			if(chatMax >= 10)
				scroller = (chatMax - 9);

			renderText(
				chatMsgs.at(i),
				face,
				-0.975f,
				-0.455f - ((i - scroller) * 0.05f),
				2.f / window.getSize().x,
				2.f / window.getSize().y
			);
		}
		// display text when the user is typing
		if(typing()){
			renderText(
				typingText,
				face,
				-0.970f,
				-0.9f,
				2.f / window.getSize().x,
				2.f / window.getSize().y
			);
		}
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// display window
		window.display();

		lastTime = currentTime;
	}

	// free terrain
	terrain1.deleteTerrain();
	// free models
	stall.deleteObj();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &texture);
	glDeleteSamplers(1, &sampler);
	// free lamps
	for(int i = 0; i < lamps.size(); i++)
		lamps[i].deleteLamp();
	// free shaders
	glDeleteProgram(lightingShader);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(lampShader);
	glDeleteProgram(textShader);
	
	return EXIT_SUCCESS; // qed
}

void renderText(const std::string &str, FT_Face face, float x, float y, float sx, float sy){
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	const FT_GlyphSlot g = face->glyph;

	for(auto c : str){
		if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			continue;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_R8,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		const float vx = x + g->bitmap_left  * sx;
		const float vy = y + g->bitmap_top   * sy;
		const float w  =     g->bitmap.width * sx;
		const float h  =     g->bitmap.rows  * sy;

		struct {
			float x, y, s, t;
		}data[6] = {
			{ vx,     vy,     0, 0 },
			{ vx,     vy - h, 0, 1 },
			{ vx + w, vy,     1, 0 },
			{ vx + w, vy,     1, 0 },
			{ vx,     vy - h, 0, 1 },
			{ vx + w, vy - h, 1, 1 },
		};

		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}*/