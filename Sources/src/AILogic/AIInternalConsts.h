#ifndef __AI_INTERNAL_CONSTS_H__
#define __AI_INTERNAL_CONSTS_H__

#pragma once

#include "AIConsts.h"

struct SConsts : SAIConsts
{
private:
  static void LoadRevealInfo(class CTableAccessor &constsTbl);

public:
  // distance in which to recalculate fog for units when removing stats. 
  static const int MAX_DIST_TO_RECALC_FOG;

  // What is the maximum angle to turn instantly?
  static const int TURN_TOLERANCE;

  // standard viewing angle
  static const WORD STANDART_VIS_ANGLE;

  // At what angle difference should you stop and turn?
  static const WORD DIR_DIFF_TO_SMOOTH_TURNING;

  // the maximum length of the path in tiles during which you can drive backwards
  static const int MAX_LEN_TO_GO_BACKWARD;

  // how forward iteration is to look forward when walking along a spline
  static const int NUMBER_ITERS_TO_LOOK_AHEAD;

  // for a rectangle in front of a moving unit
  static const int SPEED_FACTOR;

  // how many path tiles to measure when constructing a spline
  static const short int SPLINE_STEP;

  // size of "neighbors scan" cell
  static const int CELL_COEFF;
  static const int CELL_SIZE;// must be divisible by TILE_SIZE

  static const int BIG_CELL_COEFF;
  static const int BIG_CELL_SIZE;

  // max number of tiles, occupied by a unit
  static const int MAX_UNIT_TILE_RADIUS;
  // maximum unit radius in points
  static const int MAX_UNIT_RADIUS;

  static const int BIG_PATH_SHIFT;

  // starting sizes of vectors
  static const int AI_START_VECTOR_SIZE;

  //
  // maximum distance for units to be in the same "simple" group
  static const int GROUP_DISTANCE;

  // periods of scanning while searching for the nearest enemy for anti-artillery combat
  static const float ANTI_ARTILLERY_SCAN_TIME;

  // coefficient for thickening/lengthening boundRect
  static const float BOUND_RECT_FACTOR;
  // coefficient for thickening/lengthening boundRect when locating tiles
  static const float COEFF_FOR_LOCK;
  // the distance from the transport landing point to the point where you need to immediately run from there (to find the way)
  static const float DIST_FOR_LAND;
  // the distance from the transport drop-off point to the point where you need to immediately run from there (to check the “suitability” of the point)
  static float GOOD_LAND_DIST;

  //
  // dividing the map into cells for static objects
  static const int STATIC_OBJ_CELL;
  static const int STATIC_CONTAINER_OBJ_CELL;

  // cell size for recording exploding shells
  static const int HIT_CELL_COEFF;
  static const int HIT_CELL_SIZE;

  // number of directions soldiers have
  static const int NUMBER_SOLDIER_DIRS;

  // constants read from an external file
  //
  // healing rate in buildings HP/tick
  static float CURE_SPEED_IN_BUILDING;
  // time between the end of the queue and the beginning of the turret returning to its default position
  static int TIME_TO_RETURN_GUN;
  // number of scans per segment
  static int NUM_TO_SCAN_IN_SEGM;
  // time between updates behavior
  static NTimer::STime BEH_UPDATE_DURATION;
  // time between behavior updates for soldiers
  static NTimer::STime SOLDIER_BEH_UPDATE_DURATION;
  // same for AA
  static NTimer::STime AA_BEH_UPDATE_DURATION;
  // for long-range artillery
  static NTimer::STime LONG_RANGE_ARTILLERY_UPDATE_DURATION;
  // the time that the dead see
  static NTimer::STime DEAD_SEE_TIME;
  // time during which the alarm in the building
  static int TIME_OF_BUILDING_ALARM;
  // time after which you need to camouflage in idle state
  static int TIME_BEFORE_CAMOUFLAGE;
  // time after which the sniper needs to camouflage in idle state
  static int TIME_BEFORE_SNIPER_CAMOUFLAGE;
  // time spent under fire
  static int TIME_OF_LYING_UNDER_FIRE;
  // cover for those lying down - the probability that they will hit
  static float LYING_SOLDIER_COVER;
  // radius in points, which is polled to find out whether it is under fire or not
  static int RADIUS_OF_HIT_NOTIFY;
  // frequency of check, under fire or not
  static int TIME_OF_HIT_NOTIFY;

  // time in milliseconds between 2 scans min for engineers
  static NTimer::STime ENGINEER_MINE_CHECK_PERIOD;
  // radius (in points) in which the engineer sees mines
  static int MINE_VIS_RADIUS;
  // radius (in points) in which the engineer removes mines
  static int MINE_CLEAR_RADIUS;
  // maximum escape distance from the center of the formation
  static int RADIUS_OF_FORMATION;

  // radius within which you can run in guard state
  static float GUARD_STATE_RADIUS;

  // crawling speed multiplier
  static float LYING_SPEED_FACTOR;

  // radius for call for help
  static int CALL_FOR_HELP_RADIUS;
  static int AI_CALL_FOR_HELP_RADIUS;

  // the time that soldiers gather in a building before participating in an assault
  static NTimer::STime CAMPING_TIME;
  // multiplier on weapon range when shooting soldiers inside an object
  static float INSIDE_OBJ_WEAPON_FACTOR;
  // the period of time over which two soldiers fight inside an object
  static NTimer::STime INSIDE_OBJ_COMBAT_PERIOD;
  // the time it takes for a technique to disappear after death
  static NTimer::STime TIME_TO_DISAPPEAR;

  // threshold install/uninstall time for guns to install/uninstall themselves
  static NTimer::STime THRESHOLD_INSTALL_TIME;

  // number of shots to zero in artillery in an area
  static int SHOOTS_TO_RANGE;
  // coefficient for dispersion if shooting is carried out at the targeted area
  static float RANDGED_DISPERSION_RADIUS_BONUS;
  // shooting area radius
  static float RANGED_AREA_RADIUS;

  // the distance the artillery needs to move in order to reset information about its location
  static float RELOCATION_RADIUS;
  // Max. 
  static float MAX_ANTI_ARTILLERY_RADIUS;
  // min. 
  static float MIN_ANTI_ARTILLERY_RADIUS;
  // number of shots to reduce MAX_ANTI_ARTILLERY_RADIUS to MIN_ANTI_ARTILLERY_RADIUS
  static int SHOTS_TO_MINIMIZE_LOCATION_RADIUS;
  // the time it takes for circles to be drawn after a cannon fires
  static NTimer::STime AUDIBILITY_TIME;
  // frequency of drawing anti-artillery circles
  static NTimer::STime REVEAL_CIRCLE_PERIOD;

  // probability of penetrating armor without the need to maneuver
  static float GOOD_ATTACK_RPOBABILITY;

  // for fighter
  static NTimer::STime FIGHTER_PATROL_TIME;// after that time fighter will cancel patrolling
  static NTimer::STime FIGHTER_PATH_UPDATE_TIME;// path is updated once per this time
  static NTimer::STime SHTURMOVIK_PATH_UPDATE_TIME;
  static float FIGHTER_VERTICAL_SPEED_UP;// climb speed
  static float FIGHTER_VERTICAL_SPEED_DOWN;// altitude release rate

  // for paratrooper
  static float PARATROOPER_FALL_SPEED;
  static int PARADROP_SPRED;// to find a free tile to jump onto.

  // the radius in which engineers are looking for units that will be provided with ammunition from the truck
  static int RESUPPLY_RADIUS;
  // for morality
  static int RESUPPLY_RADIUS_MORALE;

  static NTimer::STime TIME_QUANT;// time of quant repear operation
  static float ENGINEER_LOAD_RU_PER_QUANT;// to fill a truck
  static float ENGINEER_REPEAR_HP_PER_QUANT;// additional health
  static float ENGINEER_FENCE_LENGHT_PER_QUANT;// for building fence.
  static float ENGINEER_ENTRENCH_LENGHT_PER_QUANT;// for infantry trenches
  static float ENGINEER_RESUPPLY_PER_QUANT;// for recharging
  static float ENGINEER_ANTITANK_HALTH_PER_QUANT;// for the construction of a hedgehog
  static float ENGINEER_RU_CARRY_WEIGHT;// when loading a truck, engineers carry so many RU

  // binocular viewing radius
  static float SPY_GLASS_RADIUS;
  // binocular viewing angle
  static WORD SPY_GLASS_ANGLE;

  // area damage coefficient
  static float AREA_DAMAGE_COEFF;
  // the minimum angle at which you can rotate the base during a turret attack to improve your position
  static WORD MIN_ROTATE_ANGLE;

  // radius of circles from shots, starting from which artillery begins anti-artillery combat
  static float RADIUS_TO_START_ANTIARTILLERY_FIRE;

  // truck capacity in RU
  static float TRANSPORT_RU_CAPACITY;
  // the distance at which a truck must approach the warehouse
  static float TRANSPORT_LOAD_RU_DISTANCE;
  // the maximum length of the path (from the warehouse, in tiles) along which supply occurs
  static int RESUPPLY_MAX_PATH;

  // time that alarm stays on when a unit is fired upon by ballistics shells
  static float TIME_OF_ALARM_UNDER_FIRE;
  // range of action for warehouses. 
  static float STORAGE_RESUPPLY_RADIUS;

  // air resistance for bombs
  static float TRAJ_BOMB_ALPHA;

  // Having approached this distance to the desired point, the artillerymen will teleport to it.
  static float GUN_CREW_TELEPORT_RADIUS;

  // square of the distance between parachutists' emissions
  static float PLANE_PARADROP_INTERVAL;
  // the maximum displacement of the parachutes is perpendicular to the aircraft movement vector.
  static float PLANE_PARADROP_INTERVAL_PERP_MIN;
  static float PLANE_PARADROP_INTERVAL_PERP_MAX;

  // Once during this interval, parachutists check whether they are falling onto a locked cage
  static NTimer::STime PARATROOPER_GROUND_SCAN_PERIOD;

  // added morale to units from the general's vehicle in 1 BEH_UPDATE_DURATION
  static float MORALE_ADDITION_PER_TICK;
  // decrease in morality over time
  static float MORALE_DECREASE_PER_TICK;
  // chance of decreasing morale
  static float PROBABILITY_TO_DECREASE_MORALE;
  static float MORALE_MIN_VALUE;

  // from this distance the truck behind the gun will drive up backwards
  static float TRANSPORT_MOVE_BACK_DISTANCE;

  // so many times the vehicle will try to unhook the artillery.
  static int TRIES_TO_UNHOOK_ARTILLERY;

  // for aircraft roll
  static float PLANE_TILT_PER_SECOND;

  static float PLANE_GUARD_STATE_RADIUS;
  static float PLANES_HEAVY_FORMATION_SIZE;
  static float PLANES_SMALL_FORMATION_SIZE;
  static float PLANES_START_RANDOM;

  // caterpillar health
  static float TANK_TRACK_HIT_POINTS;

  // what fraction of the distance does the projectile fly straight along a flat trajectory?
  static float TRAJECTORY_LOW_LINE_RATIO;

  static float TRAJECTORY_BOMB_G;

  // square distance to start an attack
  static float SHTURMOVIK_APPROACH_RADIUS_SQR;
  static float SHTURMOVIK_APPROACH_RADIUS;

  // minimum flight altitude for airplanes
  static float PLANE_MIN_HEIGHT;

  // dive exit radius to target
  static float PLANE_DIVE_FINISH_DISTANCE_SQR;
  static float DIVEBOMBER_VERT_MANEUR_RATIO;
  static float GUNPLANES_VERT_MANEUR_RATIO;


  static float SNIPER_CAMOUFLAGE_DECREASE_PER_SHOOT;
  static float SNIPER_CAMOUFLAGE_INCREASE;

  // proportion of units that should be able to hit
  // ambush the target so that the entire Ambush begins to attack.
  static float AMBUSH_ATTACK_BEGIN_CIRTERIA;
  // this value is divided by revealRadous and multiplied by AntiArtilleryRadius
  static float ARTILLERY_REVEAL_COEEFICIENT;
  // minimum and maximum spread for all types of trajectories
  static float dispersionRatio[6][2];

  static float COEFF_FOR_RANDOM_DELAY;

  // what height do you need to climb to increase your visibility radius by one tile?
  static float HEIGHT_FOR_VIS_RADIUS_INC;

  // building burning rate (percentage in tick)
  static float BURNING_SPEED;

  // follow constants
  // radius close enough to the leader
  static float FOLLOW_STOP_RADIUS;
  // the radius in which you need to equate the speed to the speed of the leader
  static float FOLLOW_EQUALIZE_SPEED_RADIUS;
  // radius due to which you need to follow the leader
  static float FOLLOW_GO_RADIUS;
  // radius beyond which the leader must wait
  static float FOLLOW_WAIT_RADIUS;

  static float FATALITY_PROBABILITY;
  static float DAMAGE_FOR_MASSIVE_DAMAGE_FATALITY;
  static float MASSIVE_DAMAGE_FATALITY_PROBABILITY;

  static float BOMB_START_HEIGHT;

  static float STAND_LIE_RANDOM_DELAY;
  // to assess whether to throw bombs to the attack aircraft
  static int MIN_MECH_TO_DROP_BOMBS;
  static int MIN_INFANTRY_TO_DROP_BOMBS;

  static float TRANSPORT_RESUPPLY_OFFSET;
  static float HP_BALANCE_COEFF;

  // when leaving the house, the soldiers in the squad will leave one at a time after this interval
  static NTimer::STime SQUAD_MEMBER_LEAVE_INTERVAL;

  // cost of a soldier in RU. 
  static float SOLDIER_RU_PRICE;
  static float LOW_HP_PERCENTAGE;

  // damage along a straight path in the period of time necessary for the situation to be combat-oriented
  static float DIRECT_HIT_DAMAGE_COMBAT_SITUATION;
  static NTimer::STime DIRECT_HIT_TIME_COMBAT_SITUATION;
  static int NUMBER_ENEMY_MECH_MOVING_TO_COMBAT_SITUATION;
  static int NUMBER_ENEMY_INFANTRY_MOVING_TO_COMBAT_SITUATION;

  // coefficient on the firerange of the officer's weapon in which he scans ( scan radius firerange * OFFICER_COEFFICIENT_FOR_SCAN )
  static float OFFICER_COEFFICIENT_FOR_SCAN;

  static float MAIN_STORAGE_HEALING_SPEED;
  static float RADIUS_TO_TAKE_STORAGE_OWNERSHIP;
  static float TANKPIT_COVER;
  static const float CLOSEST_TO_RAILROAD_POINT_TOLERANCE;

  static float FENCE_SEGMENT_RU_PRICE;
  static float ENTRENCHMENT_SEGMENT_RU_PRICE;
  static float MINE_RU_PRICE[2];
  static float ANTITANK_RU_PRICE;

  // the time that a unit is visible after it has left the visibility area
  static NTimer::STime RESIDUAL_VISIBILITY_TIME;

  static NTimer::STime MED_TRUCK_HEAL_RADIUS;
  static float MED_TRUCK_HEAL_PER_UPDATEDURATION;

  //
  // the frequency with which a unit tries to find a way to its formation if it is blocked somewhere
  static NTimer::STime PERIOD_OF_PATH_TO_FORMATION_SEARCH;

  static NTimer::STime ENTRENCH_SELF_TIME;

  // maximum number of units that can be scanned per segment
  static int N_SCANNING_UNITS_IN_SEGMENT;

  // size of cell for general
  static int GENERAL_CELL_SIZE;

  // flags
  // radius of the flag zone (in points)
  static float FLAG_RADIUS;
  // time to capture the flag (seconds)
  static float FLAG_TIME_TO_CAPTURE;
  // points per flag per second
  static float FLAG_POINTS_SPEED;
  // "flag glasses" that always go, regardless of the presence of flags
  static float PLAYER_POINTS_SPEED;
  // number of points for reinforcements to arrive
  static float FLAG_POINTS_TO_REINFORCEMENT;

  // the time it takes to fur. 
  static NTimer::STime TIME_OF_PRE_DISAPPEAR_NOTIFY;
  // if a bomber has a dive angle greater than this, then it is a dive bomber
  static WORD ANGLE_DIVEBOMBER_MIN_DIVE;
  //
  // maximum armor pierced area damage
  static int ARMOR_FOR_AREA_DAMAGE;

  // default cover for fireplace
  static float BUILDING_FIREPLACE_DEFAULT_COVER;

  static NTimer::STime DIVE_BEFORE_EXPLODE_TIME;
  static NTimer::STime DIVE_AFTER_EXPLODE_TIME;


  static NTimer::STime WEATHER_TIME;
  static NTimer::STime WEATHER_TIME_RANDOM;
  static NTimer::STime WEATHER_TURN_PERIOD;
  static NTimer::STime WEATHER_TURN_PERIOD_RANDOM;

  static float BAD_WEATHER_FIRE_RANGE_COEFFICIENT;
  static int TIME_TO_WEATHER_FADE_OFF;

  static int AA_AIM_ITERATIONS;

  // how many times faster is it to lower morale without an officer?
  static float COEFF_TO_LOW_MORALE_WITHOUT_OFFICER;
  static float MORALE_DISPERSION_COEFF;
  static float MORALE_RELAX_COEFF;
  static float MORALE_AIMING_COEFF;

  // maximum distance for a soldier to try to throw a grenade
  static float MAX_DISTANCE_TO_THROW_GRENADE;

  // target resolution constants
  static float TR_GUNPLANE_ALPHA_ATTACK_1;// 1.0f;
  static float TR_GUNPLANE_ALPHA_ATTACK_2;// 0.3f;
  static float TR_GUNPLANE_ALPHA_GO;// 0.005f;
  static float TR_GUNPLANE_ALPHA_KILL;// 1.0f;
  static float TR_GUNPLANE_ALPHA_PRICE;// 1.0f;
  static float TR_GUNPLANE_LIMIT_TIME;// 1000
  static float TR_DISTANCE_TO_CENTER_FACTOR;

  static float MAX_FIRE_RANGE_TO_SHOOT_BY_LINE;

  static int SHOW_ALL_TIME_COEFF;

  static float HP_PERCENT_TO_ESCAPE_FROM_BUILDING;

  static float REINFORCEMENT_GROUP_DISTANCE;

  struct SRevealInfo
  {
    // probability of a unit revealing itself when it fires at someone (throws once at a certain time, 1-2 seconds)
    float fRevealByQuery;
    // the probability of the opening disappearing after some time has passed without shelling
    float fRevealByMovingOff;
    // the distance at which the deployment is reset when moving away from the point of fire
    float fForgetRevealDistance;
    // time after which the deployment is reset without firing
    int nTimeOfReveal;

    SRevealInfo() : fRevealByQuery(0.0f), fRevealByMovingOff(0.0f), fForgetRevealDistance(0.0f), nTimeOfReveal(0) {}
  };

  static std::hash_map<int, SRevealInfo> REVEAL_INFO;

  //
  static void Load();
};

#endif // __AI_INTERNAL_CONSTS_H__