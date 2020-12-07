#pragma once

void Rax6Build();

void Rax6OnUnitIdle(const Unit* unit);

/*
* Build a new command centre at the expansion location.
*/
bool TryBuildCommandCentreExpansion(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type);

/*
 * Takes in position and builds the specified structure at the specified location
*/
bool TryBuildStructureAtCP(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type, Point2D toBuildPos);

/*
 * Set the positions of chike points for SD and rax
 * for each of the four corners.
*/
void setChokePoints();

void TryScout();