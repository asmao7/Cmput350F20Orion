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
#include <queue>
#include <map>

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
	//ADDED
	bool TryBuildStructureRandom(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);

	//Global State Tracker
	//Lets us know what strategy we are playing
	enum RushStrategy { RUSH_BANSHEE = 0, RUSH_12MARINES, RUSH_6RAX, RUSH_FINAL };
	//int RUSH_STRATEGY = RUSH_BANSHEE;
	//int RUSH_STRATEGY = RUSH_6RAX;
	//int RUSH_STRATEGY = RUSH_12MARINES;
	int RUSH_STRATEGY = RUSH_FINAL;

	/*  
	 * FINAL STRATEGY VARIABLES
	*/
	struct FinalStrategy {
		FinalStrategy() :
			orbital_upgrade(false), produce_hellion(false), produce_banshee(false),
			morph_techlab(false), morph_reactor(false), current_build(0), num_units_scouting(0), scouting(true)
		{
		}
		bool orbital_upgrade;
		bool produce_hellion;
		bool produce_banshee;
		bool morph_techlab;
		bool morph_reactor;
		bool scouting;
		int current_build;
		int num_units_scouting;
		bool newCommandCentre = false;
		bool expand = false;
		bool toExpand = false;
		bool BVMap = false;
		Point2D wait_location;

		Point2D BOTTOM_LEFT_WAIT_POINT = Point2D(30, 66.1);
		Point2D BOTTOM_RIGHT_WAIT_POINT = Point2D(125.992, 30.5756);
		Point2D TOP_LEFT_WAIT_POINT = Point2D(66.008, 161.424);
		Point2D TOP_RIGHT_WAIT_POINT = Point2D(160.9, 125.925);
	};
	FinalStrategy FINALSTRATEGY_STATE;
	enum Final_Strategy { STAGE1_FINALSTRATEGY = 0, STAGE2_FINALSTRATEGY, STAGE3_FINALSTRATEGY, STAGE4_FINALSTRATEGY, STAGE5_FINALSTRATEGY};


	

	//6Rax Rush Strategy Variables
	//Made by: Asma
	int max_worker_count_ = 70;
	struct Rax6 {
		Rax6() :
			upgradeOrbital(false), newCommandCentre(false), currentBuild(0),
			expand(false), attacking(false), num_units_scouting(0) {}
		bool upgradeOrbital;
		bool newCommandCentre;
		int currentBuild;
		bool expand;
		int num_units_scouting;
		bool attacking;
		bool enemy_found = false;
		Point2D enemy_base = Point2D(100, 100);
	
		int raxs = 0;
		Point2D barracks;

		Point2D tobuildSD;
		Point2D tobuildRaxs;

		Point2D BOTTOM_LEFT = Point2D(33.5, 33.5);	
		Point2D BOTTOM_RIGHT = Point2D(158.5, 33.5);
		Point2D TOP_LEFT = Point2D(33.5, 158.5);
		Point2D TOP_RIGHT = Point2D(158.5, 158.5);
	};
	Rax6 RAX6_STATE;
	enum Rax6_Strategy { STAGE1_RAX6 = 0, STAGE2_RAX6, STAGE3_RAX6, STAGE4_RAX6, STAGE5_RAX6 };
	

	//Banshee Rush Strategy Variables
	//Made by: Joe
	struct Banshee {
		Banshee() :
			orbital_upgrade(false), produce_hellion(false), produce_banshee(false), 
			morph_techlab(false), morph_reactor(false), current_build(0), num_units_scouting(0)
		{
		}
		bool orbital_upgrade;
		bool produce_hellion;
		bool produce_banshee;
		bool morph_techlab;
		bool morph_reactor;
		int current_build;
		int num_units_scouting;
	};
	Banshee BANSHEE_STATE;
	enum Banshee_Strategy { STAGE1_BANSHEE = 0, STAGE2_BANSHEE, STAGE3_BANSHEE, STAGE4_BANSHEE, STAGE5_BANSHEE};


	//12 Marines Rush Strategy Variables

	struct Marines12 {
		Marines12() : orbital_upgrade(false), produce_scv(true), current_build(0),
			orbital_command_upgraded(false), supplies_called(false),
			attacking(false), num_units_scouting(0), i_location(0)
		{}
		int current_build;
		bool orbital_upgrade;
		bool produce_scv;
		bool orbital_command_upgraded;
		bool supplies_called;
		bool attacking;
		int num_units_scouting;
		int i_location;
	};

	Marines12 MARINES12_STATE;
	enum Marines12_Strategy { STAGE1_MARINES = 0, STAGE2_MARINES, STAGE3_MARINES, STAGE4_ATTACK_MARINES };

	std::vector<Point2D> locations_enemy_seen;
	std::queue<Point2D> locations_enemy_seen2;

	std::vector<Point2D> possible_enemy_bases;
	std::vector<Point3D>expansion_locations;
	std::vector<int> enemyBaseValue;

	bool found_locations = false;
	bool next = false;
	bool wait;

	Point2D base_location;

	Point2D current_location;

	//std::map<Point2D, int> possible_enemy_bases;
	#include "BuildManager.h"
	#include "Banshee.h"
	#include "Rax6.h"
	#include "Marines12.h"
	#include "CombinedStrategy.h" 
	#include "MapManager.h"
};

#endif