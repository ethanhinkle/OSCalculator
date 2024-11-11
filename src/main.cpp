#include <iostream>
#include <string>
#include <stdio.h>
#include <csignal>
#include <memory>
#include "httpclient.h"
#include "playerskill.h"

int main() {
    std::unique_ptr<HttpClient> client = std::make_unique<HttpClient>();

    std::string hiscores = PlayerInfo::grabHiscores(client.get());
    std::cout << hiscores << '\n';

    return 0;

}