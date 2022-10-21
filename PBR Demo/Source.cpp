#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);	// acount for resizing the window
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);	// for processing all inputs
unsigned int loadTexture(const char* path);
void loadTextureSet(std::string setName, int i);
void renderSphere();


// CAMERA
//=======
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 1600 / 2.0f;
float lastY = 900 / 2.0f;	// (screen size / 2) so the mouse is palced in the centre of the screen
bool firstMouse = true;		// true if the mouse is entering the screen for the first time

// TIME
//=====
float deltaTime = 0.0f;	// time between current and last frame
float lastFrame = 0.0f;

// TEXTURES
//=========
unsigned int albedo_0, albedo_1, albedo_2, albedo_3, albedo_4;
unsigned int normal_0, normal_1, normal_2, normal_3, normal_4;
unsigned int metallic_0, metallic_1, metallic_2, metallic_3, metallic_4;
unsigned int roughness_0, roughness_1, roughness_2, roughness_3, roughness_4;
unsigned int ao_0, ao_1, ao_2, ao_3, ao_4;

int textureMapVars[5][5] =
{
	{albedo_0, albedo_1, albedo_2, albedo_3, albedo_4},
	{normal_0, normal_1, normal_2, normal_3, normal_4},
	{metallic_0, metallic_1, metallic_2, metallic_3, metallic_4},
	{roughness_0, roughness_1, roughness_2, roughness_3, roughness_4},
	{ao_0, ao_1, ao_2, ao_3, ao_4},
};


int main()
{
	// initialize GLFW, set version and set to core profile
	//=====================================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4); // MSAA
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// create GLFW window object
	//==========================
	GLFWwindow* window = glfwCreateWindow(1600, 900, "PBR Demo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);											// create the context for the window object
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);		// acount for resizing the window
	glfwSetCursorPosCallback(window, mouse_callback);						// register mouse and scroll callback
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);			// capture mouse cursor


	// initialize GLAD
	//================
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL Settings
	//================
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);					// wireframe mode
	glEnable(GL_DEPTH_TEST);										// enable depth testing
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//stbi_set_flip_vertically_on_load(true);
	

	// SHADERS
	//=========
	// create a shader program using the supplied vertex and fragment shaders
	Shader shader_PBR("E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Shaders/vs_PBR.glsl", "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Shaders/fs_PBR.glsl");

	shader_PBR.use();
	shader_PBR.setInt("albedoMap", 0);
	shader_PBR.setInt("normalMap", 1);
	shader_PBR.setInt("metallicMap", 2);
	shader_PBR.setInt("roughnessMap", 3);
	shader_PBR.setInt("aoMap", 4);


	// TEXTURES
	//=========
	loadTextureSet("cobble", 0);	// loads a set of texture maps for each texture
	loadTextureSet("space", 1);
	loadTextureSet("rusted", 2);
	loadTextureSet("granite", 3);
	loadTextureSet("wood", 4);


	// lights
	// ------
	glm::vec3 lightPos[] = 
	{
		glm::vec3(0.0f, 0.0f, 10.0f)
	};

	glm::vec3 lightCol[] = 
	{
		glm::vec3(150.0f, 150.0f, 150.0f)
	};

	int sphereCount = 5;
	float spacing = 2.5;

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)1600 / (float)900, 0.1f, 100.0f);
	shader_PBR.use();
	shader_PBR.setMat4("projection", projectionMatrix);

	// RENDER LOOP
	//============
	while (!glfwWindowShouldClose(window))
	{
		// time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window); 
		
		// rendering
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);					// set the colour with which the buffer will be cleared
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the buffer

		// model/view matrix transformations
		shader_PBR.use();
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		shader_PBR.setMat4("view", viewMatrix);
		shader_PBR.setVec3("viewPos", camera.Position);

		// draw spheres
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		for (int i = 0; i < sphereCount; ++i)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureMapVars[0][i]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureMapVars[1][i]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureMapVars[2][i]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textureMapVars[3][i]);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, textureMapVars[4][i]);

			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3((float)(i - (sphereCount / 2)) * spacing, 0.0f, 0.0f));

			shader_PBR.setMat4("model", modelMatrix);
			renderSphere();
		}

		// draw light
		shader_PBR.setVec3("lightPos[0]", lightPos[0]);
		shader_PBR.setVec3("lightCol[0]", lightCol[0]);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, lightPos[0]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		shader_PBR.setMat4("model", modelMatrix);

		renderSphere();

		// check for and call events, swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	shader_PBR.stopUsing();

	// deallocate glfw resources
	glfwTerminate();
	return 0;
}


// FUNCTIONS
//==========
unsigned int sphereVAO = 0;
unsigned int indexCount;
// function for rendering spheres from LearnOpenGL
void renderSphere()	
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;

				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (std::size_t i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}

		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// process inputs by checking if keys are pressed/released
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// called when window size changes to adjust the viewport dimensions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// called whenever the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// called whenever the scroll wheel is used
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// load texture
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void loadTextureSet(std::string setName, int i)
{
	std::string albedoName = "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Textures/" + setName + "/" + setName + "_albedo.png";
	const char* charAlbedoName = albedoName.c_str();
	textureMapVars[0][i] = loadTexture(charAlbedoName);

	std::string normalName = "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Textures/" + setName + "/" + setName + "_normal.png";
	const char* charNormalName = normalName.c_str();
	textureMapVars[1][i] = loadTexture(charNormalName);

	std::string metallicName = "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Textures/" + setName + "/" + setName + "_metallic.png";
	const char* charMetallicName = metallicName.c_str();
	textureMapVars[2][i] = loadTexture(charMetallicName);

	std::string roughnessName = "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Textures/" + setName + "/" + setName + "_roughness.png";
	const char* charRougnessName = roughnessName.c_str();
	textureMapVars[3][i] = loadTexture(charRougnessName);

	std::string AOname = "E:/Documents/University/Year3/GCP/PBR/PBR Project Submission/PBR Demo/Textures/" + setName + "/" + setName + "_ao.png";
	const char* charAOname = AOname.c_str();
	textureMapVars[4][i] = loadTexture(charAOname);
}