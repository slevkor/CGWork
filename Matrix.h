#ifndef MATRIX_H_
#define MATRIX_H_

const int MATRIX_SIZE = 4;

enum Axis {
	X, Y, Z,
};

class matrix {
	int rows;
	int cols;
	double* data;

public:
	matrix();
	matrix(double xyz[3]);
	matrix(matrix &m);
	matrix(int rows1, int cols1);
	matrix(int rows1, int cols1, double* numbers1);
	~matrix();
	int getRows();
	int getCols();
	double* getXYZ();
	double* getData();
	double getX();
	double getY();
	double getZ();
	void setData(double* data1);
	void setXYZ(double* xyz);
	void initializeData(double* data1);

	matrix mult(matrix* matrix1, matrix* matrix2);
	void selfMult(matrix* matrix2);
	matrix orthogonal(Axis a);
	matrix scaling(double Sx, double Sy, double Sz);
	matrix translation(double Tx, double Ty, double Tz);
	void selfTranslate(double Tx, double Ty, double Tz);
	matrix rotation(double alpha, Axis a);
	void selfRotation(double alpha, Axis a);
	matrix prespectiveProjection(double d);
};

#endif /* MATRIX_H_ */
