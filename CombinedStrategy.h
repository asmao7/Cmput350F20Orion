
#include "OrionBot.h"
#include <iostream>

// some random
/*
void OrionBot::CombinedBuild() {
	switch (FINALSTRATEGY_STATE.currentBuild) {
	case STAGE1_FINALSTRATEGY:
		OrionBot::setChokePoints();
		if (Observation()->GetMinerals() >= 100) {
			OrionBot::TryBuildStructureAtCP(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, FINALSTRATEGY_STATE.tobuildSD);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 1) {
				if (Observation()->GetMinerals() >= 75) {
					const ObservationInterface* observation = Observation();
					Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
					if (!workers.empty()) {
						bool vespene_target = false;
						FindNearestVespeneGeyser(workers.front()->pos);
					}
				}
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) {
			OrionBot::TryBuildStructureAtCP(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV, FINALSTRATEGY_STATE.tobuildRaxs);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			FINALSTRATEGY_STATE.upgradeOrbital = true;
		}

		if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
			FINALSTRATEGY_STATE.currentBuild++;
		}
		break;

	//TRY EXOAND TO EXPANSION LOCATION AND TO THE OTHER CORNERS
	case STAGE2_FINALSTRATEGY:
		TryBuildCommandCentreExpansion(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
		FINALSTRATEGY_STATE.expand = true;
		if ((FINALSTRATEGY_STATE.newCommandCentre == true)) {
			FINALSTRATEGY_STATE.currentBuild++;
			break;
		}

	//JUST BUILD MORE SUPPLY DEPOTS AND BARRACKS FOR NOW
	case STAGE3_FINALSTRATEGY:
		TryBuildSupplyDepot();
		TryBuildBarracks();
		break;

}
*/