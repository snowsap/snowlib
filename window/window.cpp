#include "window.h"
#include <filesystem>
#include <thread>


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

int window::dotProduct(vec2 vector1, vec2 vector2) {
	int result = vector1.x * vector2.x + vector1.y * vector2.y;
	return result;
}

void window::renderScreen() {
	
	processInputMethod(this->windowInstance);

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = currentTime - previousTime;

	glUseProgram(this->shaderProgram);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "textureSampler"), 0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glBindVertexArray(this->vao);
	diffusion();
	addVection();
	computeCurl2D();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(this->windowInstance);
	glfwPollEvents();

	auto sleepTime = this->frameDuration - elapsedTime;
	if (sleepTime > std::chrono::milliseconds(0)) {
		std::this_thread::sleep_for(sleepTime);
	}

	this->previousTime = std::chrono::high_resolution_clock::now();

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

	this->allPixelInfo.resize(this->height * this->width);

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

float window::approxTheDiff(float x0, float x1, float x2, float y1, float y2, float constant) {
	float answer;
	answer = (x0 + constant * ( ( x1 + x2 + y1 + y2 ) / (4) ) ) / (1 + constant);
	return answer;
}

void window::diffusion() {
	std::vector<pixelInfo> newAllPixelInfo(this->height * this->width);  

	for (int i = 1; i < this->width * this->height - 1; ++i) {  
		for (int x = 0; x < 4; ++x) {
			float left = (i % this->width > 0) ? newAllPixelInfo[i - 1].density : this->allPixelInfo[i].density;
			float right = (i % this->width < this->width - 1) ? newAllPixelInfo[i + 1].density : this->allPixelInfo[i].density;
			float top = (i / this->width > 0) ? newAllPixelInfo[i - this->width].density : this->allPixelInfo[i].density;
			float bottom = (i / this->width < this->height - 1) ? newAllPixelInfo[i + this->width].density : this->allPixelInfo[i].density;

			newAllPixelInfo[i].density = approxTheDiff(this->allPixelInfo[i].density, left, right, top, bottom, this->constantOfViscosity);
		}
	}

	this->allPixelInfo = newAllPixelInfo;  
}

void window::addVection() {

	std::vector<pixelInfo> newAllPixelInfo{ pixelInfo{} };
	newAllPixelInfo.resize(this->height * this->width);

	for (int i = 0; i < this->width * this->height; ++i) {

		float directionX = this->allPixelInfo[i].velocity.x;
		float directionY = this->allPixelInfo[i].velocity.y;

		float currentPosF = (float)i;
		currentPosF -= directionX;
		currentPosF -= (directionY * this->width);

		if (currentPosF >= 0 && currentPosF < this->width * this->height - this->width - 1) {
			vec2 closestWhole = {
				(int(floor(currentPosF)) % this->width),
				(int)floor(currentPosF / this->width)
			};

			vec2 relitiveToWhole = {
				currentPosF - (int)(closestWhole.x + closestWhole.y * this->width) % this->width,
				currentPosF - (closestWhole.x + closestWhole.y * this->width)
			};

			int idx = closestWhole.x + closestWhole.y * this->width;
			int idxRight = idx + 1;
			int idxDown = idx + this->width;
			int idxDownRight = idxDown + 1;

			if (idxDownRight < this->width * this->height) {
				float closestWholeLerp1 = std::lerp(
					this->allPixelInfo[idx].density,
					this->allPixelInfo[idxRight].density,
					relitiveToWhole.x
				);

				float closestWholeLerp2 = std::lerp(
					this->allPixelInfo[idxDown].density,
					this->allPixelInfo[idxDownRight].density,
					relitiveToWhole.x
				);

				float newDensity = std::lerp(closestWholeLerp1, closestWholeLerp2, relitiveToWhole.y);
				newAllPixelInfo[i].density = newDensity;
			}
		}
	}

	this->allPixelInfo = newAllPixelInfo;
}

void window::computeCurl2D() {
	float h = 0.01;
	std::vector<float> curl(this->width * this->height, 0.0);

	for (int x = 1; x < this->width - 1; ++x) {
		for (int y = 1; y < this->height - 1; ++y) {
			int idx = x + y * this->width;
			float dFy_dx = (this->allPixelInfo[(x + 1) + y * this->width].velocity.y -this->allPixelInfo[(x - 1) + y * this->width].velocity.y) / (2 * h);
			float dFx_dy = (this->allPixelInfo[x + (y + 1) * this->width].velocity.x -this->allPixelInfo[x + (y - 1) * this->width].velocity.x) / (2 * h);
			curl[idx] = dFy_dx - dFx_dy;
		}
	}
	for (int i = 0; i < this->width * this->height; ++i) {
		this->allPixelInfo[i].density = curl[i];
	}
}