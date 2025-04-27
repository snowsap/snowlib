
#include "main.h"


int main(void) {
	std::unique_ptr<window> windowInstance = std::make_unique<window>(window(1000, 1000));
		
	while (true) {
		windowInstance->renderScreen();
	}



	return 0;
}
