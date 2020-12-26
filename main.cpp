/*
Student Information
Student ID: 1155092042
Student Name: Wang Jiahe

Student ID: 1155111034
Student Name: Lau Ping Tung
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "Dependencies/stb_image/stb_image.h"


//#include "Shader.h"
//#include "Texture.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include<time.h>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

GLint programID;

//Shader skyshader;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;


int TexNum = 0;
int objArrayLength;
GLuint texture[10][2];
GLuint vao[10], ebo[10], vboID[10];

GLuint vaoL, vboL;
float specBrightness = 15.0, diffBrightness = 0.25;
float xLight = 1.0f, yLight = 1.0f;
float xMouse = 0.0f, yMouse = 0.0f, xMouseAdd = 0.0f, yMouseAdd = 0.0f;
float projectRatio;
float dolxMove = 0.0f, dolyMove = 0.0f, dolzMove = 0.0f;
bool viewFixed = false;
//parameter for passignment 2 not important


int timecounter = 0;
//project content:
// skybox:
GLuint skyboxVAO, skyboxVBO;
int Skybox_programID;



GLuint spaceTextureID[20];
GLuint earth_cubemapTexture;
GLuint TextureID0;
GLuint TextureID1;

float oldx = 0.0f;
glm::mat4 Model_matrix;

GLuint spacecraftTexture[2];
bool spacecraftTextureSwitch = true;
GLuint alienVehicleTexture[2];
bool alienVehicleTextureSwitch1 = true;
bool alienVehicleTextureSwitch2 = true;
bool alienVehicleTextureSwitch3 = true;
GLuint planetTexture[2];
bool planetTextureSwitch = true;
GLuint foodTexture[2];
bool foodTextureSwitch = true;
GLuint alienTexture;

GLuint rockTexture;

bool food_gone[3] = { false,false,false };
float SCInitialPos[3] = { 0.0f,0.0f,0.0f };
float SCTranslation[3] = { 0.0f,0.0f,0.0f };
float alien_rotate = 0.0f;

glm::mat4 spacecraftModel, alienModel[3], planetModel, foodModel[3];
glm::mat4 SC_Rot_M;
glm::vec3 SC_local_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 SC_local_right = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 SC_local_pos = glm::vec3(0.0f, 0.0f, 0.0f);
float viewRotateDegree[2] = { 0.0f,0.0f };
glm::vec4 SC_world_pos;
glm::vec4 SC_world_Front_Direction;
glm::vec4 SC_world_Right_Direction;
glm::vec4 Camera_world_position;
int touch_alien_num = 0;
bool touch_food_num[3] = { false,false,false };
float x_translate = 0.0f, y_rotate = 90.0f, y_rotate_buffer = 0.0f, z_translate = 0.0f;
bool cd;
glm::vec4 planet_vec;
glm::vec4 spacecraft_vec;

unsigned int slot = 0;

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model obj[13];

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			char ch;
			char st[100];
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			char* buf = new char[1024];
			file.getline(buf, '\r\n');
			//file >> st;
			//std::cerr << st << std::endl;
			//sscanf_s(st, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			char* buf = new char[1024];
			file.getline(buf, '\r\n');
			// Check if there are more than three vertices in one face.
			//std::string redundency;
			//std::getline(file, redundency);
			//if (redundency.length() >= 5) {
			//	std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
			//	std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
			//	std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
			//	exit(1);
			//}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

GLuint loadCubeMap(std::vector<const GLchar*> faces)
{
	int width = 0, height = 0, bpp;
	unsigned char* image;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++) {
		//        unsigned char* data = stbi_load(texturePath, &Width, &Height, &BPP, 0);
		//        GLenum format=3;
		//std::cout << "width and height is" << width << height;
		//        image = loadBMP_custom(faces[i],width,height);
		image = stbi_load(faces[i], &width, &height, &bpp, 0);
		//std::cout << "width and height is" << width << height;
		//std::cout << "image is " << image << sizeof(image);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		//width = height = 0;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

GLuint loadTexture(const char* texturePath) {
	stbi_set_flip_vertically_on_load(true);

	int Width, Height, BPP;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = stbi_load(texturePath, &Width, &Height, &BPP, 0);
	GLenum format = 3;
	switch (BPP) {
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);


	return textureID;
}

glm::mat4 modelMatrices[100];

void CreateRand_ModelM() {
	int amount = 100;
	srand(time(0));
	GLfloat radius = 6.0f;
	GLfloat offset = 0.4f;
	GLfloat displacement;
	glm::mat4 model;
	for (GLuint i = 0; i < amount; i++) {
		model = glm::mat4(1.0f);
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		//x
		displacement = (rand() % (GLuint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		//y
		displacement = (rand() % (GLuint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f + 1;
		//z
		displacement = (rand() % (GLuint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));
		GLfloat scale = (rand() % 10) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		modelMatrices[i] = model;
		//printf("%f %f %f %f\n",modelMatrices[i][0][0],modelMatrices[i][0][1], modelMatrices[i][1][0], modelMatrices[i][1][1]);
	}

}


void sendDataToOpenGL()
{
	CreateRand_ModelM();
	char objArray[][101] = {
			"resources/dolphin/dolphin.obj",
			"resources/sea/sea.obj",
			//        "object models and textures/alienpeople.obj",

			//

			//                    "resources/rock.obj",
						"resources/spacecraft.obj",
						"resources/alienvehicle.obj",
						"resources/planet.obj",
						"resources/chicken.obj",
						"resources/orange.obj",
						"resources/alienpeople.obj",
						"resources/rock.obj"


						//            "resources/Rubik/Robik.obj"
								// I am sorry that I cannot load other .obj file here!:(((
	};
	char textureList[][2][101] = {
		{
			"resources/dolphin/dolphin_01.jpg",
			"resources/dolphin/dolphin_02.jpg"
		},
		{
			"resources/sea/sea_01.jpg",
			"resources/sea/sea_02.jpg"
		},
		//        {
				//            "object models and textures/alienTexture.bmp",
				//        },
				//        {
				//            "object models and textures/chickenTexture.bmp",
				//        },
				//        {
				//            "object models and textures/planetTexture.bmp",
				//        },
				//        {
				//            "object models and textures/rockTexture.bmp",
				//        },
		//
		//        {
		//            "resources/texture/planetTexture.bmp"
		//        },
		//        {
		//            "resources/texture/rockTexture.bmp"
		//        },
				//        }
				{
					"resources/texture/spacecraftTexture.bmp",
					"resources/texture/leisure_spacecraftTexture.bmp"
				},
				{
					"resources/texture/alienTexture.bmp",
					"resources/texture/colorful_alien_vehicleTexture.bmp"
				},
				{
					"resources/texture/planetNormal.bmp",
					"resources/texture/planetTexture.bmp"

				},
				{
					"resources/texture/chickenTexture.bmp"
				},
				{
					"resources/texture/orange.bmp",
					"resources/texture/orange.jpg"
				},
				{
					"resources/texture/alienTexture.bmp",
				},
				{
					"resources/texture/rockTexture.bmp",
				}

	};
	//objArrayLength = sizeof(objArray) / sizeof(objArray[0]);
	objArrayLength = 9;
	for (int i = 2; i < objArrayLength; i++)
	{
		if (i <= 8) {
			obj[i] = loadOBJ(objArray[i]);
		}
		if (i == 9 || i == 10) {
			obj[i] = obj[3];
		}
		if (i == 11 || i == 12) {
			obj[i] = obj[7];
		}

		switch (i)
		{
		case 0:
			texture[i][0] = loadTexture(textureList[i][0]);
			texture[i][1] = loadTexture(textureList[i][1]);
			break;
		case 1:
			texture[i][0] = loadTexture(textureList[i][0]);
			texture[i][1] = loadTexture(textureList[i][1]);
			break;
		case 2:
			spacecraftTexture[0] = loadTexture(textureList[i][0]);
			spacecraftTexture[1] = loadTexture(textureList[i][1]);
			break;
		case 3:
			alienVehicleTexture[0] = loadTexture(textureList[i][0]);
			alienVehicleTexture[1] = loadTexture(textureList[i][1]);
			break;
		case 4:
			planetTexture[0] = loadTexture(textureList[i][0]);
			planetTexture[1] = loadTexture(textureList[i][1]);
			break;
		case 5:
			foodTexture[0] = loadTexture(textureList[i][0]);
			break;
		case 6:
			foodTexture[1] = loadTexture(textureList[i][0]);
			texture[i][1] = loadTexture(textureList[i][1]);
			break;
		case 7:
			alienTexture = loadTexture(textureList[i][0]);
			break;
		case 8:
			rockTexture = loadTexture(textureList[i][0]);
			break;
		default:
			break;
		}


		// vertex array
		glGenVertexArrays(1, &vao[i]);    //create GLuint vao[0], ebo[0]; outside the sendDataToOpenGL() function because you will use them in paintGL() function
		glBindVertexArray(vao[i]);  //first VAO

		// vertex buffer

		glGenBuffers(1, &vboID[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[i]);
		glBufferData(
			GL_ARRAY_BUFFER,
			obj[i].vertices.size() * sizeof(Vertex),
			&obj[i].vertices[0],
			GL_STATIC_DRAW
		);

		//vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, position)
		); //vertex texture coordinate
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, uv)
		); //vertex normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, normal)
		); // index buffer
		glGenBuffers(1, &ebo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			obj[i].indices.size() * sizeof(unsigned int),
			&obj[i].indices[0],
			GL_STATIC_DRAW
		);
		TextureID0 = glGetUniformLocation(programID, "myTextureSampler0");
		TextureID1 = glGetUniformLocation(programID, "myTextureSampler1");



		//

	//        glBindBuffer(GL_ARRAY_BUFFER, 0);
	//
	//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	//    Skybox_programID = glCreateProgram();
	//    glUseProgram(Skybox_programID);
	// project Content:
	GLfloat skyboxVertices[] =
	{
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
	};

	std::vector<const GLchar*> earth_faces;
	earth_faces.push_back("resources/texture/universe skybox/right.bmp");
	earth_faces.push_back("resources/texture/universe skybox/left.bmp");
	earth_faces.push_back("resources/texture/universe skybox/bottom.bmp");
	earth_faces.push_back("resources/texture/universe skybox/top.bmp");
	earth_faces.push_back("resources/texture/universe skybox/back.bmp");
	earth_faces.push_back("resources/texture/universe skybox/front.bmp");
	earth_cubemapTexture = loadCubeMap(earth_faces);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
	std::ifstream meInput(fileName);
	if (!meInput.good()) {
		std::cout << "File failed to load ... " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders() {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("VertexShaderCode1.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode1.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	Skybox_programID = glCreateProgram();
	glAttachShader(Skybox_programID, vertexShaderID);
	glAttachShader(Skybox_programID, fragmentShaderID);
	glLinkProgram(Skybox_programID);

	if (!checkProgramStatus(Skybox_programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	installShaders();
}


bool collision_detection(glm::vec4 vectorA, glm::vec4 vectorB)
{
	if (glm::distance(vectorA, vectorB) <= 3.0f)
		return true;
	else
		return false;
}


void paintGL(void)  //always run
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glDepthMask(GL_FALSE);
	//    shaderSkb.use();
	glUseProgram(Skybox_programID);
	//    std::cout << "Program Id is :"<< Skybox_programID;

	glm::mat4 skb_viewMatrix = glm::lookAt(
		glm::vec3(Camera_world_position),
		glm::vec3(SC_world_pos),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	//project content

	bool normalMapping_flag = false;
	GLint normalMapping_flagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
	glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);

	glm::mat4 Skb_modelMatrix = glm::mat4(1.0f);
	GLint Skb_modelMatrixUniformLocation =
		glGetUniformLocation(Skybox_programID, "modelMatrix");
	glUniformMatrix4fv(Skb_modelMatrixUniformLocation, 1,
		GL_FALSE, &Skb_modelMatrix[0][0]);

	//shaderSkb.setMat4("modelTranfsormMatrix", Skb_modelMatrix);

	//    GLuint Skb_modelUniformLocation = glGetUniformLocation(Skybox_programID,"M");
	//
	//    glUniformMatrix4fv(Skb_modelUniformLocation,1,GL_FALSE,&Skb_modelMatrix[0][0]);
	glm::mat4 Skb_viewMatrix = glm::mat4(glm::mat3(skb_viewMatrix));
	glm::mat4 Skb_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 500.0f);

	GLint Skb_viewMatrixUniformLocation =
		glGetUniformLocation(Skybox_programID, "viewMatrix");
	glUniformMatrix4fv(Skb_viewMatrixUniformLocation, 1,
		GL_FALSE, &Skb_viewMatrix[0][0]);
	//shaderSkb.setMat4("viewMatrix", Skb_viewMatrix);

	GLint Skb_projectionMatrixUniformLocation =
		glGetUniformLocation(Skybox_programID, "projectionMatrix");
	glUniformMatrix4fv(Skb_projectionMatrixUniformLocation, 1,
		GL_FALSE, &Skb_projectionMatrix[0][0]);
	//shaderSkb.setMat4("projectionMatrix", Skb_projectionMatrix);

	//    glUniformMatrix4fv(glGetUniformLocation(Skybox_programID,"view"),1,GL_FALSE,glm::value_ptr(Skb_view));
	//    glUniformMatrix4fv(glGetUniformLocation(Skybox_programID,"projection"),1,GL_FALSE,glm::value_ptr(Skb_projection));
	//    glUseProgram(Skybox_programID);

	glUniform1i(glGetUniformLocation(Skybox_programID, "skybox"), 1);
	//shaderSkb.setInt("sky_box", 1);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	//    glUniform1i(glGetUniformLocation(Skybox_programID,"sky_box"),0);

	//    std::cout <<"skybox texture:"<<earth_cubemapTexture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, earth_cubemapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);





	glDepthMask(GL_TRUE);
	glUseProgram(programID);
		//

		//TODO:
		//Set lighting information, such as position and color of lighting source
		//Set transformation matrix
		//Bind different textures

	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f + x_translate, 0.5f, 15.0f + z_translate));
	glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(y_rotate), glm::vec3(0, 1, 0));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f, 0.0005f, 0.0005f));

	spacecraftModel = translateMatrix * rotateMatrix;

	//world space modelling
	glm::vec4 camera = spacecraftModel * glm::vec4(0.0f, 0.5f, 0.8f, 1.0f);
	//std::cout << camera[1] << std::endl;
	glm::vec4 viewport = spacecraftModel * glm::vec4(0.0f, 0.0f, -0.8f, 1.0f);
	// projectionMatrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 500.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(camera), glm::vec3(viewport), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm:: mat4 view = glm::lookAt(glm::vec3(+0.0f, +0.0f, +20.0f),
		//glm::vec3(+0.0f, +0.0f, +1.0f),
		//glm::vec3(+0.0f, +1.0f, +20.0f));

	//    std::cout << "xddddd" << Camera_world_position[0] << Camera_world_position[1] << Camera_world_position[2] <<
	//    SC_world_pos[0] << SC_world_pos[1] << SC_world_pos[2];

	spacecraftModel = spacecraftModel * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) * scaleMatrix;


	//    glm::mat4 viewMatrix = glm::lookAt(
	//                glm::vec3(Camera_world_position),
	//                glm::vec3(SC_world_pos),
	//                glm::vec3(0.0f, 1.0f, 0.0f)
	//            );

	GLint viewMatrixUniformLocation =
		glGetUniformLocation(programID, "viewMatrix");
	glUniformMatrix4fv(viewMatrixUniformLocation, 1,
		GL_FALSE, &view[0][0]);
	//shader.setMat4("viewMatrix", view);

	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);

	GLint modelTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "modelMatrix");
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
		GL_FALSE, &modelTransformMatrix[0][0]);
	//shader.setMat4("modelTransformMatrix", modelTransformMatrix);

	//glm::mat4 projectionMatrix = glm::perspective(
		//glm::radians(45.0f + 10.0f * projectRatio),
		//4.0f / 3.0f,
		//0.1f,
		//500.0f
	//);
	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		500.0f
	);
	GLint projectionMatrixUniformLocation =
		glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1,
		GL_FALSE, &projection[0][0]);
	//shader.setMat4("projectionMatrix", projection);


	//    setupLight(); //Set lighting parameters and forward to your shader in this function


	glm::vec3 ambientLight(0.5f, 0.5f, 0.5f);
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	//shader.setVec3("ambientLight", ambientLight);

	glm::vec3 lightPosition(camera);
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPosition");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	//shader.setVec3("lightPositionWorld", lightPosition);

	glm::vec3 diffBrightnessvec(diffBrightness, diffBrightness, diffBrightness);
	GLint diffBrightnessUniformLocation = glGetUniformLocation(programID, "diffBrightness");
	glUniform3fv(diffBrightnessUniformLocation, 1, &diffBrightnessvec[0]);
	//shader.setVec3("diffBrightness", diffBrightnessvec);

	glm::vec3 eyePosition(camera);
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePosition");
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	//shader.setVec3("eyePositionWorld", eyePosition);

	//shader.setInt("specBrightness", specBrightness);
	//int specBrightnessLocation = glGetUniformLocation(programID, "specBrightness");
	//glUniform1i(specBrightnessLocation, specBrightness);
	GLint specBrightnessUniformLocation = glGetUniformLocation(programID, "specBrightness");
	glUniform1i(specBrightnessUniformLocation, specBrightness);

	//Light2:sunlight
	glm::vec3 ambientLight1(1.0f, 1.0f, 1.0f);
	GLint ambientLightUniformLocation1 = glGetUniformLocation(programID, "ambientLight1");
	glUniform3fv(ambientLightUniformLocation1, 1, &ambientLight1[0]);

	glm::vec3 lightPosition1(-200.0f, 0.0f, 0.0f);
	GLint lightPositionUniformLocation1 = glGetUniformLocation(programID, "lightPosition1");
	glUniform3fv(lightPositionUniformLocation1, 1, &lightPosition1[0]);

	glm::vec3 diffBrightness1(0.5f, 0.2f, 0.2f);
	GLint diffBrightnessUniformLocation1 = glGetUniformLocation(programID, "diffBrightness1");
	glUniform3fv(diffBrightnessUniformLocation1, 1, &diffBrightness1[0]);

	int specBrightness1 = 120;
	GLint specBrightnessUniformLocation1 = glGetUniformLocation(programID, "specBrightness1");
	glUniform1i(specBrightnessUniformLocation1, specBrightness1);
	//Light2:sunlight




	//    shader.setMat4/setInt/setVec3... //general setting of your shaders, like transformation matrixes

	projectionMatrixUniformLocation =
		glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1,
		GL_FALSE, &projectionMatrix[0][0]);
	//shader.setMat4("projectionMatrix", projectionMatrix);

	//shader.use();  //Important!    the shader is shader instance you create
	//setupLight(); //Set lighting parameters and forward to your shader in this function
	//shader.setMat4 / setInt / setVec3... //general setting of your shaders, like transformation matrixes



	for (int i = 2; i < objArrayLength; i++)
	{
		if (i == 2) {
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, spaceTextureID[i - 2]);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, texture[i][TexNum]);
		}
		GLuint activeTexture;
		switch (i)
		{
		case 0:
			glBindTexture(GL_TEXTURE_2D, texture[i][TexNum]);
			break;
		case 1:
			glBindTexture(GL_TEXTURE_2D, texture[i][TexNum]);
			break;
		case 2:
			if (spacecraftTextureSwitch) {
				activeTexture = spacecraftTexture[0];
				glBindTexture(GL_TEXTURE_2D, spacecraftTexture[0]);
			}
			else {
				activeTexture = spacecraftTexture[1];
				glBindTexture(GL_TEXTURE_2D, spacecraftTexture[1]);
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			//glBindTexture(GL_TEXTURE_2D, activeTexture);
			break;
		case 3:
			//glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);
			if (alienVehicleTextureSwitch1) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			break;
		case 4:
			normalMapping_flag = true;
			normalMapping_flagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
			glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);
			//if (planetTextureSwitch) {
			//	activeTexture = planetTexture[0];
			//}
			//else {
			//	activeTexture = planetTexture[1];
			//}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, planetTexture[1]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, planetTexture[0]);
			break;
		case 5:
			normalMapping_flag = false;
			normalMapping_flagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
			glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);
			activeTexture = foodTexture[0];
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			break;
		case 6:
			//                    activeTexture = foodTexture[1];
			//                    glActiveTexture(GL_TEXTURE0 + 0);
			//                    glBindTexture(GL_TEXTURE_2D, activeTexture);
			glBindTexture(GL_TEXTURE_2D, texture[i][1]);
			break;
		case 7:
			activeTexture = alienTexture;
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
		case 8:
			activeTexture = rockTexture;
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, rockTexture);
		case 9:
			//glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);
			if (alienVehicleTextureSwitch1) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			break;
		case 10:
			//glUniform1i(normalMapping_flagUniformLocation, normalMapping_flag);
			if (alienVehicleTextureSwitch1) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			break;
		case 11:
			activeTexture = alienTexture;
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);

		case 12:
			activeTexture = alienTexture;
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);

		default:
			break;
		}


		//index 0 for example
//            std::cout << "yooooo";
//            std::cout << vao[i];
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
		timecounter += 1;
		switch (i)
		{
		case 0:
			modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(0.01f));


			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(0.0f + 10.0f * dolxMove, -100.0f + 10 * dolyMove, 0.0f + 10.0f * dolzMove));
			modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 1:
			modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(30.0f));
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
			break;
		case 2:
			//std::cout << glm::to_string(spacecraftModel) << std::endl;
			modelTransformMatrix = spacecraftModel;
			//                    modelTransformMatrix = scaleMatrix;
			break;
		case 3:

			//                    std::cout << timecounter << std::endl;
			//                    modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(0.01f));
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -0.0f));
			alienModel[0] = modelTransformMatrix;
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 4)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			if (alienVehicleTextureSwitch1) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0); 
			
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, -0.0f));
			alienModel[1] = modelTransformMatrix;
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 4)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			if (alienVehicleTextureSwitch2) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
			
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, -0.0f));
			alienModel[2] = modelTransformMatrix;
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 4)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			if (alienVehicleTextureSwitch3) {
				activeTexture = alienVehicleTexture[0];
			}
			else {
				activeTexture = alienVehicleTexture[1];
			}
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, activeTexture);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case 4:
			//                    modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f,0.0f,-50.0f));
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, 0.0f, 0.0f));
			//modelTransformMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(float(timecounter / 8)), glm::vec3(0.0f, 1.0f, 0.0f));;
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
			planetModel = modelTransformMatrix;
			break;
		case 5:
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 4.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
			foodModel[0] = modelTransformMatrix;
			break;
		case 6:
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 4.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
			foodModel[1] = modelTransformMatrix;
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			if (food_gone[1] == false) {
				glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
			}

			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, 4.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
			foodModel[2] = modelTransformMatrix;
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			if (food_gone[2] == false) {
				glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
			}
			break;
		case 7:
			modelTransformMatrix = glm::mat4(0.0005f);
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-10.0f, 0.0f, -0.0f));
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);

			modelTransformMatrix = glm::mat4(0.0005f);
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-20.0f, 0.0f, -0.0f));
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);

			modelTransformMatrix = glm::mat4(0.0005f);
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-30.0f, 0.0f, -0.0f));
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
			modelTransformMatrixUniformLocation =
				glGetUniformLocation(programID, "modelMatrix");
			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
				GL_FALSE, &modelTransformMatrix[0][0]);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case 8:
			glm::mat4 rockOrbitIni = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, -2.0f, 0.0f));
			glm::mat4 rockOrbit_M = glm::rotate(rockOrbitIni, glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			rockOrbit_M = rockOrbit_M * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
			glm::mat4 rockModelMat_temp;
			for (GLuint t = 0; t < 100; t++) {
				rockModelMat_temp = modelMatrices[t];
				rockModelMat_temp = rockOrbit_M * rockModelMat_temp;
				glBindVertexArray(vao[i]);
				modelTransformMatrixUniformLocation =
					glGetUniformLocation(programID, "modelMatrix");
				glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
					GL_FALSE, &rockModelMat_temp[0][0]);
				glBindTexture(GL_TEXTURE_2D, rockTexture);
				glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0);
				//glDrawArrays(GL_TRIANGLES, 0, obj[i].indices.size());
			}
		case 9:

			//                    std::cout << timecounter << std::endl;
			//                    modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(0.01f));
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, 0.0f));
			alienModel[1] = modelTransformMatrix;
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 4)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

			break;
		case 10:

			//                    std::cout << timecounter << std::endl;
			//                    modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(0.01f));
			modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 0.0f));
			alienModel[2] = modelTransformMatrix;
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 4)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

			break;

		case 11:
			modelTransformMatrix = glm::mat4(0.0005f);
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-30.0f, 0.0f, 0.0f));
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
			break;

		case 12:
			modelTransformMatrix = glm::mat4(0.0005f);
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-20.0f, 0.0f, -0.0f));
			modelTransformMatrix *= glm::rotate(glm::mat4(0.01f), glm::radians(float(timecounter / 6)), glm::vec3(0.0f, 1.0f, 0.0f));
			modelTransformMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
			break;
		default:

			modelTransformMatrix = glm::mat4(0.001f);
			break;
		}
		spacecraft_vec = spacecraftModel * glm::vec4(0, 0, 0, 1);

		planet_vec = planetModel * glm::vec4(0, 0, 0, 1);
		
		cd = collision_detection(spacecraft_vec, planet_vec);

		glm::vec4 alien_vec1 = alienModel[0] * glm::vec4(0, 0, 0, 1);
		glm::vec4 food_vec1 = foodModel[0] * glm::vec4(0, 0, 0, 1);

		glm::vec4 alien_vec2 = alienModel[1] * glm::vec4(0, 0, 0, 1);
		glm::vec4 food_vec2 = foodModel[1] * glm::vec4(0, 0, 0, 1);

		glm::vec4 alien_vec3 = alienModel[2] * glm::vec4(0, 0, 0, 1);
		glm::vec4 food_vec3 = foodModel[2] * glm::vec4(0, 0, 0, 1);
		//std::cout << "distance:" << glm::distance(spacecraft_vec, alien_vec) << std::endl;
		switch (i) {
		case 5:

			break;

		default:
			break;
		}
		// hide food
		if (!foodTextureSwitch && i == 5) {
			continue;
		}
		if (foodTextureSwitch && i == 6) {
			continue;
		}
		if (food_gone[0] && (i == 5 || i == 6)) {
			continue;
		}
		// check collision and change color and hide alien
		bool result1 = collision_detection(spacecraft_vec, alien_vec1);
		if (result1) {
			alienVehicleTextureSwitch1 = false;
		}
		bool result2 = collision_detection(spacecraft_vec, alien_vec2);
		if (result2) {
			alienVehicleTextureSwitch2 = false;
		}
		bool result3 = collision_detection(spacecraft_vec, alien_vec3);
		if (result3) {
			alienVehicleTextureSwitch3 = false;
		}
		if (!alienVehicleTextureSwitch1 && i == 7) {
			continue;
		}
		//set collision condition
		if (collision_detection(spacecraft_vec, food_vec1)) {
			touch_food_num[0] = true;
			food_gone[0] = true;
		}
		if (collision_detection(spacecraft_vec, food_vec2)) {
			touch_food_num[1] = true;
			food_gone[1] = true;
		}
		if (collision_detection(spacecraft_vec, food_vec3)) {
			touch_food_num[2] = true;
			food_gone[2] = true;
		}
		//set food condition
		if (touch_food_num[0] && touch_food_num[1] && touch_food_num[2]
			&& !alienVehicleTextureSwitch1 && !alienVehicleTextureSwitch2 && !alienVehicleTextureSwitch3) {
			spacecraftTextureSwitch = false;
		}

		modelTransformMatrixUniformLocation =
			glGetUniformLocation(programID, "modelMatrix");
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
			GL_FALSE, &modelTransformMatrix[0][0]);
		//shader.setMat4("modelTransformMatrix", modelTransformMatrix);
		if (i == 2 && spacecraftTextureSwitch == false) {
			glBindTexture(GL_TEXTURE_2D, spacecraftTexture[1]);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0); //render primitives from array data

		}
		if (i == 2 && spacecraftTextureSwitch == true) {
			glBindTexture(GL_TEXTURE_2D, spacecraftTexture[0]);
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0); //render primitives from array data

		}

		if (i > 3 && i != 6 && i != 7 && i != 8) {
			glDrawElements(GL_TRIANGLES, obj[i].indices.size(), GL_UNSIGNED_INT, 0); //render primitives from array data
		}
	}
}

void UpdateStatus() {
	float scale = 0.0005f;
	glm::mat4 SC_scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	glm::mat4 SC_trans_M = glm::translate
	(
		glm::mat4(1.0f),
		glm::vec3(SCInitialPos[0] + SCTranslation[0], SCInitialPos[1] + SCTranslation[1], SCInitialPos[2] + SCTranslation[2])
	);
	//
	Model_matrix = SC_trans_M * SC_Rot_M * SC_scale_M;
	SC_world_pos = Model_matrix * glm::vec4(SC_local_pos, 1.0f);
	SC_world_Front_Direction = Model_matrix * glm::vec4(SC_local_front, 1.0f);
	SC_world_Right_Direction = Model_matrix * glm::vec4(SC_local_right, 1.0f);
	SC_world_Front_Direction = glm::normalize(SC_world_Front_Direction);
	SC_world_Right_Direction = glm::normalize(SC_world_Right_Direction);
	glm::vec3 Camera_local_position = glm::vec3(0, 2.0f, 2.0f);
	Camera_world_position = Model_matrix * glm::vec4(Camera_local_position, 1.0f);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Sets the mouse-button callback for the current window.
	if (button == GLFW_MOUSE_BUTTON_LEFT && viewFixed == false) {
		viewFixed = true;

	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && viewFixed == true) {
		viewFixed = false;
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	// Sets the cursor position callback for the current window
//    if(!viewFixed){
//        xMouseAdd = (float)(300 - x) / 60;
//        yMouseAdd = (float)(y - 300) / 60;
//    }
//    if(viewFixed){
//        xMouse += xMouseAdd;
//        yMouse += yMouseAdd;
//        xMouseAdd = 0;
//        yMouseAdd = 0;
//    }
	if (x < oldx) {
		viewRotateDegree[1] += 1.0f;
		SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(viewRotateDegree[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		//        y_rotate_buffer += 5.0f ;
		y_rotate += 1 / (y_rotate / 10 + y_rotate_buffer);
	}
	if (x > oldx) {
		viewRotateDegree[1] -= 1.0f;
		SC_Rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(viewRotateDegree[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		//        y_rotate_buffer -= 5.0f ;
		y_rotate -= 1 / (y_rotate / 10 + y_rotate_buffer);
	}
	oldx = x;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Sets the scoll callback for the current window.
	projectRatio = yoffset;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.


	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {

		foodTextureSwitch = true;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {

		foodTextureSwitch = false;
	}





	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		SCTranslation[0] = SCTranslation[0] + 0.5f * SC_world_Front_Direction[0];
		SCTranslation[2] = SCTranslation[2] + 0.5f * SC_world_Front_Direction[2];
		std::cout << SCTranslation[0] << SCTranslation[2];
		x_translate -= 1.0f;
		//        x_translate += cos(y_rotate);
		//        z_translate += sin(y_rotate);

		//std::cout << "y_ratate:" << y_rotate << "||" << sin(y_rotate) << cos(y_rotate) << sin(45) << cos(45) << std::endl;
		//        SCTranslation[0] += 1.0f;
		//        SCTranslation[2] += 1.0f;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		x_translate += 1.0f;
		SCTranslation[0] = SCTranslation[0] - 0.5f * SC_world_Front_Direction[0];
		SCTranslation[2] = SCTranslation[2] - 0.5f * SC_world_Front_Direction[2];
		//        x_translate -= cos(y_rotate);
		//        z_translate -= sin(y_rotate);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		z_translate += 1.0f;
		SCTranslation[0] = SCTranslation[0] - 0.5f * SC_world_Right_Direction[0];
		SCTranslation[2] = SCTranslation[2] - 0.5f * SC_world_Right_Direction[2];
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		z_translate -= 1.0f;
		SCTranslation[0] = SCTranslation[0] + 0.5f * SC_world_Right_Direction[0];
		SCTranslation[2] = SCTranslation[2] + 0.5f * SC_world_Right_Direction[2];
	}
}



int main(int argc, char* argv[])
{
	//CreateRand_ModelM();
	//system("pause");
	
	GLFWwindow* window;

	glewExperimental = GL_TRUE;
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		UpdateStatus();
		
		paintGL();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}






