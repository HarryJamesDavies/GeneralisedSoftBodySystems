#ifndef _VERTEX_MSO_H_
#define _VERTEX_MSO_H_
#include "VertexGO.h"
#include <list>

class MSMesh;
class Triangle;
class Edge;

class VertexMSO: public VertexGO
{
public:
	VertexMSO();
	virtual ~VertexMSO();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD) { VertexGO::Draw(_DD); };
	void Reset();

	Vertex* GetInitialVertex(int i) { return &m_intialVertices[i]; }
	MSMesh* GetMesh() { return m_mesh; };

	void CreateIntialVertices();

protected:
	MSMesh* m_mesh;
	std::list<Triangle*> m_triangles;

	Vertex* m_intialVertices;

	void UpdateNormals();

private:
};

#endif
