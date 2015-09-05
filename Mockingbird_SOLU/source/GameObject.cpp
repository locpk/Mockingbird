#include "GameObject.h"
#include "Define.h"
#include "DDSTextureLoader.h"
#include "CSH\Cube_VS.csh"
void LoadModelFromOBJ(string _filepath, vector<VERTEX>& _model , string& _texture)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<XMFLOAT3> temp_vertices;
	vector<XMFLOAT3> temp_uvs;
	vector<XMFLOAT3> temp_normals;
	string mtl;
	char dim = 0;
	char mtllib[6];
	ifstream fin(_filepath.c_str());
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
					float u, v;
					fin >> u >> v;
					temp_uvs.push_back(XMFLOAT3(u, v,0.0f));
				}
				else if (letter == 'n')
				{
					float x, y, z;
					fin >> x >> y >> z;
					temp_normals.push_back(XMFLOAT3(x, y, z));
				}
				break;
			case 'f':
				letter = fin.get();
				if (letter == ' ')
				{
					unsigned int vun[9];
					fin >> vun[0] >> dim >> vun[1] >> dim >> vun[2] >> vun[3] >> dim >> vun[4] >> dim >> vun[5] >> vun[6] >> dim >> vun[7] >> dim >> vun[8];
					vertexIndices.push_back(vun[0]);
					vertexIndices.push_back(vun[3]);
					vertexIndices.push_back(vun[6]);
					uvIndices.push_back(vun[1]);
					uvIndices.push_back(vun[4]);
					uvIndices.push_back(vun[7]);
					normalIndices.push_back(vun[2]);
					normalIndices.push_back(vun[5]);
					normalIndices.push_back(vun[8]);
				}
				break;
			case 'm':
				fin.readsome(mtllib,5);
				mtllib[5] = '\0';
				if (0 == strcmp(mtllib,"tllib"))
				{
					fin >> mtl;
				}
				break;
			default:

				break;
			}
		}
		fin.close();
	}


	fin.open(mtl);
	if (fin.is_open())
	{
		while (fin)
		{
			char letter = fin.get();
			switch (letter)
			{
			case 'm':
				letter = fin.get();
				if (letter == 'a')
				{
					string gra;
					fin >> gra;
					fin >> _texture;
				}
				break;
			default:
				break;
			}
		}
		
	}


	for (size_t i = 0; i < vertexIndices.size(); i++)
	{
		VERTEX temp;
		unsigned int index = vertexIndices[i];
		temp.pos = temp_vertices[index - 1];
		index = uvIndices[i];
		temp.uv = temp_uvs[index - 1];
		index = normalIndices[i];
		temp.normal = temp_normals[index - 1];
		_model.push_back(temp);
	}
	int i = 0;
	//formating data

#pragma region Output Test
	/*cout << "temp_vertices " << temp_vertices.size() << endl;
	for (size_t i = 0; i < temp_vertices.size(); i++)
	{
	cout << temp_vertices[i].x << " " << temp_vertices[i].y << " " << temp_vertices[i].z << endl;
	}

	cout << "temp_uvs " << temp_uvs.size() << endl;
	for (size_t i = 0; i < temp_uvs.size(); i++)
	{
	cout << temp_uvs[i].x << " " << temp_uvs[i].y << endl;
	}

	cout << "temp_normals " << temp_normals.size() << endl;
	for (size_t i = 0; i < temp_normals.size(); i++)
	{
	cout << temp_normals[i].x << " " << temp_normals[i].y << " " << temp_normals[i].z << endl;
	}

	cout << "vertexIndices " << vertexIndices.size() << endl;
	for (size_t i = 0; i < vertexIndices.size(); i++)
	{
	cout << vertexIndices[i] << endl;
	}
	cout << "uvIndices " << uvIndices.size() << endl;
	for (size_t i = 0; i < uvIndices.size(); i++)
	{
	cout << uvIndices[i]  << endl;
	}
	cout << "normalIndices " << normalIndices.size() << endl;
	for (size_t i = 0; i < normalIndices.size(); i++)
	{
	cout << normalIndices[i] << endl;
	}
	cout << "end" << endl;*/
#pragma endregion
}

GameObject::GameObject()
{

}

void GameObject::CreateGameObject(ID3D11Device  * _device, string _filepath, const BYTE * _VS, unsigned int _VSize)
{
	LoadModelFromOBJ(_filepath, GOrawData, textureName);


	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.ByteWidth = sizeof(VERTEX) * (UINT)GOrawData.size();
	BufferDesc.CPUAccessFlags = NULL;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA VerticesData;
	ZeroMemory(&VerticesData, sizeof(VerticesData));
	VerticesData.pSysMem = GOrawData.data();
	_device->CreateBuffer(&BufferDesc, &VerticesData, &pGOvertices);

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	_device->CreateInputLayout(InputLayout, 3, _VS, _VSize, &pGO_inputLayout);

	wstring ws = wstring(textureName.begin(), textureName.end());
	CreateDDSTextureFromFile(_device, ws.c_str(), NULL, &pGO_ShaderResourceView);
	


	D3D11_RASTERIZER_DESC RasterDESC;
	ZeroMemory(&RasterDESC, sizeof(RasterDESC));
	RasterDESC.AntialiasedLineEnable = true;
	RasterDESC.FrontCounterClockwise = true;
	RasterDESC.FillMode = D3D11_FILL_SOLID;
	RasterDESC.CullMode = D3D11_CULL_FRONT;
	_device->CreateRasterizerState(&RasterDESC, &pGORSf);

	ZeroMemory(&RasterDESC, sizeof(RasterDESC));
	RasterDESC.AntialiasedLineEnable = true;
	RasterDESC.FrontCounterClockwise = true;
	RasterDESC.FillMode = D3D11_FILL_SOLID;
	RasterDESC.CullMode = D3D11_CULL_BACK;
	_device->CreateRasterizerState(&RasterDESC, &pGORSb);
}


GameObject::~GameObject()
{

	SecureRelease(pGOvertices);
	SecureRelease(pGO_inputLayout);
	if (pGO_PSShader)
	{
		SecureRelease(pGO_PSShader);
	}
	if (pGO_VSShader)
	{
		SecureRelease(pGO_VSShader);
	}
	SecureRelease(pGO_ShaderResourceView);
	SecureRelease(pGORSf);
	SecureRelease(pGORSb);
}