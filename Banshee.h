#include "OrionBot.h"
//Banshee Rush Strategy
//Made by: Joe.

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
		case STAGE1_BANSHEE:
			// 10 - Supply Depot
			if (Observation()->GetMinerals() >= 100) {
				OrionBot::TryBuildSupplyDepot();
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
					/*OrionBot::BuildRefinery();*/
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
				OrionBot::TryBuildSupplyDepot();
				BANSHEE_STATE.current_build++;
			}
			break;
		case STAGE2_BANSHEE:
			/*16 - Marine(1) + Supply Depot
			20 - Starport + Hellion(1 only)
			@100 % Hellion(1) - Tech Lab(1) > Banshees(to 2)
			23 - Refinery(2) + Supply Depot*/
			BANSHEE_STATE.orbital_upgrade = false;
			//16 - Supply Depot
			OrionBot::TryBuildSupplyDepot();
			//16 - Marine(1) 
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 1) {
				TryBuildMarine();
			}
			OrionBot::TryBuildSupplyDepot();
			//if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
			//	/*OrionBot::TryBuildCommandCentre();*/
			//	OrionBot::TryBuildSupplyDepot();
			//}
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
				OrionBot::TryBuildSupplyDepot();
			}
			//23 - Refinery(2) 
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 3) {
					if (Observation()->GetMinerals() >= 75) {
						const ObservationInterface* observation = Observation();
						Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
						bool vespene_target = false;
						if (!workers.empty()) {
							FindNearestVespeneGeyser(workers.front()->pos);
							/*while (!vespene_target) {
								vespene_target = FindNearestVespeneGeyser(workers.front()->pos);
							}*/
						}
					}
					/*OrionBot::BuildRefinery();*/
				}
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 2) {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB) > 0) {
					if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_HELLION) >= 1) {
						BANSHEE_STATE.produce_hellion = false;
						BANSHEE_STATE.morph_techlab = false;
						OrionBot::TryBuildSupplyDepot();
						BANSHEE_STATE.current_build++;
					}
				}
			}
			break;
		case STAGE3_BANSHEE:
			//30 - Barracks > Reactor + Supply Depot
			//32 - Factory > Tech Lab(2)
			//Supply Depot
			std::cout << STAGE3_BANSHEE << std::endl;
			if (Observation()->GetMinerals() >= 100) {
				OrionBot::TryBuildSupplyDepot();
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_BANSHEE) > 2) {
				BANSHEE_STATE.produce_banshee = false;
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
				OrionBot::TryBuildCommandCentre();
			}
			if (Observation()->GetMinerals() >= 100) {
				OrionBot::TryBuildSupplyDepot();
			}
			BANSHEE_STATE.morph_reactor = true;
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 2) {
				OrionBot::TryBuildFactory();
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) >= 2) {
				BANSHEE_STATE.morph_techlab = true;
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
				OrionBot::TryBuildCommandCentre();
			}
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_FACTORYTECHLAB) >= 2) {
				BANSHEE_STATE.morph_reactor = false;
				BANSHEE_STATE.morph_techlab = false;
				OrionBot::TryBuildSupplyDepot();
				BANSHEE_STATE.current_build++;
			}
			break;
		case STAGE4_BANSHEE:
			//39 - Siege Tanks + Supply Depot
			//52 - Siege Tech > @100 % -Attack
			//39 Supply Depot
			std::cout << STAGE4_BANSHEE << std::endl;
			if (Observation()->GetMinerals() >= 100) {
				OrionBot::TryBuildSupplyDepot();
			}
			//39 Siege Tanks
			OrionBot::TryBuildCommandCentre();
			/*if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
				OrionBot::TryBuildCommandCentre();
			}*/
			if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 0) {
				if (OrionBot::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 3) {
					//if (Observation()->GetMinerals() >= 75) {
					//	const ObservationInterface* observation = Observation();
					//	Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
					//	bool vespene_target = false;
					//	if (!workers.empty()) {
					//		FindNearestVespeneGeyser(workers.front()->pos);
					//		/*while (!vespene_target) {
					//			vespene_target = FindNearestVespeneGeyser(workers.front()->pos);
					//		}*/
					//	}
					//}
					OrionBot::BuildRefinery();
				}
			}
			break;
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
		if (unit->energy > 50) {
			const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
			if (!mineral_target) {
				break;
			}
			Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE,mineral_target);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_SCV: {
		/*Units geysers = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
		for (const auto& geyser : geysers) {
			if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
				Actions()->UnitCommand(unit, ABILITY_ID::HARVEST_GATHER, geyser);
				return;
			}
		}*/
		if (AddWorkersToRefineries(unit)) {
			break;
		}
		OrionBot::TryBuildSupplyDepot();
		const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
		// const bool vespene_target = FindNearestVespeneGeyser(unit->pos);
		/*if (AddWorkersToRefineries(unit)) {
			break;
		}*/
		if (!mineral_target) {
			break;
		}
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		break;
	}
	case UNIT_TYPEID::TERRAN_BARRACKS: {
		if (BANSHEE_STATE.morph_reactor) {
			Actions()->UnitCommand(unit, ABILITY_ID::BARRACKSREACTORMORPH_REACTOR);
		}
		/*if (BANSHEE_STATE.morph_reactor) {
			Actions()->UnitCommand(unit, ABILITY_ID::BARRACKSREACTORMORPH_REACTOR);
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
		}*/
		/*if (BANSHEE_STATE.current_build < STAGE4_BANSHEE) {
			if (BANSHEE_STATE.morph_reactor == false) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
			}
		}
		else {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER);
		}*/
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORY: {
		if (BANSHEE_STATE.produce_hellion) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_HELLION);
		}
		if (BANSHEE_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_FACTORY);
		}
		if (BANSHEE_STATE.current_build == STAGE4_BANSHEE) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SIEGETANK);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_STARPORT: {
		if (BANSHEE_STATE.morph_techlab) {
			Actions()->UnitCommand(unit, ABILITY_ID::BUILD_TECHLAB_STARPORT);
		}
		if (BANSHEE_STATE.produce_banshee) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_BANSHEE);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_FACTORYTECHLAB: {
		if (BANSHEE_STATE.current_build == STAGE4_BANSHEE) {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SIEGETANK);
		}
		break;
	}
	case UNIT_TYPEID::TERRAN_MARINE: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		break;
	}
	case UNIT_TYPEID::TERRAN_HELLION: {
		break;
	}
	case UNIT_TYPEID::TERRAN_BANSHEE: {
		break;
	}
	case UNIT_TYPEID::TERRAN_REAPER: {
		const GameInfo& game_info = Observation()->GetGameInfo();
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
		break;
	}
	case UNIT_TYPEID::TERRAN_SIEGETANK: {
		Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
		break;
	}
	default: {
		break;
	}
	}
}