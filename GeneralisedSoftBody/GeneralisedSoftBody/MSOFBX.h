#ifndef _MSO_FBX_H_
#define _MSO_FBX_H_
#include "VertexMSO.h"
#include <string>

class MSOFBX : public VertexMSO
{
public:
	MSOFBX(const char* _fileName, std::string _name, bool _internal, GlobalData* _GD);
	~MSOFBX();

	//Initialise the Vertex and Index buffers for the cube
	void Initilaise(bool _internal, ID3D11Device* _ID);

	virtual void Tick(GlobalData* _GD) { VertexMSO::Tick(_GD); };
	virtual void Draw(DrawData3D* _DD) { VertexMSO::Draw(_DD); };

private:
	void CreateVertices();

	const char* m_fileName;

private:

};

#endif;
