//#include "Marines12.h"
#include "OrionBot.h"

void OrionBot::Marines12Build() {
	switch (MARINES12_STATE.current_build) {
	
	case STAGE1_MARINES:
		if (Observation()->GetMinerals() >= 100 && OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) <= 2) {
			OrionBot::TryBuildSupplyDepot();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			MARINES12_STATE.current_build++;
		}

		scout();

		break;

	case STAGE2_MARINES:
		if (Observation()->GetMinerals() >= 150) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			// Start producing Marines, stop producing CSVs
			// Allow Orbital Upgrade
			MARINES12_STATE.produce_scv = false;
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

		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND)) {
			MARINES12_STATE.orbital_command_upgraded = true;
		}
		if (Observation()->GetMinerals() >= 150 && OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 4) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 3) { // NEED TO CHECK TIME 4:30 ???
			MARINES12_STATE.current_build++;
		}

		break;
	case STAGE4_ATTACK_MARINES:
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_MARINE) >= 15) {
			MARINES12_STATE.attacking = true;

			attack();
			/*
			// Send all units to fight
			const ObservationInterface* observation = Observation();
			Units bases = observation->GetUnits();

			for (const auto& base : bases) {
				if (base->unit_type == UNIT_TYPEID::TERRAN_SCV || base->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
					Actions()->UnitCommand(base, ABILITY_ID::ATTACK_ATTACK, FindEnemyBase());
				}
			}*/
		}
		
	}
}

void OrionBot::Marines12OnUnitIdle(const Unit* unit) {
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (MARINES12_STATE.orbital_upgrade) {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
		}
		else if (MARINES12_STATE.produce_scv) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: {	
		//std::cout << "here";
		if (unit->energy < 50) {
			break;
		}

		//std::cout << "AAAA";
		if (!MARINES12_STATE.supplies_called) {
			tryCalldownExtraSupplies(unit);
			break;
		}
		tryCalldownMULE(unit);
		
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, base_location);
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
			break;
		}

		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		if (MARINES12_STATE.attacking) {
			next = true;
		}
		
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		if (MARINES12_STATE.attacking) {
			next = true;
		}
		/*
		if (MARINES12_STATE.attacking && MARINES12_STATE.i_location < locations_enemy_seen.size()) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, locations_enemy_seen[MARINES12_STATE.i_location]);
			MARINES12_STATE.i_location++;
		}*/
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		break;
	}
	default: {
		break;
	}
	}
}

void OrionBot::scout() {
	const ObservationInterface* observation = Observation();
	Units bases = observation->GetUnits();
	const GameInfo& game_info = Observation()->GetGameInfo();

	base_location = Observation()->GetStartLocation();
	if (!found_locations) {
		expansion_locations = search::CalculateExpansionLocations(Observation(), Query());
		found_locations = true;
	}
	
	for (const auto& base : bases) {
		if (base->unit_type == UNIT_TYPEID::TERRAN_SCV) {
			if (MARINES12_STATE.num_units_scouting < game_info.enemy_start_locations.size()) {
				// send csv to one of the corners and save base location to possible_enemy_bases
				Point2D location = game_info.enemy_start_locations[MARINES12_STATE.num_units_scouting];
				Actions()->UnitCommand(base, ABILITY_ID::MOVE_MOVE, location);

				possible_enemy_bases.push_back(location);
				enemyBaseValue.push_back(0);
				MARINES12_STATE.num_units_scouting++;
			}
		}
	}
}

void OrionBot::tryCalldownMULE(const Unit* unit) {
	if (FindNearestMineralPatch(unit->pos)) {
		Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE, FindNearestMineralPatch(unit->pos));
	}
}

void OrionBot::tryCalldownExtraSupplies(const Unit* unit) {
	const ObservationInterface* observation = Observation();
	Units bases = observation->GetUnits();

	for (const auto& base : bases) {
		if (base->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT) {
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_SUPPLYDROP, base);
			MARINES12_STATE.supplies_called = true;
		}
	}
}

void OrionBot::OnUnitEnterVision(const Unit* unit) {
	if (unit->alliance == Unit::Alliance::Enemy) {
		//std::cout << "See enemy" << std::endl;
		locations_enemy_seen.push_back(Point2D(unit->pos));
		locations_enemy_seen2.push(Point2D(unit->pos));

		// find to what base this location is closest to
		closestToBase(Point2D(unit->pos));
	}
}

void OrionBot::closestToBase(Point2D coord) {
	float min_distance = FLT_MAX;
	int min_i = 0;

	for (int i = 0; i < possible_enemy_bases.size(); ++i) {
		auto distance = DistanceSquared2D(coord, possible_enemy_bases[i]);
		if (distance < min_distance) {
			min_distance = distance;
			min_i = i;
		}
	}
	enemyBaseValue[min_i] += 1;
	return;
}

Point2D OrionBot::FindEnemyBase() {
	auto position = std::distance(enemyBaseValue.begin(), std::max_element(enemyBaseValue.begin(), enemyBaseValue.end()));
	Point2D point = possible_enemy_bases[position];
	return point;
}


void OrionBot::attack() {
	// Send all units to fight
	std::cout << "attack" << std::endl;
	const ObservationInterface* observation = Observation();
	Units bases = observation->GetUnits();

	if (locations_enemy_seen2.empty()) {
		std::cout << "stuck" << std::endl;
		for (int i = 0; i < expansion_locations.size(); ++i) {
			locations_enemy_seen2.push(expansion_locations[i]);
		}
	}
	for (const auto& base : bases) {
		if (base->unit_type == UNIT_TYPEID::TERRAN_MARINE || base->unit_type == UNIT_TYPEID::TERRAN_SCV) {
			Actions()->UnitCommand(base, ABILITY_ID::ATTACK_ATTACK, locations_enemy_seen2.front());
		}
	}
	if (next) {
		locations_enemy_seen2.pop();
	}
	next = false;
}