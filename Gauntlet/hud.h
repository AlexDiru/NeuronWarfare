#pragma once

#include <irrlicht.h>

#include "string_enhancements.h"
#include "AIState.h"
#include "EntityStats.h"

class hud
{
public:
	irr::scene::ISceneNode* Plane;
	irr::core::dimension2d<int> ScreenDimension;
	irr::gui::IGUIStaticText *Text;
	irr::gui::IGUIFont *Font;

	hud( irr::gui::IGUIEnvironment *gui, irr::core::dimension2d<int> screenCoords, const irr::io::path& fontFile );

	void Draw( irr::gui::IGUIEnvironment *gui, const EntityStats& es, irr::video::ITexture* Avatar );

	/*
	void DrawAIDebug( irr::gui::IGUIEnvironment *gui, AI::State AI_State, Entity& e, EntityManager& PM )
	{
		std::string StateString = "State: ";

		switch ( AI_State )
		{
			case AI::Attack: StateString += "Attack"; break;
			case AI::HealthSearch: StateString += "HealthSearch"; break;
			case AI::Patrol: StateString += "Patrol"; break;
			case AI::Run: StateString += "Run"; break;
			case AI::SeekRouteRoundCover: StateString += "Seek Route Round Cover"; break;
			default: break;
		}

		std::string TargetString = "Target: ( " + toString(e.AI_DestinationX) + ", " + toString(e.AI_DestinationY) + ")";
		
		//Get name of target, if entity is in attack mode
		if (AI_State == AI::Attack)
			TargetString += " => " + PM.GetEntity(e.AI_TargetIndex)->Stats.Name;

		Font->draw( wstring_cast( (StateString + "\n" + TargetString + //coord and target
			        + "\n" + toString(e.Stats.CurrentHealth) + "/" + toString(e.Stats.MaxHealth)).c_str()).c_str() , //health
					irr::core::rect<irr::s32>(0,0,100,100), irr::video::SColor());
	}
	*/
};