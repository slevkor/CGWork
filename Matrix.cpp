#include "matrix.h"
#include <stdio.h>
#include <math.h>

const int X_PLACE = 0;
const int Y_PLACE = 1;
const int Z_PLACE = 2;

static void printMatrix(matrix matrix1) {
	int counter = 1;
	double* data = matrix1.getData();
	for (int i = 0; i < matrix1.getRows() * matrix1.getCols(); i++) {
		printf("%f ", data[i]);
		if (counter == 4) {
			printf("\n");
			counter = 1;
		}
		else {
			counter++;
		}
	}
}

matrix::matrix(){
}

matrix::matrix(double xyz[3]) :rows(MATRIX_SIZE), cols(MATRIX_SIZE){
	rows = MATRIX_SIZE;
	cols = MATRIX_SIZE;
	data = new double[MATRIX_SIZE * MATRIX_SIZE];
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		for (int j = 0; j < MATRIX_SIZE; j++)
		{
			if (i == j && i != (MATRIX_SIZE-1))
			{
				data[i*MATRIX_SIZE + j] = xyz[i];
			}
			else
			{
				data[i*MATRIX_SIZE + j] = 0;
			}
		}
	}
}

matrix::matrix(matrix &m){
	rows = m.rows;
	cols = m.cols;
	data = new double[rows * cols];
	double* mdata = m.getData();
	for (int i = 0; i < rows*cols; i++)
	{
		data[i] = mdata[i];
	}
}

matrix::matrix(int rows1, int cols1) :
rows(rows1), cols(cols1) {
	data = new double[rows * cols];
	for (int i = 0; i < rows * cols; i++) {
		data[i] = 0;
	}
}

matrix::matrix(int rows1, int cols1, double* data1) :
rows(rows1), cols(cols1) {
	data = new double[rows * cols];
	data = data1;
	for (int i = 0; i < rows * cols; i++) {
		data[i] = data1[i];
	}
}

matrix::~matrix() {
	/*if (data != NULL){
		delete[] data;
	}*/
}

int matrix::getRows() {
	return rows;
}

int matrix::getCols() {
	return cols;
}

double* matrix::getXYZ(){
	double* xyz = new double[3];
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		xyz[i] = data[i*MATRIX_SIZE];
	}
	return xyz;
}

double matrix::getX(){
	return data[X_PLACE];
}
double matrix::getY(){
	return data[Y_PLACE];
}
double matrix::getZ(){
	return data[Z_PLACE];
}

void matrix::setXYZ(double* xyz){
	data[X_PLACE] = xyz[X_PLACE];
	data[Y_PLACE] = xyz[Y_PLACE];
	data[Z_PLACE] = xyz[Z_PLACE];
	data[3] = 1;
}

double* matrix::getData() {
	return data;
}

void matrix::setData(double* data1){
	for (int i = 0; i < rows * cols; i++) {
		data[i] = data1[i];
	}
}
void matrix::initializeData(double* data1) {
	data = new double[rows * cols];
	for (int i = 0; i < rows * cols; i++) {
		data[i] = data1[i];
	}
}

matrix matrix::mult(matrix* matrix1, matrix* matrix2) {
	matrix* multMatrix = new matrix(matrix1->rows, matrix2->cols);
	for (int i = 0; i < matrix1->rows; i++) {
		for (int j = 0; j < matrix1->cols; j++) {
			double sum = 0;
			for (int k = 0; k < matrix2->cols; k++) {
				sum += matrix1->getData()[i * matrix1->cols + k]
					* matrix2->getData()[k * matrix2->cols + j];
			}
			multMatrix->data[i * matrix2->cols + j] = sum;
		}
	}
	return *multMatrix;
}

void matrix::selfMult(matrix* matrix2){
	matrix* multMatrix = new matrix(this->rows, matrix2->cols);
	for (int row = 0; row < this->rows; row++) {
		for (int col = 0; col < this->cols; col++) {
			double sum = 0;
			for (int k = 0; k < matrix2->cols; k++) {
				sum += this->getData()[row * this->cols + k]
					* matrix2->getData()[k * matrix2->cols + col];
			}
			multMatrix->data[row * matrix2->cols + col] = sum;
		}
	}
	this->setData(multMatrix->getData());
	delete multMatrix;
}

matrix matrix::orthogonal(Axis a) {
	matrix* orthogonalMatrix = new matrix(MATRIX_SIZE, MATRIX_SIZE);
	if (a == X) {
		double orthogonalData[MATRIX_SIZE * MATRIX_SIZE] = { 0, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		orthogonalMatrix->setData(orthogonalData);
	}
	else if (a == Y) {
		double orthogonalData[MATRIX_SIZE * MATRIX_SIZE] = { 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		orthogonalMatrix->setData(orthogonalData);
	}
	else if (a == Z) {
		double orthogonalData[MATRIX_SIZE * MATRIX_SIZE] = { 1, 0, 0, 0, 0, 1,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
		orthogonalMatrix->setData(orthogonalData);
	}
	return mult(this, orthogonalMatrix);
}

matrix matrix::scaling(double Sx, double Sy, double Sz) {
	double scalingData[MATRIX_SIZE * MATRIX_SIZE] = { Sx, 0, 0, 0, 0, Sy, 0, 0,
		0, 0, Sz, 0, 0, 0, 0, 1 };
	matrix* scalingMatrix = new matrix(1, MATRIX_SIZE, scalingData);
	return mult(this, scalingMatrix);
}

matrix matrix::translation(double Tx, double Ty, double Tz) {
	double translationData[MATRIX_SIZE * MATRIX_SIZE] = { 1, 0, 0, Tx, 0, 1, 0,
		Ty, 0, 0, 1, Tz, 0, 0, 0, 1 };
	matrix* translationMatrix = new matrix(1, MATRIX_SIZE, translationData);
	return mult(this, translationMatrix);
}

void matrix::selfTranslate(double Tx, double Ty, double Tz){
	data[X_PLACE] = data[X_PLACE] + Tx;
	data[Y_PLACE] = data[Y_PLACE] + Ty;
	data[Z_PLACE] = data[Z_PLACE] + Tz;
}

matrix matrix::rotation(double alpha, Axis a) {
	matrix* rotationMatrix = new matrix(MATRIX_SIZE, MATRIX_SIZE);
	if (a == X) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] =
		{ 1, 0, 0, 0, 0, cos(alpha), -sin(alpha), 0, 0, sin(alpha), cos(
		alpha), 0, 0, 0, 0, 1 };
		rotationMatrix->setData(rotateData);
	}
	else if (a == Y) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] =
		{ cos(alpha), 0, sin(alpha), 0, 0, 1, 0, 0, -sin(alpha), 0, cos(
		alpha), 0, 0, 0, 0, 1 };
		rotationMatrix->setData(rotateData);
	}
	else if (a == Z) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] = { cos(alpha), -sin(
			alpha), 0, 0, sin(alpha), cos(alpha), 0, 0, 0, 0, 1, 0, 0, 0, 0,
			1 };
		rotationMatrix->setData(rotateData);
	}

	return mult(this, rotationMatrix);
}

void matrix::selfRotation(double alpha, Axis a){
	matrix* rotationMatrix = new matrix(MATRIX_SIZE, MATRIX_SIZE);
	if (a == X) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] =
		{ 1, 0, 0, 0, 0, cos(alpha), -sin(alpha), 0, 0, sin(alpha), cos(
		alpha), 0, 0, 0, 0, 1 };
		rotationMatrix->setData(rotateData);
	}
	else if (a == Y) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] =
		{ cos(alpha), 0, sin(alpha), 0, 0, 1, 0, 0, -sin(alpha), 0, cos(
		alpha), 0, 0, 0, 0, 1 };
		rotationMatrix->setData(rotateData);
	}
	else if (a == Z) {
		double rotateData[MATRIX_SIZE * MATRIX_SIZE] = { cos(alpha), -sin(
			alpha), 0, 0, sin(alpha), cos(alpha), 0, 0, 0, 0, 1, 0, 0, 0, 0,
			1 };
		rotationMatrix->setData(rotateData);
	}
	this->selfMult(rotationMatrix);
	delete rotationMatrix;
}

matrix matrix::prespectiveProjection(double d) {
	matrix* prespectiveMatrix = new matrix(1, MATRIX_SIZE);
	double prespectiveData[MATRIX_SIZE] = { data[0] / (data[2] / d), data[1] / (data[2] / d), d, 1 };
	prespectiveMatrix->setData(prespectiveData);
	return *prespectiveMatrix;
}
