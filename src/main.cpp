#include <iostream>
#include <string>
#include <stdio.h>
#include <csignal>
#include <memory>
#include "httpclient.h"
#include "playerskill.h"

int main() {
    HttpClient client;
    HttpResponse response;
    PlayerInfo playerInfo;

    std::string player = PlayerInfo::getPlayer();

    PlayerType type = PlayerInfo::getPlayerType(&client, player);
    switch(type) {
        case PlayerType_Normal:
            std::cout << "\nAccount Type: Main\n\n";
            break;
        case PlayerType_Ironman:
            std::cout << "\nAccount Type: Ironman\n\n";
            break;
        case PlayerType_Hardcore:
            std::cout << "\nAccount Type: Hardcore Ironman\n\n";
            break;
        case PlayerType_Ultimate:
            std::cout << "\nAccount Type: Ultimate Ironman\n\n";
            break;
        default:
            break;
    }

    if(PlayerInfo::getScores(&client, player, type, playerInfo)){
        playerInfo.dump();
    }

    return 0;
}