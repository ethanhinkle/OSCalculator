#include "httpclient.h"
#include "curl/curl/curl.h"
#include <sstream>

void encodeSpaces(std::string& s){
    size_t pos = 0;
    while ((pos = s.find(' ', pos)) != std::string::npos) {
        s.replace(pos, 1, "%20");
        pos += 3; // Move past the newly replaced "%20"
    }
}

HttpResponse::HttpResponse() {

}

HttpResponse::HttpResponse(const std::string &responseHeader) {
    HttpResponse::tryParse(responseHeader, *this);
}

int HttpResponse::getStatusCode() const {
    return statusCode;
}

const HttpHeaders &HttpResponse::getHeaders() const {
    return headers;
}

const std::string &HttpResponse::getContent() const {
    return content;
}

bool HttpResponse::tryParse(const std::string &header, HttpResponse &request) {
    std::string h = header;
    stringReplace(h, "\r", "");
    auto lines = stringSplit(h, '\n');

    if(lines.size() == 0)
        return false;

    for(size_t i = 0; i < lines.size(); ++i) {
        auto headerParts = stringSplit(lines[i], ':');

        if(headerParts.size() < 2)
            continue;

        auto key = stringTrim(headerParts[0]);
        auto value = stringTrim(headerParts[1]);

        if(key.size() == 0 || value.size() == 0)
            continue;

        request.headers[key] = value;
    }

    return true;
}

std::vector<std::string> HttpResponse::stringSplit(const std::string &str, char separator) {
    std::vector<std::string> substrings;
    std::istringstream ss(str);
    std::string token;

    while (std::getline(ss, token, separator)) {
        if(token.size() > 0)
            substrings.push_back(token);
    }

    return substrings;
}

std::string HttpResponse::stringTrim(const std::string &str) {
    // Find the first non-whitespace character from the beginning
    size_t start = str.find_first_not_of(" \t\n\r\f\v");

    // Find the last non-whitespace character from the end
    size_t end = str.find_last_not_of(" \t\n\r\f\v");

    // Handle the case where the string is all whitespace
    if (start == std::string::npos)
        return "";

    // Extract the substring between start and end
    return str.substr(start, end - start + 1);
}

void HttpResponse::stringReplace(std::string &str, const std::string &target, const std::string &replacement) {
    size_t startPos = 0;

    while ((startPos = str.find(target, startPos)) != std::string::npos) {
        str.replace(startPos, target.length(), replacement);
        startPos += replacement.length();
    }
}

size_t httpclient_on_data(void* contents, size_t size, size_t nmemb, void* userdata) {
    if(userdata) {
        HttpClient* client = reinterpret_cast<HttpClient*>(userdata);
        return HttpClient::onData(contents, size, nmemb, client);
    }
    return size * nmemb;
}

size_t httpclient_on_header(char* buffer, size_t size, size_t nitems, void* userdata) {
    if(userdata) {
        HttpClient* client = reinterpret_cast<HttpClient*>(userdata);
        return HttpClient::onHeader(buffer, size, nitems, client);
    }
    return nitems * size;
}

HttpClient::HttpClient() {
    curl = curl_easy_init();
    verbose = false;
}

HttpClient::~HttpClient(){
    curl_easy_cleanup(curl);
}

int HttpClient::get(const std::string& url, HttpResponse &response, HttpHeaders *headers) {
    if(!curl)
        return CURLE_FAILED_INIT;

    std::string encodedUrl = url;
    encodeSpaces(encodedUrl);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpclient_on_data);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, httpclient_on_header);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(curl, CURLOPT_URL, encodedUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");

    if(headers) {
        HttpHeaders &h = *headers;

        curl_slist *headers = nullptr;

        for(auto &k : h) {
            std::string s = k.first + ": " + k.second;
            headers = curl_slist_append(headers, s.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    headerBuffer.clear();
    contentBuffer.clear();
    CURLcode result = curl_easy_perform(curl);

    if(result == CURLE_OK) {
        long statusCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
        response = HttpResponse(headerBuffer);
        response.statusCode = static_cast<int>(statusCode);
        response.content = contentBuffer;
    }

    return static_cast<int>(result);
}

void HttpClient::setVerbose(bool v) {
    verbose = v;
}

size_t HttpClient::onHeader(char* buffer, size_t size, size_t nitems, HttpClient* client) {
    client->headerBuffer.append(buffer, size * nitems);
    return size * nitems;
}

size_t HttpClient::onData(void* contents, size_t size, size_t nmemb, HttpClient* client) {
    client->contentBuffer.append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& response) {
    os << "status-code: " << response.getStatusCode() << std::endl;

    const HttpHeaders &headers = response.getHeaders();
    for (const auto &header : headers) {
        os << header.first << ": " << header.second << std::endl;
    }

    os << response.getContent() << std::endl;

    return os;
}