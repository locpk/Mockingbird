#include "ObjLoader.h"
void main()
{
	vector<VERTEX> myshape;
	OBJLOADER::LoadModelFromOBJ("test pyramid.obj", myshape);
	int i = 0;
}