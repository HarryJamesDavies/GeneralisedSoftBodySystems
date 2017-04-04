#include "MSMass.h"
#include "MSSpring.h"
#include "MSMesh.h"
#include "VertexMSO.h"

MSMass::MSMass(int _massIndex, float _mass, Vector3 _pos, MSMesh* _mesh, bool _fixed, bool _exteriorForces)
{
	m_index = _massIndex;
	m_mass = _mass;
	m_orignalPos = _pos;
	m_pos = _pos;
	m_mesh = _mesh;
	m_fixed = _fixed;
	m_exteriorForces = _exteriorForces;

	//m_displacement = Vector3::Zero;
	m_velocity = Vector3::Zero;
	m_oldVelocity = Vector3::Zero;
}

MSMass::~MSMass()
{
	m_mesh = nullptr;
	//m_springs.clear();
	m_boundVertices.clear();
}

void MSMass::CalculateDisplacement(GlobalData* _GD)
{
	Vector3 overallAcceleration = m_velocity;
	m_oldVelocity = m_velocity;

	if (!m_fixed)
	{
		if (m_exteriorForces)
		{
			overallAcceleration += _GD->m_physicsManager->GetGravityForce();
			overallAcceleration += _GD->m_physicsManager->GetWindForce() / m_mass;
			overallAcceleration += _GD->m_physicsManager->CalculateColliderForce(m_pos, m_oldVelocity);
			if (m_mesh->Get3D())
			{
				overallAcceleration += _GD->m_physicsManager->m_physicsData.m_pressureCoefficient * m_normal;
			}
		}

		overallAcceleration += m_springForce / m_mass;

		//Converts force to velocity
		m_velocity = (m_oldVelocity + (overallAcceleration *_GD->m_dt)) - _GD->m_physicsManager->m_physicsData.m_dragCoefficient * m_velocity;

		//Limits velocity based on user defined upper velocity limit
		if (m_velocity.Length() > _GD->m_physicsManager->m_physicsData.m_velocityLimit)
		{
			m_velocity.Normalize();
			m_velocity *= _GD->m_physicsManager->m_physicsData.m_velocityLimit;
		}

		m_displacement = m_velocity * _GD->m_dt;
	}
	else
	{
		if (m_mesh->GetParent() != nullptr)
		{
			/*m_velocity = m_mesh->GetParent()->GetVelocity() * m_mesh->GetParent()->GetScale();
			m_displacement = m_velocity * _GD->m_dt;*/
		}
	}
}

void MSMass::ApplyDisplacement()
{
	m_pos = m_pos + m_displacement;

	for (auto it = m_boundVertices.begin(); it != m_boundVertices.end(); it++)
	{
		m_mesh->GetParent()->SetVertexPos((*it), (m_mesh->GetParent()->GetVertexPos((*it)) + m_displacement));
	}
}

void MSMass::BindVertex(int _vertexIndex)
{
	m_boundVertices.push_back(_vertexIndex);
	return;
}

void MSMass::UnbindVertex(int _vertexIndex)
{
	list<int>::iterator iter;

	for (auto it = m_boundVertices.begin(); it != m_boundVertices.end(); it++)
	{
		if ((*it) == _vertexIndex)
		{
			iter = it;
		}
	}

	m_boundVertices.remove((*iter));
}