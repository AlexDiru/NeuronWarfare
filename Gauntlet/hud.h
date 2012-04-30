#pragma once

#include <irrlicht.h>

#include "Entity.h"
#include "string_enhancements.h"
#include "EntityManager.h"

class hud
{
public:
	irr::scene::ISceneNode* Plane;
	irr::core::dimension2d<int> ScreenDimension;
	irr::gui::IGUIStaticText *Text;
	irr::gui::IGUIFont *Font;

	hud( irr::gui::IGUIEnvironment *gui, irr::core::dimension2d<int> screenCoords, const irr::io::path& fontFile )
	{
		ScreenDimension = screenCoords;
		//default values
	
		Font = gui->getFont(fontFile);

		gui->getSkin()->setColor(irr::gui::EGUI_DEFAULT_COLOR::EGDC_ACTIVE_CAPTION, irr::video::SColor(255,255,255,255));
		
		gui->getSkin()->setFont(Font);

		Text = gui->addStaticText(L"",irr::core::rect<irr::s32>(ScreenDimension.Width/2 - 100,ScreenDimension.Height/2 - 100,
																ScreenDimension.Width/2 + 100,ScreenDimension.Height/2 + 100));
		
	}

	void Draw( irr::gui::IGUIEnvironment *gui, const Entity& e )
	{
		//have to use font->draw, otherwise the font isn't drawn correctly
		Font->draw( wstring_cast(e.Stats.ConvertToString()).c_str(),
					irr::core::rect<irr::s32>(138, ScreenDimension.Height/2 - 100,
											  338,ScreenDimension.Height/2 + 100),
					irr::video::SColor());
		
		//Draw avatar
		irr::gui::IGUIImage* guiimg = gui->addImage(irr::core::rect<irr::s32> (0,ScreenDimension.Height/2 - 100,
																						128,ScreenDimension.Height/2 + 100));
		guiimg->setImage(e.Avatar);
	}

	void DrawAIDebug( irr::gui::IGUIEnvironment *gui, Entity& e, EntityManager& PM )
	{
		std::string StateString = "State: ";

		switch ( e.AI_State )
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
		if (e.AI_State == AI::Attack)
			TargetString += " => " + PM.GetEntity(e.AI_TargetIndex)->Stats.Name;

		Font->draw( wstring_cast( (StateString + "\n" + TargetString + //coord and target
			        + "\n" + toString(e.Stats.CurrentHealth) + "/" + toString(e.Stats.MaxHealth)).c_str()).c_str() , //health
					irr::core::rect<irr::s32>(0,0,100,100), irr::video::SColor());
	}
};