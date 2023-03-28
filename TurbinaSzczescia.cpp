#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#undef main
#define PI 3.14159265f
//Defining shaders
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";
//oifsofasoimfa

//Creating variables for graph functioning
const int sample_size = 32000;
const int max_speed = 6400;
struct points_in_graph
{
	GLfloat x;
	GLfloat y;
};
points_in_graph graph[max_speed][sample_size];
void set_points_in_graph(float petals, float number_of_images)
{
	float degree = 0; // value of which thetta will be shifted
	float degree_increase_per_iter = 6.28318531f / number_of_images; // speed
	float thetta = 0.0f;//required to convert Cartesian cords into polar cords
	float polar_r;// as above
	float rad = 6.28318531 / sample_size;
	for (int i = 0; i < number_of_images; i++)
	{
		for (int j = 0; j < sample_size; j++)
		{
			thetta += rad;
			polar_r = sin(petals * thetta);
			graph[i][j].x = polar_r * cos(thetta + degree);
			graph[i][j].y = polar_r * sin(thetta + degree);
		}
		degree += degree_increase_per_iter;
	}
}

void sort_graph_by_nearest(std::vector <points_in_graph> temp_graph)
{
	float shortest_distance_to_point = 100.0f;
	float distances_between_points;
	int index_of_closest;
	points_in_graph temp_point; //used to swap points
	for (int i = 0; i + 1 < temp_graph.size(); i++)
	{
		index_of_closest = i + 1;
		for (int j = i + 1; j < temp_graph.size(); j++)
		{
			distances_between_points = hypot(temp_graph[j].x - temp_graph[i].x, temp_graph[j].y - temp_graph[i].y);
			if (distances_between_points < shortest_distance_to_point)
			{
				shortest_distance_to_point = distances_between_points;
				index_of_closest = j;
			}
		}
		temp_point = temp_graph[i + 1];
		temp_graph[i + 1] = temp_graph[index_of_closest];
		temp_graph[index_of_closest] = temp_point;
		//std::cout << temp_graph[i].x << " " << temp_graph[i].y << "\n";
		shortest_distance_to_point = 100.0f;
	}
}

bool aliasing_requested = false;
void start_aliasing(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		std::cout << "A";
		aliasing_requested = true;
	}
}

void aliasing(GLFWwindow* window, int picture_number, int number_of_lines, int number_of_images)
{
	std::cout << "\n" << "aliasing started";
	std::vector <points_in_graph> aliased;

	int iterator_in_graph; // will be increased as the loop goes on
	float upper_boundry = 1.0f;
	float decrease_pixels = 2.0f / number_of_lines;
	float lower_boundry = upper_boundry - decrease_pixels;
	for (int i = 0; i < number_of_lines; i++) // 16 is the number of lines
	{
		for (int j = 0; j < sample_size; j++)
		{
			if (graph[picture_number][j].y >= lower_boundry && graph[picture_number][j].y <= upper_boundry)
			{
				aliased.push_back(graph[picture_number][j]);
			}
		}
		picture_number++;

		upper_boundry = lower_boundry;
		lower_boundry = upper_boundry - decrease_pixels;

		if (picture_number == number_of_images)
		{
			picture_number = 0;
		}
	}
	points_in_graph aliased_array[100000];
	int array_size = aliased.size();
	//sort_graph_by_nearest(aliased);
	for (int i = 0; i < array_size; i++)
	{
		aliased_array[i] = aliased[i];
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	GLuint VAO, VBO;
	while (!glfwWindowShouldClose(window))
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof aliased_array, aliased_array, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glPointSize(2);
		glLineWidth(2);
		//glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, array_size);
		glDrawArrays(GL_POINTS, 0, array_size);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	aliasing_requested = false;
}

int main()
{
	// Initialize GLFW
	glfwInit();

	/*std::cout << "Enter width and height of window ";
	int width;
	std::cin >> width;*/
	// Tell GLFW what version of OpenGL we are using
	// In this case we are using OpenGL 3.3

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(512, 512, "SMIGLO", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Loads data into graph TBC later change into main loop

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 512, 512);

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	GLuint shaderProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(shaderProgram);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Vertices coordinates

	// Create reference containers for the Vertex Array Object and the Vertex Buffer Object
	GLuint VAO, VBO;
	float petals;
	int number_of_images, number_of_lines;
	// Generate the VAO and VBO with only 1 object each
	std::cout << "Wybierz ilosc smigiel, ilosc obrazow, ilosc lini skanera " << "\n";
	std::cin >> petals >> number_of_images >> number_of_lines;
	if ((int)petals % 2 == 0 && petals != 2)
	{
		petals /= 2;
	}

	//Debug delete later
	float degree = 0.0f; // for each loop iterationit will incrase by 0.098, after reaching 6,28 it will go back to 0
	// Main while loop
	int iterator = 0;
	set_points_in_graph(petals, number_of_images);
	glfwSetKeyCallback(window, start_aliasing);
	while (!glfwWindowShouldClose(window))
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// Make the VAO the current Vertex Array Object by binding it
		glBindVertexArray(VAO);

		// Bind the VBO specifying it's a GL_ARRAY_BUFFER
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// Loading graph into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof graph[iterator], graph[iterator], GL_DYNAMIC_DRAW);
		// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
		// Enable the Vertex Attribute so that OpenGL knows to use it
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (iterator == number_of_images - 1) {
			iterator = 0;
		}
		// Specify the color of the background
		glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		glUseProgram(shaderProgram);
		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		glPointSize(5.0);
		glDrawArrays(GL_LINE_STRIP, 0, sample_size);
		iterator++;
		// Draws graph
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
		if (aliasing_requested)
		{
			aliasing(window, iterator, number_of_lines, number_of_images);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}