#include "httpclient.h"
#include "curl/curl/curl.h"
#include <cstdlib>
#include <iostream>

void encodeSpaces(std::string& s){
     size_t pos = 0;
    while ((pos = s.find(' ', pos)) != std::string::npos) {
        s.replace(pos, 1, "%20");
        pos += 3; // Move past the newly replaced "%20"
    }
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    if(userp) {
        HttpClient* client = reinterpret_cast<HttpClient*>(userp);
        return HttpClient::onData(contents, size, nmemb, client);
    }
    return size * nmemb;
}

HttpClient::HttpClient() {
    curl = curl_easy_init();
    verbose = false;
}

HttpClient::~HttpClient(){
    curl_easy_cleanup(curl);
}
int HttpClient::get(const std::string& url) {
    if(!curl)
        return CURLE_FAILED_INIT;

    std::string encodedUrl = url;
    encodeSpaces(encodedUrl);
    curl_easy_setopt(curl, CURLOPT_URL, encodedUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");


    if(verbose)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    else
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

    readBuffer.clear();
    CURLcode res = curl_easy_perform(curl);
    return static_cast<int>(res);

}

std::string HttpClient::getResponse() const {
    return readBuffer;
}

void HttpClient::setVerbose(bool v) {
    verbose = v;
}

size_t HttpClient::onData(void* contents, size_t size, size_t nmemb, HttpClient* client) {
    client->readBuffer.append((char*)contents, size * nmemb);
    return size * nmemb;
}