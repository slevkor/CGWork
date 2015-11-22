#pragma once
#include "CGWorkView.h"
#include <vector>
using std::vector;

class hw2models
{
public:
	hw2models();
	~hw2models();

};

class vertex{
	double x;
	double y;
	double z;
public:
	vertex();
	vertex(double x,double y,double z);
	~vertex();
	vertex(const vertex& v);
	vertex& operator=(const vertex& v);
	double getX();
	double getY();
	double getZ();
};

class line{
	vertex v1;
	vertex v2;
public:
	line();
	line(vertex v1, vertex v2);
	~line();
	void normalize();
	double length();
	vertex getV1();
	vertex getV2();
};

class polygon{
	vector<vertex*>* verList;
	vertex center;
	vertex normal;

public:
	polygon(vector<vertex*>* list);
	~polygon();
	void calculatePolygonCenter();
	void calculatePolygonNormal();
	vertex getCenter();	
};

class object{
	vector<polygon*>* polygons;
	vertex minVertex;
	vertex maxVertex;
public:
	object(vector<vertex*>* polygons);
	~object();
	void drawBoundingBox();
};