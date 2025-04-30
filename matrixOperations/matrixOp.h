#include "iostream"
#include "vector"
#pragma once
using namespace std;

unique_ptr<vector<float>> matrixAdd(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width);
unique_ptr<vector<float>> matrixSubtract(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width);
unique_ptr<vector<float>> matrixSubtractAbs(unique_ptr<vector<float>>(givenVector1), unique_ptr<vector<float>>(givenVector2), int width);
unique_ptr<vector<float>> matrixScalarMultiply(unique_ptr<vector<float>>(givenVector1), float scaler, int width);
unique_ptr<vector<float>> matrixMultiply(unique_ptr<vector<float>> givenVector1, unique_ptr<vector<float>> givenVector2, int width);