#include "matrixOp.h"
using namespace std;



unique_ptr<vector<float>> matrixAdd(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width) {
	int height = givenVector1->size() / width;
	vector<float> returnVector;
	unsigned int totalElements = width * height;
	
	returnVector.resize(totalElements);
	for (unsigned int i = 0; i < totalElements; ++i) {
		returnVector[i] = givenVector1->at(i) + givenVector2->at(i);
	}
	return move(make_unique<vector<float>>(returnVector));
}

unique_ptr<vector<float>> matrixSubtract(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width) {
	int height = givenVector1->size() / width;
	vector<float> returnVector;
	unsigned int totalElements = width * height;

	returnVector.resize(totalElements);
	for (unsigned int i = 0; i < totalElements; ++i) {
		returnVector[i] = givenVector1->at(i) - givenVector2->at(i);
	}
	return move(make_unique<vector<float>>(returnVector));
}

unique_ptr<vector<float>> matrixSubtractAbs(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width) {
	int height = givenVector1->size() / width;
	vector<float> returnVector;
	unsigned int totalElements = width * height;

	returnVector.resize(totalElements);
	for (unsigned int i = 0; i < totalElements; ++i) {
		returnVector[i] = abs(givenVector1->at(i) - givenVector2->at(i));
	}
	return move(make_unique<vector<float>>(returnVector));
}

unique_ptr<vector<float>> matrixScalarMultiply(unique_ptr<vector<float>>(givenVector1), float scaler, int width) {

	int height = givenVector1->size() / width;
	vector<float> returnVector;
	unsigned int totalElements = width * height;

	returnVector.resize(totalElements);
	for (unsigned int i = 0; i < totalElements; ++i) {
		returnVector[i] = givenVector1->at(i) * scaler;
	}
	return move(make_unique<vector<float>>(returnVector));

}

unique_ptr<vector<float>> matrixMultiply(unique_ptr<vector<float>> givenVector1, unique_ptr<vector<float>> givenVector2, int width1) {

	//height of the first width of the second.
	int height1 = givenVector1->size() / width1;
	vector<float> returnVector;
	unsigned int totalElements = height1 * height1;
	float tempAddition = 0;
	returnVector.resize(totalElements);

	for (unsigned int x = 0; x < height1; ++x) {
		for (unsigned int y = 0; y < height1; ++y) {

			tempAddition = 0;

			for (unsigned int xAdd = 0; xAdd < width1; ++xAdd) {
				
				tempAddition += givenVector1->at(y * width1 + xAdd) * givenVector2->at(x + height1 * xAdd);
			}

			returnVector[x + y * height1] = tempAddition;
		}
	}

	return make_unique<vector<float>>(returnVector);
}
