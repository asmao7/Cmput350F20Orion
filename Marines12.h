//#include "OrionBot.h"
// 12 Marines @4:30 Strategy
// Made by Ana

/*
	STAGE 1
	9 - Supply Depot (1)
	Still produce SCVs

	STAGE 2
	11 - Barracks (1)

	Stop producing SCVs
	Start producing marines
	Allow orbital Command Upgrade

	STAGE 3
	12 - Barracks (2-3)

	Once upgraded:
		If have enough resources:
			then Calldown: MULE
			then  Scanner Sweep (in the corners)
		
	STAGE 4 ATTACK
	Attack
*/

void OrionBot::Marines12Build();

void OrionBot::Marines12OnUnitIdle(const Unit* unit);
