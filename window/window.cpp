#include "window.h"
#include <filesystem>
#include <thread>
#include <unordered_set>
#include <cmath>
using namespace std;

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

	diffusion();

	addVection2();

	mapDensityToPx();

	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glActiveTexture(GL_TEXTURE0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.data());
	glBindVertexArray(this->vao);


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

	this->allPixelInfo.resize(this->height * this->width, pixelInfo{});

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
	return x0 + constant * (x1 + x2 + y1 + y2 - 4 * x0);
}



void window::diffusion() {
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;

	for (int y = 2; y < this->height - 2; ++y) {
		for (int x = 2; x < this->width - 2; ++x) {
			int i = y * this->width + x;

			newAllPixelInfo[i].density = approxTheDiff(
				this->allPixelInfo[i].density,
				this->allPixelInfo[i - 1].density,
				this->allPixelInfo[i + 1].density,
				this->allPixelInfo[i - this->width].density,
				this->allPixelInfo[i + this->width].density,
				this->constantOfViscosity
			);
		}
	}

	this->allPixelInfo = std::move(newAllPixelInfo);
}


void window::addVection2() {
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;

	for (int x = 1; x < this->width - 1; x++) {
		for (int y = 1; y < this->height - 1; y++) {
			int i = y * this->width + x;

			float backX = x - this->allPixelInfo[i].velocity.x;
			float backY = y - this->allPixelInfo[i].velocity.y;

			if (backX < 0.5f) backX = 0.5f;
			if (backX > this->width - 0.5f) backX = this->width - 0.5f;
			if (backY < 0.5f) backY = 0.5f;
			if (backY > this->height - 0.5f) backY = this->height - 0.5f;

			int i0 = static_cast<int>(backX);
			int i1 = i0 + 1;
			int j0 = static_cast<int>(backY);
			int j1 = j0 + 1;

			float s1 = backX - i0;
			float s0 = 1.0f - s1;
			float t1 = backY - j0;
			float t0 = 1.0f - t1;

			newAllPixelInfo[i].density =
				s0 * (t0 * this->allPixelInfo[j0 * this->width + i0].density +
					t1 * this->allPixelInfo[j1 * this->width + i0].density) +
				s1 * (t0 * this->allPixelInfo[j0 * this->width + i1].density +
					t1 * this->allPixelInfo[j1 * this->width + i1].density);
		}
	}
	double xPos, yPos;
	glfwGetCursorPos(this->windowInstance, &xPos, &yPos);
	int centerX = static_cast<int>(xPos);
	int centerY = static_cast<int>(yPos);

	vec2 mouseVelocity = {
		static_cast<float>(mousePos.x - xPos),
		static_cast<float>(mousePos.y - yPos)
	};

	for (int y = -this->halfSize; y <= this->halfSize; ++y) {
		for (int x = -this->halfSize; x <= this->halfSize; ++x) {
			int px = centerX + x;
			int py = centerY + y;

			if (px < 0 || px >= this->width || py < 0 || py >= this->height)
				continue;

			int index = py * this->width + px;
			newAllPixelInfo[index].velocity.x += mouseVelocity.x;
			newAllPixelInfo[index].velocity.y += mouseVelocity.y;

			newAllPixelInfo[index].density += 10;
		}
	}
	std::cout << mouseVelocity.x << std::endl;
	std::cout << mouseVelocity.y << std::endl;

	this->mousePos.x = static_cast<float>(xPos);
	this->mousePos.y = static_cast<float>(yPos);

	this->allPixelInfo = std::move(newAllPixelInfo);

}


void window::solveHelmholtzEquation() {
	const int imageWidth = this->width;
	const int imageHeight = this->height;
	const float k = 1.0f; 

	std::vector<float> laplacianKernel = {
		0,  1,  0,
		1, -4,  1,
		0,  1,  0
	};

	std::vector<float> inputDensity(imageWidth * imageHeight, 0.0f);

	for (int y = 0; y < imageHeight; ++y) {
		for (int x = 0; x < imageWidth; ++x) {
			int index = y * imageWidth + x;
			if (index < allPixelInfo.size())
				inputDensity[index] = allPixelInfo[index].density;
		}
	}

	std::vector<float> laplacianResult(imageWidth * imageHeight, 0.0f);

	for (int y = 1; y < imageHeight - 1; ++y) {
		for (int x = 1; x < imageWidth - 1; ++x) {
			float sum = 0.0f;
			for (int ky = -1; ky <= 1; ++ky) {
				for (int kx = -1; kx <= 1; ++kx) {
					int ix = x + kx;
					int iy = y + ky;
					int imageIndex = iy * imageWidth + ix;
					int kernelIndex = (ky + 1) * 3 + (kx + 1);
					sum += inputDensity[imageIndex] * laplacianKernel[kernelIndex];
				}
			}
			laplacianResult[y * imageWidth + x] = sum;
		}
	}

	std::vector<float> result(imageWidth * imageHeight, 0.0f);
	for (int i = 0; i < result.size(); ++i) {
		result[i] = laplacianResult[i] - k * k * inputDensity[i];
	}

	for (int i = 0; i < result.size() && i < allPixelInfo.size(); ++i) {
		allPixelInfo[i].density = result[i];
	}
}


void window::mapDensityToPx() {
	for (int x = 0; x < this->width * this->height; ++x) {

		float pixelDen = this->allPixelInfo[x].density;
		unsigned char pixelRGB[4]{};
		this->pixels [4 * x] = 1 / (1 + std::exp((-pixelDen + 5) / 2)) * 255;
		this->pixels[4 * x + 1] = 1 / (1 + std::exp(pixelDen / 10)) * 255;
		this->pixels[4 * x + 2] = 1 / (1 + std::exp(pixelDen / 2)) * 255;
		this->pixels[4 * x + 3] = 255;
	}
	this->flipImageVertically(this->width, this->height);
	return;
}


void window::flipImageVertically(int width, int height) {
	const int rowSize = width * 4;
	unsigned char* rowBuffer = new unsigned char[rowSize]; 

	for (int y = 0; y < height / 2; ++y) {
		unsigned char* topRow = &this->pixels[y * rowSize];
		unsigned char* bottomRow = &this->pixels[(height - 1 - y) * rowSize];
		memcpy(rowBuffer, topRow, rowSize);
		memcpy(topRow, bottomRow, rowSize);
		memcpy(bottomRow, rowBuffer, rowSize);
	}

	delete[] rowBuffer;
}



float window::accessPixel(accessPixelEnum pixelDirection, uint16_t referencePixel, pixelInfoEnum accessValue) {
	int ifVerticalBorderTop = referencePixel / this->width;
	int ifVerticalBorderBottom = referencePixel / this->width;

	switch (pixelDirection) {
	case window::up:
		if (referencePixel - this->width < 0) {
			return -1;
		}

		referencePixel = referencePixel - this->width;
		
		break;
	
	case window::left:
		if (referencePixel % this->width == 1) {
			return -1;
		}

		referencePixel = referencePixel - 1;

		break;
	
	case window::right:
		if (referencePixel % this->width == 0) {
			return -1;
		}

		referencePixel = referencePixel + 1;
		break;
	
	case window::down:
		if ((referencePixel / this->width) > (this->height - 1)) {
			return -1;
		}
		referencePixel = referencePixel + this->width;
		break;
	
	default:
		return -1;
		break;
	}


	switch (accessValue) {
	case window::density:
		return this->allPixelInfo[referencePixel].density;
		break;
	case window::velocityX:
		return this->allPixelInfo[referencePixel].velocity.x;
		break;
	case window::velocityY:
		return this->allPixelInfo[referencePixel].velocity.y;
		break;
	default:
		break;
	}

}