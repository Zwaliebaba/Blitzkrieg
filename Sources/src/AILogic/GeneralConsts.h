#ifndef __GENERAL_CONSTS_H__
#define __GENERAL_CONSTS_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
struct SGeneralConsts
{
  static int TIME_DONT_SEE_ENEMY_BEFORE_FORGET;

  static int SCOUT_FREE_POINT;
  static int SCOUT_POINTS;
  static NTimer::STime TIME_SONT_SEE_AA_BEFORE_FORGET;

  static int AVIATION_PERIOD_MAX;
  static int AVIATION_PERIOD_MIN;

  static int FIGHTER_PERIOD_MAX;
  static int FIGHTER_PERIOD_MIN;
  static float FIGHTER_INTERCEPT_OFFSET;

  static float AVATION_LONG_PERIOD_PROBABILITY;
  static float AVATION_PERIOD_MULTIPLY;
  static float PLAYER_FORCE_COEFFICIENT;

  // "resistance cell" size
  static const int RESISTANCE_CELL_SIZE;
  // anti-artillery circle "fade" time (msec)
  static float TIME_TO_FORGET_ANTI_ARTILLERY;
  // time to forget empty artillery
  static float TIME_DONT_SEE_EMPTY_ARTILLERY_BEFORE_FORGET;
  // time for the unit to "fade out" (msec)
  static float TIME_TO_FORGET_UNIT;
  // average time after which to attempt to shell/bomb enemy positions (msec)
  static float TIME_TO_ARTILLERY_FIRE;
  // the probability that after TIME_TO_ARTILLERY_FIRE it will be decided to fire
  static float PROBABILITY_TO_SHOOT_AFTER_ARTILLERY_FIRE;
  // average number of shots a general fires with artillery
  static float SHOOTS_OF_ARTILLERY_FIRE;
  // the likelihood that the general will send bombers instead of firing artillery
  // static float PROBABILITY_TO_SEND_BOMBERS;
  // minimum cell weight to start artillery shelling
  static float MIN_WEIGHT_TO_ARTILLERY_FIRE;
  // minimum cell weight to send bombers
  static float MIN_WEIGHT_TO_SEND_BOMBERS;
  // minimal resistance weight to send tank swarm to it
  static float MIN_WEIGHT_TO_SEND_SWARM;
  // general will perform maximum of swarms deep.
  static int SWARM_ADDITIONAL_ITERATIONS;
  // maximum distance to swarm to
  static float SWARM_WEIGHT_COEFFICIENT;

  // update period duration
  static int GENERAL_UPDATE_PERIOD;

  // general sends truck to repair with this probability.( estimates every GENERAL_UPDATE_PERIOD )
  static float REPAIR_STORAGE_PROBABILITY;
  // probability to recapture storage, that was initially owned by general (Every GENERAL_UPDATE_PERIOD )
  static float RECAPTURE_STORAGE_PROBALITY;
  // mobile reserves can be sent to recapture storage only if they are near.
  static float RECAPTURE_STORAGE_MAX_DISTANCE;
  // number of tanks that general send to recapture lost storage
  static int RECAPTURE_ARTILLERY_TANKS_NUMBER;

  // intendant will send transport to cell where transport dead only
  // after this time interval.
  static int RESUPPLY_CELL_AFTER_TRANSPORT_DEATH;
  static int RESUPPLY_CELL_AFTER_TRANSPORT_DEATH_RAND;
  static float INTENDANT_DANGEROUS_CELL_RADIUS;

  static int TIME_TO_WAIT_SWARM_READY;
  static int TIME_TO_WAIT_SWARM_READY_RANDOM;

  static int TIME_SWARM_DURATION;
  static int TIME_SWARM_DURATION_RANDOM;
  static void Init();
};

// //////////////////////////////////////////////////////////// 
#endif // __GENERAL_CONSTS_H__