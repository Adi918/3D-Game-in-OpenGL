#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

void CheckBLockPos(bool var);
void initGL (GLFWwindow* window, int width, int height);

glm::mat4 translateBlock;

GLFWwindow* window; // window desciptor/handle

int width = 600;
int height = 600;

float camera_rotation_angle = 60;
float camera_rotation_angle1 = 0;

int Level=0;
int level = 1;
int horizontal = 0;
int vertical = 1;
int stand = 2;
double time1, time2;
int xi, yi, lives = 5;

struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;

typedef struct Tiles
{
	struct VAO* tile;
	glm::mat4 T;
	int type;
	float speed;
	bool state;
}tiles;

typedef struct Block
{
	struct VAO* block;
	glm::mat4 T;
	int x1;
	int x2;
	int y1;
	int y2;
	float z;
	float speed;
	bool fallingstatus;
	int state;
	float left;
	float right;
	float up;
	float down;
	bool roundstart;
	bool roundfinish;
	bool fragilefall;
}block;

block Block;
tiles Tile[140];


int Level1[30][14] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 1, 2, 1, 1, 1, 1, 0, 0, 0, 
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 1, 0, 0, 
	0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 1, 0, 0, 
	0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 3, 1, 0, 0, 
	0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 
	0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	//    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE && !Block.fallingstatus && !Block.roundstart ) {
		switch (key) {
			case GLFW_KEY_UP:
				if ( Block.state == stand )
				{
					Block.state = vertical;
					Block.y1 -= 1;
					Block.y2 -= 2;
				}
				else if ( Block.state == vertical )
				{
					Block.state = stand;
					Block.y1 -= 2;
					Block.y2 -= 1;
				}
				else
				{
					Block.y1 -= 1;
					Block.y2 -= 1;
				}
				break;
			case GLFW_KEY_DOWN:
				if ( Block.state == stand )
				{
					Block.state = vertical;
					Block.y1 += 2;
					Block.y2 += 1;
				}
				else if ( Block.state == vertical )
				{
					Block.state = stand;
					Block.y1 += 1;
					Block.y2 += 2;
				}
				else
				{
					Block.y1 += 1;
					Block.y2 += 1;
				}
				break;
			case GLFW_KEY_LEFT:
				if ( Block.state == stand )
				{
					Block.state = horizontal;
					Block.x1 -= 2;
					Block.x2 -= 1;
				}
				else if ( Block.state == horizontal )
				{
					Block.state = stand;
					Block.x1 -= 1;
					Block.x2 -= 2;
				}
				else
				{
					Block.x1 -= 1;
					Block.x2 -= 1;
				}
				break;
			case GLFW_KEY_RIGHT:
				if ( Block.state == stand )
				{
					Block.state = horizontal;
					Block.x1 += 1;
					Block.x2 += 2;
				}
				else if ( Block.state == horizontal )
				{
					Block.state = stand;
					Block.x1 += 2;
					Block.x2 += 1;
				}
				else
				{
					Block.x1 += 1;
					Block.x2 += 1;
				}
				break;
			case GLFW_KEY_A:
				if ( camera_rotation_angle < 90 )
					camera_rotation_angle += 10;
				break;
			case GLFW_KEY_D:
				if ( camera_rotation_angle > 30 )
					camera_rotation_angle -= 10;
				break;
			case GLFW_KEY_W:
				if ( camera_rotation_angle1 < 30 )
					camera_rotation_angle1 += 10;
				break;
			case GLFW_KEY_S:
				if ( camera_rotation_angle1 > -30 )
					camera_rotation_angle1 -= 10;
				break;
			case GLFW_KEY_R:
				camera_rotation_angle = 60;
				camera_rotation_angle1 = 60;
				break;
			default:
				break;
		}
		CheckBLockPos(false);
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			default:
				break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_RELEASE)
			{
				if ( Block.state == stand )
				{
					Block.state = horizontal;
					Block.x1 -= 2;
					Block.x2 -= 1;
				}
				else if ( Block.state == horizontal )
				{
					Block.state = stand;
					Block.x1 -= 1;
					Block.x2 -= 2;
				}
				else
				{
					Block.x1 -= 1;
					Block.x2 -= 1;
				}
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_RELEASE)
			{
				if ( Block.state == stand )
				{
					Block.state = horizontal;
					Block.x1 += 1;
					Block.x2 += 2;
				}
				else if ( Block.state == horizontal )
				{
					Block.state = stand;
					Block.x1 += 2;
					Block.x2 += 1;
				}
				else
				{
					Block.x1 += 1;
					Block.x2 += 1;
				}
			break;
		}
		default:
			break;
	}
	CheckBLockPos(false);
}

void scroll_callback ( GLFWwindow* window, double xoffset, double yoffset )
{
	if ( yoffset > 0 )
	{
		if ( Block.state == stand )
		{
			Block.state = vertical;
			Block.y1 -= 1;
			Block.y2 -= 2;
		}
		else if ( Block.state == vertical )
		{
			Block.state = stand;
			Block.y1 -= 2;
			Block.y2 -= 1;
		}
		else
		{
			Block.y1 -= 1;
			Block.y2 -= 1;
		}
	}
	else if ( yoffset < 0 )
	{
		if ( Block.state == stand )
		{
			Block.state = vertical;
			Block.y1 += 2;
			Block.y2 += 1;
		}
		else if ( Block.state == vertical )
		{
			Block.state = stand;
			Block.y1 += 1;
			Block.y2 += 2;
		}
		else
		{
			Block.y1 += 1;
			Block.y2 += 1;
		}
	}
	CheckBLockPos(false);
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	// Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}


void createRectangle (float width,float length, float height)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		//front
		-width/2, -length/2, height/2,
		-width/2, -length/2, -height/2,
		width/2, -length/2, -height/2,

		width/2, -length/2, -height/2,
		width/2, -length/2, height/2,
		-width/2, -length/2, height/2,

		//back
		-width/2, length/2, height/2,
		-width/2, length/2, -height/2,
		width/2, length/2, -height/2,

		width/2, length/2, -height/2,
		width/2, length/2, height/2,
		-width/2, length/2, height/2,

		//left
		-width/2, length/2, -height/2,
		-width/2, -length/2, -height/2,
		-width/2, -length/2, height/2,

		-width/2, -length/2, height/2,
		-width/2, length/2, height/2,
		-width/2, length/2, -height/2,

		//right
		width/2, length/2, height/2,
		width/2, length/2, -height/2,
		width/2, -length/2, -height/2,

		width/2, -length/2, -height/2,
		width/2, -length/2, height/2,
		width/2, length/2, height/2,

		//base
		-width/2, length/2, -height/2,
		-width/2, -length/2, -height/2,
		width/2, -length/2, -height/2,

		width/2, -length/2, -height/2,
		width/2, length/2, -height/2,
		-width/2, length/2, -height/2,

		//top
		-width/2, length/2, height/2,
		-width/2, -length/2, height/2,
		width/2, -length/2, height/2,

		width/2, -length/2, height/2,
		width/2, length/2, height/2,
		-width/2, length/2, height/2,


	};

	static const GLfloat color_buffer_data [] = {
		0.4,0.5,0.5,
		0.4,0.5,0.5,
		0.4,0.5,0.5,

		0.4,0.5,0.5,
		0.4,0.5,0.5,
		0.4,0.5,0.5,

		0.2,0.4,0.6,
		0.2,0.4,0.6,
		0.2,0.4,0.6,

		0.2,0.4,0.6,
		0.2,0.4,0.6,
		0.2,0.4,0.6,

		0.1,0.2,0.6,
		0.1,0.2,0.6,
		0.1,0.2,0.6,

		0.1,0.2,0.6,
		0.1,0.2,0.6,
		0.1,0.2,0.6,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,

		0.1,0.7,0.7,
		0.1,0.7,0.7,
		0.1,0.7,0.7,

		0.1,0.7,0.7,
		0.1,0.7,0.7,
		0.1,0.7,0.7,

		0.2,0.1,0.7,
		0.2,0.1,0.7,
		0.2,0.1,0.7,

		0.2,0.1,0.7,
		0.2,0.1,0.7,
		0.2,0.1,0.7,
	};

	//create3DObject creates and returns a handle to a VAO that can be used later
	Block.block = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

VAO* createBoard (float width,float length, float height,float r, float g, float b, float type)
{
	// GL3 accepts only Triangles. Quads are not supported
	GLfloat vertex_buffer_data [] = {

		-width/2,-length/2,-height/2, // vertex 1
		width/2,-length/2,-height/2, // vertex 2
		width/2, length/2,height/2, // vertex 3

		width/2, length/2,-height/2, // vertex 3
		-width/2, length/2,-height/2, // vertex 4
		-width/2,-length/2,-height/2 , // vertex 1

		width/2,length/2,-height/2,
		-width/2,length/2,-height/2,  
		-width/2,length/2,height/2,

		-width/2,length/2,height/2,
		width/2,length/2,height/2,
		width/2,length/2,-height/2,

		width/2,length/2,height/2,
		width/2,length/2,-height/2,
		width/2,-length/2,-height/2,

		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,
		width/2,length/2,height/2,



		-width/2,-length/2,-height/2,
		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,

		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,

		width/2,length/2,height/2,
		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,

		-width/2,-length/2,height/2,
		-width/2,length/2,height/2,
		width/2,length/2,height/2,

		-width/2,-length/2,-height/2,
		-width/2,length/2,-height/2, 
		-width/2,length/2,height/2,

		-width/2,length/2,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,
	};

	GLfloat color_buffer_data1 []={

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.7,0.7,0.1,
		0.7,0.7,0.1,
		0.7,0.7,0.1,

		0.7,0.7,0.1,
		0.7,0.7,0.1,
		0.7,0.7,0.1,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,
	};

	GLfloat color_buffer_data2 []={

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.1,0.1,0.1,
		0.1,0.1,0.1,
		0.1,0.1,0.1,

		0.1,0.1,0.1,
		0.1,0.1,0.1,
		0.1,0.1,0.1,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,
	};

	GLfloat color_buffer_data3 []={

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.7,0.1,0.2,
		0.7,0.1,0.2,
		0.7,0.1,0.2,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.5,0.5,0.4,
		0.5,0.5,0.4,
		0.5,0.5,0.4,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.4,0.2,
		0.6,0.4,0.2,
		0.6,0.4,0.2,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.6,0.2,0.1,
		0.6,0.2,0.1,
		0.6,0.2,0.1,

		0.1,0.1,0.1,
		0.1,0.1,0.1,
		0.1,0.1,0.1,

		0.1,0.1,0.1,
		0.1,0.1,0.1,
		0.1,0.1,0.1,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,

		0.4,0.4,0.4,
		0.4,0.4,0.4,
		0.4,0.4,0.4,
	};

	if ( type == 1 || type == 0 )
		return create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data1, GL_FILL);
	else if ( type == 2 )
	{
		return create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data2, GL_FILL);
	}
}

void CheckBLockPos ( bool var )
{
	if ( var || Level1[(10*(Level-1))+Block.y1][Block.x1] == 0 || Level1[(10*(Level-1))+Block.y2][Block.x2] == 0 )
	{
		Block.state = stand;
		if ( Level1[(10*(Level-1))+Block.y2][Block.x2] == 0 )
		{
			Block.y1 = Block.y2;
			Block.x1 = Block.x2;
		}
		Block.fallingstatus = 1;
		time1 = glfwGetTime();
	}
}

void ResetBlock ()
{
	Block.roundfinish = false;
	Block.fallingstatus = false;
	Block.x1 = xi;
	Block.x2 = xi;
	Block.y1 = yi;
	Block.y2 = yi;
	Block.state = stand;
}

float t = 0;

void BlockFall ()
{
	Block.roundstart = true;
	if ( 6 *cos(1*t) > 0.1 )
		Block.T[3][2] = 4*cos(1*t);
	else
		Block.T[3][2] = 0;
}

float rectangle_rotation = 0;
float triangle_rotation = 0;

void BlockFall ( int i )
{
	Tile[i].T[3][2] = -8*(time2-time1);
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
	t+=0.05;
	time2 = glfwGetTime();
	float x_rotate=0, y_rotate=0, z_rotate=0;
	float x=0, y=0;
	if ( Block.state == horizontal )
	{
		y_rotate = 1;
		x = 0.25;
		Block.z = 0.25;
	}
	else if ( Block.state == vertical )
	{
		Block.z = 0.25;
		y = 0.25;
		z_rotate = 1;
	}
	else
	{
		Block.z = 0.5;
		x_rotate = 1;
	}
	// clear the color and depth in the frame buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram (programID);

	// Eye - Location of camera. Don't change unless you are sure!!
	glm::vec3 eye ( -5*cos(camera_rotation_angle*M_PI/180.0f), -5*sin(camera_rotation_angle*M_PI/180.0f)*cos(camera_rotation_angle1*M_PI/180.0f), 3.2*(1+sin(camera_rotation_angle*M_PI/180.0f)*sin(camera_rotation_angle1*M_PI/180.0f)) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (0, 0, 1);

	// Compute Camera matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	Matrices.view = glm::lookAt(eye, target, up); // Fixed camera for 2D (ortho) in XY plane
	//Matrices.view = glm::lookAt(glm::vec3(-2,-2,3.2), glm::vec3(0,0,0), glm::vec3(0,1,1)); // Fixed camera for 2D (ortho) in XY plane

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);
	for ( int i = 0 ; i < 140 ; i++ )
	{
		Matrices.model = glm::mat4(1.0f);
		Matrices.model *= Tile[i].T;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		if ( Level1[(10*(Level-1))+(i/14)][i%14] == 1 || Level1[(10*(Level-1))+(i/14)][i%14] == 2 )
			draw3DObject(Tile[i].tile);
	}

	if ( time2 - time1 > 1 && Block.fallingstatus )
	{
		if ( Block.fragilefall )
		{
			Block.fragilefall = false;
			Level1[10*(Level-1)+Block.y1][Block.x1] = 2;
			Tile[14*Block.y1+Block.x1].T[3][2] = 0;
			Block.fragilefall = false;
		}
		ResetBlock();
		lives--;
		if ( !lives )
			glfwTerminate();
	}

	if ( Block.roundstart )
	{
		BlockFall();
		if ( time2-time1 > 0.8 )
		{
			Block.roundstart = false;
			Block.T[3][2] = 0;
		}
	}

	if ( Level == 0 )
		Level++;

	cout << Block.T[3][2] << endl;

	if ( Level == 0 || Block.state == stand && Level1[10*(Level-1)+Block.y1][Block.x1] == 3 )
	{
		if ( !Block.roundfinish )
		{
			time1 = glfwGetTime();
			Block.roundfinish = true;
			CheckBLockPos(true);
		}
		if ( time2-time1 > 0.8 )
		{
			Level++;
			if ( Level == 1 )
			{
				Level = 2;
				t = 0;
				xi = 3;
				yi = 3;
				Block.roundfinish = false;
				time1 = glfwGetTime();
				ResetBlock();
				BlockFall();
			}
			if ( Level == 2 )
			{
				t = 0;
				xi = 2;
				yi = 5;
				level = 2;
				time1 = glfwGetTime();
				initGL(window,width,height);
				ResetBlock();
				BlockFall();
			}
		}
	}

	bool var = false;
	if ( Block.state == stand && Level1[10*(Level-1)+Block.y1][Block.x1] == 2 && !Block.fragilefall )
	{
		Tile[14*Block.y1+Block.x1].state = true;
		CheckBLockPos(true);
		Block.fragilefall = true;
	}
	if ( Tile[14*Block.y1+Block.x1].state )
		BlockFall(14*Block.y1+Block.x1);

	Matrices.model = glm::mat4(1.0f);
	translateBlock = glm::translate(glm::vec3(x+0.52*(Block.x1-7),y-0.52*(Block.y1-5),Block.z-(Block.fallingstatus*Block.speed*(time2-time1))-(Block.roundfinish*Block.speed*(time2-time1))));
	glm::mat4 rotateRectanglex = glm::rotate((float)(x_rotate*90*M_PI/180), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
	glm::mat4 rotateRectangley = glm::rotate((float)(-y_rotate*90*M_PI/180), glm::vec3(1,0,0)); // rotate about vector (-1,1,1)
	glm::mat4 rotateRectanglez = glm::rotate((float)(z_rotate*90*M_PI/180), glm::vec3(0,0,1));
	Matrices.model *= (Block.T*translateBlock*rotateRectanglex*rotateRectangley*rotateRectanglez);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(Block.block);
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		//        exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 64);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		glfwTerminate();
		//        exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwSetScrollCallback(window, scroll_callback);

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	/* Objects should be created before any other gl function and shaders */
	// Create the models
	for ( int i = 0 ; i < 140 ; i++ )
	{
		Tile[i].tile = createBoard(0.5,0.5,0.2,0,0,0,Level1[(10*(level-1))+(i/14)][i%14]);
		Tile[i].T = glm::translate(glm::vec3(0.52*((i%14)-7),-0.52*((i/14)-5),0));
	}
	createRectangle(1,0.5,0.5);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	Block.z = 0.5;
	Block.state = stand;
	Block.fallingstatus = false;
	Block.speed = 7;
	Block.fragilefall = false;
	xi = 3;
	yi = 3;
	ResetBlock();

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

	double last_update_time = glfwGetTime(), current_time;



	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {

		// OpenGL Draw commands
		draw();

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 2) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
	}

	glfwTerminate();
	//    exit(EXIT_SUCCESS);
}
