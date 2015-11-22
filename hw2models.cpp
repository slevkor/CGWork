#include "hw2models.h"

hw2models::hw2models()
{
}

hw2models::~hw2models()
{
}

#pragma region vertex

vertex::vertex()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

vertex::vertex(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vertex::~vertex()
{
}

vertex::vertex(const vertex& v)
{
	this->x = v.getX;
	this->y = v.getY;
	this->z = v.getZ;
}

vertex& vertex::operator=(const vertex& v)
{
	vertex newVertex(v.getX, v.getY, v.getZ);
	return newVertex;
}

double vertex::getX()
{
	return x;
}
double vertex::getY()
{
	return y;
}
double vertex::getZ()
{
	return z;
}

#pragma endregion vertex

#pragma region line

line::line()
{
}

line::line(vertex v1, vertex v2)
{
	this->v1 = v1;
	this->v2 = v2;
}

line::~line()
{
}

double line::length()
{
	//pow(v1.getX-v2.getX,2)
}

void line::normalize()
{
	//double size = sqrt(x*x + y*y + z*z);
	//x /= size;
	//y /= size;
	//z /= size;
}

vertex line::getV1()
{
	return v1;
}
vertex line::getV2()
{
	return v2;
}

#pragma endregion line

#pragma region polygon

polygon::polygon(vector<vertex*>* list)
{
	verList = list;
	calculatePolygonCenter();
	calculatePolygonNormal();
}

polygon::~polygon()
{
	for (vector<vertex*>::iterator it = verList->begin();
		it != verList->end();
		++it) {
		delete(*it);
	}
	delete(verList);
}

void polygon::calculatePolygonCenter() {
	double x = 0;
	double y = 0;
	double z = 0;
	int count = 0;
	for (vector<vertex*>::iterator iter = verList->begin(); iter != verList->end(); ++iter) {
		vertex* v = *iter;
		count++;
		x += v->getX();
		y += v->getY();
		z += v->getZ();
	}
	x /= (double)count;
	y /= (double)count;
	z /= (double)count;
	center = vertex(x, y, z);
}

void polygon::calculatePolygonNormal() {
	double x = 0;
	double y = 0;
	double z = 0;
	if (verList->size() > 1) {
		vertex* v1 = verList->at(0);
		vertex* v2 = verList->at(1);
		vertex* v3 = verList->at(2);
		double x1 = v2->getX() - v1->getX();
		double y1 = v2->getY() - v1->getY();
		double z1 = v2->getZ() - v1->getZ();
		double x2 = v3->getX() - v2->getX();
		double y2 = v3->getY() - v2->getY();
		double z2 = v3->getZ() - v2->getZ();
		//cross
		x = y1*z2 - z1*y2;
		y = z1*x2 - x1*z2;
		z = x1*y2 - y1*x2;
	}
	double size = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	normal = vertex(x / size, y / size, z / size);
}

vertex polygon::getCenter()
{
	return center;
}

#pragma endregion polygon

#pragma region object

void object::drawBoundingBox(){

}

#pragma endregion object