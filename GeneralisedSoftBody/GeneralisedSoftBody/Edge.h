#ifndef _EDGE_H_
#define _EDGE_H_

class Triangle;
struct Vertex;

class Edge
{
public:
	Edge();
	Edge(int _index, Vertex* _vertex1, Vertex* _vertex2, Triangle* _triangle);
	~Edge();

	int m_index;

	Vertex* m_vertex1;
	Vertex* m_vertex2;

	int m_localVertex1Index;
	int m_localVertex2Index;

	int m_neighbouringTriangleIndex = -1;
	int m_sharedEdgeIndex = -1;

	Triangle* m_triangle;

private:

};

#endif;
