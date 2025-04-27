#include "render/render.h"


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
	float positions[];





public:

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
	window (
		int width,
		int height,
		std::string windowName = "un-named",
		bool resizeable = true,
		bool visible = true,
		bool doFloat = false,
		bool fullScreen = false,
		bool resizeToMonitor = false,
		bool windowDecorated = true,
		int displayNumber = 0 );
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

	int addATriangle(int pos1, int pos2, int pos3);
	
};