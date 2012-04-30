#pragma once

#include <string>
#include "int_enhancements.h"
#include "attackResult.h"

// Accuracy - probabilty that shot will hit target
// Attack ~ Variance - damage that will be inflicted on enemy


// totalAccuracy = ( (totalAccuracy - (int)EnemyBehindCover*50) > 1 ) ? totalAccuracy - (int)EnemyBehindCover*50 : 1;
// bool Shot_Hit = (rand()%(101-totalAccuracy) == 0);
// int AtkVarScalar = (rand()%2 == 0) ? 1 : -1; //variance can either be +ve or -ve
// int totalAttack = Attack + AtkVarScalar * rand()%(AtkVar+1);
// int finalAttack = (totalAttack - 0.4*totalEnemyDef);
// int Damage = (Shot_Hit && finalAttack > 0) ? finalAttack : 0;

class EntityStats
{
public:
	std::string Name;
	int CurrentHealth;
	int MaxHealth;
	int MovementSteps;
	int StepsMovedThisTurn;
	int RealFiringDistance;

	int Accuracy;
	int Attack;
	int AttackVariance;

	int Defense;
	int DefenseVariance;

	attackResult CalculateDamageOnTarget(const EntityStats& enemy, bool EnemyBehindCover);

	void RestoreAll();

	void MonitorAll();

	std::string ConvertToString() const;
};