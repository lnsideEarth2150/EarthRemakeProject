#ifndef TESTGAMESTATE_INCLUDED
#define TESTGAMESTATE_INCLUDED

#include "client/AbstractGameState.h"

#include <position2d.h>

namespace irr {
class IEventReceiver;
namespace scene {
class ISceneManager;
class ICameraSceneNode;
}
}

class TestGameState : public AbstractGameState {
	private:
		TestGameState(const TestGameState&);
		TestGameState& operator=(const TestGameState&);

		/// Aktive Kamera
		irr::scene::ICameraSceneNode* camera;

		/// Speichere die Maus Position für die GUI behandlung
		irr::core::position2di mousePosition;

		/// Event Receiver für die GUI
		irr::IEventReceiver* subEventReceiver;

		/// Erstellt die Frei bewegbare Kamera
		void createCamera(irr::scene::ISceneManager* smgr);

		/// Entfernt die Kamera
		void removeCamera();

		/// Setzt den EventReceiver an den Events weitergereicht werden, die nicht von dieser
		/// Klasse behandelt wurden
		void setSubEventReceiver(irr::IEventReceiver* receiver);

	public:
		TestGameState(irr::IrrlichtDevice* device);
		~TestGameState();

		/// Main Methode des Zustandes, Rückgabe bestimmt den Folgezustand.
		virtual AbstractGameState* run();

		/// Irrlicht Events von GUI, Maus, Tastatur ect...
		bool OnEvent(const irr::SEvent& event);

};

#endif // TESTGAMESTATE_INCLUDED