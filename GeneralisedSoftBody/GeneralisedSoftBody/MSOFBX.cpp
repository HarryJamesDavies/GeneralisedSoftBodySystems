#include "MSOFBX.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "TriangulateVertices.h"
#include <fstream>
#include <sstream>
#include <string>

MSOFBX::MSOFBX(const char* _fileName, std::string _name, bool _internal, GlobalData* _GD) : VertexMSO()
{
	m_fileName = _fileName;

	m_name = "MSO" + _name;
	m_tag = ObjectTag::OT_NULL;
	m_layer = ObjectLayer::OL_NULL;

	SetScale(1.0f);

	m_mesh = new MSMesh(this, _GD->m_MSOManager->m_fixIndex, _GD->m_MSOManager->m_fixformat, _GD->m_MSOManager->m_fixDimension);

	Initilaise(_internal, _GD->m_IDevice);
}

MSOFBX::~MSOFBX()
{

}

void MSOFBX::Initilaise(bool _internal, ID3D11Device* _ID)
{
	m_mesh->Set3D(true);

	TriangulateVertices* math = new TriangulateVertices();

	CreateVertices();
	CreateIntialVertices();
	UpdateNormals();
	math->CalculateTriangles(&m_triangles, m_vertices, m_indices, m_numPrims);
	m_mesh->CreateSurfaceMasses(m_numVerts, m_triangles.size(), m_triangles.begin(), m_triangles.end());
	m_mesh->CreateSurfaceSprings();
	m_mesh->UpdateNormals();

	if (_internal)
	{
		m_mesh->CreateInternalMasses();
		m_mesh->CreateInternalSprings();
	}

	delete(math);

	BuildDIB(_ID, m_indices);
	BuildDVB(_ID, m_numVerts, m_vertices);

	//Setup Raster State
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT hr = _ID->CreateRasterizerState(&rasterDesc, &m_pRasterState);
}

void MSOFBX::CreateVertices()
{
	std::ifstream infile;
	infile.open(m_fileName);

	while (infile.good())
	{
		int vertexIter = 0;
		int indexIter = 0;
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "*MESH_NUMVERTEX")
			{
				in >> m_numVerts;
				m_vertices = new Vertex[m_numVerts];

			}
			else if (type == "*MESH_NUMFACES")
			{
				in >> m_numPrims;
				m_indices = new WORD[m_numPrims * 3];
			}
			else if (type == "*MESH_VERTEX")
			{
				in >> vertexIter;
				in >> m_vertices[vertexIter].Pos.x;
				in >> m_vertices[vertexIter].Pos.y;
				in >> m_vertices[vertexIter].Pos.z;
				m_vertices[vertexIter].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else if (type == "*MESH_FACE")
			{
				in.ignore(line.length(), ':');
				in.ignore(line.length(), ':');
				in >> m_indices[indexIter];
				indexIter++;
				in.ignore(line.length(), ':');
				in >> m_indices[indexIter];
				indexIter++;
				in.ignore(line.length(), ':');
				in >> m_indices[indexIter];
				indexIter++;
			}
		}
	}

	infile.close();
}