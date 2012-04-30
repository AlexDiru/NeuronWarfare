#include "hud.h"

hud::hud( irr::gui::IGUIEnvironment *gui, irr::core::dimension2d<int> screenCoords, const irr::io::path& fontFile )
{
	ScreenDimension = screenCoords;
	//default values
	
	Font = gui->getFont(fontFile);

	gui->getSkin()->setColor(irr::gui::EGUI_DEFAULT_COLOR::EGDC_ACTIVE_CAPTION, irr::video::SColor(255,255,255,255));
		
	gui->getSkin()->setFont(Font);

	Text = gui->addStaticText(L"",irr::core::rect<irr::s32>(ScreenDimension.Width/2 - 100,ScreenDimension.Height/2 - 100,
															ScreenDimension.Width/2 + 100,ScreenDimension.Height/2 + 100));
		
}

void hud::Draw( irr::gui::IGUIEnvironment *gui, const EntityStats& es, irr::video::ITexture* Avatar )
{
	//have to use font->draw, otherwise the font isn't drawn correctly
	Font->draw( wstring_cast(es.ConvertToString()).c_str(),
				irr::core::rect<irr::s32>(138, ScreenDimension.Height/2 - 100,
											338,ScreenDimension.Height/2 + 100),
				irr::video::SColor());
		
	//Draw avatar
	irr::gui::IGUIImage* guiimg = gui->addImage(irr::core::rect<irr::s32> (0,ScreenDimension.Height/2 - 100,
																					128,ScreenDimension.Height/2 + 100));
	guiimg->setImage(Avatar);
}