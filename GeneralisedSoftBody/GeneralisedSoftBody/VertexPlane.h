#pragma once
#ifndef _VB_PLANE_H_
#define _VB_PLANE_H_
#include "VertexGO.h"
#include "Vertex.h"
#include <list>

//=================================================================
//procedurally generate a VBGO Plane
//each side be divided in to _size * _size squares (2 triangles per square)
//=================================================================

struct GlobalData;
struct DrawData3D;

class VertexPlane : public VertexGO
{
public:
	VertexPlane(int _width, int _height, Vector2 _numSections, ID3D11Device* _ID, GlobalData* _GD);
	virtual ~VertexPlane();

	//initialise the Veretx and Index buffers for the cube
	void init(ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD);

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	virtual void Transform() {}

	int m_size;

private:

	void CalculateNormals();
	void CreateVertices();

	int m_width;
	int m_height;

	Vector2 m_numSections;
};

#endif