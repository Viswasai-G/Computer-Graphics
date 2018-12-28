//----------------------------------------------------------------------------
// Homework 1- CISC 640
// Viswasai Gollamudi, tejaswi@cis.udel.edu
// Spring - 2018
//----------------------------------------------------------------------------

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<math.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include<glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

//----------------------------------------------------------------------------

// these along with Model matrix make MVP transform

glm::mat4 Projection;
glm::mat4 View;

// some globals necessary to get information to shaders

GLuint MatrixID;
GLuint vertexbuffer;
GLuint colorbuffer;

void draw_pyramid(glm::mat4, float, float, float);
void draw_pattern_1(glm::mat4 Model, float r, float g, float b);
void draw_pattern_2(glm::mat4 Model);
void draw_pyramidoid(glm::mat4 Model, float r, float g, float b);
void delay(float secs);

//----------------------------------------------------------------------------

//  Function to Delay Time. Used to Change Angle of Camera with a time delay.Used just for changing camera angle only)
void delay(float secs)
{
	float end = clock() / CLOCKS_PER_SEC + secs;
	while ((clock() / CLOCKS_PER_SEC) < end);
}

// Use the 3D model to create a spiral model. 
void draw_pattern_1(glm::mat4 Model, float r, float g, float b)
{
	int y = 0;
	float i = 0.0; 
		for (float theta = 0; theta < 6.32; theta = theta + 0.1) {// Use the loop to change the angle of trajectory of spiral
			float x = 40 * (theta)*cos(theta); float z = 40 * (theta)*sin(theta);//Changing co-ordinates x, y, z to create the trajectory of a spiral
				glm::mat4 DRZ = glm::rotate((float)(i), glm::vec3(0.0f, 0.0f, 1.0f));// Rotate the object with respect to Z axis
				draw_pyramidoid(Model *glm::scale(glm::vec3( 1.0+ 0.2*i, 1.0 + 0.2*i, 1.0 + 0.2*i))* glm::translate(glm::vec3(x, y, z))* DRZ, r*x*0.15, g*tan(theta)*0.63, b*z*0.25);
			y = y + 2;  //Increase the value of y to make the spiral 3 Dimensional 
			i = i + 0.5;  //Increase the angle each 3D object in each spiral to rotate
		}
}

void draw_pattern_2(glm::mat4 Model)
{
	for (float i = 0; i < 6.20; i = i + 0.57) {     //Using a Loop to create Multiple Spiral structures of 3D objects rotated w.r.t Y-axis
		glm::mat4 DRY = glm::rotate((float)(i),
			glm::vec3(0.0f,	1.0f, 0.0f));
		draw_pattern_1(Model * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * DRY, 0.4, 0.6, 0.2);
		for (int j = 0; j < 15; j = j + 1) {
			glm::mat4 RZ = glm::rotate((float)(0.17),   // Rotate w.r.t Z-axis by 90  degrees
				glm::vec3(1.0f,
					0.0f,
					0.0f));
			draw_pattern_1(Model * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * DRY, 0.4, 0.6, 0.2f);
		}
	}
}

// Using the Pyramid to create a 6-Pyramid Sided polygon
void draw_pyramidoid(glm::mat4 Model, float r, float g, float b)
{
	// Rotate w.r.t Z-axis by 180 degrees
	glm::mat4 DRZ = glm::rotate((float)(3.1416),
		glm::vec3(0.0f,
			0.0f,
			1.0f));

	// +Z, -Z
	draw_pyramid(Model * glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)), r, g, b);
	draw_pyramid(Model * glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)) * DRZ, 0.5*r, 0.5*g, 0.5*b);

	// +X, -X
	glm::mat4 RZ = glm::rotate((float)(1.571),   // Rotate w.r.t Z-axis by 90  degrees
		glm::vec3(0.0f,
			0.0f,
			1.0f));
	glm::mat4 NRZ = glm::rotate((float)(4.712),  // Rotate w.r.t Z-axis 270 degrees
		glm::vec3(0.0f,
			0.0f,
			1.0f));

	draw_pyramid(Model * glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)) * RZ, g, b, r);
	draw_pyramid(Model * glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)) * NRZ, 0.7*g, 0.5*b, 0.5*r);

	// +Y, -Y

	glm::mat4 RX = glm::rotate((float)(4.712),     //Rotate w.r.t X-axis by 270 degrees
		glm::vec3(1.0f,
			0.0f,
			0.0f));
	glm::mat4 NRX = glm::rotate((float)(1.571),      // Rotate w.r.t X-axis by 90 degrees
		glm::vec3(1.0f,
			0.0f,
			0.0f));


	draw_pyramid(Model * glm::translate(glm::vec3(0.0f, 0.0f, 2.0f)) * NRX, b, r, g);
	draw_pyramid(Model * glm::translate(glm::vec3(0.0f, 0.0f, -2.0f))* RX, 0.5*b, 0.5*r, 0.5*g);

}


//----------------------------------------------------------------------------

// Draw Pyramid with particular modeling transformation and color (r, g, b) (in range [0, 1])
// Refers to globals in section above (but does not change them)

void draw_pyramid(glm::mat4 Model, float r, float g, float b)
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	// make this transform available to shaders  
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0,                  // attribute. 0 to match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Different Colors for each Vertex to give a shading effect and a different color to each side

	GLfloat g_color_buffer_data[] = {
		r, g, b,
		r, g, b,
		0.2*r, 0.2*g, 0.2*b,
		
		0.2*r, 0.2*g, 0.2*b,
		0.3*r, 0.4*g, 0.4*b,
		r, g, b,

		0.3*r, 0.4*g, 0.4*b,
		0.3*r, 0.4*g, 0.4*b,
		0.2*r, 0.2*g, 0.2*b,
		
		0.2*r, 0.2*g, 0.2*b,
		0.3*r, 0.4*g, 0.4*b,
		0.2*r, 0.2*g, 0.2*b,
	};

	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1,                                // attribute. 1 to match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw 4  triangles to create a pyramid !
	glDrawArrays(GL_TRIANGLES, 0, 12); // 12 indices of 4 triangles

	glDisableVertexAttribArray(0);
	
}

//----------------------------------------------------------------------------

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Darker blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("MultiColorSimpleTransform.vertexshader", "MultiColor.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 90° Field of View, 16:9 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	
	// Or, for an ortho camera :
	//Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f); // In camera coordinates(Use for Pyramidoid only)
	//Projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f); // In camera coordinates(Use for Pattern)
	

	// Camera matrix 

	View = glm::lookAt(glm::vec3(30, 30, 30), // Camera is at (20,35,10), in World Space
		glm::vec3(0, 3, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//Change lookat() function to get Elevaion View
	//View = glm::lookAt(glm::vec3(30, 0, 30), // Camera is at (20,35,10), in World Space
		//glm::vec3(0, 3, 0), // and looks at the origin
		//glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	//);
	// geometry of pyramid, giving the 12 vertices of the 4 triangles needed to form a pyramid

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		0.0f,  0.732f, 0.0f,

		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		0.0f,  0.732f, 0.0f,

		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		0.0f,  0.732f, 0.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		0.0f,  0.732f, 0.0f,

	};

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// handle for color information.  we don't set it here because it can change for each triangle

	glGenBuffers(1, &colorbuffer);

	// Model matrix -- changed for each triangle drawn
	glm::mat4 Model;

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		//Draw the 1 object of pyramidoid shape.
		//draw_pyramidoid(glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)), 0.47,0.35,0.67);
		//draw_pattern_1(glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)), 0.47, 0.35, 0.67);
		//Draw a spiral pattern evolving from the origin
		draw_pattern_2(glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)));

		
			// Create a delay to stay in the first camera co-ordinates for a while
			//delay(5);
			// Changing Camera co-ordinates to another location to have a different view
			//View = glm::lookAt(glm::vec3(0, 35, 1), // Camera is at (0, 35, 1), in World Space
			//	glm::vec3(0, 0, 0), // and looks at the origin
			//	glm::vec3(0, 1, 0));  // Head is up (set to 0,-1,0 to look upside-down)

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
