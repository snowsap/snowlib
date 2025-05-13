#include "render/render.h"
#include "chrono"
#include "unordered_set"
#include "matrixOperations/matrixOp.h"

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
	render renderInstance;
	unsigned int shaderProgram;
	unsigned int vao;
	unsigned int textureID;
	float constantOfViscosity = 0.2;
	int targetFrameRate = 60;
	int halfSize = 5;


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

	enum accessPixelEnum {
		up = 0,
		left = 1,
		right = 2,
		down = 3,
	};
	
	enum pixelInfoEnum {
		density = 1,
		velocityX = 2,
		velocityY = 3
	};
	
	
	std::vector<pixelInfo> allPixelInfo{ pixelInfo{0.25, {0, 0}} };
	vec2 mousePos{0,0};

public:

	std::chrono::duration<double, std::milli> frameDuration = std::chrono::duration<double, std::milli>(1000.0 / 60);
	std::chrono::steady_clock::time_point previousTime = std::chrono::high_resolution_clock::now();

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

	/**
	* @param x0 return value target as well as the passing value
	* @param x1 passing value (x - 1)
	* @param x2 passing value (x + 1)
	* @param y1 passing value (y - 1)
	* @param y2 passing value (y + 1)
	*/
	float approxTheDiff(float x0, float x1, float x2, float y1, float y2, float constant);

	void diffusion();

	void addVection();

	int dotProduct(vec2 vector1, vec2 vector2);

	void mapDensityToPx();

	void mousePointerAddVelocity();

	void flipImageVertically(int width, int height);

	void addVection2();

	void solveHelmholtzEquation();

	float accessPixel(accessPixelEnum pixel, uint16_t referencePixel, pixelInfoEnum accessValue);
};