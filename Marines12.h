#include "OrionBot.h"
// 12 Marines @4:30 Strategy
// Made by Ana

/*
	STAGE 1
	9 - Supply Depot (1)
	Still produce SCVs

	STAGE 2
	11 - Barracks (1)

	Stop producing SCVs
	Start producing marines
	Allow orbital Command Upgrade

	STAGE 3
	12 - Barracks (2-3)

	Once upgraded:
		If have enough resources:
			then Calldown: MULE
			then  Scanner Sweep (in the corners)
		
	STAGE 4 ATTACK
	Attack
*/

void OrionBot::Marines12Build() {
	switch (MARINES12_STATE.current_build) {
	case STAGE1_MARINES:
		if (Observation()->GetMinerals() >= 100 && OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) <= 1) {
			OrionBot::TryBuildSupplyDepot();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			MARINES12_STATE.current_build++;
		}
		break;

	case STAGE2_MARINES:
		if (Observation()->GetMinerals() >= 150) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			// Start producing Marines, stop producing CSVs
			// Allow Orbital Upgrade
			MARINES12_STATE.produce_scv = false;
			MARINES12_STATE.produce_marine = true;
			MARINES12_STATE.orbital_upgrade = true;

			MARINES12_STATE.current_build++;
		}
		break;

	case STAGE3_MARINES:
		/*	STAGE 3
			12 - Barracks (2-3)

			Once upgraded:
				If have enough resources:
					then Calldown: MULE
					then  Scanner Sweep (in the corners)*/

		if (Observation()->GetMinerals() >= 150 && OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 4) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 3) { // NEED TO CHECK TIME 4:30 ???
			MARINES12_STATE.current_build++;
		}




		break;

	}
}

void OrionBot::Marines12OnUnitIdle(const Unit* unit) {

}