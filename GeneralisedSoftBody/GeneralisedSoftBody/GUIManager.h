#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_
#include <windows.h>
#include "Effects.h"
#include <AntTweakBar.h>

using namespace std;
using namespace DirectX;

struct GlobalData;

class GUIManager
{
public:
	GUIManager();
	~GUIManager();

	void Initialise(GlobalData* _GD);

	void Update();

private:
	void CreateMSOManagerBar();
	void CreatePhysicsBar();

	void CreateFileBar();
	void CreateFBXBar();

	void CreateChainBar();
	void CreateSquareBar();
	void CreateCubeBar();

	TwBar *msoManagerBar;
	TwBar *msoBar;
	TwBar *physicsBar;

	GlobalData* m_GD;

	bool m_pauseOnSpawn;

	bool m_createFile;
	bool m_spawnFile;

	bool m_createFBX;
	bool m_spawnFBX;

	bool m_createPrimitive;
	bool m_spawnPrimitive;

	bool m_savePrimitive;

	char* m_fileName;
};

#endif;
