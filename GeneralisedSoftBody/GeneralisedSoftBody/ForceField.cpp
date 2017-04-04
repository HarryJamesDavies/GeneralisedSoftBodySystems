#include "ForceField.h"
#include "GlobalData.h"

ForceField::ForceField(GameObject* _parent, GlobalData* _GD, float _fieldStrength, float _minimumDistance) : Collider(_parent, _GD)
{
	m_fieldStrength = _fieldStrength;
	m_minimumDistanceSqrd = _minimumDistance * _minimumDistance;
}

ForceField::~ForceField()
{
}

Vector3 ForceField::CalculateForce(Vector3 _otherPosition, Vector3 _otherVelocity)
{
	float distanceSqrd = Vector3::DistanceSquared(m_pos, _otherPosition);
	float radiusSqrd = m_GD->m_physicsManager->m_physicsData.m_forceFieldRadius * m_GD->m_physicsManager->m_physicsData.m_forceFieldRadius;

	if (radiusSqrd < m_minimumDistanceSqrd)
	{
		radiusSqrd = m_minimumDistanceSqrd;
	}

	if (distanceSqrd <= radiusSqrd)
	{
		Vector3 forceVector = m_pos - _otherPosition;
		return -forceVector * (m_fieldStrength *  m_GD->m_physicsManager->m_physicsData.m_forceFieldCoefficient);
	}
	return Vector3::Zero;
}