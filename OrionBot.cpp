#include "OrionBot.h"

void OrionBot::OnGameStart() {
    std::cout << "Hello, World!" << std::endl;
}

void OrionBot::OnStep() {

    // build supply depots
    TryBuildSupplyDepot();

    // Build Barracks
    TryBuildBarracks();

    // build as much marines while possible
    TryBuildMarine();

    /*TryScouting();*/
    TryAttacking();
}

void OrionBot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
        Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
        break;
    }
    case UNIT_TYPEID::TERRAN_SCV: {
        const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
        const bool vespene_target = FindNearestVespeneGeyser(unit->pos);
        if (!mineral_target) {
            break;
        }
        if (vespene_target) {
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
        const GameInfo& game_info = Observation()->GetGameInfo();
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
        break;
    }
    default: {
        break;
    }
    }
}

size_t OrionBot::CountUnitType(UNIT_TYPEID unit_type) {
    return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

bool OrionBot::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
    const ObservationInterface* observation = Observation();

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

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    if (ability_type_for_structure == ABILITY_ID::BUILD_SUPPLYDEPOT && CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) == 0) {
			BuildSupplyDepot(unit_to_build);
		}

    Actions()->UnitCommand(unit_to_build,
        ability_type_for_structure,
        Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

    return true;
}

bool OrionBot::TryBuildStructureTargeted(ABILITY_ID ability_type_for_structure,
  Tag location_tag, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
    const ObservationInterface* observation = Observation();
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    const Unit* target = observation->GetUnit(location_tag);

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

    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, target->pos)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, target);
        return true;
    }
    return false;
}

/* from bot_examples.cc line 2904 - 2922
* debbie
 to be modified */

bool OrionBot::TryBuildRefinery() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_REFINERY) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base->assigned_harvesters >= base->ideal_harvesters) {
            if (base->build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_SCV, base->pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool OrionBot::TryBuildSupplyDepot() {
  const ObservationInterface* observation = Observation();

  // If we are not supply capped, don't build a supply depot.
  if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
     return false;
  }

  if (observation->GetMinerals() < 100) {
     return false;
  }

  //check to see if there is already on building
  Units units = observation->GetUnits(Unit::Alliance::Self, IsUnits(supply_depot_types));
  if (observation->GetFoodUsed() < 40) {
     for (const auto& unit : units) {
         if (unit->build_progress != 1) {
             return false;
         }
     }
  }

  // Try and build a supply depot. Find a random SCV and give it the order.
  float rx = GetRandomScalar();
  float ry = GetRandomScalar();
  Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
  return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, build_location);

}

bool OrionBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 3) {
        return false;
    }

    if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 1) {
        return false;
    }

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

/* example code from bot_example.cc
* to be modified and called
*debbie
*/
bool OrionBot::TryBuildSCVs() {
    if (CountUnitType(Observation(), UNIT_TYPEID::TERRAN_SCV) >= TargetSCVCount)
        return false;

    return TryBuildUnit(ABILITY_ID::TRAIN_SCV, UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

bool OrionBot::TryBuildMarine() {
    return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
}


//
const Unit* OrionBot::FindNearestMineralPatch(const Point2D& start) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;
}


//Find nearest vespene geyser to the scv.
//Takes in a point.
//Made by: Joe
const bool OrionBot::FindNearestVespeneGeyser(const Point2D& start) {
    Units geysers = Observation()->GetUnits(Unit::Alliance::Neutral,IsGeyser());
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    float minimum_distance = 15.0f;
    Tag closestGeyser = 0;
    for (const auto& u : geysers) {
        float d = DistanceSquared2D(u->pos, start);
        if (d < distance) {
            distance = d;
            target = u;
            closestGeyser = u->tag;
        }
    }
    if (closestGeyser == 0) {
        return false;
    }
    return TryBuildStructureTargeted(ABILITY_ID::BUILD_REFINERY,closestGeyser);
}


//Try to build factory, once we have 9 supply depots.
//Made by: Joe
bool OrionBot::TryBuildOrbitalCommand() {
    const ObservationInterface* observation = Observation();
    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 9) {
        return false;
    }
    if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 1) {
        return false;
    }
    //Fix
    return TryBuildStructure(ABILITY_ID::LAND_ORBITALCOMMAND);
}

//Try to build factory, once we have 12 SCVs
//Made by: Joe
bool OrionBot::TryBuildFactory() {
    const ObservationInterface* observation = Observation();
    if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) > 12) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::BUILD_FACTORY);
}

/*
 * Fix!
 * ~Asma
*/
bool OrionBot::TryBuildMarine() {
    //return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
    const ObservationInterface* observation = Observation();
    //If we are at supply cap, don't build anymore units, unless its an overlord.
    if (observation->GetFoodUsed() >= observation->GetFoodCap()
    && ABILITY_ID::TRAIN_MARINE != ABILITY_ID::TRAIN_OVERLORD) {
        return false;
    }
    const Unit* unit = nullptr;
    Units my_units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto u : my_units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            unit = u;
            if (!unit->orders.empty()) {
                return false;
            }
            if (unit->build_progress != 1) {
                return false;
            }
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
            return true;
        }
    }
    return false;

}

/*
 * TODO!
 * ~Asma
*/
bool OrionBot::TryBuildCommandCentre() {
    const ObservationInterface* observation = Observation();

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER);
}



/*
 * ~ in progress!
 * Attacking only if enemy
 * is in close proximity,
 * ~Asma
*/
void OrionBot::TryAttacking() {
    const ObservationInterface* observation = Observation();

    Units enemyUnits = observation->GetUnits(Unit::Alliance::Enemy);
    Units selfMarrineUnits = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
    Units SVCs = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));

    int ATTACK_RADIUS = 30;     // picked randomly, need to find a better way*
    const Point3D& start = observation->GetStartLocation();
    for (const auto& u : enemyUnits) {
        float d = DistanceSquared2D(u->pos, Point2D(start.x, start.y));//baseUnit->pos);
        if (d <= ATTACK_RADIUS) {
            const GameInfo& game_info = Observation()->GetGameInfo();
            //TryScouting();
            Actions()->UnitCommand(selfMarrineUnits, ABILITY_ID::ATTACK_ATTACK, u);  //enemyUnits.front());
            if (!SVCs.empty()) {
                Actions()->UnitCommand(SVCs, ABILITY_ID::ATTACK_ATTACK, u); // game_info.enemy_start_locations.front()); // enemyUnits.front()); //
            }
        }
    }

}

/*
 * in progress!
 * if SVCs > 10  then scout
 * ~Asma
*/
void OrionBot::TryScouting() {
    const ObservationInterface* observation = Observation();
    const GameInfo& game_info = Observation()->GetGameInfo();
    Units SVCs = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
    if (SVCs.size() > 10) {
        //Actions()->UnitCommand(SVCs.front(), ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
        Actions()->UnitCommand(SVCs.front(), ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
    }
}

/*
 * Utility function for scouting
 * Find any enemy structures
 * ~Asma
*/
bool OrionBot::FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit) {
    Units my_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : my_units) {
        if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT ||
            unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            enemy_unit = unit;
            return true;
        }
    }
    return false;
}

/*
 * Tries to find a random location that can be pathed to on the map.
 * ~Asma
*/
bool OrionBot::FindEnemyPosition(Point2D& target_pos) {
    const GameInfo& game_info = Observation()->GetGameInfo();
    if (game_info.enemy_start_locations.empty()) {
        return false;
    }
    target_pos = game_info.enemy_start_locations.front();
    return true;
}
