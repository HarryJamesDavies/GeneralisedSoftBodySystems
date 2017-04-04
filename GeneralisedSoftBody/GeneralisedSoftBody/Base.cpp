#include "Base.h"
#include "DrawData2D.h"
#include "DrawData3D.h"
#include "GlobalData.h"
#include "PhysicsData.h"
#include "FollowCamera.h"
#include "Light.h"
#include "VertexGO.h"
#include <AntTweakBar.h>

#include "VertexMSO.h"
#include "Player.h"

#include "GUIManager.h"

Base::Base(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance)
{
	m_globalData = new GlobalData();
	m_globalData->m_playTime = 0;
	m_globalData->m_gameObjectPool = std::make_unique<GameObjectPool>();
	m_globalData->m_IDevice = _pd3dDevice;

	InitialisePhysics();

	m_globalData->m_MSOManager = std::make_unique<MSOManager>(m_globalData);
	m_globalData->m_MSOManager->m_currentType = MSOTypes::CUBE;
	m_globalData->m_MSOManager->CreateMSOPrimitive(m_globalData);

	InitialiseRender(_hWnd);
	InitialiseUI();

	m_globalData->m_keyboard = std::make_unique<Keyboard>();
	m_globalData->m_mouse = std::make_unique<Mouse>();
	m_globalData->m_mouse->SetWindow(_hWnd);
}

void Base::InitialiseRender(HWND _hWnd)
{
	//Create DirectXTK spritebatch stuff
	ID3D11DeviceContext* pd3dImmediateContext;
	m_globalData->m_IDevice->GetImmediateContext(&pd3dImmediateContext);
	m_globalData->m_IDeviceContext = pd3dImmediateContext;
	m_globalData->m_DD2D = new DrawData2D();
	m_globalData->m_DD2D->m_Sprites.reset(new SpriteBatch(m_globalData->m_IDeviceContext));
	m_globalData->m_DD2D->m_Font.reset(new SpriteFont(m_globalData->m_IDevice, L"italic.spritefont"));

	//init render system for VBGOs
	VertexGO::Init(m_globalData->m_IDevice);

	//calculate aspect ratio
	RECT rc;
	GetClientRect(_hWnd, &rc);
	m_globalData->m_width = rc.right - rc.left;
	m_globalData->m_height = rc.bottom - rc.top;
	float AR = (float)m_globalData->m_width / (float)m_globalData->m_height;

	//set up DirectXTK Effects system
	m_globalData->m_fxFactory = new EffectFactory(m_globalData->m_IDevice);

	//Tell the fxFactory to look to the correct build directory to pull stuff in from
#ifdef DEBUG
	((EffectFactory*)m_globalData->m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_globalData->m_fxFactory)->SetDirectory(L"../Release");
#endif

	InitialisePlayer();

	FollowCamera* m_currentCamera = new FollowCamera(m_globalData->m_gameObjectPool->GetGameObject("Player", ObjectLayer::OL_NULL),
		Vector3(150.0f, 150.0f, 150.0f), 0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY);
	m_currentCamera->SetName("MainCamera");
	m_currentCamera->SetTag(ObjectTag::OT_MAINCAMERA);
	m_globalData->m_gameObjectPool->StoreGameObject(m_currentCamera);
	Light* m_light = new Light(Vector3(-50.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.6f, 0.4f, 0.4f, 1.0f));
	m_globalData->m_gameObjectPool->StoreGameObject(m_light);

	//create DrawData3D struct and populate its pointers
	m_globalData->m_DD3D = new DrawData3D;
	m_globalData->m_DD3D->m_pd3dImmediateContext = nullptr;
	m_globalData->m_states = new CommonStates(m_globalData->m_IDevice);
	m_globalData->m_DD3D->m_states = m_globalData->m_states;
	m_globalData->m_DD3D->m_cam = m_currentCamera;
	m_globalData->m_DD3D->m_light = m_light;

	return;
}

void Base::InitialiseUI()
{
	m_globalData->m_GUIManager = std::make_unique<GUIManager>();
	m_globalData->m_GUIManager->Initialise(m_globalData);
	return;
}

void Base::InitialisePhysics()
{
	m_globalData->m_physicsManager = std::make_unique<PhysicsManager>();
	m_globalData->m_physicsManager->m_physicsData.m_dragCoefficient = 0.5f;
	m_globalData->m_physicsManager->m_physicsData.m_gravityCoefficient = 0.0f;
	m_globalData->m_physicsManager->m_physicsData.m_velocityLimit = 100.0f;
	m_globalData->m_physicsManager->m_physicsData.m_springCoefficient = 25.0f;
	m_globalData->m_physicsManager->m_physicsData.m_dampingCoefficient = 5.0f;
	m_globalData->m_physicsManager->m_physicsData.m_pressureCoefficient = 20.0f;
	m_globalData->m_physicsManager->m_physicsData.m_forceFieldCoefficient = 10.0f;
	m_globalData->m_physicsManager->m_physicsData.m_forceFieldRadius = 15.0f;
	m_globalData->m_physicsManager->m_physicsData.m_windCoefficientMin = 0.0f;
	m_globalData->m_physicsManager->m_physicsData.m_windCoefficientMax = 0.0f;
	m_globalData->m_physicsManager->m_physicsData.m_windDirection = Vector3(0.0f, 0.0f, 90.0f);

	return;
}

void Base::InitialisePlayer()
{
	Player* player = new Player(50.0f, 2.0f, "BirdModelV1.cmo", m_globalData);
	m_globalData->m_gameObjectPool->StoreGameObject(player);

	return;
}

Base::~Base()
{
	delete m_globalData->m_states;
	delete m_globalData->m_fxFactory;
	delete m_globalData->m_DD2D;
	delete m_globalData->m_DD3D;
	delete m_globalData;

	VertexGO::CleanUp();
}

bool Base::Update()
{
	auto kb = m_globalData->m_keyboard->GetState();
	if (kb.Escape)
		PostQuitMessage(0);

	auto mouse = m_globalData->m_mouse->GetState();

	if (!m_globalData->m_pause)
	{
		//calculate frame time-step dt for passing down to game objects
		DWORD currentTime = GetTickCount();
		m_globalData->m_dt = min((float)(currentTime - m_globalData->m_playTime) / 1000.0f, 0.1f);
		m_globalData->m_playTime = currentTime;

		m_globalData->m_physicsManager->UpdatePhysics(m_globalData->m_dt);
		m_globalData->m_GUIManager->Update();

		m_globalData->m_gameObjectPool->Tick(m_globalData);
	}

	return true;
}

void Base::Render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	m_globalData->m_DD3D->m_pd3dImmediateContext = _pd3dImmediateContext;

	VertexGO::UpdateConstantBuffer(m_globalData->m_DD3D);

	m_globalData->m_gameObjectPool->Draw(m_globalData->m_DD3D);

	TwDraw();

	return;
}