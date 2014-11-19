/**
 * This file is part of libmysmartgrid.
 *
 * (c) Fraunhofer ITWM - Mathias Dalheimer <dalheimer@itwm.fhg.de>,    2010
 *                       Ely de Oliveira   <ely.oliveira@itwm.fhg.de>, 2013
 *                       Stephan Platz     <platz@itwm.fhg.de>,        2014
 *
 * libklio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libklio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libklio. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <curl/curl.h>
#include "webclient.h"
#include <nacl/crypto_stream.h>
#include <nacl/crypto_auth.h>
#include <nacl/crypto_sign.h>
#include "libmysmartgrid/error.h"

using namespace libmsg;

const std::string Webclient::GET = "GET";
const std::string Webclient::POST = "POST";
const std::string Webclient::DELETE = "DELETE";

Secret Secret::fromKey(const std::string& key) {

    Secret s;
    s._type = SecretType::Key;
    s._secret = key;
    return s;
};

Secret Secret::fromToken(const std::string& token) {

    Secret s;
    s._type = SecretType::Token;
    s._secret = token;
    return s;
};

std::string Secret::key() const {

    if (_type == SecretType::Key) {
        return _secret;
    }
    throw GenericException("Key requested from secret that is no key");
}

std::string Secret::token() const {

    if (_type == SecretType::Token) {
        return _secret;
    }
    throw GenericException("Token requested from secret that is no token");
}

const std::string Webclient::digest_message(const std::string& data, const std::string& key) {

    return crypto_sign(data, key);
}

/* Store curl responses in memory. Curl needs a custom callback for that. Otherwise it tries to write the response to a file. */
size_t Webclient::curlWriteCustomCallback(char *ptr, size_t size, size_t nmemb, void *data) {

    size_t realsize = size * nmemb;
    std::string *response = static_cast<std::string *> (data);
    response->append(ptr, realsize);
    return realsize;
}

Webclient::ReadingList Webclient::getReadings(const std::string& url, const std::string& id, const Secret& secret, const std::string& unit) {

    Webclient::ParamList p;
    p["interval"] = "hour";
    p["unit"] = unit;

    std::string sensorUrl = Webclient::composeSensorUrl(url, id, p);
    JsonPtr result = Webclient::performHttpGet(sensorUrl, secret);

    ReadingList readings;
    for (auto it = result->begin(), end = result->end(); it != end; ++it) {

        Json::Value timestamp = (*it)[0];
        Json::Value value = (*it)[1];
        if (value.isConvertibleTo(Json::realValue)) {
            readings.push_back(std::make_pair(timestamp.asInt(), value.asDouble()));
        }
    }
    return readings;
}

Webclient::Reading Webclient::getLastReading(const std::string& url, const std::string& id, const Secret& secret, const std::string& unit) {

    ReadingList list = getReadings(url, id, secret, unit);

    // TODO: Do we have to ensure here that the list is sorted by ascending timestamp? Or is that guaranteed by the api?
    auto it = list.rbegin(), end = list.rend();
    while (it != end && it->second == 0) {
        it++;
    }

    if (it != end) {
        return *it;
    }
    return std::make_pair(0, 0.0);
}

JsonPtr Webclient::performHttpRequest(const std::string& method, const std::string& url, const Secret& secret, const JsonPtr& body) {

    long int httpCode = 0;
    CURLcode curlCode;
    std::string response = "";
    CURL *curl = nullptr;
    Json::Value *jsonValue = new Json::Value();
    curl_slist *headers = nullptr;

    try {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (!curl) {
            throw EnvironmentException("CURL could not be initiated.");
        }

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCustomCallback);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        //Activate this line to print requests and responses to the console
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

        //Signal-handling is NOT thread-safe.
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        //Required if next router has an ip-change.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

        headers = curl_slist_append(headers, "User-Agent: libmysmartgrid");
        headers = curl_slist_append(headers, "X-Version: 1.0");
        headers = curl_slist_append(headers, "Accept: application/json,text/html");

        std::string strbody;
        if (!body->isNull()) {
            Json::FastWriter w;
            strbody = w.write(*body);
        }
        std::ostringstream oss;
        if (secret.isKey()) {
            oss << "X-Digest: " << digest_message(strbody, secret.key());
        } else if (secret.isToken()) {
            oss << "X-Token: " << secret.token();
        }
        headers = curl_slist_append(headers, oss.str().c_str());

        if (method == "POST") {
            headers = curl_slist_append(headers, "Content-type: application/json");

            oss.str(std::string());
            oss << "Content-Length: " << strbody.length();
            headers = curl_slist_append(headers, oss.str().c_str());

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strbody.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curlCode = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        if (curlCode == CURLE_OK && httpCode == 200) {

            bool ok;
            Json::Reader r;
            ok = r.parse(response, *jsonValue, false);
            if (!ok) {
                throw GenericException("Error parsing response: " + response);
            }

            //Clean up
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            curl_global_cleanup();

            return boost::shared_ptr<Json::Value>(jsonValue);

        } else {
            oss.str(std::string());
            oss << "HTTPS request failed." <<
                    " cURL Error: " << curl_easy_strerror(curlCode) << ". " <<
                    " HTTPS code: " << httpCode;

            if (httpCode >= 400 && httpCode <= 499) {
                throw DataFormatException(oss.str());

            } else if (httpCode >= 500 || httpCode == 0 || httpCode == 200) {
                throw CommunicationException(oss.str());

            } else {
                throw GenericException(oss.str());
            }
        }

    } catch (std::exception const& e) {

        //Clean up
        if (headers != nullptr) {
            curl_slist_free_all(headers);
        }
        if (curl != nullptr) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        throw;
    }
    //Some compilers require a return here
    throw GenericException("This point should never be reached.");
}

JsonPtr Webclient::performHttpGet(const std::string& url, const Secret& secret) {

    return performHttpRequest(GET, url, secret);
}

JsonPtr Webclient::performHttpPost(const std::string& url, const Secret& secret, const JsonPtr& body) {

    return performHttpRequest(POST, url, secret, body);
}

JsonPtr Webclient::performHttpDelete(const std::string& url, const Secret& secret) {

    return performHttpRequest(DELETE, url, secret);
}

const std::string Webclient::composeDeviceUrl(const std::string& url, const std::string& id) {

    return composeUrl(url, std::string("device"), id);
}

const std::string Webclient::composeSensorUrl(const std::string& url, const std::string& sensorId, const ParamList& params) {

    std::stringstream oss;
    const char* seperator = "";

    for (auto it = params.begin(), end = params.end(); it != end; ++it) {
        oss << seperator << it->first << "=" << it->second;
        seperator = "&";
    }

    return composeUrl(url, std::string("sensor"), sensorId, oss.str());
}

const std::string Webclient::composeUrl(const std::string& url, const std::string& object, const std::string& id, const std::string& query) {

    std::ostringstream oss;
    oss << url << "/" << object << "/" << id;
    if (!query.empty()) {
        oss << "?" << query;
    }
    return oss.str();
}
