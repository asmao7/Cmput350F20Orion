#include "OrionBot.h"

void OrionBot::OnGameStart() { 
    std::cout << "Hello, World!" << std::endl;
}

void OrionBot::OnStep() { 
    TryBuildOrbitalCommand();
    TryBuildSupplyDepot();
    /*TryBuildRefinery();*/
    TryBuildBarracks();
    TryBuildCommandCenter();
    TryBuildFactory();
    /*TryScouting();*/
    TryAttacking();
    /*TryBuildStarport();
    TryBuildHellion();*/
    /*std::cout << refinery_list.size() << std::endl;*/
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
        if (AddWorkersToRefineries(unit)) {
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

    Actions()->UnitCommand(unit_to_build,
        ability_type_for_structure,
        Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
    return true;
}
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
        refinery_list.push_back(location_tag);
        return true;
    }
    return false;
}

bool OrionBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
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
bool OrionBot::AddWorkersToRefineries(const Unit* unit) {
    /*if (!refinery_list.empty()) {
        std::cout << "try" << std::endl;
        size_t refinery_workers = CountUnitType(UNIT_TYPEID::TERRAN_REFINERY);
        size_t max_workers = refinery_list.size() * 3;
        if (refinery_workers < max_workers) {
            for (const auto& x : refinery_list) {
                const Unit* target = Observation()->GetUnit(x);
                Actions()->UnitCommand(unit, ABILITY_ID::HARVEST_GATHER, target);
                return true;
            }
        }
    }
    return false;*/
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

bool OrionBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();
    /*if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }
    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 3) {
        return false;
    }
    if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) < 1) {
        return false;
    }*/
    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }
    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 4) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

//Try to build factory, once we have 9 supply depots.
//Made by: Joe
//bool OrionBot::TryBuildOrbitalCommand() {
//    const ObservationInterface* observation = Observation();
//    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 9) {
//        return false;
//    }
//    if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 1) {
//        return false;
//    }
//    //Fix
//    return TryBuildStructure(ABILITY_ID::LAND_ORBITALCOMMAND);
//}

//Try to build factory, once we have 12 SCVs
//Made by: Joe
bool OrionBot::TryBuildFactory() {
    const ObservationInterface* observation = Observation();
    if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < 15) {
        return false;
    }
    if (CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) == 1) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::BUILD_FACTORY);
}

bool OrionBot::TryBuildOrbitalCommand() {
    /*if (CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) > 1) {
        return false;
    }*/
    Units command_centers = Observation()->GetUnits(Unit::Alliance::Self, IsTownHall());
    if (command_centers.size() != 0) {
        Actions()->UnitCommand(command_centers.front(), ABILITY_ID::MORPH_ORBITALCOMMAND);
        return true;
    }
    return false;
}

bool OrionBot::TryBuildStarport() {
    if (CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) > 1) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::BUILD_STARPORT);
}

bool OrionBot::TryBuildHellion() {
    if (CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 1) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::MORPH_HELLION);
}

bool OrionBot::TryBuildCommandCenter() {
    if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 2) {
        return false;
    }
    return TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER);
}


/*
 * Attacking only if enemy
 * is in close proximity,
*/
void OrionBot::TryAttacking() {
    const ObservationInterface* observation = Observation();

    Units enemyUnits = observation->GetUnits(Unit::Alliance::Enemy);
    Units selfMarrineUnits = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
    Units SVCs = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));

    int ATTACK_RADIUS = 60;     // picked randomly, need to find a better way*
    const Point2D& start = observation->GetStartLocation();
    for (const auto& u : enemyUnits) {
        float d = DistanceSquared2D(u->pos, start); 
        if (d <= ATTACK_RADIUS) {
            const GameInfo& game_info = Observation()->GetGameInfo();
            //TryScouting();
            Actions()->UnitCommand(selfMarrineUnits, ABILITY_ID::ATTACK_ATTACK, enemyUnits.front());
            if (!SVCs.empty()) {
                Actions()->UnitCommand(SVCs.front(), ABILITY_ID::ATTACK_ATTACK, enemyUnits.front()); // game_info.enemy_start_locations.front());
            }
        }
    }

}

/*
 * if SVCs > 10  then scout
 * 
*/
void OrionBot::TryScouting() {
    const ObservationInterface* observation = Observation();
    const GameInfo& game_info = Observation()->GetGameInfo();
    Units SVCs = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
    if (SVCs.size() > 10) {
        //Actions()->UnitCommand(SVCs.front(), ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
        Actions()->UnitCommand(SVCs, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
    }
}
