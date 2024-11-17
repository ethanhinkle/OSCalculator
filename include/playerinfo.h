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

enum PlayerType_
{
    PlayerType_Normal = 1 << 0,
    PlayerType_Ironman = 1 << 1,
    PlayerType_Ultimate = 1 << 2,
    PlayerType_Hardcore = 1 << 3
};

typedef int PlayerType;

class PlayerInfo

{
public:
    std::string playerName;
    std::vector<PlayerSkill> skills;

    void dump();
    bool operator==(const PlayerInfo& other) const;
    static PlayerInfo createFromResponse(const std::string& response);
    static std::string skillTypeToString(SkillType type);
    static std::string getHiscores(HttpClient* httpclient, const std::string& playerName);
    static std::string getPlayer();
    static PlayerType getPlayerType(HttpClient* httpclient, const std::string& playerName);
    static bool getScores(HttpClient* httpclient, const std::string& playerName, PlayerType playerType, PlayerInfo &playerInfo);
    static bool isPlayerTypeNormal(HttpClient* httpclient, const std::string& playerName);
    static bool isPlayerTypeIronman(HttpClient* httpclient, const std::string& playerName);
    static bool isPlayerTypeUltimate(HttpClient* httpclient, const std::string& playerName);
    static bool isPlayerTypeHardcore(HttpClient* httpclient, const std::string& playerName);
};

#endif