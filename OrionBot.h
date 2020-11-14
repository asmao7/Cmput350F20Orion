#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "sc2api/sc2_unit_filters.h"
#include <iostream>

#include <cstddef>

using namespace sc2;

class OrionBot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);

private:
	size_t CountUnitType(UNIT_TYPEID unit_type);
	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type);
	bool TryBuildStructureTargeted(ABILITY_ID ability_type_for_structure, Tag location_tag, UNIT_TYPEID unit_type);
	bool TryBuildSupplyDepot();
	bool TryBuildBarracks();
	void TryAttacking();
	void TryScouting();

	const Unit* FindNearestMineralPatch(const Point2D& start);
	const bool FindNearestVespeneGeyser(const Point2D& start);
	bool AddRefineryWorkers();
	bool TryBuildAcademy();
	bool TryBuildOrbitalCommand();
	bool TryBuildFactory();
	bool TryBuildSCVs();

	bool TryBuildCommandCentre();
	const Unit* scouts = NULL;
	const Unit* baseUnit = NULL;

	bool FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit);
	bool FindEnemyPosition(Point2D& target_pos);
	

};

#endif