#include "client/GameState/MainMenu.h"

#include "client/GameState/TestGameState.h"

#include <irrlicht.h>

using namespace irr;

using namespace video;
using namespace gui;
using namespace core;

MainMenu::MainMenu(irr::IrrlichtDevice* device) :
	AbstractGameState(device),
	changeToState(0),
	gameStateChanged(false) {
}

AbstractGameState* MainMenu::run() {
	IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	createGUI(guienv);

	while (device->run() && !gameStateChanged) {
		driver->beginScene(true, true);

		guienv->drawAll();

		driver->endScene();
	}

	removeGUI();

	return changeToState;
}

void MainMenu::createGUI(irr::gui::IGUIEnvironment * guienv) {
	IGUIWindow* window = guienv->addWindow(recti(50, 50, 450, 650));
	guiElements[GUI_WINDOW_MAIN] = window;

	window->setDrawTitlebar(false);
	window->setDraggable(false);
	window->getCloseButton()->setVisible(false);

	guiElements[GUI_BUTTON_NEWGME] = guienv->addButton(recti(50, 50, 350, 100), window, -1,
		L"Testkarte Starten");

	IGUIButton* btn_network = guienv->addButton(recti(50, 150, 350, 200), window, -1,
		L"Netzwerktest Starten");

	btn_network->setEnabled(false);

	guiElements[GUI_BUTTON_NETWORKGAME] = btn_network;

	guiElements[GUI_BUTTON_QUIT] = guienv->addButton(recti(50, 250, 350, 300), window, -1,
		L"Anwendung Beenden");
}

void MainMenu::removeGUI() {
	guiElements[GUI_WINDOW_MAIN]->remove();
}

void MainMenu::changeGameState(AbstractGameState* newState) {
	changeToState = newState;
	gameStateChanged = true;
}

bool MainMenu::OnEvent(const SEvent& event) {
	if (event.EventType != EET_GUI_EVENT)
		return false;

	if (event.GUIEvent.EventType != gui::EGET_BUTTON_CLICKED)
		return false;

	IGUIElement* caller = event.GUIEvent.Caller;

	if (caller == guiElements[GUI_BUTTON_NEWGME]) {
		changeGameState(new TestGameState(device));
		return true;
	}

	if (caller == guiElements[GUI_BUTTON_QUIT]) {
		changeGameState(0);
		return true;
	}

	return false;
}
