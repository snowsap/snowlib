#include "window.h"
#include <filesystem>

/**
 * @param width Width of the window.
 * @param height Height of the window.
 * @param windowName Name of the window.
 * @param resizeable If the user can change the window size.
 * @param visible If the window is visible to the user.
 * @param doFloat Should the window be able to be put behind others.
 * @param fullScreen If the window should be on full screen (recommended to have resizeToMonitor set to true if this is true).
 * @param resizeToMonitor Resizes the window to the dimensions of the monitor displaying it.
 * @param windowDecorated If the window should have borders, an exit button, etc. (For borderless fullscreen: set fullScreen = false, resizeToMonitor = true, windowDecorated = false, resizeable = false).
 * @param displayNumber The display number where the window should be shown.
 */
window::window(
int width /* width of the window */ ,
int height /* height of the window */,
std::string windowName /* name of the window */,
bool resizeable /* if the user can change the window size */,
bool visible/* if the window is visible to the user */,
bool doFloat/* should the window be able to be put behind others */,
bool fullScreen/* if the window should be on full screen recommended to have resize to monitor if true*/,
bool resizeToMonitor/* resizes the window to the dimensions of the monitor displaying it */,
bool windowDecorated /* if the window should have exit sides exit button ect. to achieve borderless fullscreen (fullscreen false, resizeToMonitor true, decorated false, resizeable false) */,
int displayNumber  /* what display to put the window */) {
	this->width = width;
	this->height = height;
	this->windowName = windowName;
	this->resizeable = resizeable;
	this->visible = visible;
	this->doFloat = doFloat;
	this->fullScreen = fullScreen;
	this->resizeToMonitor = resizeToMonitor;
	this->windowDecorated = windowDecorated;
	this->displayNumber = displayNumber;

	// creates the window instance
	glfwInit();

	// settings as described in function arguements 
	glfwWindowHint(GLFW_RESIZABLE, resizeable);
	glfwWindowHint(GLFW_VISIBLE, visible);
	glfwWindowHint(GLFW_MAXIMIZED, fullScreen);
	glfwWindowHint(GLFW_FLOATING, doFloat);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, resizeToMonitor);
	glfwWindowHint(GLFW_DECORATED, windowDecorated);
	
	// assigned version of the Content
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	// declare that that glfw is using openGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// forward compatibility 
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// MAC compatibility
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// creates an instance of the window 
	this->windowInstance = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);

	// error checks
	if (this->windowInstance == NULL) {
		std::cout << "failed glfw window creation" << std::endl;
		glfwTerminate();
		return;
	}

	// other glfw setup for the window 
	glfwMakeContextCurrent(this->windowInstance);
	glfwSetFramebufferSizeCallback(this->windowInstance, this->framebuffer_size_callback);


	// more error checks 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	
	screenCover();

	initTestPixels();

}

void window::framebuffer_size_callback(GLFWwindow* windowInstance, int width, int height) {}

void window::renderScreen() {
	
	this->processInputMethod(this->windowInstance);

	glUseProgram(this->shaderProgram);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glBindVertexArray(this->vao);

	glfwSwapBuffers(this->windowInstance);
	glfwPollEvents();



}

void window::screenCover() {

	this->shaderProgram = glCreateProgram();


	std::string fragShaderSource = this->readFile("shaders/fragment.glsl");
	std::string vertShaderSource = this->readFile("shaders/vertex.glsl");

	unsigned int vertexShader = this->createShader(vertShaderSource, GL_VERTEX_SHADER, this->shaderProgram);
	unsigned int fragmentShader = this->createShader(fragShaderSource, GL_FRAGMENT_SHADER, this->shaderProgram);

	glAttachShader(this->shaderProgram, vertexShader);
	glAttachShader(this->shaderProgram, fragmentShader);

	float screenCover[]{
		-1.0f, -1.0f,
		0.0f, -1.0f,
		-1.0f, 0.0f,
		1.0f, 1.0f
	};

	unsigned int screenCoverIndex[]{
		1, 2, 3,
		4, 2, 3
	};

	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	unsigned int screenCoverBuffer;
	glGenBuffers(1, &screenCoverBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, screenCoverBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCover), screenCover, GL_STATIC_DRAW);

	unsigned int screenCoverIndexBuffer;
	glGenBuffers(1, &screenCoverIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenCoverIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenCoverIndex), screenCoverIndex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(0);

	glValidateProgram(this->shaderProgram);
}

void window::processInputMethod(GLFWwindow* windowInstance) {


}

std::string window::readFile(const std::string& fileAddress) {

	std::ifstream givenFile(fileAddress);

	std::stringstream ss{};

	ss << givenFile.rdbuf();

	givenFile.close();

	return ss.str();

}

unsigned int window::createShader(const std::string& shaderText, unsigned int shaderType, unsigned int shaderProgram) {
	unsigned int shaderID = glCreateShader(shaderType);
	const char* sourceString = shaderText.c_str();
	glShaderSource(shaderID, 1, &sourceString, NULL);
	glCompileShader(shaderID);

	int success;
	char infoLog[1024];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog<< std::endl;
	}
	return shaderID;
}

int window::changeTheDimensions(int width, int height) {
	
	if (width == -1) {
		width = this->width;
	}
	if (height == -1) {
		height = this->height;
	}
	glfwSetWindowSize(this->windowInstance, width, height);
	return 0;
}

void window::initTestPixels() {

	float blackColor[]{
	0.0f, 0.0f, 0.0f, 1.0f
	};


	for (int x = 0; x < this->height; ++x) {
		for (int y = 0; y < this->height; ++y) {
			this->pixels[x][y] = blackColor;
		}
	}

	float nonBlackColor[]{
		0.5f, 0.5f, 0.5f, 0.5f
	};


	this->pixels[100][100] = nonBlackColor;
	return;
}