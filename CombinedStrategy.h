
#include "OrionBot.h"

// some random

void OrionBot::CombinedBuild() {
	switch (FINALSTRATEGY_STATE.currentBuild) {
		case STAGE1_FINALSTRATEGY: {
			OrionBot::setChokePoint();
			OrionBot::TryScout();
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
							OrionBot::FindNearestVespeneGeyser(workers.front()->pos);
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
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
				FINALSTRATEGY_STATE.currentBuild++;
			}
			break;
		}
		case STAGE2_FINALSTRATEGY: {
			OrionBot::TryBuildCommandCentreExpansion(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
			FINALSTRATEGY_STATE.expand = true;
			if ((FINALSTRATEGY_STATE.newCommandCentre == true)) {
				FINALSTRATEGY_STATE.currentBuild++;
				break;
			}
			break;
		}
		//JUST BUILD MORE SUPPLY DEPOTS AND BARRACKS FOR NOW
		case STAGE3_FINALSTRATEGY: {
			OrionBot::TryBuildSupplyDepot();
			OrionBot::TryBuildBarracks();
			break;
		}
		default: {
			break;
		}
	}
}

void OrionBot::CombinedOnUnitIdle(const Unit* unit) {
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (FINALSTRATEGY_STATE.upgradeOrbital) {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}

		break;
	}
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: {
		if (unit->energy > 50) {
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (!mineral_target) {
				break;
			}
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE, mineral_target);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		const GameInfo& game_info = Observation()->GetGameInfo();

		if (FINALSTRATEGY_STATE.num_units_scouting < game_info.enemy_start_locations.size()) {
			// send csv to one of the corners and save base location to possible_enemy_bases
			Point2D location = game_info.enemy_start_locations[FINALSTRATEGY_STATE.num_units_scouting];
			Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, location);

			possible_enemy_bases.push_back(location);
			enemyBaseValue.push_back(0);
			FINALSTRATEGY_STATE.num_units_scouting++;

			if (FINALSTRATEGY_STATE.expand) {
				Point2D enemyPos = FindEnemyBase();
				for (int i = 0; i < 3; i++) {
					if ((possible_enemy_bases[i]) != enemyPos) {
						TryBuildStructureAtCP(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV, possible_enemy_bases[i]);
					}
				}
			}
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (!mineral_target) {
				break;
			}
			Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
			break;

		}
	}

	case UNIT_TYPEID::TERRAN_BARRACKS: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		break;
	}

	case UNIT_TYPEID::TERRAN_MARINE: {
		if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 50) {
			FINALSTRATEGY_STATE.attacking = true;
		}
		if (FINALSTRATEGY_STATE.attacking) {
			const GameInfo& game_info = Observation()->GetGameInfo();
			// there are 3 enemy_start_locations
			//Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, locations_enemy_seen.front());
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, FINALSTRATEGY_STATE.tobuildRaxs);
		}
		break;
	}

	case UNIT_TYPEID::TERRAN_REAPER: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		break;
	}

	default: {
		break;
	}
	}

}



/*
 * Set the positions of chike points for SD and rax
 * for each of the four corners.
*/
void OrionBot::setChokePoint() {
	const ObservationInterface* observation = Observation();

	if (observation->GetStartLocation().x == FINALSTRATEGY_STATE.BOTTOM_LEFT.x && observation->GetStartLocation().y == FINALSTRATEGY_STATE.BOTTOM_LEFT.y) {
		FINALSTRATEGY_STATE.tobuildSD = Point2D(29, 54);
		FINALSTRATEGY_STATE.tobuildRaxs = Point2D(29, 51);	//Point2D(30, 51);
		FINALSTRATEGY_STATE.barracks = Point2D(29, 43.5);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_RIGHT.y) {
		FINALSTRATEGY_STATE.tobuildSD = Point2D(138, 29);
		FINALSTRATEGY_STATE.tobuildRaxs = Point2D(140, 29);		//Point2D(140, 30);
		FINALSTRATEGY_STATE.barracks = Point2D(148, 29);
	}
	else if (observation->GetStartLocation().x == FINALSTRATEGY_STATE.TOP_LEFT.x && observation->GetStartLocation().y == FINALSTRATEGY_STATE.TOP_LEFT.y) {
		FINALSTRATEGY_STATE.tobuildSD = Point2D(51, 160);
		FINALSTRATEGY_STATE.tobuildRaxs = Point2D(51, 162);		//Point2D(52, 162);
		FINALSTRATEGY_STATE.barracks = Point2D(148, 160);
	}
	else if (observation->GetStartLocation().x == FINALSTRATEGY_STATE.TOP_RIGHT.x && observation->GetStartLocation().y == FINALSTRATEGY_STATE.TOP_RIGHT.y) {
		FINALSTRATEGY_STATE.tobuildSD = Point2D(160, 141);
		FINALSTRATEGY_STATE.tobuildRaxs = Point2D(162, 140);
		FINALSTRATEGY_STATE.barracks = Point2D(160, 148);
	}
}
