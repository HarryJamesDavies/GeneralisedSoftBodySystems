#ifndef _MSO_SQUARE_H_
#define _MSO_SQUARE_H_
#include "VertexMSO.h"
#include <list>

class MSOSquare: public VertexMSO
{
public:
	MSOSquare(int _width, int _height, Vector2 _numSections, bool _generateInternals, GlobalData* _GD);
	~MSOSquare();

	//Initialise the Vertex and Index buffers for the cube
	void Initilaise(bool _generateInternals, ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD) { VertexMSO::Tick(_GD); };
	virtual void Draw(DrawData3D* _DD) { VertexMSO::Draw(_DD); };

private:
	void CreateVertices();

	int m_width;
	int m_height;

	Vector2 m_numSections;
};

#endif;
