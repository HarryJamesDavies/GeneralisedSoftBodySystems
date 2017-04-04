#include "PhysicsManager.h"
#include <math.h>

PhysicsManager::PhysicsManager()
{
	m_sinCount = 0.0f;
	m_windPercentage = 0.0f;
	m_colliders.clear();
}

PhysicsManager::~PhysicsManager()
{

}

Vector3 PhysicsManager::GetGravityForce()
{
	Vector3 gravityForce = m_physicsData.m_gravityCoefficient * Vector3(0.0f, 10.0f, 0.0f);
	return gravityForce;
}

Vector3 PhysicsManager::GetWindForce()
{
	Vector3 normWindDir = m_physicsData.m_windDirection;
	Vector3 windForce = m_physicsData.m_windCoefficient * normWindDir;
	return windForce;
}
void PhysicsManager::UpdatePhysics(float _dt)
{
	UpdateWindCoefficent(_dt);
	UpdateColliders();
}

void PhysicsManager::UpdateWindCoefficent(float _dt)
{
	m_sinCount += _dt;

	float windDif = m_physicsData.m_windCoefficientMax - m_physicsData.m_windCoefficientMin;

	m_windPercentage = sin(m_sinCount);

	windDif *= m_windPercentage;

	m_physicsData.m_windCoefficient = m_physicsData.m_windCoefficientMin + windDif;
}

void PhysicsManager::UpdateColliders()
{
	for (auto it = m_colliders.begin(); it != m_colliders.end(); it++)
	{
		(*it)->UpdatePosition();
	}
}

Vector3 PhysicsManager::CalculateColliderForce(Vector3 _position, Vector3 _otherVelocity)
{
	Vector3 force = Vector3::Zero;

	for (auto it = m_colliders.begin(); it != m_colliders.end(); it++)
	{
		force += (*it)->CalculateForce(_position, _otherVelocity);
	}

	return force;
}

void PhysicsManager::AddCollider(Collider* _collider)
{
	m_colliders.push_back(_collider);
}

void PhysicsManager::RemoveCollider(Collider* _collider)
{
	Collider* colliderToRemove = nullptr;

	for (auto it = m_colliders.begin(); it != m_colliders.end(); it++)
	{
			if ((*it) == _collider)
			{
				colliderToRemove = (*it);
			}
	}

	if (colliderToRemove != nullptr)
	{
		m_colliders.remove(colliderToRemove);
	}
	colliderToRemove = nullptr;
}