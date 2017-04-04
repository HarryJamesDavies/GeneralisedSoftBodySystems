#include "MSOManager.h"
#include "MSOFile.h"
#include "MSOFBX.h"
#include "MSOChain.h"
#include "MSOSquare.h"
#include "MSOCube.h"
#include "MSMesh.h"
#include "GlobalData.h"
#include "ObjectLayer.h"

#include <fstream>
#include <sstream>
#include <string>

MSOManager::MSOManager(GlobalData* _GD)
{
	m_width = 15;
	m_height = 15;
	m_depth = 15;

	m_prevType = MSOTypes::SQUARE;
	m_currentType = MSOTypes::SQUARE;
	m_currentMSOName = "";

	//CreateMSOPrimitive(_GD);
}

MSOManager::~MSOManager()
{
}

void MSOManager::CreateMSOPrimitive(GlobalData* _GD)
{
	DeleteMSO(_GD);

	switch (m_currentType)
	{
	case MSOTypes::CHAIN:
	{
		MSOChain* MSChain = new MSOChain(m_width, m_height, m_sectionsX, m_internal, _GD);
		_GD->m_gameObjectPool->StoreGameObject(MSChain);
		m_currentMSOName = MSChain->GetName();
		break;
	}
	case MSOTypes::SQUARE:
	{
		MSOSquare* MSSquare = new MSOSquare(m_width, m_height, Vector2(static_cast<float>(m_sectionsX), static_cast<float>(m_sectionsY)), m_internal, _GD);
		_GD->m_gameObjectPool->StoreGameObject(MSSquare);
		m_currentMSOName = MSSquare->GetName();
		break;
	}
	case MSOTypes::CUBE:
	{
		MSOCube* MSCube = new MSOCube(m_width, m_height, m_depth, Vector3(static_cast<float>(m_sectionsX), static_cast<float>(m_sectionsY), static_cast<float>(m_sectionsZ)), m_internal, _GD);
		_GD->m_gameObjectPool->StoreGameObject(MSCube);
		m_currentMSOName = MSCube->GetName();
		break;
	}
	default:
		break;
	}

	return;
}

void MSOManager::CreateMSOFile(const char* _fileName, std::string _name, GlobalData* _GD)
{
	DeleteMSO(_GD);

	std::ifstream infile;
	infile.open(_fileName);

	if (infile.good())
	{
		MSOFile* MSFile = new MSOFile(_fileName, _name, _GD);
		_GD->m_gameObjectPool->StoreGameObject(MSFile);
		m_currentMSOName = MSFile->GetName();
	}

	return;
}

void MSOManager::CreateMSOFBX(const char* _fileName, std::string _name, GlobalData* _GD)
{
	DeleteMSO(_GD);

	MSOFBX* MSFBX = new MSOFBX(_fileName, _name, m_internal, _GD);
	_GD->m_gameObjectPool->StoreGameObject(MSFBX);
	m_currentMSOName = MSFBX->GetName();

	return;
}

void MSOManager::DeleteMSO(GlobalData* _GD)
{
	_GD->m_gameObjectPool->DeleteObject(m_currentMSOName);
	return;
}

void MSOManager::SaveMSO(GlobalData* _GD)
{
	VertexMSO* object = (VertexMSO*)_GD->m_gameObjectPool->GetGameObject(m_currentMSOName, ObjectLayer::OL_NULL);
	object->GetMesh()->SaveMesh();
}