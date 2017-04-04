#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include <list>

class Edge;

struct SharedVertexData
{
	std::list<int> m_vertex1;
	std::list<int> m_vertex2;
	std::list<int> m_vertex3;

	std::list<int>* GetList(int _index)
	{
		switch (_index)
		{
		case 0:
		{
			return &m_vertex1;
			break;
		}
		case 1:
		{
			return &m_vertex2;
			break;
		}
		case 2:
		{
			return &m_vertex3;
			break;
		}
		default:
		{
			break;
		}
		}
		return nullptr;
	};
};

class Triangle
{
public:
	Triangle();
	Triangle(int _index, int _vertex1Index, int _vertex2Index, int _vertex3Index, Edge* _edge1, Edge* _edge2, Edge* _edge3);
	~Triangle();

	int m_index;

	Edge* m_edge1;
	Edge* m_edge2;
	Edge* m_edge3;

	int m_vertexIndex[3];

	SharedVertexData m_vertexData;

	std::list<int>* GetVertexList(int _index)
	{
		switch (_index)
		{
		case 0:
		{
			return &m_vertexData.m_vertex1;
			break;
		}
		case 1:
		{
			return &m_vertexData.m_vertex2;
			break;
		}
		case 2:
		{
			return &m_vertexData.m_vertex3;
			break;
		}
		default:
			break;
		}
	}

private:

};

#endif;
