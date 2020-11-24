#include "OrionBot.h"
//Rax6 Rush Strategy
//Made by: Asma.

/*
	STAGE1
	10 - Supply Depot
	12 - Refinery
	13 - Barracks
	16 - Orbital Command 

	STAGE2
	Base expansion

	STAGE3
	need to figure out to find the corners of map
	to do scanner sweep!!

	STAGE4
	more barracks to train marines for attacking
*/

void OrionBot::Rax6Build() {
	switch (RAX6_STATE.currentBuild) {
	case STAGE1_RAX6:
		OrionBot::setChokePoints();
		if (Observation()->GetMinerals() >= 100) {
			//OrionBot::TryBuildSupplyDepot();
			RAX6_STATE.SD1 = true;
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
			//OrionBot::TryBuildBarracks();
			OrionBot::TryBuildStructureAtCP(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV, RAX6_STATE.tobuildRaxs);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			RAX6_STATE.upgradeOrbital = true;
		}
		
		if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
			RAX6_STATE.upgradeOrbital = false;
			//RAX6_STATE.newCommandCentre = true;
			RAX6_STATE.currentBuild++;
		}
		break;
		
	case STAGE2_RAX6:
		//TryBuildExpansionCom();
		TryBuildCommandCentreChokeP(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
		if ((RAX6_STATE.newCommandCentre == true)) {
			RAX6_STATE.currentBuild++;
			break;
		}
		//TryBuildExpansionCom();
		//break;

	case STAGE3_RAX6:
		TryBuildSupplyDepot();
		TryBuildBarracks();
		TryBuildMarine();
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
			//Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE);
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_SCAN, Point2D(150, 33.5));
			std::cout << "SCANNEDDDDD" << std::endl;
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		// const bool vespene_target = FindNearestVespeneGeyser(unit->pos);
		if (!mineral_target) {
			break;
		}
		if (AddWorkersToRefineries(unit)) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
		if (BANSHEE_STATE.morph_reactor) {
			Actions()->UnitCommand(unit, ABILITY_ID::BARRACKSREACTORMORPH_REACTOR);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		}
		
		break;
	}
	
	case UNIT_TYPEID::TERRAN_MARINE: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
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






bool OrionBot::TryBuildSupplyDepotChokePoint() {
	return false;
}

bool OrionBot::TryBuildBarracksChokePoint() {
	return false;
}




//Build a new command centre at the choke point.
bool OrionBot::TryBuildCommandCentreChokeP(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {

	const ObservationInterface* observation = Observation();
	Point3D startLocation_ = Observation()->GetStartLocation();
	std::cout << "STARTING POS: " << startLocation_.x << "," << startLocation_.y << std::endl;
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

	/*
	if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_LEFT.y) {
		rx = 30;
		ry = 66.1;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_RIGHT.y) {
		rx = 126;
		ry = 30.5;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_LEFT.y) {
		rx = 30;		  //125;
		ry = 125.9;   // 130;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_RIGHT.y) {
		rx = 160.9;  // 156.5;
		ry = 125.9;  // 130.5;
	}
	*/

	std::cout << "TO BUILD.X = " << toBuildCC.x << " ," << toBuildCC.y << std::endl;
	Actions()->UnitCommand(unit_to_build,
		ability_type_for_structure,
		Point2D(rx, ry));

	//RAX6_STATE.currentBuild++;
	RAX6_STATE.newCommandCentre = true;
	return true;

}


/* from the API - need to figure out this. 
   ~Not using for now*/
bool OrionBot::TryExpand(AbilityID build_ability, UnitTypeID worker_type) {
	const ObservationInterface* observation = Observation();
	Point3D startLocation_ = Observation()->GetStartLocation();
	Point3D staging_location_ = startLocation_;
	std::vector<Point3D> expansions_ = search::CalculateExpansionLocations(Observation(), Query());
	Point3D closest_expansion;
	float minimum_distance = std::numeric_limits<float>::max();
	
	for (const auto& expansion : expansions_) {
		float current_distance = Distance2D(startLocation_, expansion);
		if (current_distance < .01f) {
			continue;
		}

		if (current_distance < minimum_distance) {
			if (Query()->Placement(build_ability, expansion)) {
				closest_expansion = expansion;
				minimum_distance = current_distance;
			}
		}
	}
	//only update staging location up till 3 bases.
	if (TryBuildStructureChokePoint(build_ability, worker_type, closest_expansion, true) && observation->GetUnits(Unit::Self, IsTownHall()).size() < 4) {
		staging_location_ = Point3D(((staging_location_.x + closest_expansion.x) / 2), ((staging_location_.y + closest_expansion.y) / 2),
			((staging_location_.z + closest_expansion.z) / 2));
		RAX6_STATE.currentBuild++;
		return true;
	}
	RAX6_STATE.currentBuild++;
	return false;
}


//Try build structure given a location. This is used most of the time
bool OrionBot::TryBuildStructureChokePoint(AbilityID ability_type_for_structure, UnitTypeID unit_type, Point2D location, bool isExpansion = false) {

	const ObservationInterface* observation = Observation();
	std::vector<Point3D> expansions_ = search::CalculateExpansionLocations(Observation(), Query());
	Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));

	//if we have no workers Don't build
	if (workers.empty()) {
		return false;
	}

	// Check to see if there is already a worker heading out to build it
	for (const auto& worker : workers) {
		for (const auto& order : worker->orders) {
			if (order.ability_id == ability_type_for_structure) {
				return false;
			}
		}
	}

	// If no worker is already building one, get a random worker to build one
	const Unit* unit = GetRandomEntry(workers);

	// Check to see if unit can make it there
	if (Query()->PathingDistance(unit, location) < 0.1f) {
		return false;
	}
	if (!isExpansion) {
		for (const auto& expansion : expansions_) {
			if (Distance2D(location, Point2D(expansion.x, expansion.y)) < 7) {
				return false;
			}
		}
	}
	// Check to see if unit can build there
	if (Query()->Placement(ability_type_for_structure, location)) {
		Actions()->UnitCommand(unit, ability_type_for_structure, location);
		return true;
	}
	return false;

}


bool OrionBot::TryBuildExpansionCom() {
	const ObservationInterface* observation = Observation();
	Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
	//Don't have more active bases than we can provide workers for
	if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) > max_worker_count_) {
		return false;
	}
	// If we have extra workers around, try and build another Hatch.
	if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) < observation->GetFoodWorkers() - 10) {
		return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
	}
	//Only build another Hatch if we are floating extra minerals
	if (observation->GetMinerals() > std::min<size_t>(bases.size() * 400, 1200)) {
		return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
	}
	return false;
}


//An estimate of how many workers we should have based on what buildings we have
int OrionBot::GetExpectedWorkers(UNIT_TYPEID vespene_building_type) {
	const ObservationInterface* observation = Observation();
	Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
	Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));
	int expected_workers = 0;
	for (const auto& base : bases) {
		if (base->build_progress != 1) {
			continue;
		}
		expected_workers += base->ideal_harvesters;
	}

	for (const auto& geyser : geysers) {
		if (geyser->vespene_contents > 0) {
			if (geyser->build_progress != 1) {
				continue;
			}
			expected_workers += geyser->ideal_harvesters;
		}
	}

	return expected_workers;
}




// From Sc2 Cpp Tutorial
bool OrionBot::TryBuildBarracks2(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {
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

	float rx; // = GetRandomScalar();
	float ry; // = GetRandomScalar();

	if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_LEFT.y) {
		rx = 51;
		ry = 33.5;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_RIGHT.y) {
		rx = 140;
		ry = 33.5;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_LEFT.y) {
		rx = 51;		  //125;
		ry = 125.9;   // 130;
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_RIGHT.y) {
		rx = 140;  // 156.5;
		ry = 125.9;  // 130.5;
	}


	if (!bases.empty()) {
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			Point2D(rx, ry));
	}
	else {
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			Point2D(rx, ry));
	}
	return true;
}


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
	/*
	Point2D toBuildPos;
	if (RAX6_STATE.SD1) {
		toBuildPos = RAX6_STATE.tobuildSD;
	}
	else if(RAX6_STATE.Rax){
		toBuildPos = RAX6_STATE.tobuildRaxs;
	}
	*/
	if (!bases.empty()) {
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			Point2D(toBuildPos.x, toBuildPos.y));
	}
	else {
		float rx = GetRandomScalar();
		float ry = GetRandomScalar();
		Actions()->UnitCommand(unit_to_build,
			ability_type_for_structure,
			Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
	}
	return true;
}


void OrionBot::setChokePoints() {
	const ObservationInterface* observation = Observation();

	if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_LEFT.y) {
		RAX6_STATE.tobuildSD = Point2D(29, 54);
		RAX6_STATE.tobuildRaxs = Point2D(29, 51);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.BOTTOM_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.BOTTOM_RIGHT.y) {
		RAX6_STATE.tobuildSD = Point2D(138, 29);
		RAX6_STATE.tobuildRaxs = Point2D(140, 29);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_LEFT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_LEFT.y) {
		RAX6_STATE.tobuildSD = Point2D(51, 160);
		RAX6_STATE.tobuildRaxs = Point2D(51, 162);
	}
	else if (observation->GetStartLocation().x == RAX6_STATE.TOP_RIGHT.x && observation->GetStartLocation().y == RAX6_STATE.TOP_RIGHT.y) {
		RAX6_STATE.tobuildSD = Point2D(160, 141);
		RAX6_STATE.tobuildRaxs = Point2D(162,140);
	}
}