#include "window.h"

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
	
	// creates a shader instance responseable for holding the shaders are displaying the infomation.
	unsigned int shaderProgram = glCreateProgram();

	// create the vertex shader source to be used as the vertex shader ( refer to openGL documentation under "vertex shader" )
	const GLchar* vertShaderSource = readFile("../shaders/vertex.glsl").c_str();
	
	// creates a vertex shader address for referencing
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	// adds the vertex shader source code to the referencing address of the vertex shader 
	glShaderSource(vertexShader, 1, &vertShaderSource, NULL);
	
	// compiles it to useable infomation
	glCompileShader(vertexShader);

	// attaches the shader to the actual program
	glAttachShader(shaderProgram, vertexShader);

	// sources the shader
	const GLchar* fragShaderSource = readFile("../shaders/fragment.glsl").c_str();

	// creates a shader on the memory
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// assigns the shader propeties
	glShaderSource(fragShader, 1, &vertShaderSource, NULL);

	// deleting shaders to free memory ( minor peformance hit but good for future changes that may include a large shader"
	glDeleteShader(vertexShader);

	// adds the shader to the instance of the shader program  
	glAttachShader(shaderProgram, vertexShader);

}

void window::framebuffer_size_callback(GLFWwindow* windowInstance, int width, int height) {
	glViewport(0, 0, width, height);
}

void window::render() {
	this->processInputMethod(this->windowInstance);
	
	glfwSwapBuffers(this->windowInstance);
	glfwPollEvents();
}

void window::processInputMethod(GLFWwindow* windowInstance) {


}

const std::string window::readFile( const std::string& fileAddress) {

	std::ifstream givenFile(fileAddress);

	std::stringstream ss{};

	ss << givenFile.rdbuf();

	givenFile.close();

	return ss.str();

}