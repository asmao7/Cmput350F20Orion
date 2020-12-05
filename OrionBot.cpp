#include "OrionBot.h"

void OrionBot::OnGameStart() { 
    std::cout << "Hello, World!" << std::endl;
    OrionBot::FindMap();
}

void OrionBot::OnStep() { 
    switch (RUSH_STRATEGY) {
        case RUSH_BANSHEE: {
            OrionBot::BansheeBuild();
            break;
        }
        case RUSH_12MARINES: {
            OrionBot::Marines12Build();
            break;
        }
        case RUSH_6RAX: {
            OrionBot::Rax6Build();
            break;
        }
        case RUSH_FINAL: {
            OrionBot::CombinedBuild();
            break;
        }
    }
}

void OrionBot::OnUnitIdle(const Unit* unit) {
    switch (RUSH_STRATEGY) {
        case RUSH_BANSHEE: {
            OrionBot::BansheeOnUnitIdle(unit);
            break;
        }
        case RUSH_12MARINES: {
            OrionBot::Marines12OnUnitIdle(unit);
            break;
        }
        case RUSH_6RAX: {
            OrionBot::Rax6OnUnitIdle(unit);
            break;
        }
        case RUSH_FINAL: {
            OrionBot::CombinedOnUnitIdle(unit);
            break;
        }
    }
}

// From Sc2 Cpp Tutorial
size_t OrionBot::CountUnitType(UNIT_TYPEID unit_type) {
    return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

// From Sc2 Cpp Tutorial
bool OrionBot::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    // Also get an scv to build the structure.
    const Unit* unit_to_build = nullptr;
    Units units = observation->GetUnits(Unit::Alliance::Self);
    //Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
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

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    if (!bases.empty()) {
        Actions()->UnitCommand(unit_to_build,
            ability_type_for_structure,
            Point2D(bases.front()->pos.x + rx * 15.0f, bases.front()->pos.y + ry * 15.0f));
    }
    else {
        Actions()->UnitCommand(unit_to_build,
            ability_type_for_structure,
            Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
    }
    return true;
}



////NEW ADDED
bool OrionBot::TryBuildStructureTest(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type) {
    //std::cout << "Try build" << std::endl;
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    //if we have no workers Don't build
    if (bases.empty()) {
        return false;
    }
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(bases.front()->pos.x + rx * 15, bases.front()->pos.y + ry * 15);

    Units units = observation->GetUnits(Unit::Alliance::Self);
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) {
            continue;
        }
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 5.5) {
        return false;
    }

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
    if (Query()->PathingDistance(unit, build_location) < 0.1f) {
        return false;
    }
    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, build_location)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, build_location);
        return true;
    }
    return false;
}




// From Sc2 Cpp Tutorial
bool OrionBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    /*
    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;
    */

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

// From Sc2 Cpp Tutorial
// Modified Slightly
bool OrionBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}



// From Sc2 Cpp Tutorial
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


