#include "Collider.h"
#include "GlobalData.h"

Collider::Collider(GameObject* _parent, GlobalData* _GD)
{
	m_parent = _parent;
	m_pos = _parent->GetPos();
	m_GD = _GD;
	m_GD->m_physicsManager->AddCollider(this);
}

Collider::~Collider()
{
	m_parent = nullptr;
	m_GD->m_physicsManager->RemoveCollider(this);
	m_GD = nullptr;
}