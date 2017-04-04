#include "MSMesh.h"
#include "MSMass.h"
#include "MSSpring.h"
#include "VertexMSO.h"
#include "GlobalData.h"
#include "Triangle.h"
#include "Edge.h"
#include "MSTriangle.h"
#include "MSEdge.h"
#include "TriangulateMasses.h"
#include <fstream>
#include <sstream>
#include <string>

#include <iostream>
#include <fstream>

MSMesh::MSMesh(VertexMSO* _parent, int _fixAll, FixFormat _format, FixDimension _dimension)
{
	m_parent = _parent;
	m_verticesToAssign.clear();

	m_fixIndex = _fixAll;
	m_fixFormat = _format;
	m_fixDimension = _dimension;
}
MSMesh::~MSMesh()
{
	m_masses.clear();
	m_verticesToAssign.clear();
	m_parent = nullptr;
}

void MSMesh::CreateMasses1D(int _maxVertices, int _width, int _height, int _numSectionsX)
{
	int numberOfVetricesPerSection = _maxVertices / _numSectionsX;

	int index = 0;
	int currentSection = 0;
	Vector3 massPos = Vector3(static_cast<float>((_width / 2)), static_cast<float>(-(currentSection * _height)), 0.0f);
	bool fixed = true;

	for (int i = 0; i <= _numSectionsX; i++)
	{
		massPos.y = static_cast<float>(-currentSection * _height);

		m_masses.push_back(new MSMass(index, 1.0f, massPos, this, fixed, true));

		if (i != _numSectionsX)
		{
			m_verticesToAssign.push_back(0 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(2 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(3 + (currentSection * numberOfVetricesPerSection));
		}

		if (i != 0)
		{
			m_verticesToAssign.push_back(-1 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(-5 + (currentSection * numberOfVetricesPerSection));
			m_verticesToAssign.push_back(-2 + (currentSection * numberOfVetricesPerSection));
		}

		AssignVertices(m_masses.back());

		index++;
		currentSection++;
		fixed = false;
	}

	return;
}

void MSMesh::CreateSurfaceMasses(int _maxVertices, int _maxTriangles, std::list<Triangle*>::iterator _trianglesIter, std::list<Triangle*>::iterator _trianglesEnd)
{
	TriangulateMasses* math = new TriangulateMasses();
	math->CalculateTriangles(this, _maxVertices, _maxTriangles, _trianglesIter, _trianglesEnd);
	delete(math);

	return;
}

void MSMesh::CreateInternalMasses()
{
	Vector3 massPos = Vector3::Zero;

	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		massPos = (*massIter)->GetPos() + (*massIter)->GetNormal();
		m_internalMasses.push_back(new MSMass((*massIter)->GetIndex(), 1.0f, massPos, this, false, true));

		(*massIter)->AssignOpposingMass(m_internalMasses.back());
		m_internalMasses.back()->AssignOpposingMass((*massIter));
	}

	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		for (auto neighbourIter = (*massIter)->GetOpposingMass()->GetNeighboursBeginning(); 
			neighbourIter != (*massIter)->GetOpposingMass()->GetNeighbourEnd(); neighbourIter++)
		{
			(*massIter)->AddNeighbour((*neighbourIter)->GetOpposingMass());
		}
	}

	return;
}

void MSMesh::AssignVertices(MSMass* _mass)
{
	for (auto it = m_verticesToAssign.begin(); it != m_verticesToAssign.end(); it++)
	{
		_mass->BindVertex((*it));
	}

	m_verticesToAssign.clear();
}

void MSMesh::CreateSprings1D(int _numSectionsX)
{
	int massAIter = 1;
	int massBIter = 0;

	MSMass* massA = GetMass(massAIter);
	MSMass* massB = GetMass(massBIter);
	m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));
	//massA->AddSpring(massB);

	for (int iter = 1; iter <= _numSectionsX - 1; iter++)
	{
		massAIter++;
		
		massA = GetMass(massAIter);
		massB = GetMass(massBIter);
		m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));

		massBIter++;

		massA = GetMass(massAIter);
		massB = GetMass(massBIter);
		m_springs.push_back(new MSSpring(massA, massB, m_springs.size()));
	}

	return;
}

void MSMesh::CreateSurfaceSprings()
{
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}
	}

	int iter = 0;
	iter++;

	return;
}

void MSMesh::CreateInternalSprings()
{
	MSMass* massA = nullptr;
	MSMass* massB = nullptr;

	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}

		m_springs.push_back(new MSSpring((*massIter), (*massIter)->GetOpposingMass(), m_springs.size()));

		for (auto neighbourIter = (*massIter)->GetOpposingMass()->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetOpposingMass()->GetNeighbourEnd(); neighbourIter++)
		{
			if (!CheckExistingSpring((*massIter), (*neighbourIter)))
			{
				m_springs.push_back(new MSSpring((*massIter), (*neighbourIter), m_springs.size()));
			}

			for (auto neighboursNeighbourIter = (*neighbourIter)->GetNeighboursBeginning(); neighboursNeighbourIter != (*neighbourIter)->GetNeighbourEnd(); neighboursNeighbourIter++)
			{
				if (!CheckExistingSpring((*massIter), (*neighboursNeighbourIter)))
				{
					m_springs.push_back(new MSSpring((*massIter), (*neighboursNeighbourIter), m_springs.size()));
				}
			}
		}
	}
}

bool MSMesh::CheckExistingSpring(MSMass* _massA, MSMass* _massB)
{
	for (auto springIter = m_springs.begin(); springIter != m_springs.end(); springIter++)
	{
		if ((*springIter)->ContainsMass(_massA) && (*springIter)->ContainsMass(_massB))
		{
			return true;
		}
	}
	return false;
}

void MSMesh::UpdateNormals()
{
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetNormal(Vector3::Zero);
	}

	for (auto triangleIter = m_triangles.begin(); triangleIter != m_triangles.end(); triangleIter++)
	{
		CalculateNormals((*triangleIter));
	}

	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetNormal((*massIter)->GetNormal());
	}

	//Internals
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		(*massIter)->SetNormal(Vector3::Zero);
	}

	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		Vector3 normal = (*massIter)->GetOpposingMass()->GetPos() - (*massIter)->GetPos();
		normal.Normalize();
		(*massIter)->SetNormal(normal);
	}
}

void MSMesh::UpdateSprings(GlobalData* _GD)
{
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->SetSpringForce(Vector3(0.0f, 0.0f, 0.0f));
	}

	for (auto it = m_springs.begin(); it != m_springs.end(); it++)
	{
		(*it)->CalculateForce(_GD);
		(*it)->CalculateNormal();
	}
}

void MSMesh::UpdateMasses(GlobalData* _GD)
{
	/*CalculateVolume();
	CalculatePressure(_GD);*/

	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->CalculateDisplacement(_GD);
	}

	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		(*it)->CalculateDisplacement(_GD);
	}
}

void MSMesh::ApplyChanges()
{
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		(*it)->ApplyDisplacement();
	}

	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		(*it)->ApplyDisplacement();
	}
}

MSMass* MSMesh::GetMass(int _index)
{
	for (auto it = m_masses.begin(); it != m_masses.end(); it++)
	{
		if ((*it)->GetIndex() == _index)
		{
			return (*it);
		}
	}

	return nullptr;
}

MSMass* MSMesh::GetExternalMass(int _index)
{
	for (auto it = m_internalMasses.begin(); it != m_internalMasses.end(); it++)
	{
		if ((*it)->GetIndex() == _index)
		{
			return (*it);
		}
	}

	return nullptr;
}

void MSMesh::CalculateVolume()
{
	m_volume = 0.0f;

	for (auto it = m_springs.begin(); it != m_springs.end(); it++)
	{
		Vector3 endPoint1 = (*it)->GetMassA()->GetPos();
		Vector3 endPoint2 = (*it)->GetMassB()->GetPos();

		float distance = sqrt(Vector3::DistanceSquared(endPoint1, endPoint2));

		m_volume += 0.5f * fabs(endPoint1.x - endPoint2.x) * fabs((*it)->GetNormal().x) * distance;
	}
}

void MSMesh::CalculatePressure(GlobalData* _GD)
{
	m_pressure = 0.0f;

	for (auto it = m_springs.begin(); it != m_springs.end(); it++)
	{
		Vector3 endPoint1 = (*it)->GetMassA()->GetPos();
		Vector3 endPoint2 = (*it)->GetMassB()->GetPos();

		float distance = sqrt(Vector3::DistanceSquared(endPoint1, endPoint2));

		m_pressure += distance * _GD->m_physicsManager->m_physicsData.m_pressureCoefficient * (1.0f / m_volume);

		(*it)->SetMassPressure(m_pressure);
	}
}

void MSMesh::CalculateNormals(MSTriangle* _triangle)
{
	MSMass* massA = GetMass(_triangle->m_massA);
	MSMass* massB = GetMass(_triangle->m_massB);
	MSMass* massC = GetMass(_triangle->m_massC);

	//build normals
	Vector3 norm;
	Vector3 vec1 = massB->GetPos() - massA->GetPos();
	Vector3 vec2 = massC->GetPos() - massA->GetPos();
	norm = vec1.Cross(vec2);
	norm.Normalize();

	float angle = CalculateAngle(massA->GetPos(), massB->GetPos(), massC->GetPos());
	massA->AddNormal(norm * angle);

	angle = CalculateAngle(massB->GetPos(), massC->GetPos(), massA->GetPos());
	massB->AddNormal(norm * angle);

	angle = CalculateAngle(massC->GetPos(), massA->GetPos(), massB->GetPos());
	massC->AddNormal(norm * angle);
}

float MSMesh::CalculateAngle(Vector3 _angleVertex, Vector3 _nextVertexA, Vector3 _prevVertex)
{
	Vector3 edgeA = _angleVertex - _nextVertexA;
	Vector3 edgeB = _angleVertex - _prevVertex;

	edgeA.Normalize();
	edgeB.Normalize();

	float angle = edgeA.Dot(edgeB);

	if (angle > 1.0f)
	{
		angle = 1.0f;
	}
	else if (angle < -1.0f)
	{
		angle = -1.0f;
	}

	return acos(angle);
}

void MSMesh::ResetMasses()
{
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		(*massIter)->SetPos((*massIter)->GetOrignalPos());
		(*massIter)->SetSpringForce(Vector3::Zero);
		(*massIter)->SetVelocity(Vector3::Zero);
	}

	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		(*massIter)->SetPos((*massIter)->GetOrignalPos());
		(*massIter)->SetSpringForce(Vector3::Zero);
		(*massIter)->SetVelocity(Vector3::Zero);
	}
}

void MSMesh::CreateFromFile(const char* _fileName)
{
	std::ifstream infile;
	infile.open(_fileName);

	while (infile.good())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "SURMASSES:")
			{
				int numberOfSurfaceMasses = 0;
				in >> numberOfSurfaceMasses;
				for (int surfaceIter = 0; surfaceIter <= numberOfSurfaceMasses - 1; surfaceIter++)
				{
					Vector3 pos = Vector3(0.0, 0.0, 0.0);
					float mass = 0.0;
					int numberOfVertices = 0;
					int opposite = 0;
					int fixed = 0;

					std::getline(infile, line);

					std::getline(infile, line);
					std::istringstream posLine(line);
					posLine.ignore(line.length(), ':');
					posLine >> pos.x >> pos.y >> pos.z;

					std::getline(infile, line);
					std::istringstream massLine(line);
					massLine.ignore(line.length(), ':');
					massLine >> mass;

					std::getline(infile, line);
					std::istringstream fixedLine(line);
					fixedLine.ignore(line.length(), ':');
					fixedLine >> fixed;

					m_masses.push_back(new MSMass(surfaceIter, mass, pos, this, static_cast<bool>(fixed), true));

					std::getline(infile, line);
					std::istringstream vertexLine(line);
					vertexLine.ignore(line.length(), ':');
					vertexLine >> numberOfVertices;

					std::getline(infile, line);
					std::istringstream single(line);
					for (int vertexIter = 0; vertexIter <= numberOfVertices - 1; vertexIter++)
					{
						int vertexIndex = 0;
						single >> vertexIndex;
						m_masses.back()->BindVertex(vertexIndex);
					}

					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
				}
			}
			else if (type == "INTMASSES:")
			{
				int numberOfInternalMasses = 0;
				in >> numberOfInternalMasses;
				for (int internalIter = 0; internalIter <= numberOfInternalMasses - 1; internalIter++)
				{
					Vector3 pos = Vector3(0.0, 0.0, 0.0);
					float mass = 0.0;
					int fixed = 0;

					std::getline(infile, line);

					std::getline(infile, line);
					std::istringstream posLine(line);
					posLine.ignore(line.length(), ':');
					posLine >> pos.x >> pos.y >> pos.z;

					std::getline(infile, line);
					std::istringstream massLine(line);
					massLine.ignore(line.length(), ':');
					massLine >> mass;

					std::getline(infile, line);
					std::istringstream fixedLine(line);
					fixedLine.ignore(line.length(), ':');
					fixedLine >> fixed;

					m_internalMasses.push_back(new MSMass(internalIter, mass, pos, this, static_cast<bool>(fixed), false));

					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

				}
			}
			else if (type == "TRIANGLES:")
			{
				int numberOfTriangle = 0;
				in >> numberOfTriangle;
				for (int triangleIter = 0; triangleIter <= numberOfTriangle - 1; triangleIter++)
				{
					int massAIndex = -1;
					int massBIndex = -1;
					int massCIndex = -1;

					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');

					single >> massAIndex >> massBIndex >> massCIndex;

					MSTriangle* triangle = new MSTriangle();
					triangle->m_index = triangleIter;
					triangle->m_massA = massAIndex;
					triangle->m_massB = massBIndex;
					triangle->m_massC = massCIndex;

					MSEdge* edgeA = new MSEdge();
					edgeA->m_index = 0;
					edgeA->m_massAIndex = massAIndex;
					edgeA->m_massBIndex = massBIndex;
					edgeA->m_triangle = triangle;
					triangle->m_edgeA = edgeA;

					MSEdge* edgeB = new MSEdge();
					edgeB->m_index = 1;
					edgeB->m_massAIndex = massBIndex;
					edgeB->m_massBIndex = massCIndex;
					edgeB->m_triangle = triangle;
					triangle->m_edgeB = edgeB;

					MSEdge* edgeC = new MSEdge();
					edgeC->m_index = 2;
					edgeC->m_massAIndex = massCIndex;
					edgeC->m_massBIndex = massAIndex;
					edgeC->m_triangle = triangle;
					triangle->m_edgeC = edgeC;

					m_triangles.push_back(triangle);
				}
			}
			else if (type == "SPRINGS:")
			{
				int numberOfSprings = 0;
				in >> numberOfSprings;
				for (int springIter = 0; springIter <= numberOfSprings - 1; springIter++)
				{
					int massAIndex = -1;
					int massBIndex = -1;

					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');

					single >> massAIndex >> massBIndex;

					m_springs.push_back(new MSSpring(GetMass(massAIndex), GetMass(massBIndex), springIter));
				}
			}
		}
	}

	infile.close();

	infile.open(_fileName);

	while (infile.good())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "SURMASSES:")
			{
				int numberOfSurfaceMasses = 0;
				in >> numberOfSurfaceMasses;
				for (int surfaceIter = 0; surfaceIter <= numberOfSurfaceMasses - 1; surfaceIter++)
				{
					int numberOfNeighbours = 0;
					int opposite = 0;

					MSMass* mass = GetMass(surfaceIter);

					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');
					single >> numberOfNeighbours;
					std::getline(infile, line);
					std::istringstream neighbourLine(line);
					for (int neighbourIter = 0; neighbourIter <= numberOfNeighbours - 1; neighbourIter++)
					{
						int neighbour = -1;
						neighbourLine >> neighbour;
						mass->AddNeighbour(GetMass(neighbour));
					}

					std::getline(infile, line);
					std::istringstream oppositeLine(line);
					oppositeLine.ignore(line.length(), ':');
					oppositeLine >> opposite;
					mass->AssignOpposingMass(GetMass(opposite));
				}
			}
			else if (type == "INTMASSES:")
			{
				int numberOfInternalMasses = 0;
				in >> numberOfInternalMasses;
				for (int internalIter = 0; internalIter <= numberOfInternalMasses - 1; internalIter++)
				{
					int numberOfNeighbours = 0;
					int opposite = 0;

					MSMass* mass = GetExternalMass(internalIter);

					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);
					std::getline(infile, line);

					std::getline(infile, line);
					std::istringstream single(line);
					single.ignore(line.length(), ':');
					single >> numberOfNeighbours;
					std::getline(infile, line);
					std::istringstream neighbourLine(line);
					for (int neighbourIter = 0; neighbourIter <= numberOfNeighbours - 1; neighbourIter++)
					{
						int neighbour = -1;
						neighbourLine >> neighbour;
						mass->AddNeighbour(GetMass(neighbour));
					}

					std::getline(infile, line);
					std::istringstream oppositeLine(line);
					oppositeLine.ignore(line.length(), ':');
					oppositeLine >> opposite;
					mass->AssignOpposingMass(GetMass(opposite));
				}
			}
		}
	}

	infile.close();
}

void MSMesh::SaveMesh()
{
	std::string fileName = "../Assets/MSO/" + m_parent->GetName() + ".txt";
	std::ofstream outfile(fileName);

	outfile << m_parent->GetName() << std::endl;
	outfile << std::endl;

	outfile << "3D: " << (int)m_3D << std::endl;
	outfile << std::endl;

	outfile << "VERTICES: " << m_parent->GetVertexCount() << std::endl;
	for (int vertexIter = 0; vertexIter <= m_parent->GetVertexCount() - 1; vertexIter++)
	{
		outfile << "VERTEX " << vertexIter << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << m_parent->GetInitialVertex(vertexIter)->Pos.x << " " << m_parent->GetInitialVertex(vertexIter)->Pos.y << " " << m_parent->GetInitialVertex(vertexIter)->Pos.z << std::endl;
		outfile << "	Colour: " << std::fixed << m_parent->GetInitialVertex(vertexIter)->Color.R() << " " << m_parent->GetInitialVertex(vertexIter)->Color.G() << " " << m_parent->GetInitialVertex(vertexIter)->Color.B() << " " << m_parent->GetInitialVertex(vertexIter)->Color.A() << std::endl;
	}
	outfile << std::endl;

	outfile << "INDICES: " << m_parent->GetIndexCount() << std::endl;
	for (int indexIter = 0; indexIter <= m_parent->GetIndexCount() - 1; indexIter++)
	{
		outfile << indexIter << ": " << std::fixed << m_parent->GetIndex(indexIter) << std::endl;
	}
	outfile << std::endl;

	outfile << "SURMASSES: " << m_masses.size() << std::endl;
	for (auto massIter = m_masses.begin(); massIter != m_masses.end(); massIter++)
	{
		outfile << "SURMASS " << (*massIter)->GetIndex() << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << (*massIter)->GetOrignalPos().x << " " << (*massIter)->GetOrignalPos().y << " " << (*massIter)->GetOrignalPos().z << std::endl;
		outfile << "	Mass: " << std::fixed << (*massIter)->GetMass() << std::endl;
		outfile << "	Fixed: " << std::fixed << (int)(*massIter)->GetFixed() << std::endl;

		outfile << "	Vertices: " << std::fixed << (*massIter)->GetNumberOfVertices() << std::endl;
		outfile << "		";
		for (auto vertexIter = (*massIter)->GetVerticesBegin(); vertexIter != (*massIter)->GetVerticesEnd(); vertexIter++)
		{
			outfile << std::fixed << (*vertexIter) << " ";
		}
		outfile << std::endl;

		outfile << "	Neighbours: " << std::fixed << (*massIter)->GetNumberOfNeighbours() << std::endl;
		outfile << "		";
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			outfile << std::fixed << (*neighbourIter)->GetIndex() << " ";
		}
		outfile << std::endl;

		MSMass* opposingMass = (*massIter)->GetOpposingMass();
		if (opposingMass)
		{
			outfile << "	Opposing: " << std::fixed << opposingMass->GetIndex() << std::endl;
		}
	}
	outfile << std::endl;


	outfile << "INTMASSES: " << m_internalMasses.size() << std::endl;
	for (auto massIter = m_internalMasses.begin(); massIter != m_internalMasses.end(); massIter++)
	{
		outfile << "INTMASS " << (*massIter)->GetIndex() << ": " << std::endl;
		outfile << "	Pos: " << std::fixed << (*massIter)->GetOrignalPos().x << " " << (*massIter)->GetOrignalPos().y << " " << (*massIter)->GetOrignalPos().z << std::endl;
		outfile << "	Mass: " << std::fixed << (*massIter)->GetMass() << std::endl;
		outfile << "	Fixed: " << std::fixed << (int)(*massIter)->GetFixed() << std::endl;

		outfile << "	Neighbours: " << std::fixed << (*massIter)->GetNumberOfNeighbours() << std::endl;
		outfile << "		";
		for (auto neighbourIter = (*massIter)->GetNeighboursBeginning(); neighbourIter != (*massIter)->GetNeighbourEnd(); neighbourIter++)
		{
			outfile << std::fixed << (*neighbourIter)->GetIndex() << " ";
		}
		outfile << std::endl;

		MSMass* opposingMass = (*massIter)->GetOpposingMass();
		if (opposingMass)
		{
			outfile << "	Opposing: " << std::fixed << opposingMass->GetIndex() << std::endl;
		}
	}
	outfile << std::endl;

	outfile << "TRIANGLES: " << m_triangles.size() << std::endl;
	for (auto triangleIter = m_triangles.begin(); triangleIter != m_triangles.end(); triangleIter++)
	{
		outfile << (*triangleIter)->m_index << ": " << std::fixed << (*triangleIter)->m_massA << " " << (*triangleIter)->m_massB << " " << (*triangleIter)->m_massC << std::endl;
	}
	outfile << std::endl;

	outfile << "SPRINGS: " << m_springs.size() << std::endl;
	for (auto springIter = m_springs.begin(); springIter != m_springs.end(); springIter++)
	{
		outfile << (*springIter)->GetIndex() << ": " << std::fixed << (*springIter)->GetMassA()->GetIndex() << " " << (*springIter)->GetMassB()->GetIndex() << std::endl;
	}
	outfile << std::endl;

	outfile.close();

}