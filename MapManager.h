#include "OrionBot.h"

void OrionBot::FindMap() {
	enum class Maps { CactusValleyLE, BelShirVestigeLE, ProximaStationLE };

	const char* map_name = Observation()->GetGameInfo().map_name.c_str();

	Maps map;
	if (strcmp(map_name, "Cactus Valley LE (Void)") == 0) {
		map = Maps::CactusValleyLE;
		std::cout << "map is: Cactus" << std::endl;
	}
	else if (strcmp(map_name, "BelShir Vestige LE (Void)") == 0) {
		map = Maps::BelShirVestigeLE;
		std::cout << "map is: BelShir" << std::endl;
	}
	else if(strcmp(map_name, "Proxima Station LE (Void)") == 0) {
		map = Maps::ProximaStationLE;
		std::cout << "map is: Proxima" << std::endl;
	}
	else {
		std::cout << "unknown map" << std::endl;
	}
}