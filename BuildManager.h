bool TryBuildStructureTargeted(ABILITY_ID ability_type_for_structure, Tag location_tag, UNIT_TYPEID unit_type);

const bool FindNearestVespeneGeyser(const Point2D& start);
bool AddRefineryWorkers();
bool TryBuildAcademy();
bool AddWorkersToRefineries(const Unit* unit);
bool TryBuildFactory();
void TryAttacking();
void TryScouting();
void TryBuildOrbitalCommand();
bool TryBuildStarport();
bool TryBuildHellion();
bool TryBuildSCVs();
bool TryBuildMarine();
//added
bool BuildRefinery();
bool TryBuildGas(AbilityID build_ability, UnitTypeID worker_type, Point2D base_location);
bool TryBuildEngineeringBay();
bool TryBuildGhostAcademy();
bool FillRefineries();
bool TryBuildMissleTurret();

bool TryBuildCommandCentre();
const Unit* scouts = NULL;
const Unit* baseUnit = NULL;

bool FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit);
bool FindEnemyPosition(Point2D& target_pos);

std::vector<Tag> refinery_list;
