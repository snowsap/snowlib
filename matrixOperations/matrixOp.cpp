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

unique_ptr<vector<float>> matrixScalarMultiply(unique_ptr<vector<float>> givenVector1, unique_ptr<vector<float>> givenVector2, int width) {

	int height = givenVector1->size() / width;
	vector<float> returnVector;
	unsigned int totalElements = width * height;
	unsigned int column = 0;
	float tempAddition = 0;

	returnVector.resize(totalElements);
	for (unsigned int x = 0; x < width; ++x) {
		for (unsigned int y = 0; y < height; ++y) {

			tempAddition = 0;

			for (unsigned int xAdd = 0; xAdd < width; ++xAdd) {

				tempAddition += givenVector1->at(y * width + xAdd);
			}

			for (unsigned int yAdd = 0; yAdd < height; ++yAdd) {

				tempAddition += givenVector2->at(x + width * yAdd);
			}

			returnVector[x + y * width] = tempAddition;
		}
	}

	return make_unique<vector<float>>(returnVector);
}
