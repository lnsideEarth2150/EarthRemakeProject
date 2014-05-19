#include "client/GameState/TestGameState.h"

#include "client/VisualMap.h"
#include "client/MapMarker.h"
#include "GUI/IngameGUI.h"
#include "GUI/IngameGUIEventReceiver.h"
#include "renderer/NormalScreenRenderer.h"
#include "GUI/ResizeEvent.h"

#include "tf/time.h"

#include <irrlicht.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

TestGameState::TestGameState(irr::IrrlichtDevice* device) :
	AbstractGameState(device),
	camera(0),
	mousePosition(),
	subEventReceiver(0) {
}

TestGameState::~TestGameState() {
	removeCamera();
}

// Temp, Todo: In eigene Klasse Packen
static const irr::s32 ID_MAPPICK = 1 << 0;

AbstractGameState* TestGameState::run() {
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	 // Kleines Text Feld zur FPS Anzeige erstellen
	IGUIStaticText* fpsDisplay = guienv->addStaticText(L"FPS: -", rect<int>(10,10,100,22), true);
	IGUIStaticText* frameTimeDisplay = guienv->addStaticText(L"", rect<s32>(10, 25, 100, 37), true);
	IGUIStaticText* renderTimeDisplay = guienv->addStaticText(L"", rect<s32>(10, 40, 100, 52), true);

	// Erstelle Kamera
	createCamera(smgr);

	// Karte erstellen
	VisualMap map(driver, smgr, 1024, 1024);

	// Raw-Heightmap laden
	if (!map.loadHeightMapRAW("map1024x1024.bin"))
		return 0;

	// Selections-ID setzen
	map.setMeshID(ID_MAPPICK);

	// Komplette Map als Mesh aufbauen
	map.build();

	// Lichtquelle erstellen, Radius und Typ festlegen
	scene::ILightSceneNode* light = smgr->addLightSceneNode();
	light->setRadius(1024);
	light->setLightType(video::ELT_DIRECTIONAL);

	// Dieses Licht automatisch Rotieren lassen (Ausrichtung ändern, da Direktionales Licht)
	scene::ISceneNodeAnimator * rotation = smgr->createRotationAnimator(vector3df(0, 0.2f, 0));
	light->addAnimator(rotation);
	rotation->drop();	// Wir brauchen das Handle nicht mehr, das Objekt (light) hat nun selbst eine referenz darauf

	// Erstelle Ingame GUI
	IngameGUI gui(guienv, device, camera);
	gui.setMapName(L"Testkarte");

	// Füge in diesen EventReceiver den GUI-EventReceiver ein (Themoräre Lösung...)
	//setSubEventReceiver(&gui);
	setSubEventReceiver(new IngameGUIEventReceiver(&gui));

	// Testweiße Marker für die Maus Position erstellen (siehe Hauptschleife)
	video::ITexture* tex = driver->getTexture("position.png");	// Textur Laden

	// Material für das Rendern bestimmen
	video::SMaterial m;

	m.AmbientColor.set(255,255,255,255);
	m.Lighting = false;
	m.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;

	m.setTexture(0, tex);

	// Den Marker mit dem Material erstellen/holen
	MapMarker* marker = map.getMapMarkerManager().getMarkerForMaterial(m);

	// Ausgabevariablen von der Zeitmessung
	uint64_t lastRenderTime = 0;
	uint64_t lastFrameTime = 0;

	uint64_t startTimeFrame;
	uint64_t endTimeFrame;

	NormalScreenRenderer renderer(device, SColor(0, 200, 200, 200));

	// Hauptschleife
	while (device->run()) {
		HighResolutionTime(&startTimeFrame);
		renderer.render();

		// Mesh Seletor Test
		core::line3d<f32> ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(mousePosition, camera);

		MapPosition pos = map.pickMapPosition(ray.start, ray.getVector());

		if (pos.isValid()) {
			marker->clear();
			marker->addField(pos);
		}

		{	// Schreibe die aktuellen FPS Werte neu in das GUI Element (Textfeld)
			core::stringw text("FPS: ");
			text += driver->getFPS();

			fpsDisplay->setText(text.c_str());
		}{
			core::stringw text("FrameTime:  ");
			text += (u32)(lastFrameTime / 1000);
			text += L"µs";

			frameTimeDisplay->setText(text.c_str());
		}{
			core::stringw text("RenderTime: ");
			text += (u32)(lastRenderTime / 1000);
			text += L"µs";

			renderTimeDisplay->setText(text.c_str());
		}

		HighResolutionTime(&endTimeFrame);

		lastRenderTime = renderer.getLastRenderTime();
		lastFrameTime = HighResolutionDiffNanoSec(startTimeFrame, endTimeFrame);

		// Wenn das Fenster den Fokus verliert, dann nicht weiter rendern
		while (!device->isWindowActive()) {
			device->sleep(10);
			device->run();
		}
	}

	return 0;
}


bool TestGameState::OnEvent(const irr::SEvent& event) {
	// Bei betätigen der rechten Maustaste zwischen Maus fangen umschalten
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.isRightPressed()) {
		camera->setInputReceiverEnabled(!camera->isInputReceiverEnabled());

		return true;
	}
	// Speichere Maus Position für weitere behandlung
	else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {

		mousePosition.X = event.MouseInput.X;
		mousePosition.Y = event.MouseInput.Y;
	}

	if (ResizeEvent::isResizeEvent(event)) {
		const core::dimension2du newSize = ResizeEvent::getResizeEventData(event);

		// Setze Aspekt Ratio der Kamera auf neue Auflösung
		camera->setAspectRatio((float)newSize.Width / (float)newSize.Height);

		return true;
	}

	if (subEventReceiver != 0)
		return subEventReceiver->OnEvent(event);

	return false;
}

void TestGameState::createCamera(irr::scene::ISceneManager* smgr) {
	// Tastenbelegung für die Kamera zuweißen
	SKeyMap keymap[8];

	// Vorwärts
	keymap[0].Action = keymap[1].Action = EKA_MOVE_FORWARD;
	keymap[0].KeyCode = KEY_UP; keymap[1].KeyCode = KEY_KEY_W;

	// Rückwärts
	keymap[2].Action = keymap[3].Action = EKA_MOVE_BACKWARD;
	keymap[2].KeyCode = KEY_DOWN; keymap[3].KeyCode = KEY_KEY_S;

	// Nach rechts
	keymap[4].Action = keymap[5].Action = EKA_STRAFE_RIGHT;
	keymap[4].KeyCode = KEY_RIGHT; keymap[5].KeyCode = KEY_KEY_D;

	// Nach links
	keymap[6].Action = keymap[7].Action = EKA_STRAFE_LEFT;
	keymap[6].KeyCode = KEY_LEFT; keymap[7].KeyCode = KEY_KEY_A;

	// FirstPerson Kamera erstellen (Steuerung mit Maus + Pfeiltasten)
	camera = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.25f, -1, keymap, 8);

	camera->setPosition(core::vector3df(100, 100, 100));
	camera->setTarget(core::vector3df(512, 0, 512));
}

void TestGameState::removeCamera() {
	camera->remove();
	camera = 0;
}

void TestGameState::setSubEventReceiver(irr::IEventReceiver* receiver) {
	subEventReceiver = receiver;
}