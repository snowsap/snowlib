
#include "main.h"


int main() {
	std::unique_ptr<window> windowInstance(new window(1000, 1000));
		
	while (true) {
		windowInstance->render();
	}



	return 0;
}
