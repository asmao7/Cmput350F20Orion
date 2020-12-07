#include "OrionBot.h"

/*
 * Find the map we are playing on to determine whether to expand to 
 * the corners or no and to find the choke points accordingly for
 * defense.

 * Since Cactus valley and Belshir maps have their expansion 
 * points infront, send the units their to defend while waiting. 
 * For now, no sending defense units for ProximaStationLE map, coz
 * the expansion point is behind.
*/

void OrionBot::FindMap() {
	enum class Maps { CactusValleyLE, BelShirVestigeLE, ProximaStationLE };
	const char* map_name = Observation()->GetGameInfo().map_name.c_str();
	Maps map;

	std::cout << "map is: Cactus Valley LE (Void)" << std::endl;
	if (strcmp(map_name, "Cactus Valley LE (Void)") == 0) {
		map = Maps::CactusValleyLE;
		FINALSTRATEGY_STATE.toExpand = true;

		if (Observation()->GetStartLocation().x == 33.5 && Observation()->GetStartLocation().y == 158.5) {
			FINALSTRATEGY_STATE.wait_location = FINALSTRATEGY_STATE.TOP_LEFT_WAIT_POINT;
		}
		else if (Observation()->GetStartLocation().x == 158.5 && Observation()->GetStartLocation().y == 158.5) {
			FINALSTRATEGY_STATE.wait_location = FINALSTRATEGY_STATE.TOP_RIGHT_WAIT_POINT;
		}
		else if (Observation()->GetStartLocation().x == 33.5 && Observation()->GetStartLocation().y == 33.5) {
			FINALSTRATEGY_STATE.wait_location = FINALSTRATEGY_STATE.BOTTOM_LEFT_WAIT_POINT;
		}
		else {
			FINALSTRATEGY_STATE.wait_location = FINALSTRATEGY_STATE.BOTTOM_RIGHT_WAIT_POINT;
		}
	}
	else if (strcmp(map_name, "Bel'Shir Vestige LE (Void)") == 0) {
		map = Maps::BelShirVestigeLE;
		FINALSTRATEGY_STATE.BVMap = true;
		std::cout << "map is: Bel'Shir Vestige LE (Void)" << std::endl;

		const ObservationInterface* observation = Observation();
		Point3D startLocation_ = Observation()->GetStartLocation();
		Point3D staging_location_ = startLocation_;
		std::vector<Point3D> expansions_ = search::CalculateExpansionLocations(Observation(), Query());
		Point2D closest_expansion;
		float minimum_distance = std::numeric_limits<float>::max();
		for (const auto& expansion : expansions_) {
			float current_distance = Distance2D(startLocation_, expansion);
			if (current_distance < .01f) {
				continue;
			}
			if (current_distance < minimum_distance) {
				if (Query()->Placement(ABILITY_ID::BUILD_COMMANDCENTER, expansion)) {
					closest_expansion = expansion;
					minimum_distance = current_distance;
				}
			}
		}
		FINALSTRATEGY_STATE.wait_location = closest_expansion;
	}
	else if(strcmp(map_name, "Proxima Station LE (Void)") == 0) {
		map = Maps::ProximaStationLE;
		std::cout << "map is: Proxima Station LE (Void)" << std::endl;
	}
	else {
		// should not get to this point
		std::cout << "unknown map" << std::endl;
	}
}