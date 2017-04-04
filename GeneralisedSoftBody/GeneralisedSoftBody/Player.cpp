#include "Player.h"

Player::Player(float _speed, float _rotSpeed, string _meshFileName, GlobalData* _GD) : Pawn(_speed, _rotSpeed)
{
	m_name = "Player";
	m_tag = ObjectTag::OT_PLAYER;
	m_pos = Vector3(10.0, -50.0, 100.0);
	
	m_forceField = new ForceField((GameObject*)this, _GD, 10.0f, 15.0f);

	m_mesh = new MeshGO(_meshFileName, _GD->m_IDevice, _GD->m_fxFactory);
	_GD->m_gameObjectPool->StoreGameObject(m_mesh);

	m_mesh->SetPos(m_pos);
	m_mesh->SetScale(Vector3(2.0, 2.0, 2.0));
}

Player::~Player()
{
}

void Player::Tick(GlobalData* _GD)
{
	Pawn::Tick(_GD);
	m_mesh->SetPos(m_pos);

	return;
}