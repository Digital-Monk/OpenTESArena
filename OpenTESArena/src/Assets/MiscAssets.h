#ifndef MISC_ASSETS_H
#define MISC_ASSETS_H

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CityDataFile.h"
#include "ExeData.h"
#include "WorldMapMask.h"
#include "../Entities/CharacterClass.h"
#include "../Game/CharacterClassGeneration.h"
#include "../Game/CharacterQuestion.h"

// This class stores various miscellaneous data from Arena assets.

// All relevant text files (TEMPLATE.DAT, QUESTION.TXT, etc.) should be read in 
// when this object is created.

class ArenaRandom;

class MiscAssets
{
public:
	// Each artifact text file (ARTFACT1.DAT, ARTFACT2.DAT) contains conversation strings
	// about artifacts. Supposedly ARTFACT2.DAT is used when the player declines and
	// returns to the individual later.
	// - The format is like: [[3][3][3][3][1]] ... [[3][3][3][3][1]]
	// - The first artifact is split between the front and back of the file.
	struct ArtifactText
	{
		struct Chunk
		{
			std::array<std::string, 3> playerTooGreedy, npcQuits, npcCountersOffers, npcGreets;
			std::string accept;
		};

		std::array<ArtifactText::Chunk, 16> chunks;
	};

	// Each trade text file (EQUIP.DAT, MUGUILD.DAT, SELLING.DAT, TAVERN.DAT) is an array
	// of 75 null-terminated strings. Each function array wraps conversation behaviors
	// (introduction, price agreement, etc.). Each personality array wraps personalities.
	// Each random array contains three strings for each personality.
	// - The format is like: [[3][3][3][3][3]] ... [[3][3][3][3][3]]
	struct TradeText
	{
		typedef std::array<std::string, 3> RandomArray;
		typedef std::array<RandomArray, 5> PersonalityArray;
		typedef std::array<PersonalityArray, 5> FunctionArray;
		FunctionArray equipment, magesGuild, selling, tavern;
	};
private:
	ExeData exeData; // Either floppy version or CD version (depends on ArenaPath).
	std::unordered_map<std::string, std::string> templateDat;
	std::vector<CharacterQuestion> questionTxt;
	CharacterClassGeneration classesDat;
	std::vector<CharacterClass> classDefinitions;
	std::vector<std::pair<std::string, std::string>> dungeonTxt;
	ArtifactText artifactText1, artifactText2;
	TradeText tradeText;
	std::vector<std::vector<std::string>> nameChunks;
	CityDataFile cityDataFile;
	std::array<WorldMapMask, 10> worldMapMasks;

	// Loads the executable associated with the current Arena data path (either A.EXE
	// for the floppy version or ACD.EXE for the CD version).
	void parseExecutableData();

	// Load TEMPLATE.DAT, grouping blocks of text by their #ID.
	void parseTemplateDat();

	// Load QUESTION.TXT and separate each question by its number.
	void parseQuestionTxt();

	// Load CLASSES.DAT and also read class data from the executable.
	void parseClasses(const ExeData &exeData);

	// Load DUNGEON.TXT and pair each dungeon name with its description.
	void parseDungeonTxt();

	// Loads ARTFACT1.DAT and ARTFACT2.DAT.
	void parseArtifactText();

	// Loads EQUIP.DAT, MUGUILD.DAT, SELLING.DAT, and TAVERN.DAT.
	void parseTradeText();

	// Loads NAMECHNK.DAT into a jagged list of name chunks.
	void parseNameChunks();

	// Reads the mask data from TAMRIEL.MNU.
	void parseWorldMapMasks();
public:
	MiscAssets();
	~MiscAssets();

	// Gets the ExeData object. There may be slight differences between A.EXE and ACD.EXE,
	// but only one will be available at a time for the lifetime of the program (dependent
	// on the Arena path in the options).
	const ExeData &getExeData() const;

	// Finds the text in TEMPLATE.DAT given a key (i.e., "#0000a").
	const std::string &getTemplateDatText(const std::string &key);

	// Returns all of the questions in QUESTION.TXT.
	const std::vector<CharacterQuestion> &getQuestionTxtQuestions() const;

	const CharacterClassGeneration &getClassGenData() const;
	const std::vector<CharacterClass> &getClassDefinitions() const;

	// Returns all of the main quest dungeon names paired with their description. 
	// These are just the dungeons with a unique icon on the world map, not the 
	// lesser dungeons.
	const std::vector<std::pair<std::string, std::string>> &getDungeonTxtDungeons() const;

	// Gets the artifact text used in tavern conversations.
	const MiscAssets::ArtifactText &getArtifactText1() const;
	const MiscAssets::ArtifactText &getArtifactText2() const;

	// Gets the trade text object for trade conversations.
	const MiscAssets::TradeText &getTradeText() const;

	// Creates a random NPC name from the given parameters.
	std::string generateNpcName(int raceID, bool isMale, ArenaRandom &random) const;

	// Gets the data object for world map locations.
	const CityDataFile &getCityDataFile() const;

	// Gets the mask rectangles used for registering clicks on the world map. There are
	// ten entries -- the first nine are provinces and the last is the "Exit" button.
	const std::array<WorldMapMask, 10> &getWorldMapMasks() const;

	void init();
};

#endif
