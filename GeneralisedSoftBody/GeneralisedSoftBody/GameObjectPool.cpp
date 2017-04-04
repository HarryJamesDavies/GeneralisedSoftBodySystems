#include "GameObjectPool.h"
#include "GameObject.h"
#include "GlobalData.h"
#include "ObjectLayer.h"

//========================================================================================================
//Improved system which allows groups of objects to be selected
//========================================================================================================

GameObjectPool::GameObjectPool()
{
	//Adds a new list of game objects per layer
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_NULL, new ObjectList));
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_CAMERA, new ObjectList));
	m_GameObjects.insert(ListLayer(ObjectLayer::OL_LIGHT, new ObjectList));
}

GameObjectPool::~GameObjectPool()
{
	//Deletes all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			delete(*listIt);
		}
		it->second->clear();
	}
	m_GameObjects.clear();
}

GameObject* GameObjectPool::GetGameObject(std::string _objectName, ObjectLayer _layer)
{
	//Gets a specific object by passing in the object's name(GameObject2D)
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		if ((*it)->GetName() == _objectName)
		{
			return (*it);
		}
	}
	return nullptr;
}

void GameObjectPool::StoreGameObject(GameObject * _gameObject)
{
	//Stores an object on the relative layer, specify in the objects constructor
	ObjectLayer layer = _gameObject->GetLayer();
	m_GameObjects.find(layer)->second->push_back(_gameObject);
	return;
}

void GameObjectPool::TickObject(GlobalData* _GD, std::string _objectName)
{
	//Draws all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				(*listIt)->Tick(_GD);
			}
		}
	}
}

void GameObjectPool::TickLayer(GlobalData* _GD, ObjectLayer _layer)
{
	//Draws all 2D game objects on a set layer
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		(*it)->Tick(_GD);
	}
}

void GameObjectPool::Tick(GlobalData* _GD)
{
	//Ticks all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			(*listIt)->Tick(_GD);
		}
	}
}

void GameObjectPool::DrawObject(DrawData3D* _DD, std::string _objectName)
{
	//Draws all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				(*listIt)->Draw(_DD);
			}
		}
	}
}

void GameObjectPool::DrawLayer(DrawData3D* _DD, ObjectLayer _layer)
{
	//Draws all 2D game objects on a set layer
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		(*it)->Draw(_DD);
	}
}

void GameObjectPool::Draw(DrawData3D* _DD)
{
	//Draws all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			(*listIt)->Draw(_DD);
		}
	}
}

void GameObjectPool::SetDrawable(bool _drawable, std::string _objectName, ObjectLayer _layer)
{
	//Draws all 2D game objects on a set layer
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		if ((*it)->GetName() == _objectName)
		{
			(*it)->SetDrawable(_drawable);
		}
	}
}

void GameObjectPool::DeleteObject(std::string _objectName)
{
	GameObject* objectToRemove = nullptr;
	ObjectLayer layer = ObjectLayer::OL_NULL;

	//Draws all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			if ((*listIt)->GetName() == _objectName)
			{
				layer = it->first;
				objectToRemove = (*listIt);
			}
		}
	}

	if (objectToRemove != nullptr)
	{
		m_GameObjects.find(layer)->second->remove(objectToRemove);
	}
	objectToRemove = nullptr;
}

void GameObjectPool::DeleteLayer(ObjectLayer _layer)
{
	//Draws all 2D game objects on a set layer
	for (auto it = GetObjectListBegin(_layer); it != GetObjectListEnd(_layer); it++)
	{
		delete (*it);
	}
	m_GameObjects.find(_layer)->second->clear();

	return;
}

void GameObjectPool::Delete()
{
	//Draws all 2D game objects
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		for (auto listIt = it->second->begin(); listIt != it->second->end(); listIt++)
		{
			delete (*listIt);
		}
		it->second->clear();
	}
}