#include "chrono"
#include "unordered_set"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "vector"
#include "array"

class window {

protected:
	int width;
	int height;
	std::string windowName;
	bool resizeable;
	bool visible;
	bool doFloat;
	bool fullScreen;
	bool resizeToMonitor;
	bool windowDecorated;
	GLFWwindow* windowInstance;
	int displayNumber = 0;
	unsigned int shaderProgram;
	unsigned int vao;
	unsigned int textureID;
	float constantOfViscosity = 0.5;
	int targetFrameRate = 1000;
	int halfSize = 10;
	float energyLost = 1;


	struct vec2 {
		float x;
		float y;
	};

	struct vec4 {
		float r;
		float g;
		float b;
		float a;
	};

	struct vec3 {
		float r;
		float b;
		float g;
	};

	struct pixelInfo {
		float density = 1;
		vec2 velocity = {0.0f, 0.0f};
	};

	enum lerp {
		xV = 1,
		yV = 2,
		zV = 3, 
		aV = 4
	};

	enum accessPixelEnum {
		up = 0,
		left = 1,
		right = 2,
		down = 3,
		none = 4
	};
	
	enum pixelInfoEnum {
		density = 1,
		velocityX = 2,
		velocityY = 3
	};
	
	
	std::vector<pixelInfo> allPixelInfo{ pixelInfo{1, {0.0, 0.0}} };
	vec2 mousePos{0,0};
	int totalPixelAmount;

public:

	std::chrono::duration<double, std::milli> frameDuration = std::chrono::duration<double, std::milli>(1000.0 / 60);
	std::chrono::steady_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	float deltaTime;
	std::vector<unsigned char> pixels{0};
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
	window(
		int width,
		int height,
		std::string windowName = "un-named",
		bool resizeable = true,
		bool visible = true,
		bool doFloat = false,
		bool fullScreen = false,
		bool resizeToMonitor = false,
		bool windowDecorated = true,
		int displayNumber = 0);
	/**
	* @param width width to change to, input -1 to not change.
	* @param hight height to change to, input -1 to not change.
	*/
	int changeTheDimensions(int width = -1, int height = -1);

	void renderScreen();

private:
	static void framebuffer_size_callback(GLFWwindow* windowInstance, int width, int height);

	void processInputMethod(GLFWwindow* windowInstance);

	std::string readFile(const std::string& fileAddress);

	unsigned int createShader(const std::string& shaderText, unsigned int shaderType, unsigned int shaderProgram);

	void initTestPixels();

	void screenCover();


	void diffusion();

	void addVection();

	int dotProduct(vec2 vector1, vec2 vector2);

	void mapDensityToPx();

	void mousePointerAddVelocity();

	void flipImageVertically(int width, int height);

	float accessPixel(accessPixelEnum pixelDirection, int referencePixel, pixelInfoEnum accessValue, std::vector<pixelInfo>* pixelArray, float invReturnValue = -1);
	float accessPixel(accessPixelEnum pixelDirection, int referencePixel, std::vector<float>* pixelArray, float invReturnValue);

	float approxTheDiff(float x0, float x2, float y0, float y1, float k, float oldTarg);
	float approxTheDiff(float x0, float x2, float y0, float y1, float oldTarg);

	void addVectionVel();

	void projectVel();

	void extendVelocity();


};