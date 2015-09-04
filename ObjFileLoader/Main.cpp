#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

void main()
{


	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<XMFLOAT3> temp_vertices;
	vector<XMFLOAT2> temp_uvs;
	vector<XMFLOAT3> temp_normals;
	string filepath = "test pyramid.obj";
	ifstream fin(filepath.c_str());
	if (fin.is_open())
	{
		while (fin)
		{
			char letter = fin.get();
			switch (letter)
			{
			case '#':
				while (letter != '\n')
					letter = fin.get();
				break;
			case 'v':
				letter = fin.get();
				if (letter == ' ')
				{
					float x, y, z;
					fin >> x >> y >> z;
					temp_vertices.push_back(XMFLOAT3(x, y, z));
				}
				if (letter == 't')
				{
					float u,v;
					fin >> u >> v;
					temp_uvs.push_back(XMFLOAT2(u,v));
				}
				else if (letter == 'n')
				{
					float x, y, z;
					fin >> x >> y >> z;
					temp_normals.push_back(XMFLOAT3(x, y, z));
				}
				break;
			case 'f':
				unsigned int vun[9];
				fin >> vun[0] >> vun[1] >> vun[2] >> vun[3] >> vun[4] >> vun[5] >> vun[6] >> vun[7] >> vun[8];
				/*vertexIndices
					uvIndices
					normalIndices*/

				while (letter != '\n')
					letter = fin.get();
				break;
			default:
				break;
			}
		}
		fin.close();
	}

	//main
}