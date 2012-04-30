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

	attackResult CalculateDamageOnTarget(const EntityStats& enemy, bool EnemyBehindCover)
	{
		attackResult AR;

		int totalAccuracy = Accuracy - (int)EnemyBehindCover*50;
		totalAccuracy = totalAccuracy > 0 ? totalAccuracy : 1; //make sure total accuracy is above 0

		bool Shot_Hit = (rand()%(101-totalAccuracy) == 0); //if the shot hit the enemy

		AR.Missed = !Shot_Hit;

		//calculate base attack
		int AtkVarScalar = (rand()%2 == 0) ? 1 : -1; //variance can either be +ve or -ve
		int totalAttack = Attack + AtkVarScalar * rand()%(AttackVariance+1);
		if (totalAttack < 0) totalAttack = 0;
		
		//calculate base enemy defense
		int DefVarScalar = (rand()%2 == 0) ? 1 : -1; //variance can either be +ve or -ve
		int totalDefense = enemy.Defense + DefVarScalar * rand()%(enemy.DefenseVariance+1);
		if (totalDefense < 0) totalDefense = 0;

		//calculate actual attack
		int finalAttack = totalAttack - 0.4*totalDefense;

		AR.Damage = (Shot_Hit && finalAttack > 0) ? finalAttack : 0;

		return AR;
	}

	void RestoreAll()
	{
		CurrentHealth = MaxHealth;
	}

	void MonitorAll() //makes sure the stats can't past certain ranges => 0 and max___
	{
		if (CurrentHealth < 0)
			CurrentHealth = 0;
		else if (CurrentHealth > MaxHealth)
			CurrentHealth = MaxHealth;
	}

	std::string ConvertToString() const
	{
		return (std::string)(Name + 
							 "\nHealth: " + toString(CurrentHealth) + "/" + toString(MaxHealth) + 
							 "\nMovement: " + toString(MovementSteps-StepsMovedThisTurn) + "/" + toString(MovementSteps) +
							 "\nRange: " + toString(RealFiringDistance/10)) +
							 "\nAttack: " + toString(Attack) + " ~ " + toString(AttackVariance) +
							 "\nDefense: " + toString(Defense) + " ~ " + toString(DefenseVariance) ;
	}
};