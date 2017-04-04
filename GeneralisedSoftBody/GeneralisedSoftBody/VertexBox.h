#ifndef _VB_BOX_H_
#define _VB_BOX_H_
#include "VertexGO.h"
#include "Vertex.h"

//=================================================================
//procedurally generate a VBGO Cube
//each side be divided in to _size * _size squares (2 triangles per square)
//=================================================================

class VertexBox : public VertexGO
{
public:
	VertexBox(int _size, ID3D11Device* _ID, GlobalData* _GD);
	virtual ~VertexBox();

	//initialise the Veretx and Index buffers for the cube
	void init(int _size, ID3D11Device* _ID, GlobalData* _GD);

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	virtual void Transform() {};

	int m_size;

	int m_numVerts;
	WORD* m_indices;
};

#endif