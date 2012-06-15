#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"

#define FLAG_INBUILDING 1

class Unit : public Entity {
    private:
        char controlableFlags;
    public:
        Unit();
        virtual ~Unit();

        /**
        * Gibt an ob auf Spielerbefehle direkt reagiert werden kann
        * Nicht der fall wenn Einheit aus Werkshalle fährt, unter einfluss von Signalstörern steht ect
        */
        bool isControlable() const{return !controlableFlags;} //controlableFlags == 0
};

#endif
