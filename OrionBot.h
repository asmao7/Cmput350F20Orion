#ifndef ORION_SC2_BOT_H_
#define ORION_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "sc2api/sc2_unit_filters.h"
#include <iostream>
#include <vector>

#include <cstddef>

using namespace sc2;

class OrionBot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);

private:
	size_t CountUnitType(UNIT_TYPEID unit_type);
	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
	const Unit* FindNearestMineralPatch(const Point2D& start);
	bool TryBuildSupplyDepot();
	bool TryBuildBarracks();

	//Global State Tracker
	//Lets us know what strategy we are playing
	enum RushStrategy { RUSH_BANSHEE = 0, RUSH_12MARINES, RUSH_6RAX };
	//int RUSH_STRATEGY = RUSH_BANSHEE;
	int RUSH_STRATEGY = RUSH_6RAX;

	//6Rax Rush Strategy Variables
	//Made by: Asma
	struct Rax6 {
		Rax6() :
			upgradeOrbital(false), newCommandCentre(false), currentBuild(0) {}
		bool upgradeOrbital;
		bool newCommandCentre;
		int currentBuild;
	};
	Rax6 RAX6_STATE;
	enum Rax6_Strategy { STAGE1_RAX6 = 0, STAGE2_RAX6, STAGE3_RAX6, STAGE4_RAX6 };


	//12 Marines Rush Strategy Variables


	//Banshee Rush Strategy Variables
	//Made by: Joe
	struct Banshee {
		Banshee() :
			orbital_upgrade(false), produce_hellion(false), produce_banshee(false), 
			morph_techlab(false), morph_reactor(false), current_build(0)
		{
		}
		bool orbital_upgrade;
		bool produce_hellion;
		bool produce_banshee;
		bool morph_techlab;
		bool morph_reactor;
		int current_build;
	};
	Banshee BANSHEE_STATE;
	enum Banshee_Strategy { STAGE1_BANSHEE = 0, STAGE2_BANSHEE, STAGE3_BANSHEE, STAGE4_BANSHEE };


	

	#include "BuildManager.h"
	#include "Banshee.h"
	#include "Rax6.h"
	#include "Marines12.h"
};

#endif