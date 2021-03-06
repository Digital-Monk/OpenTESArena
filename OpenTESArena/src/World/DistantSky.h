#ifndef DISTANT_SKY_H
#define DISTANT_SKY_H

#include <cstdint>
#include <optional>
#include <vector>

#include "../Math/Vector3.h"

#include "components/utilities/Buffer.h"
#include "components/utilities/BufferView.h"
#include "components/utilities/Buffer2D.h"
#include "components/utilities/BufferView2D.h"

// Contains data for distant objects (mountains, clouds, stars). Each distant object's image
// is owned by the texture manager.

class ExeData;
class LocationDefinition;
class ProvinceDefinition;
class TextureManager;

enum class WeatherType;

class DistantSky
{
public:
	// An object that sits on the horizon.
	class LandObject
	{
	private:
		int entryIndex; // Texture entry in distant sky.
		double angleRadians;
	public:
		LandObject(int entryIndex, double angleRadians);

		int getTextureEntryIndex() const;
		double getAngleRadians() const;
	};

	// An object with an animation that sits on the horizon.
	class AnimatedLandObject
	{
	private:
		static constexpr double DEFAULT_FRAME_TIME = 1.0 / 18.0;

		int setEntryIndex; // Texture set entry in distant sky.
		double angleRadians, targetFrameTime, currentFrameTime;
		int index;
	public:
		// All textures are stored in one texture set in the distant sky.
		AnimatedLandObject(int setEntryIndex, double angleRadians, double frameTime);
		AnimatedLandObject(int setEntryIndex, double angleRadians);

		int getTextureSetEntryIndex() const;
		double getAngleRadians() const;
		double getFrameTime() const;
		int getIndex() const;

		void setFrameTime(double frameTime);
		void setIndex(int index);
		void update(double dt, const DistantSky &distantSky);
	};

	// An object in the air, like clouds.
	class AirObject
	{
	private:
		int entryIndex; // Texture entry in distant sky.
		double angleRadians, height; // 0 height == horizon, 1 height == top of sky gradient.
	public:
		AirObject(int entryIndex, double angleRadians, double height);

		int getTextureEntryIndex() const;
		double getAngleRadians() const;
		double getHeight() const;
	};

	// A moon object in space. Moons have phases, and the current one is selected based
	// on the day given to the distant sky initializer.
	class MoonObject
	{
	public:
		enum class Type { First, Second };
	private:
		int entryIndex; // Texture entry in distant sky.
		double phasePercent; // [0, 1) of the moon's orbit.
		Type type;
	public:
		MoonObject(int entryIndex, double phasePercent, Type type);

		int getTextureEntryIndex() const;
		double getPhasePercent() const;
		Type getType() const;
	};

	// A star object in space. Small stars are single points, large stars have images.
	class StarObject
	{
	public:
		enum class Type { Small, Large };
		
		struct SmallStar
		{
			uint32_t color;
		};

		struct LargeStar
		{
			int entryIndex; // Texture entry in distant sky.
		};
	private:
		Type type;

		union
		{
			SmallStar small;
			LargeStar large;
		};

		Double3 direction;

		StarObject() = default;
	public:
		static StarObject makeSmall(uint32_t color, const Double3 &direction);
		static StarObject makeLarge(int entryIndex, const Double3 &direction);

		Type getType() const;
		const SmallStar &getSmallStar() const;
		const LargeStar &getLargeStar() const;
		const Double3 &getDirection() const;
	};
private:
	// Number of unique directions in 360 degrees.
	static const int UNIQUE_ANGLES;

	// Each texture entry holds its filename and 8-bit texture.
	struct TextureEntry
	{
		std::string filename;
		Buffer2D<uint8_t> texture;

		TextureEntry(std::string &&filename, Buffer2D<uint8_t> &&texture);
	};

	// Each texture set entry holds its filename and 8-bit textures. Intended only for
	// animated distant objects.
	struct TextureSetEntry
	{
		std::string filename;
		Buffer<Buffer2D<uint8_t>> textures;

		TextureSetEntry(std::string &&filename, Buffer<Buffer2D<uint8_t>> &&textures);
	};

	// Each object's texture index points into here.
	std::vector<TextureEntry> textures;
	std::vector<TextureSetEntry> textureSets;

	std::vector<LandObject> landObjects;
	std::vector<AnimatedLandObject> animLandObjects;
	std::vector<AirObject> airObjects;
	std::vector<MoonObject> moonObjects;
	std::vector<StarObject> starObjects;

	// The sun's position is a function of time of day.
	std::optional<int> sunEntryIndex;

	// Gets the index of a texture given its filename, or none if not found.
	std::optional<int> getTextureEntryIndex(const std::string_view &filename) const;

	// Gets the index of a texture set given its filename, or none if not found.
	std::optional<int> getTextureSetEntryIndex(const std::string_view &filename) const;
public:
	// The size of textures in world space is based on 320px being 1 unit, and a 320px
	// wide texture spans a screen's worth of horizontal FOV in the original game.
	static const double IDENTITY_DIM;
	static const double IDENTITY_ANGLE_RADIANS;

	void init(const LocationDefinition &locationDef, const ProvinceDefinition &provinceDef,
		WeatherType weatherType, int currentDay, int starCount, const ExeData &exeData,
		TextureManager &textureManager);

	int getLandObjectCount() const;
	int getAnimatedLandObjectCount() const;
	int getAirObjectCount() const;
	int getMoonObjectCount() const;
	int getStarObjectCount() const;
	bool hasSun() const;

	const LandObject &getLandObject(int index) const;
	const AnimatedLandObject &getAnimatedLandObject(int index) const;
	const AirObject &getAirObject(int index) const;
	const MoonObject &getMoonObject(int index) const;
	const StarObject &getStarObject(int index) const;
	int getSunEntryIndex() const;

	BufferView2D<const uint8_t> getTexture(int index) const;

	// Gets the number of textures in the texture set at the given index.
	int getTextureSetCount(int index) const;

	// Gets the texture at the given element index in the given texture set.
	BufferView2D<const uint8_t> getTextureSetElement(int index, int elementIndex) const;

	// Added in the new engine for fun. Gets the number of stars for some density.
	static int getStarCountFromDensity(int starDensity);

	void tick(double dt);
};

#endif
