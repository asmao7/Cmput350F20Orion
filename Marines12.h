#include "OrionBot.h"
// 12 Marines @4:30 Strategy
// Made by Ana

/*
	STAGE 1
	9 - Supply Depot (1)
	Still produce SCVs

	STAGE 2
	11 - Barracks (1)

	STAGE 3
	Stop producing SCVs
	13 - Orbital Command Upgrade

	STAGE 4
	12 - Marines
	12 - Barracks (2-3)

	If have enough resources:
		then Calldown: MULE
		then  Scanner Sweep (in the corners)
		
	STAGE 5 ATTACK
	Attack
*/

void OrionBot::Marines12Build() {
	switch (MARINES12_STATE.current_build) {
	case STAGE1_MARINES:
		if (Observation()->GetMinerals() >= 100) {
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
			MARINES12_STATE.current_build++;
		}
		break;


	}
}

void OrionBot::Marines12OnUnitIdle(const Unit* unit) {

}