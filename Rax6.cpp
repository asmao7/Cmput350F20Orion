#include "OrionBot.h"
//Rax6 Rush Strategy

/*
	STAGE1
	10 - Supply Depot @ choke point	~ DONE!
	12 - Refinery	~ DONE!
	13 - Barracks @ choke point	~ DONE!
	16 - Orbital Command ~DONE!
	STAGE2
	- Base expansion ~DONE!
	STAGE3
	- Perform a scan sweep on the corners until   ~ done but need to find a way to notify the bot about the scan
	  the enemy base is found			~DONE
	- send all the marines there		~ Done
	STAGE4
	- more barracks to train marines for attacking		  ~done
	- build the barracks in a line to shield the CC
	- the build locations should be specified not random  ~not working as expected
*/

void OrionBot::Rax6Build() {
	switch (RAX6_STATE.currentBuild) {
	case STAGE1_RAX6:
		OrionBot::setChokePoints();
		OrionBot::scout();

		if (Observation()->GetMinerals() >= 100) {
			OrionBot::TryBuildStructureAtCP(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, RAX6_STATE.tobuildSD);
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
			OrionBot::TryBuildStructureAtCP(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV, RAX6_STATE.tobuildRaxs);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			RAX6_STATE.upgradeOrbital = true;
		}

		if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
			RAX6_STATE.currentBuild++;
		}
		break;

	case STAGE2_RAX6:
		TryBuildBarracks();
		TryBuildCommandCentreExpansion(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
		TryBuildBarracks();
		RAX6_STATE.expand = true;
		if ((RAX6_STATE.newCommandCentre == true)) {
			RAX6_STATE.currentBuild++;
			break;
		}

	case STAGE3_RAX6:
		TryBuildBarracks();
		TryBuildSupplyDepot();
		if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) == 4) {
			RAX6_STATE.currentBuild++;
		}
		break;


	case STAGE4_RAX6:
		TryBuildSupplyDepot();
		TryBuildBarracks();
		TryBuildBarracks();
		if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) == 6) {
			RAX6_STATE.currentBuild++;
		}
		break;

	case STAGE5_RAX6:
		TryBuildSupplyDepot();
		TryBuildBarracks();
		TryBuildBarracks();
		break;

	}
}

void OrionBot::Rax6OnUnitIdle(const Unit* unit) {
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (RAX6_STATE.upgradeOrbital) {
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
		if (RAX6_STATE.expand) {
			Point2D enemyPos = FindEnemyBase();
			for (int i = 0; i < 3; i++) {
				if ((possible_enemy_bases[i]) != enemyPos) {
					TryBuildStructureAtCP(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV, possible_enemy_bases[i]);
				}
			}
			TryBuildStructureAtCP(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV, Point2D(Observation()->GetStartLocation().x, Observation()->GetStartLocation().y));
		}
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		if (!mineral_target) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;

	}

	case UNIT_TYPEID::TERRAN_BARRACKS: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		break;
	}

	case UNIT_TYPEID::TERRAN_MARINE: {
		if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 50) {
			RAX6_STATE.attacking = true;
		}
		if (RAX6_STATE.attacking) {
			const GameInfo& game_info = Observation()->GetGameInfo();
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, RAX6_STATE.tobuildRaxs);
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
* Build a new command centre at the expansion location.
*/
bool OrionBot::TryBuildCommandCentreExpansion(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {

	const ObservationInterface* observation = Observation();
	Point3D startLocation_ = Observation()->GetStartLocation();
	Point3D staging_location_ = startLocation_;
	std::vector<Point3D> expansions_ = search::CalculateExpansionLocations(Observation(), Query());
	Point2D closest_expansion;
	float minimum_distance = std::numeric_limits<float>::max();
	for (const auto& expansion : expansions_) {
		float current_distance = Distance2D(startLocation_, expansion);
		if (current_distance < .01f) {
			continue;
		}
		if (current_distance < minimum_distance) {
			if (Query()->Placement(ABILITY_ID::BUILD_COMMANDCENTER, expansion)) {
				closest_expansion = expansion;
				minimum_distance = current_distance;
			}
		}
	}

	// If a unit already is building a supply structure of this type, do nothing.
	// Also get an scv to build the structure.
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}

		if (unit->unit_type == unit_type) {
			unit_to_build = unit;
		}
	}

	Point2D toBuildCC = closest_expansion;
	float rx = toBuildCC.x;
	float ry = toBuildCC.y;

	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		Point2D(rx, ry));

	//RAX6_STATE.currentBuild++;
	RAX6_STATE.newCommandCentre = true;
	FINALSTRATEGY_STATE.newCommandCentre = true;
	return true;

}


/*
 * Takes in position and builds the specified structure at the specified location
*/
bool OrionBot::TryBuildStructureAtCP(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type, Point2D toBuildPos) {
	const ObservationInterface* observation = Observation();

	// If a unit already is building a supply structure of this type, do nothing.
	// Also get an scv to build the structure.
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self);
	Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
	for (const auto& unit : units) {
		for (const auto& order : unit->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}

		if (unit->unit_type == unit_type) {
			unit_to_build = unit;
		}
	}

	if (!bases.empty()) {
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			Point2D(toBuildPos.x, toBuildPos.y));
	}
	return true;
}

/*
 * Set the positions of chike points for SD and rax
 * for each of the four corners.
*/
void OrionBot::setChokePoints() {
	const ObservationInterface* observation = Observation();

	if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_LEFT.y) {
		RAX6_STATE.tobuildSD = Point2D(29, 54);
		RAX6_STATE.tobuildRaxs = Point2D(29, 51);
		RAX6_STATE.barracks = Point2D(29, 43.5);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_RIGHT.y) {
		RAX6_STATE.tobuildSD = Point2D(138, 29);
		RAX6_STATE.tobuildRaxs = Point2D(140, 29);
		RAX6_STATE.barracks = Point2D(148, 29);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_LEFT.y) {
		RAX6_STATE.tobuildSD = Point2D(51, 160);
		RAX6_STATE.tobuildRaxs = Point2D(51, 162);
		RAX6_STATE.barracks = Point2D(148, 160);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_RIGHT.y) {
		RAX6_STATE.tobuildSD = Point2D(160, 141);
		RAX6_STATE.tobuildRaxs = Point2D(162, 140);
		RAX6_STATE.barracks = Point2D(160, 148);
	}
}


void OrionBot::TryScout() {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
	const GameInfo& game_info = Observation()->GetGameInfo();
	if (RAX6_STATE.num_units_scouting < game_info.enemy_start_locations.size()) {
		// send csv to one of the corners and save base location to possible_enemy_bases
		Point2D location = game_info.enemy_start_locations[RAX6_STATE.num_units_scouting];
		if (units.size() > 0) {
			Actions()->UnitCommand(units[0], ABILITY_ID::MOVE_MOVE, location);
		}

		possible_enemy_bases.push_back(location);
		enemyBaseValue.push_back(0);
		RAX6_STATE.num_units_scouting++;

		if (FindEnemyBase() == RAX6_STATE.BOTTOM_LEFT || FindEnemyBase() == RAX6_STATE.BOTTOM_RIGHT ||
			FindEnemyBase() == RAX6_STATE.TOP_RIGHT || FindEnemyBase() == RAX6_STATE.TOP_LEFT) {
			RAX6_STATE.enemy_found = true;
		}
	}
}