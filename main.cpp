
#include "main.h"

int main(void) {
	
	std::unique_ptr<window> windowInstance = std::make_unique<window>(window(1000, 1000));
	
	std::vector<float> example1{ 
		2, 3, 4,
		3, 2, 1
	};
	std::vector<float> example2{
		5, 3,
		2, 3, 
		7, 4
	};

	std::unique_ptr<std::vector<float>> yes = matrixMultiply(std::make_unique<std::vector<float>>(example1), std::make_unique<std::vector<float>>(example2), 3);
	
	while (true) {
		windowInstance->renderScreen();
	}



	return 0;
}
