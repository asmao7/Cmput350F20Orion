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
	13 - Orbital Command Upgrade

	If have enough resources:
		@100% Orbital Command - Calldown: Extra Supplies (1)
		then Calldown: MULE
		then  Scanner Sweep (in the corners)

	STAGE 3
	12 - Marines
	12 - Barracks (2-3)
	Stop producing SCVs

	If have enough resources:
		then Calldown: MULE
		then  Scanner Sweep (in the corners)
		
	STAGE 4
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
	}
}

void OrionBot::Marines12OnUnitIdle(const Unit* unit) {

}