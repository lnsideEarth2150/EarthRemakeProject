#include "e2150/MovingUnit.h"

#include "e2150/Unit.h"
#include "e2150/UnitChassis.h"
#include "e2150/Map.h"
#include "e2150/Utils.h"

MovingUnit::MovingUnit(Unit& unit, uint8_t direction, uint32_t currentTime, Map& map) :
	 unit(unit),
	 direction(direction),
	 startTime(currentTime),
	 turn(false)
	 {
		startMove(direction, currentTime, map);
}

MovingUnit::~MovingUnit() {
	unit.setStatusFlag(Unit::FLAG_ONMOVE, false);
}

void MovingUnit::startMove(uint8_t direction, uint32_t currentTime, Map& map) {
	unit.setStatusFlag(Unit::FLAG_ONMOVE, true);

	if (unit.getDirection() != direction) {
		turn = true;
	} else { // Wenn wir auf ein anderes Feld fahren, belegen wir dieses
		turn = false;
		map.setFieldStatusFlag(map.position(unit.getX(), unit.getY()), Map::STATUS_UNIT, true);
	}
}

void MovingUnit::finishMove(uint32_t currentTime, Map& map) {
	unit.setStatusFlag(Unit::FLAG_ONMOVE, false);

	if (turn) {
		unit.setDirection(direction);
	} else {
		// Vorheriges Feld freigeben
		map.setFieldStatusFlag(map.position(unit.getX(), unit.getY()), Map::STATUS_UNIT, false);
		// Und Einheit bewegen
		unit.move(direction);
	}
}

uint32_t MovingUnit::getFinishTime() const {
	if (turn) {
		uint16_t angle = Utils::getAngleDifference(unit.getDirection(), direction) * 45;
		return startTime + unit.getChassis().getTurnRate() * angle;
	}

	//Pr�fe ob wir Diagonal fahren
	if (direction && 1) {
		return startTime + unit.getChassis().getMoveRate() * 1.4142135623730950488016887242097;
	}

	//Also fahren wir gerade
	return startTime + unit.getChassis().getMoveRate();

}