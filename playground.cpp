#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Dylan LE FLOUR", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// White background
	// glClearColor(255.0f, 255.0f, 255.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Create and compile our GLSL program from the shaders
	// GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");	
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Projection matrix : 45 degrees Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0) // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;
	// Remember, matrix multiplication is the other way around

	static const GLfloat g_vertex_buffer_data[] = {
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f,  1.5f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, 0.1f,
		0.0f, 1.0f, 0.1f,
		1.0f, -1.0f, 0.1f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// One color for each vertex.
	static const GLfloat g_color_buffer_data[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	static const GLfloat g_normal_buffer_data[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data), g_normal_buffer_data, GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	
	glm::vec3 lightPos = glm::vec3(4,4,1);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	float deltaTime;
	float lastFrame = 0;
	float angle = 0;

	do{
		// Clear the screen
		// glClear( GL_COLOR_BUFFER_BIT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0, // attribute 0.
			// No particular reason for 0, but must match the layout in the shader.
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1, // attribute 1.
			// No particular reason for 1, but must match the layout in the shader.
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2, // attribute 2.
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
		);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		angle += 3.14159f/2.0f * deltaTime;

		float c = (float) cos(angle);
		float s = (float) sin(angle);
		// Model matrix : an identity matrix (model will be at the origin)
		// Model = glm::mat4(1.0f);; // A bit to the right
		Model = { c ,0.0f,s, 0.0f,
		0.0f,1.0f,0.0f, 0.0f,
		-s ,0.0f,c , 0.0f,
		0.0f,0.0f,0.0f, 1.0f};//glm::mat4(1.0f);*/

		MVP = Projection * View * Model;
		// Remember, matrix multiplication is the other way around

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 9); // 3 indices starting at 0 -> 1 triangle

		// glDrawArrays(GL_LINES, 0, 10); // 10 indices starting at 0 -> 5 lines
		// glDrawArrays(GL_LINE_STRIP, 0, 10); // 10 indices starting at 0 -> 5 lines
		// glDrawArrays(GL_POINTS, 0, 10); // 10 indices starting at 0 -> 10 points

		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glm::mat4 RotationModel = glm::mat4(0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);

		Model = Model * RotationModel;

		MVP = Projection * View * Model;
		
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		glDrawArrays(GL_TRIANGLES, 0, 9);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

