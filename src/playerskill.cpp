#include "playerskill.h"
#include <sstream>
#include <iostream>
#include <cctype>
#include <limits>

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

// std::string PlayerInfo::grabPlayerMode(HttpClient* httpclient) {

//     int page1 = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=");
//     int page2 = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_ironman/index_lite.ws?player=");
//     int page3 = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_ultimate/index_lite.ws?player=");
//     int page4 = httpclient->get("https://secure.runescape.com/m=hiscore_oldschool_hardcore_ironman/index_lite.ws?player=");


// }

std::string PlayerInfo::grabHiscores(HttpClient* httpclient) {

    std::string playerName;
    std::cout << "Enter a username (ENTER or Ctrl+C to close): ";
    std::getline(std::cin, playerName);

    if(playerName.empty()) {
        std::cout << "Program closing...\n";
        return "";
    }

    std::string url = "https://secure.runescape.com/m=hiscore_oldschool/index_lite.ws?player=" + playerName;
    std::cout << "\nGrabbing hiscores from: " + url + "\n\n";

    int result = httpclient->get(url);
    std::string outputText;

    if (result == 0) {
        PlayerInfo info = PlayerInfo::createFromResponse(httpclient->getResponse());

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