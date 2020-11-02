#include <iostream>
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

#include "OrionBot.h"
#include "LadderInterface.h"

// LadderInterface allows the bot to be tested against the built-in AI or
// played against other bots
int main(int argc, char* argv[]) {
	// Coordinator coordinator;
    // coordinator.LoadSettings(argc, argv);

    // OrionBot bot;
    // coordinator.SetParticipants({
    //     CreateParticipant(Race::Terran, &bot),
    //     CreateComputer(Race::Zerg)
    // });

    // coordinator.LaunchStarcraft();
    // coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    // while (coordinator.Update()) {
    // }
	RunBot(argc, argv, new OrionBot(), sc2::Race::Terran);
	return 0;
}