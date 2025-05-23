
#include "main.h"

int main(void) {
	
	std::unique_ptr<window> windowInstance = std::make_unique<window>(window(500, 500));
	
	while (true) {
		windowInstance->renderScreen();
	}



	return 0;
}
