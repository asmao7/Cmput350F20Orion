#include "OrionBot.h"

void OrionBot::OnGameStart() { 
    std::cout << "Hello, World!" << std::endl;
}

void OrionBot::OnStep() { 
    TryBuildSupplyDepot();

    TryBuildBarracks();

    //TryScouting();

    TryAttacking();
}
void OrionBot::OnUnitIdle(const Unit* unit){
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
        }
        case UNIT_TYPEID::TERRAN_SCV: {
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

bool OrionBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

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

bool OrionBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
        return false;
    }

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
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