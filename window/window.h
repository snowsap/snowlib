#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "fstream"
#include "sstream"


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

	int changeTheDimensions(int width = -1 /* width to change too, leave blank to not change */, int height = -1 /* height to change too, leave blank too not change */);

	void render();

private:
	static void framebuffer_size_callback(GLFWwindow* windowInstance, int width, int height);

	void processInputMethod(GLFWwindow* windowInstance);

	static const std::string readFile(const std::string& fileAddress);
};