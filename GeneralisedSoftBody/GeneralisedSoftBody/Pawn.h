#ifndef _PAWN_H_
#define _PAWN_H_
#include "GameObject.h"

struct DrawData3D;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Pawn : public GameObject
{
public:
	Pawn(float _speed, float _rotSpeed);
	virtual ~Pawn();

	virtual void Tick(GlobalData* _GD);
	virtual void Draw(DrawData3D* _DD) { return; };

	float GetSpeed() { return m_speed; };
	float GetRotSpeed() { return m_rotSpeed; };

	void SetSpeed(float _speed) { m_speed = _speed; return; };
	void SetRotSpeed(float _rotSpeed) { m_rotSpeed = _rotSpeed; return; };

private:
	void InputHandler(GlobalData* _GD);

	float m_speed;
	float m_rotSpeed;
};

#endif;_

