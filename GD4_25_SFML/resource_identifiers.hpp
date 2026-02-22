#pragma once
#include "texture_id.hpp"
#include "resource_holder.hpp"
#include <SFML/Graphics/Font.hpp>
#include "sound_effect.hpp"
#include "fontID.hpp"

//Ben Arrowsmith D00257746

namespace sf
{
	class Texture;
	class SoundBuffer; //Ben Arrowsmith
}

//template<typename Identifier, typename Resource>

typedef ResourceHolder<TextureID, sf::Texture> TextureHolder;
typedef ResourceHolder<FontID, sf::Font> FontHolder;
typedef ResourceHolder<SoundEffect, sf::SoundBuffer> SoundBufferHolder; //Ben Arrowsmith