#include "VertexMSO.h"
#include "Triangle.h"
#include "MSMesh.h"
#include "GlobalData.h"

VertexMSO::VertexMSO() : VertexGO()
{
}

VertexMSO::~VertexMSO()
{
	for (auto it = m_triangles.begin(); it != m_triangles.end(); it++)
	{
		delete((*it));
	}

	m_triangles.clear();
}

void VertexMSO::CreateIntialVertices()
{
	m_intialVertices = new Vertex[m_numVerts];
	for (int vertexIter = 0; vertexIter <= m_numVerts - 1; vertexIter++)
	{
		m_intialVertices[vertexIter] = m_vertices[vertexIter];
	}
}

void VertexMSO::UpdateNormals()
{
	int face = 0;
	//calculate the normals for the basic lighting in the base shader
	for each (Triangle* var in m_triangles)
	{
		WORD V1 = (*var).m_vertexIndex[0];
		WORD V2 = (*var).m_vertexIndex[1];
		WORD V3 = (*var).m_vertexIndex[2];

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V1].Face = face;
		m_vertices[V2].Norm = norm;
		m_vertices[V2].Face = face;
		m_vertices[V3].Norm = norm;
		m_vertices[V3].Face = face;

		face++;
	}
}

void VertexMSO::Tick(GlobalData* _GD)
{
	if (!_GD->m_MSOManager->m_pauseObject)
	{
		if (_GD->m_MSOManager->m_resetVertices)
		{
			_GD->m_MSOManager->m_resetVertices = false;
			Reset();
		}
		else
		{
			m_mesh->ApplyChanges();
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		_GD->m_IDeviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		memcpy(mappedResource.pData, m_vertices, sizeof(Vertex)*(m_numVerts));

		_GD->m_IDeviceContext->Unmap(m_VertexBuffer, 0);

		VertexGO::Tick(_GD);

		UpdateNormals();
		m_mesh->UpdateNormals();
		m_mesh->UpdateSprings(_GD);
		m_mesh->UpdateMasses(_GD);
	}
}

void VertexMSO::Reset()
{
	for (int vertexIter = 0; vertexIter <= m_numVerts - 1; vertexIter++)
	{
		m_vertices[vertexIter].Pos = m_intialVertices[vertexIter].Pos;
		m_mesh->ResetMasses();
	}

	UpdateNormals();
}