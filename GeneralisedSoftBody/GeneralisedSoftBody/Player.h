#pragma once
#include "Pawn.h"
#include "GlobalData.h"
#include "ForceField.h"
#include "MeshGO.h"

class Player : public Pawn
{
public:
	Player(float _speed, float _rotSpeed, string _meshFileName, GlobalData* _GD);
	~Player();

	void Tick(GlobalData* _GD);

private:
	ForceField* m_forceField;
	MeshGO* m_mesh;
};