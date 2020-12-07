#include "OrionBot.h"

//Banshee Rush Strategy

/*
	STAGE1
	10 - Supply Depot
	12 - Refinery
	13 - Barracks
	16 - Orbital Command + Factory

	STAGE2
	16 - Marine (1) + Supply Depot
	20 - Starport + Hellion (1 only)
	@100% Hellion (1) - Tech Lab (1) > Banshees (to 2)
	23 - Refinery (2) + Supply Depot

	STAGE3
	30 - Barracks > Reactor + Supply Depot
	32 - Factory > Tech Lab (2)

	STAGE4
	39 - Siege Tanks + Supply Depot
	52 - Siege Tech > @100% - Attack
*/

void OrionBot::BansheeBuild() {
	switch (BANSHEE_STATE.current_build) {
	case STAGE1_BANSHEE: {
		// 10 - Supply Depot
		if (Observation()->GetMinerals() >= 100) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
				OrionBot::TryBuildSupplyDepot();
			}
		}
		// 12 - Refinery
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
		// 13 - Barracks
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) {
			OrionBot::TryBuildBarracks();
		}
		// 16 - Orbital Command
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			BANSHEE_STATE.orbital_upgrade = true;
		}
		// 16 Factory
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
				OrionBot::TryBuildFactory();
			}
		}
		// Increment the build counter.
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > 0) {
			BANSHEE_STATE.current_build++;
		}
		break;
	}
	case STAGE2_BANSHEE: {
		/*16 - Marine(1) + Supply Depot
			20 - Starport + Hellion(1 only)
			@100 % Hellion(1) - Tech Lab(1) > Banshees(to 2)
			23 - Refinery(2) + Supply Depot*/
		BANSHEE_STATE.orbital_upgrade = false;
		//16 - Supply Depot
		if (Observation()->GetMinerals() >= 100) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 2) {
				OrionBot::TryBuildSupplyDepot();
			}
		}
		//16 - Marine(1) 
		//20 - Starport 
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1) {
			OrionBot::TryBuildStarport();
		}
		//20 Hellion(1 only)
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) < 1) {
			BANSHEE_STATE.produce_hellion = true;
		}
		//20 Tech Lab(1) 
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
			BANSHEE_STATE.morph_techlab = true;
			BANSHEE_STATE.produce_hellion = false;
		}
		//20 Banshees(to 2)
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB) > 0) {
			BANSHEE_STATE.morph_techlab = false;
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
				BANSHEE_STATE.produce_banshee = true;
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) > 2) {
			BANSHEE_STATE.produce_banshee = false;
		}
		//23 Supply Depot
		if (Observation()->GetMinerals() >= 100) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 4) {
				OrionBot::TryBuildSupplyDepot();
			}
		}
		//23 - Refinery(2) 
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 2) {
				if (Observation()->GetMinerals() >= 75) {
					const ObservationInterface* observation = Observation();
					Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
					bool vespene_target = false;
					if (!workers.empty()) {
						FindNearestVespeneGeyser(workers.front()->pos);
					}
				}
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 2) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB) > 0) {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
					if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) > 1) {
						BANSHEE_STATE.produce_banshee = false;
						BANSHEE_STATE.produce_hellion = false;
						BANSHEE_STATE.current_build++;
					}
				}
			}
			else {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 2) {
					OrionBot::TryBuildStarport();
				}
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 4) {
					OrionBot::TryBuildSupplyDepot();
				}
			}
		}
		break;
	}
	case STAGE3_BANSHEE: {
		//30 - Barracks > Reactor + Supply Depot
			//32 - Factory > Tech Lab(2)
			//Supply Depot
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5) {
			OrionBot::TryBuildSupplyDepot();
		}
		BANSHEE_STATE.morph_reactor = true;
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1 || OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
			BANSHEE_STATE.current_build = STAGE2_BANSHEE;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) >= 0) {
			BANSHEE_STATE.morph_techlab = true;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
			BANSHEE_STATE.produce_banshee = true;
		}
		else {
			BANSHEE_STATE.produce_banshee = false;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKSREACTOR) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORYTECHLAB) >= 1) {
				BANSHEE_STATE.morph_reactor = false;
				BANSHEE_STATE.morph_techlab = false;
				BANSHEE_STATE.current_build++;
			}
			else {
				OrionBot::TryBuildFactory();
			}
		}
		break;
	}
	case STAGE4_BANSHEE: {
		//39 - Siege Tanks + Supply Depot
			//52 - Siege Tech > @100 % -Attack
			//39 Supply Depot
		if (Observation()->GetMinerals() >= 100) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 8) {
				OrionBot::TryBuildSupplyDepot();
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 3) {
			OrionBot::TryBuildBarracks();
		}
		//39 Siege Tanks
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 3) {
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
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
			BANSHEE_STATE.produce_banshee = true;
		}
		else {
			BANSHEE_STATE.produce_banshee = false;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SIEGETANK) + OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED) > 5) {
			BANSHEE_STATE.current_build++;
		}
		break;
	}
	case STAGE5_BANSHEE: {
		OrionBot::TryBuildCommandCentre();
		if (Observation()->GetFoodUsed() > Observation()->GetFoodCap() - 2) {
			OrionBot::TryBuildSupplyDepot();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
			if (Observation()->GetMinerals() >= 75) {
				const ObservationInterface* observation = Observation();
				Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
				if (!workers.empty()) {
					bool vespene_target = false;
					FindNearestVespeneGeyser(workers.front()->pos);
				}
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 0) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
			BANSHEE_STATE.produce_banshee = true;
		}
		else {
			BANSHEE_STATE.produce_banshee = false;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1) {
			OrionBot::TryBuildEngineeringBay();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_MISSILETURRET) < 2) {
			OrionBot::TryBuildMissleTurret();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		break;
	}
	default: {
		break;
	}

	}
}

void OrionBot::BansheeOnUnitIdle(const Unit* unit) {
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (BANSHEE_STATE.orbital_upgrade) {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: {
		if (unit->energy > 75) {
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (!mineral_target) {
				break;
			}
			OrionBot::tryCalldownExtraSupplies(unit);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		const GameInfo& game_info = Observation()->GetGameInfo();

		if (BANSHEE_STATE.num_units_scouting < game_info.enemy_start_locations.size()) {
			// send csv to one of the corners and save base location to possible_enemy_bases
			Point2D location = game_info.enemy_start_locations[BANSHEE_STATE.num_units_scouting];
			Actions()->UnitCommand(unit, ABILITY_ID::MOVE_MOVE, location);

			possible_enemy_bases.push_back(location);
			enemyBaseValue.push_back(0);
			BANSHEE_STATE.num_units_scouting++;
		}
		else {
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (AddWorkersToRefineries(unit)) {
				break;
			}
			if (!mineral_target) {
				break;
			}
			Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
		if (BANSHEE_STATE.morph_reactor) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_REACTOR_BARRACKS);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORY: {

		if (BANSHEE_STATE.produce_hellion) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_HELLION);
		}
		else if (BANSHEE_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_FACTORY);
		}
		else {
			if (BANSHEE_STATE.current_build >= STAGE4_BANSHEE) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SIEGETANK);
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_THOR) < 1) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_THOR);
			}
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORYTECHLAB: {
		Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SIEGETANK);
		break;
	}
	case UNIT_TYPEID::TERRAN_STARPORT: {
		if (BANSHEE_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_STARPORT);
		}
		if (BANSHEE_STATE.produce_banshee) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_BANSHEE);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_RAVEN) < 1) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_RAVEN);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		if (BANSHEE_STATE.current_build >= STAGE5_BANSHEE) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_HELLION: {
		if (BANSHEE_STATE.current_build >= STAGE5_BANSHEE) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_BANSHEE: {
		if (BANSHEE_STATE.current_build >= STAGE5_BANSHEE) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_REAPER: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, game_info.enemy_start_locations.front());
		break;
	}
	case UNIT_TYPEID::TERRAN_THOR: {
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
	}
	case UNIT_TYPEID::TERRAN_RAVEN: {
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
	}
	case UNIT_TYPEID::TERRAN_SIEGETANK: {
		if (BANSHEE_STATE.current_build >= STAGE5_BANSHEE) {
			const ObservationInterface* observation = Observation();
			Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
			float distance = std::numeric_limits<float>::max();
			for (const auto& u : enemy_units) {
				float d = Distance2D(u->pos, unit->pos);
				if (d < distance) {
					distance = d;
				}
			}
			if (distance < 11) {
				Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
			}
			else {
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
			}
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
		if (BANSHEE_STATE.current_build >= STAGE5_BANSHEE) {
			const ObservationInterface* observation = Observation();
			Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
			float distance = std::numeric_limits<float>::max();
			for (const auto& u : enemy_units) {
				float d = Distance2D(u->pos, unit->pos);
				if (d < distance) {
					distance = d;
				}
			}
			if (distance > 13) {
				Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
			}
			else {
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
			}
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
		}
		break;
	}
	default: {
		break;
	}
	}
}
