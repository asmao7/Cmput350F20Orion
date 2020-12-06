#include "OrionBot.h"

void OrionBot::CombinedBuild() {
	switch (FINALSTRATEGY_STATE.current_build) {
	case STAGE1_FINALSTRATEGY: {
		// 10 - Supply Depot
		//OrionBot::scout();
		OrionBot::TryBuildSupplyDepot();
		// 12 - Refinery
		OrionBot::BuildRefinery();
		OrionBot::FillRefineries();
		// 13 - Barracks
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) {
			OrionBot::TryBuildBarracks();
		}
		// 16 - Orbital Command
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
			FINALSTRATEGY_STATE.orbital_upgrade = true;
		}
		// 16 Factory
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
				OrionBot::TryBuildFactory();
			}
		}
		// Increment the build counter.
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > 0) {
				FINALSTRATEGY_STATE.current_build++;
			}
		}
		break;
	}
	case STAGE2_FINALSTRATEGY: {
		/*16 - Marine(1) + Supply Depot
			20 - Starport + Hellion(1 only)
			@100 % Hellion(1) - Tech Lab(1) > Banshees(to 2)
			23 - Refinery(2) + Supply Depot*/
		FINALSTRATEGY_STATE.orbital_upgrade = false;
		//16 - Supply Depot
		OrionBot::TryBuildSupplyDepot();
		//16 - Marine(1) 

		//20 - Starport 
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1) {
			OrionBot::TryBuildStarport();
		}
		//20 Hellion(1 only)
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) < 1) {
			FINALSTRATEGY_STATE.produce_hellion = true;
		}
		//20 Tech Lab(1) 
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
			FINALSTRATEGY_STATE.morph_techlab = true;
			FINALSTRATEGY_STATE.produce_hellion = false;
		}
		//20 Banshees(to 2)
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB) > 0) {
			FINALSTRATEGY_STATE.morph_techlab = false;
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
				FINALSTRATEGY_STATE.produce_banshee = true;
			}
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) > 2) {
			FINALSTRATEGY_STATE.produce_banshee = false;
		}
		//23 Supply Depot
		OrionBot::TryBuildSupplyDepot();
		//23 - Refinery(2) 
		OrionBot::BuildRefinery();
		OrionBot::FillRefineries();

		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 2) {
			OrionBot::TryBuildFactory();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 2) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB) > 0) {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
					if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) >= 1) {
						FINALSTRATEGY_STATE.produce_banshee = false;
						FINALSTRATEGY_STATE.produce_hellion = false;
						FINALSTRATEGY_STATE.current_build++;
					}
				}
			}
			else {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 2) {
					OrionBot::TryBuildStarport();
				}
			}
		}
		break;
	}
	case STAGE3_FINALSTRATEGY: {
		//30 - Barracks > Reactor + Supply Depot
			//32 - Factory > Tech Lab(2)
			//Supply Depot
		std::cout << STAGE3_FINALSTRATEGY << std::endl;
		OrionBot::TryBuildSupplyDepot();
		FINALSTRATEGY_STATE.morph_reactor = true;
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1 || OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
			FINALSTRATEGY_STATE.current_build = STAGE2_BANSHEE;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) >= 0) {
			FINALSTRATEGY_STATE.morph_techlab = true;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 2) {
			FINALSTRATEGY_STATE.produce_banshee = true;
		}
		else {
			FINALSTRATEGY_STATE.produce_banshee = false;
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKSREACTOR) > 0) {
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORYTECHLAB) >= 1) {
				FINALSTRATEGY_STATE.morph_reactor = false;
				FINALSTRATEGY_STATE.morph_techlab = false;
				FINALSTRATEGY_STATE.current_build++;
			}
			else {
				OrionBot::TryBuildFactory();
			}
		}
		break;
	}
	case STAGE4_FINALSTRATEGY: {
		//39 - Siege Tanks + Supply Depot
			//52 - Siege Tech > @100 % -Attack
			//39 Supply Depot
		std::cout << STAGE4_FINALSTRATEGY << std::endl;
		OrionBot::TryBuildSupplyDepot();
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 3) {
			OrionBot::TryBuildBarracks();
		}
		//39 Siege Tanks
		OrionBot::BuildRefinery();
		OrionBot::FillRefineries();

		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 1) {
			OrionBot::TryBuildCommandCentre();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 3) {
			FINALSTRATEGY_STATE.produce_banshee = true;
		}
		else {
			FINALSTRATEGY_STATE.produce_banshee = false;
		}

		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SIEGETANK) + OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED) > 5) {
			FINALSTRATEGY_STATE.current_build++;
		}
		break;
	}
	case STAGE5_FINALSTRATEGY: {
		std::cout << STAGE5_FINALSTRATEGY << std::endl;
		//BANSHEE_STATE.morph_techlab = true;
		OrionBot::TryBuildCommandCentre();
		OrionBot::TryBuildSupplyDepot();
		OrionBot::BuildRefinery();
		OrionBot::FillRefineries();

		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 0) {
			OrionBot::TryBuildBarracks();
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) < 3) {
			FINALSTRATEGY_STATE.produce_banshee = true;
		}
		else {
			FINALSTRATEGY_STATE.produce_banshee = false;
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

void OrionBot::CombinedOnUnitIdle(const Unit* unit) {
	//Units enemy_units = Observation()->GetUnits(Unit::Alliance::Enemy);
	switch (unit->unit_type.ToType()) {
	case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
		if (FINALSTRATEGY_STATE.orbital_upgrade) {
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
			//OrionBot::tryCalldownExtraSupplies(unit);
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE, mineral_target);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SUPPLYDEPOT: {
		Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
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
		if (FINALSTRATEGY_STATE.morph_reactor) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_REACTOR_BARRACKS);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORY: {

		if (FINALSTRATEGY_STATE.produce_hellion) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_HELLION);
		}
		else if (FINALSTRATEGY_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_FACTORY);
		}
		else {
			if (FINALSTRATEGY_STATE.current_build >= STAGE4_BANSHEE) {
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
		if (FINALSTRATEGY_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_STARPORT);
		}
		if (FINALSTRATEGY_STATE.produce_banshee) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_BANSHEE);
		}
		if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_RAVEN) < 2) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_RAVEN);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		//const GameInfo& game_info = Observation()->GetGameInfo();
		//Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_HELLION: {
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
			//Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front()->pos);
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_BANSHEE: {
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
			//Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front()->pos);
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
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
			//Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front()->pos);
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, OrionBot::FindEnemyBase());
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
		}
	}
	case UNIT_TYPEID::TERRAN_SIEGETANK: {
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
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
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
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
		if (FINALSTRATEGY_STATE.current_build >= STAGE5_FINALSTRATEGY) {
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
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, Observation()->GetStartLocation());
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

