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


}

void window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void window::renderScreen() {
	
	processInputMethod(this->windowInstance);

	glUseProgram(this->shaderProgram);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "textureSampler"), 0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glBindVertexArray(this->vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(this->windowInstance);
	glfwPollEvents();


}

void window::screenCover() {

	this->shaderProgram = glCreateProgram();
	auto vs = createShader(readFile("shaders/vertex.glsl"), GL_VERTEX_SHADER, shaderProgram);
	auto fs = createShader(readFile("shaders/fragment.glsl"), GL_FRAGMENT_SHADER, shaderProgram);
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	glValidateProgram(shaderProgram);
	glUseProgram(shaderProgram);


	float verts[] = {
		-1, -1,     0,   0,
		 1, -1,     1,   0,
		-1,  1,     0,   1,
		 1,  1,     1,   1,
	};
	unsigned int idx[] = { 0,1,2,  3,1,2 };

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	initTestPixels();  
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	float borderColor[] = { 0,0,1,1 };

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	
	glBindVertexArray(0);
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

	unsigned char blackColor[]{
	0, 255, 255, 255
	};

	std::vector<unsigned char> allPixels(this->height * this->width * 4);

	this->pixels.resize(this->width * this->height * 4);
	for (int x = 0; x < this->width * this->height * 4; ++x) {
		this->pixels[x] = blackColor[x % 4];
	}

	unsigned char nonBlackColor[]{
		255, 255, 255, 255
	};	

	this->pixels[100] = nonBlackColor[0];
	this->pixels[101] = nonBlackColor[1];
	this->pixels[102] = nonBlackColor[2];
	this->pixels[103] = nonBlackColor[3];




	return;
}
