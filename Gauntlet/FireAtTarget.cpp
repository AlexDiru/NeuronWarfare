#include "FireAtTarget.h"

void FireAtTarget(const MyEventReceiver& Receiver, EntityManager& PlayerManager, EntityManager& EnemyManager, 
					MazeRenderer& MR, irr::scene::ISceneManager *smgr, irr::scene::ISceneNode *Cursor)
{

	if (PlayerManager.GetCurrentEntity()->CanShoot( Cursor->getPosition(), smgr->getSceneCollisionManager(), MR.MetaTriangleSelector ) )
	{
		//check if entity in target position (for now allow friendly fire)
		if (EnemyManager.GetEntityThatResidesOnCell( irr::core::position2di( (int)(Cursor->getPosition().X)/10, (int)(Cursor->getPosition().Z)/10) ) != NULL)
		{
			Entity *Target = EnemyManager.GetEntityThatResidesOnCell( irr::core::position2di( (int)(Cursor->getPosition().X)/10, (int)(Cursor->getPosition().Z)/10 ) );

			attackResult AR;

			//cover only works if the target is adjacent to it
			//check adjacency

			//pointer to make code shorter
			vector<std::string> *m = &Target->PathFinder.Map;


			//have to look at each individually as multiple covers can be surrounding a cell........
			irr::core::position2di TargetVec((int)(Target->Node->getPosition().Z/10), (int)(Target->Node->getPosition().X/10) );
			bool coverHit = false;

			//checks all the cells around the enemy and accounts for cover - to be honest i don't know wtf is going on, it works, and as long as i don't have to fix it, i'm not reading it again
			for (int y = -1; y < 2 && !coverHit; y++)
			{
				for (int x = -1; x < 2 && !coverHit; x++)
				{
					if (!(y == 0 && x == 0)) //not middle
					{
						if ((*m)[TargetVec.Y+y][TargetVec.X+x] == 'C')
						{
							if (PlayerManager.GetCurrentEntity()->CoverHit(Target->Node->getPosition(),
																			smgr->getSceneCollisionManager(),
																			MR.MetaCoverTriangleSelector,
																			irr::core::vector3df(TargetVec.Y*10, 
																								5,
																								TargetVec.X*10),x,y))
							{
								coverHit = true;
								//make sure to break
								x = 3;
								y = 3;
								break;
							}
						}
					}
				}
			}
							

			//get the attack result struct
			AR = PlayerManager.GetCurrentEntity()->Stats.CalculateDamageOnTarget( Target->Stats, coverHit);

			//calculate the result of the attack
			if (!AR.Missed)
			{
				if (AR.Damage > 0)
				{
					Target->Stats.CurrentHealth -= AR.Damage;
					Target->CheckIfDead();
				}
				else
				{
					//inform enemy too powerful
				}
			}
			else
			{
				//inform of a miss!
			}
		}
	}
	return;
}