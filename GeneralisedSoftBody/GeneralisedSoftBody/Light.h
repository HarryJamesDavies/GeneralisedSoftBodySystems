#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "GameObject.h"

//=================================================================
//a simple light defined by a colour and and ambient colour and used as a point light with no drop off in the standard shader used to draw VBGOs
//=================================================================

struct DrawData3D;

class Light : public GameObject
{
public:
	Light(Vector3 _pos, Color _colour, Color _ambientColour);
	~Light();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD) { _DD; }; //draws nothing

	//getters
	Color GetColour() { return m_colour; }
	Color GetAmbCol() { return m_ambientColour; }

	//setters
	void SetColor(Color _colour) { m_colour = _colour; }
	void SetAmbCol(Color _colour) { m_ambientColour = _colour; }

protected:

	Color m_colour;
	Color m_ambientColour;
};

#endif