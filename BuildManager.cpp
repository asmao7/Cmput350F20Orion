#include "OrionBot.h"

//Taken from Blizzard Sc2 Example Library
//Builds a structure at given location.
//Takes in a point.
//Made by: Joe
bool OrionBot::TryBuildStructureTargeted(ABILITY_ID ability_type_for_structure, Tag location_tag, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
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

//Find nearest vespene geyser to the scv.
//Adapted from find nearest minerals function.
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

//Try to build factory
//Made by: Joe
bool OrionBot::TryBuildFactory() {
    return OrionBot::TryBuildStructure(ABILITY_ID::BUILD_FACTORY);
}

//Try to build starport
//Made by: Joe
bool OrionBot::TryBuildStarport() {
    return OrionBot::TryBuildStructure(ABILITY_ID::BUILD_STARPORT);
}

//Add SCVs to refineries.
//Reason is so that refineries are at max efficiency (3 SCVs)
//Made by: Joe
bool OrionBot::AddWorkersToRefineries(const Unit* unit) {
    const ObservationInterface* observation = Observation();
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsVisibleGeyser());
    for (const auto& geyser : geysers) {
        if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
            Actions()->UnitCommand(unit, ABILITY_ID::HARVEST_GATHER, geyser);
            return true;
        }
    }
    return false;
}

/*
 * Fix!
 * ~Asma
*/
bool OrionBot::TryBuildMarine() {
    //return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
    const ObservationInterface* observation = Observation();
    //If we are at supply cap, don't build anymore units, unless its an overlord.
    if (observation->GetFoodUsed() >= observation->GetFoodCap() && ABILITY_ID::TRAIN_MARINE != ABILITY_ID::TRAIN_OVERLORD) {
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
    return OrionBot::TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER);
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

/*
 * NOTE: I USED THE API A LOT
 * WILL CITE PROPERLY LATER
 * ~Asma
*/
