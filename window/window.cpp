#include "window.h"
#include <filesystem>
#include <thread>
#include <unordered_set>
#include <cmath>
#include <thread>

using namespace std;

/**
 * @param width Width of the window.
 * @param this->height Height of the window.
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
	int width /* width of the window */,
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
	auto elapsedTime = currentTime - this->previousTime;
	this->deltaTime = ((std::chrono::duration_cast<chrono::milliseconds>(elapsedTime)).count());
	deltaTime = deltaTime / 1000;
	this->previousTime = std::chrono::high_resolution_clock::now();


	glUseProgram(this->shaderProgram);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "textureSampler"), 0);


	this->mousePointerAddVelocity();
	this->projectVel();
	this->addVectionVel();
	this->diffusion();
	this->addVection();
	this->mapDensityToPx();


	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);



	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glActiveTexture(GL_TEXTURE0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.data());
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

	this->totalPixelAmount = this->height * this->width;
	this->allPixelInfo.resize(totalPixelAmount, pixelInfo{});

	initTestPixels();
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	float borderColor[] = { 0,0,1,1 };

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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

	std::vector<unsigned char> allPixels(totalPixelAmount * 4);

	this->pixels.resize(totalPixelAmount * 4);
	for (int x = 0; x < totalPixelAmount * 4; ++x) {
		this->pixels[x] = blackColor[x % 4];
	}
	return;
}

float window::approxTheDiff(float x0, float x2, float y0, float y1, float k, float oldTarg) {
	int numberOfVars = 4;
	if (x0 == -1) { numberOfVars = numberOfVars - 1; x0 = 0; }
	if (x2 == -1) { numberOfVars = numberOfVars - 1; x2 = 0; }
	if (y0 == -1) { numberOfVars = numberOfVars - 1; y0 = 0; }
	if (y1 == -1) { numberOfVars = numberOfVars - 1; y1 = 0; }

	return (oldTarg + k * (x0 + x2 + y0 + y1)) / (1 + numberOfVars * k);
}

void window::diffusion() {
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;
	std::vector<pixelInfo> oldPixelInfo = this->allPixelInfo;
	int x0, x1, x2, y0, y1;
	for (int z = 0; z < this->totalPixelAmount; ++z) {
		newAllPixelInfo[z].density = 0;
	}
	for (int a = 0; a < 20; ++a) {
		for (int y = 0; y < this->height; ++y) {
			for (int x = 0; x < this->width; ++x) {

				int i = y * this->width + x;
				newAllPixelInfo[i].density = this->approxTheDiff(
					this->accessPixel(left, i, density, &newAllPixelInfo),
					this->accessPixel(right, i, density, &newAllPixelInfo),
					this->accessPixel(up, i, density, &newAllPixelInfo),
					this->accessPixel(down, i, density, &newAllPixelInfo),
					this->constantOfViscosity * this->deltaTime,
					this->accessPixel(none, i, density, &oldPixelInfo));
				newAllPixelInfo[i].velocity.x = this->approxTheDiff(
					this->accessPixel(left, i, velocityX, &newAllPixelInfo),
					this->accessPixel(right, i, velocityX, &newAllPixelInfo),
					this->accessPixel(up, i, velocityX, &newAllPixelInfo),
					this->accessPixel(down, i, velocityX, &newAllPixelInfo),
					this->constantOfViscosity * this->deltaTime,
					this->accessPixel(none, i, velocityX, &oldPixelInfo));
				newAllPixelInfo[i].velocity.y = this->approxTheDiff(
					this->accessPixel(left, i, velocityY, &newAllPixelInfo),
					this->accessPixel(right, i, velocityY, &newAllPixelInfo),
					this->accessPixel(up, i, velocityY, &newAllPixelInfo),
					this->accessPixel(down, i, velocityY, &newAllPixelInfo),
					this->constantOfViscosity * this->deltaTime,
					this->accessPixel(none, i, velocityY, &oldPixelInfo));
			}
		}
	}
	this->allPixelInfo = std::move(newAllPixelInfo);
	return;
}

void window::mousePointerAddVelocity() {
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;
	double xPos, yPos;
	glfwGetCursorPos(this->windowInstance, &xPos, &yPos);
	int centerX = static_cast<int>(xPos);
	int centerY = static_cast<int>(yPos);

	vec2 mouseVelocity = {
		static_cast<float>(xPos - mousePos.x),
		static_cast<float>(yPos - mousePos.y)
	};
	if (xPos < 30 || xPos > this->width + 30 || yPos < 30 || yPos > this->height - 30) { return; }

	for (int y = -this->halfSize; y <= this->halfSize; ++y) {
		for (int x = -this->halfSize; x <= this->halfSize; ++x) {
			int px = centerX + x;
			int py = centerY + y;

			if ((px < 0) || (px >= this->width) || (py < 0) || (py >= this->height))
				continue;

			int index = py * this->width + px;
			newAllPixelInfo[index].velocity.x += mouseVelocity.x * 2;
			newAllPixelInfo[index].velocity.y += mouseVelocity.y * 2;


			newAllPixelInfo[index].density = 20 + newAllPixelInfo[index].density;
		}
	}
	this->mousePos.x = static_cast<float>(xPos);
	this->mousePos.y = static_cast<float>(yPos);

	this->allPixelInfo = std::move(newAllPixelInfo);
}

// this is a forever approaching 1
float type1Eq(float x, float mag, float newY1) {
	return 1 / (1 + std::exp((-x + newY1) / mag));
}
// this spikes at newY1
float type2Eq(float x, float mag, float newY1) {
	return std::exp(-std::abs((1 / mag) * (x - newY1)));
}

// this is forever approaching 0
float type3Eq(float x, float mag, float newY1) {
	return 1 / (1 + std::exp((x - newY1) / mag));
}

void window::mapDensityToPx() {
	for (int x = 0; x < totalPixelAmount; ++x) {

		float pixelDen = this->allPixelInfo[x].density;
		unsigned char pixelRGB[4]{};
		this->pixels[4 * x] = type2Eq(pixelDen, 5, 3) * 255;
		this->pixels[4 * x + 1] = type2Eq(pixelDen, 20, 20) * 255;
		this->pixels[4 * x + 2] = type2Eq(pixelDen, 20, 50) * 255;
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

float window::accessPixel(accessPixelEnum pixelDirection, int referencePixel, pixelInfoEnum accessValue, std::vector<pixelInfo>* pixelArray, float invReturnValue) {

	switch (pixelDirection) {
	case window::up:
		if (referencePixel - this->width < 0) {
			return invReturnValue;
		}

		referencePixel = referencePixel - this->width;

		break;

	case window::left:
		if (referencePixel % this->width == 0) {
			return invReturnValue;
		}

		referencePixel = referencePixel - 1;

		break;

	case window::right:
		if (referencePixel % this->width == this->width - 1) {
			return invReturnValue;
		}

		referencePixel = referencePixel + 1;
		break;

	case window::down:
		referencePixel = referencePixel + this->width;
		if (referencePixel + this->width >= this->totalPixelAmount - 1) {
			return invReturnValue;
		}
		break;
	case window::none:
		break;

	default:
		return invReturnValue;
		break;
	}


	switch (accessValue) {
	case window::density:
		return (*pixelArray)[referencePixel].density;
		break;
	case window::velocityX:
		return (*pixelArray)[referencePixel].velocity.x;
		break;
	case window::velocityY:
		return (*pixelArray)[referencePixel].velocity.y;
		break;
	default:
		break;
		return invReturnValue;
	}
}

float window::accessPixel(accessPixelEnum pixelDirection, int referencePixel, std::vector<float>* pixelArray, float invReturnValue = -1) {

	switch (pixelDirection) {
	case window::up:
		if (referencePixel - this->width < 0) {
			return invReturnValue;
		}

		referencePixel = referencePixel - this->width;

		break;

	case window::left:
		if (referencePixel % this->width == 0) {
			return invReturnValue;
		}

		referencePixel = referencePixel - 1;

		break;

	case window::right:
		if (referencePixel % this->width == this->width - 1) {
			return invReturnValue;
		}

		referencePixel = referencePixel + 1;
		break;

	case window::down:
		referencePixel = referencePixel + this->width;
		if (referencePixel + this->width >= this->totalPixelAmount - 1) {
			return invReturnValue;
		}
		break;
	case window::none:
		break;

	default:
		return invReturnValue;
		break;
	}

	return (*pixelArray)[referencePixel];
}

void window::addVection() {
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;
	float d, dx, dy, dxy;

	for (int y = 0; y < this->height; ++y) {
		for (int x = 0; x < this->width; ++x) {
			int index = x + y * this->width;

			float xFloat = this->allPixelInfo[index].velocity.x * this->deltaTime;
			float yFloat = this->allPixelInfo[index].velocity.y * this->deltaTime;

			float xBacktrace = x - xFloat;
			float yBacktrace = y - yFloat;

			int xNewPosfloor = std::floor(xBacktrace);
			int yNewPosfloor = std::floor(yBacktrace);

			int xNewPosceil = xNewPosfloor + 1;
			int yNewPosceil = yNewPosfloor + 1;

			if (xNewPosfloor > 1 && xNewPosceil < this->width - 1 && yNewPosfloor > 1 && yNewPosceil < this->height - 1) {

				int newIndexfloor = yNewPosfloor * this->width + xNewPosfloor;
				int newIndexfloorX = yNewPosfloor * this->width + xNewPosceil;
				int newIndexfloorY = yNewPosceil * this->width + xNewPosfloor;
				int newIndexfloorXY = yNewPosceil * this->width + xNewPosceil;

				float relPosx = xBacktrace - xNewPosfloor;
				float relPosy = yBacktrace - yNewPosfloor;

				d = this->allPixelInfo[newIndexfloor].density;
				dx = this->allPixelInfo[newIndexfloorX].density;
				dy = this->allPixelInfo[newIndexfloorY].density;
				dxy = this->allPixelInfo[newIndexfloorXY].density;


				float lerp1Val = std::lerp(this->allPixelInfo[newIndexfloor].density, this->allPixelInfo[newIndexfloorX].density, relPosx);
				float lerp2Val = std::lerp(this->allPixelInfo[newIndexfloorY].density, this->allPixelInfo[newIndexfloorXY].density, relPosx);
				float lerp3Val = std::lerp(lerp1Val, lerp2Val, relPosy);

				newAllPixelInfo[index].density = lerp3Val * this->energyLost;

			}
		}
	}

	this->allPixelInfo = std::move(newAllPixelInfo);
}



void window::addVectionVel() {
	
	std::vector<pixelInfo> newAllPixelInfo = this->allPixelInfo;
	
	float i, ix, iy, ixy, j, jx, jy, jxy;

	for (int x = 0; x < this->width; ++x) {
		for (int y = 0; y < this->height; ++y) {

			int index = x + y * this->width;

			float xFloat = this->allPixelInfo[index].velocity.x * this->deltaTime;
			float yFloat = this->allPixelInfo[index].velocity.y * this->deltaTime;

			float xBacktrace = x - xFloat;
			float yBacktrace = y - yFloat;

			int xNewPosfloor = std::floor(xBacktrace);
			int yNewPosfloor = std::floor(yBacktrace);

			int xNewPosceil = xNewPosfloor + 1;
			int yNewPosceil = yNewPosfloor + 1;

			if (xNewPosfloor > 1 && xNewPosceil < this->width - 1 && yNewPosfloor > 1 && yNewPosceil < this->height - 1) {

				float relPosx = xBacktrace - xNewPosfloor;
				float relPosy = yBacktrace - yNewPosfloor;

				int newIndexfloor = yNewPosfloor * this->width + xNewPosfloor;
				int newIndexfloorX = yNewPosfloor * this->width + xNewPosceil;
				int newIndexfloorY = yNewPosceil * this->width + xNewPosfloor;
				int newIndexfloorXY = yNewPosceil * this->width + xNewPosceil;

				i = this->allPixelInfo[newIndexfloor].velocity.x;
				ix = this->allPixelInfo[newIndexfloorX].velocity.x;
				iy = this->allPixelInfo[newIndexfloorY].velocity.x;
				ixy = this->allPixelInfo[newIndexfloorXY].velocity.x;
				j = this->allPixelInfo[newIndexfloor].velocity.y;
				jx = this->allPixelInfo[newIndexfloorX].velocity.y;
				jy = this->allPixelInfo[newIndexfloorY].velocity.y;
				jxy = this->allPixelInfo[newIndexfloorXY].velocity.y;



				vec2 lerp1Val = {
					std::lerp(i, ix, relPosx),
					std::lerp(j, jx, relPosx)
				};

				vec2 lerp2Val = {
					std::lerp(iy, ixy, relPosx),
					std::lerp(jy, jxy, relPosx)
				};

				vec2 lerp3Val = {
					std::lerp(lerp1Val.x, lerp2Val.x, relPosy),
					std::lerp(lerp1Val.y, lerp2Val.y, relPosy)
				};

				newAllPixelInfo[index].velocity.x = lerp3Val.x * this->energyLost;
				newAllPixelInfo[index].velocity.y = lerp3Val.y * this->energyLost;
			}
		}
	}

	this->allPixelInfo = std::move(newAllPixelInfo);
}
void window::projectVel() {
	std::vector<float> divergence(this->totalPixelAmount, 0.0f);
	std::vector<float> pressure(this->totalPixelAmount, 0.0f);
	std::vector<pixelInfo> result = this->allPixelInfo;

	float N = float(this->width);
	float h = 1.0f / N;
	int index;
	float x0, x1, y0, y1, center;

	for (int y = 1; y < this->height - 1; ++y) {
		for (int x = 1; x < this->width - 1; ++x) {
			index = x + y * this->width;

			x1 = accessPixel(right, index, velocityX, &this->allPixelInfo, 0.0f);
			x0 = accessPixel(left, index, velocityX, &this->allPixelInfo, 0.0f);
			y1 = accessPixel(down, index, velocityY, &this->allPixelInfo, 0.0f);
			y0 = accessPixel(up, index, velocityY, &this->allPixelInfo, 0.0f);

			divergence[index] = -0.5f * h * (x1 - x0 + y1 - y0);
			pressure[index] = 0.0f;
		

		}
	}

	for (int iter = 0; iter < 20; ++iter) {
		for (int y = 1; y < this->height - 1; ++y) {
			for (int x = 1; x < this->width - 1; ++x) {
				index = x + y * this->width;

				x1 = accessPixel(right, index, &pressure, 0.0f);
				x0 = accessPixel(left, index, &pressure, 0.0f);
				y1 = accessPixel(down, index, &pressure, 0.0f);
				y0 = accessPixel(up, index, &pressure, 0.0f);
				center = divergence[index];

				float newPressure = (x0 + x1 + y0 + y1 + center) / 4.0f;
				pressure[index] = newPressure;

			}
		}
	}
	for (int y = 1; y < this->height - 1; ++y) {
		for (int x = 1; x < this->width - 1; ++x) {
			index = x + y * this->width;

			x1 = accessPixel(right, index, &pressure, 0.0f);
			x0 = accessPixel(left, index, &pressure, 0.0f);
			y1 = accessPixel(down, index, &pressure, 0.0f);
			y0 = accessPixel(up, index, &pressure, 0.0f);

			float vx = this->allPixelInfo[index].velocity.x - 0.5f * N * (x1 - x0);
			float vy = this->allPixelInfo[index].velocity.y - 0.5f * N * (y1 - y0);

			result[index].velocity.x = vx;
			result[index].velocity.y = vy;
		}
	}

	this->allPixelInfo = std::move(result);
}

