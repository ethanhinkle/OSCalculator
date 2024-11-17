#include "playerinfo.h"
#include <sstream>
#include <iostream>
#include <cctype>
#include <limits>

std::string PlayerInfo::getPlayer(){
    std::string name;
    std::cout << "Enter a username (ENTER or Ctrl+C to close): ";
    std::getline(std::cin, name);
    return name;
}

std::vector<std::string> splitStringByNewline(const std::string& input) {
    std::vector<std::string> lines;
    std::istringstream stream(input);
    std::string line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::vector<std::string> splitStringByComma(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        tokens.push_back(token);
    }

    return tokens;
}

bool TryParseInt(const std::string& str, int& value) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");

    if (start == std::string::npos || end == std::string::npos) {
        value = 0; // Empty string or only whitespace
        return false;
    }

    std::string trimmed = str.substr(start, end - start + 1);
    char* endPtr = nullptr;

    // Use strtol to attempt to convert the string to an integer
    long intResult = std::strtol(trimmed.c_str(), &endPtr, 10);

    // Check for conversion errors
    if (endPtr == trimmed.c_str() || *endPtr != '\0') {
        return false; // No valid conversion
    }

    // Check for overflow/underflow
    if (intResult < std::numeric_limits<int>::min() || intResult > std::numeric_limits<int>::max()) {
        return false; // Out of int range
    }

    value = static_cast<int>(intResult);
    return true;
}

PlayerInfo PlayerInfo::createFromResponse(const std::string& response)
{
    std::vector<std::string> lines = splitStringByNewline(response); //Split string whenever a new line character is found

    PlayerInfo playerInfo;

    for (size_t i = 0; i < lines.size(); i++)
    {
        std::vector<std::string> columns = splitStringByComma(lines[i]);


        int rank, level, xp;

        if (columns.size() >= 3)
        {
            TryParseInt(columns[0], rank);
            TryParseInt(columns[1], level);
            TryParseInt(columns[2], xp);

            PlayerSkill skill;
            skill.type = (SkillType)i; //Get skilltype from current loop index
            skill.rank = rank;
            skill.level = level;
            skill.xp = xp;

            playerInfo.skills.push_back(skill);
        }
    }

    return playerInfo;
}

std::string skillTypeString[24] = {
    "Overall",
    "Attack",
    "Defence",
    "Strength",
    "Hitpoints",
    "Ranged",
    "Prayer",
    "Magic",
    "Cooking",
    "Woodcutting",
    "Fletching",
    "Fishing",
    "Firemaking",
    "Crafting",
    "Smithing",
    "Mining",
    "Herblore",
    "Agility",
    "Thieving",
    "Slayer",
    "Farming",
    "Runecraft",
    "Hunter",
    "Construction"
};

std::string PlayerInfo::skillTypeToString(SkillType type) {
    int index = static_cast<int>(type);
    int maxCount = static_cast<int>(SkillType::COUNT);
    if(index < 0 || index >= maxCount)
        return "Unknown";
    return skillTypeString[index];
}

std::string PlayerInfo::getHiscores(HttpClient* httpclient, const std::string& playerName) {

    if(playerName.empty()) {
        std::cout << "Program closing...\n";
        return "";
    }

    std::string url = "https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=" + playerName;
    std::cout << "\nGrabbing hiscores from: " + url + "\n\n";

    HttpResponse response;
    int result = httpclient->get(url, response);
    std::string outputText;

    if (result == 0) {

        PlayerInfo info = PlayerInfo::createFromResponse(response.getContent());

        std::cout << "0:Overall, 1:Attack, 2:Defence, 3:Strength, 4:Hitpoints, 5:Ranged, 6:Prayer, 7:Magic, 8:Cooking,\n"
                << "9:Woodcutting, 10:Fletching, 11:Fishing, 12:Firemaking, 13:Crafting, 14:Smithing, 15:Mining, 16:Herblore,\n"
                << "17:Agility, 18:Thieving, 19:Slayer, 20:Farming, 21:Runecraft, 22:Hunter, 23:Construction\n\n";

        std::cout << "Enter skill number (0-23) to view the current xp in that skill: ";
        int skillIndex;
        std::cin >> skillIndex;
        std::cout << "\n";

        if (skillIndex >= 1 && skillIndex < info.skills.size()) {
            PlayerSkill skill = info.skills[skillIndex];
            std::cout << "Skill: " << PlayerInfo::skillTypeToString(skill.type) << '\n';
            std::cout << "Rank: " << skill.rank
                    << "\nLevel: " << skill.level
                    << "\nXP: " << skill.xp << '\n';
        }
        else if (skillIndex < 0) {
            std::cout << "Exiting skill lookup.\n";
        }

        if (skillIndex == 0 && skillIndex < info.skills.size()) {
            for (int i = 0; i < info.skills.size(); i++) {
                outputText += PlayerInfo::skillTypeToString(info.skills[i].type) +
                            " Rank: " + std::to_string(info.skills[i].rank) +
                            " Level: " + std::to_string(info.skills[i].level) +
                            " XP: " + std::to_string(info.skills[i].xp) + "\n";
            }
            return outputText;
        }
    }

    return "";
}

bool PlayerInfo::isPlayerTypeNormal(HttpClient* httpclient, const std::string& playerName){
    HttpResponse response;
    int status = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=" + playerName, response);

    if(status == 0) {
        return response.getStatusCode() == 200;
    }
    return false;
}

bool PlayerInfo::isPlayerTypeIronman(HttpClient* httpclient, const std::string& playerName){
    HttpResponse response;
    int status = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_ironman/index_lite.ws?player=" + playerName, response);

    if(status == 0) {
        return response.getStatusCode() == 200;
    }
    return false;
}

bool PlayerInfo::isPlayerTypeUltimate(HttpClient* httpclient, const std::string& playerName){
    HttpResponse response;
    int status = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_ultimate/index_lite.ws?player=" + playerName, response);

    if(status == 0) {
        return response.getStatusCode() == 200;
    }
    return false;
}

bool PlayerInfo::isPlayerTypeHardcore(HttpClient* httpclient, const std::string& playerName){
    HttpResponse response;
    int status = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_hardcore_ironman/index_lite.ws?player=" + playerName, response);

    if(status == 0) {
        return response.getStatusCode() == 200;
    }
    return false;
}

bool PlayerInfo::getScores(HttpClient* httpclient, const std::string& playerName, PlayerType playerType, PlayerInfo &playerInfo) {
    std::string url;

    switch(playerType) {
        case PlayerType_Normal:
            url = "https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=";
            break;
        case PlayerType_Ironman:
            url = "https://secure.runescape.com/m=hiscore_oldschool_ironman/index_lite.ws?player=";
            break;
        case PlayerType_Ultimate:
            url = "https://secure.runescape.com/m=hiscore_oldschool_ultimate/index_lite.ws?player=";
            break;
        case PlayerType_Hardcore:
            url = "https://secure.runescape.com/m=hiscore_oldschool_hardcore_ironman/index_lite.ws?player=";
            break;
        default:
            return false;
    }

    url += playerName;

    HttpResponse response;
    int result = httpclient->get(url, response);
    std::string outputText;

    if (result == 0) {
        if(response.getStatusCode() == 200){
            playerInfo = PlayerInfo::createFromResponse(response.getContent());
            playerInfo.playerName = playerName;
            return true;
        }
    }
    return false;
}

void PlayerInfo::dump() {

    std::cout << "0:Overall, 1:Attack, 2:Defence, 3:Strength, 4:Hitpoints, 5:Ranged, 6:Prayer, 7:Magic, 8:Cooking,\n"
            << "9:Woodcutting, 10:Fletching, 11:Fishing, 12:Firemaking, 13:Crafting, 14:Smithing, 15:Mining, 16:Herblore,\n"
            << "17:Agility, 18:Thieving, 19:Slayer, 20:Farming, 21:Runecraft, 22:Hunter, 23:Construction\n\n";

    std::cout << "Enter skill number (0-23) to view the current xp in that skill: ";
    int skillIndex;
    std::cin >> skillIndex;
    std::cout << "\n";

    if (skillIndex >= 1 && skillIndex < skills.size()) {
        PlayerSkill skill = skills[skillIndex];
        std::cout << "Skill: " << PlayerInfo::skillTypeToString(skill.type) << '\n';
        std::cout << "Rank: " << skill.rank
                << "\nLevel: " << skill.level
                << "\nXP: " << skill.xp << '\n';
    }
    else if (skillIndex < 0) {
        std::cout << "Exiting skill lookup.\n";
    }

    if (skillIndex == 0 && skillIndex < skills.size()) {
        std::string outputText;
        for (int i = 0; i < skills.size(); i++) {
            outputText += PlayerInfo::skillTypeToString(skills[i].type) +
                        " Rank: " + std::to_string(skills[i].rank) +
                        " Level: " + std::to_string(skills[i].level) +
                        " XP: " + std::to_string(skills[i].xp) + "\n";
        }
        std::cout << outputText << "\n";
    }
}

bool PlayerInfo::operator==(const PlayerInfo& other) const {
    if (playerName != other.playerName) return false;
    if(skills.size() != other.skills.size()) return false;

    for(size_t i = 0; i < skills.size(); i++) {
        if(skills[i].type != other.skills[i].type)
            return false;
        if(skills[i].rank != other.skills[i].rank)
            return false;
        if(skills[i].level != other.skills[i].level)
            return false;
        if(skills[i].xp != other.skills[i].xp)
            return false;
    }
    return true;
}

PlayerType PlayerInfo::getPlayerType(HttpClient* client, const std::string& playerName){
    PlayerInfo infoOverall;
    PlayerInfo infoIronman;
    PlayerInfo infoUltimate;
    PlayerInfo infoHardcore;

    // std::string player = PlayerInfo::getPlayer();
    PlayerType playerType = 0;

    if(PlayerInfo::getScores(client, playerName, PlayerType_Normal, infoOverall)){
        playerType |= PlayerType_Normal;
    }

    if(PlayerInfo::getScores(client, playerName, PlayerType_Ironman, infoIronman)){
        playerType |= PlayerType_Ironman;
    }

    if(PlayerInfo::getScores(client, playerName, PlayerType_Ultimate, infoUltimate)){
        playerType |= PlayerType_Ultimate;
    }

    if(PlayerInfo::getScores(client, playerName, PlayerType_Hardcore, infoHardcore)){
        playerType |= PlayerType_Hardcore;
    }

    if((playerType & PlayerType_Normal) && (playerType & PlayerType_Ironman) && (playerType & PlayerType_Hardcore)) {
        if(infoHardcore.skills[(int)SkillType::Overall].xp == infoIronman.skills[(int)SkillType::Overall].xp){
            return PlayerType_Hardcore;
        }
        else {
            return PlayerType_Ironman;
        }
    }
    else if((playerType & PlayerType_Normal) && (playerType & PlayerType_Ironman) && (playerType & PlayerType_Ultimate)) {
            return PlayerType_Ultimate;
    }

    else if((playerType & PlayerType_Normal) && (playerType & PlayerType_Ironman)) {
            return PlayerType_Ironman;
    } else {
            return PlayerType_Normal;
    }

    return 0;
}