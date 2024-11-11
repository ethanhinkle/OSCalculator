#ifndef PLAYER_H
#define PLAYER_H
#include "httpclient.h"
#include <string>
#include <vector>


enum class SkillType
{
    Overall = 0,
    Attack = 1,
    Defence = 2,
    Strength = 3,
    Hitpoints = 4,
    Ranged = 5,
    Prayer = 6,
    Magic = 7,
    Cooking = 8,
    Woodcutting = 9,
    Fletching = 10,
    Fishing = 11,
    Firemaking = 12,
    Crafting = 13,
    Smithing = 14,
    Mining = 15,
    Herblore = 16,
    Agility = 17,
    Thieving = 18,
    Slayer = 19,
    Farming = 20,
    Runecraft = 21,
    Hunter = 22,
    Construction = 23,
    COUNT = 24
};


struct PlayerSkill
{
    SkillType type;
    int rank;
    int level;
    int xp;
};

struct PlayerType
{
    std::string normalAccount = "https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=";
    std::string ironmanAccount = "https://secure.runescape.com/m=hiscore_oldschool_ironman/index_lite.ws?player=";
    std::string ultimateironmanAccount = "https://secure.runescape.com/m=hiscore_oldschool_ultimate/index_lite.ws?player=";
    std::string hardcoreironmanAccount = "https://secure.runescape.com/m=hiscore_oldschool_hardcore_ironman/index_lite.ws?player=";
};

class PlayerInfo

{
public:
    std::string playerName;
    int playerId;
    std::vector<PlayerSkill> skills;
    std::vector<PlayerType> mode;

    static PlayerInfo createFromResponse(const std::string& response);
    static std::string skillTypeToString(SkillType type);
    static std::string grabHiscores(HttpClient* httpclient);
    static std::string grabPlayerMode(HttpClient * httpclient);
};

#endif