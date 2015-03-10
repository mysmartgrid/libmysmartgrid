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

#ifndef LIBMSG_WEBCLIENT_HPP
#define LIBMSG_WEBCLIENT_HPP 1

#include <boost/smart_ptr.hpp>
#include <jsoncpp/json/json.h>

namespace libmsg {
	class Secret {
		public:
			enum class SecretType {
				Token,
				Key
			};
			static Secret fromKey(const std::string& key);
			static Secret fromToken(const std::string& token);
			bool isKey() const {
				return _type == SecretType::Key;
			}
			bool isToken() const {
				return _type == SecretType::Token;
			}
			std::string key() const;
			std::string token() const;

		private:
			Secret() {};
			SecretType _type;
			std::string _secret;
	};

	typedef boost::shared_ptr<Json::Value> JsonPtr;
	class Webclient {
		public:
			typedef std::map<std::string, std::string> ParamList;
			typedef std::pair<int, double> Reading;
			typedef std::vector<std::pair<int, double> > ReadingList;
			Webclient() {};
			~Webclient() {};

			static ReadingList getReadings(const std::string& url, const std::string& id, const Secret& secret, const std::string& unit = "watt");
			static Reading getLastReading(const std::string& url, const std::string& id, const Secret& secret, const std::string& unit = "watt");

			static JsonPtr performHttpRequest(const std::string& method, const std::string& url, const Secret& secret, const JsonPtr& body = JsonPtr(new Json::Value()));
			static JsonPtr performHttpGet(const std::string& url, const Secret& secret);
			static JsonPtr performHttpPost(const std::string& url, const Secret& secret, const JsonPtr& body);
			static JsonPtr performHttpDelete(const std::string& url, const Secret& secret);

			static const std::string composeDeviceUrl(const std::string& url, const std::string& deviceId);
			static const std::string composeSensorUrl(const std::string& url, const std::string& sensorId, const ParamList& params = ParamList());
			static const std::string composeUrl(const std::string& url, const std::string& object, const std::string& id, const std::string& query = "");

		protected:
			//static JsonPtr performHttpRequest(const std::string& method, const std::string& url, const std::string& token, const std::string& key, const JsonPtr& body);

			static const std::string digest_message(const std::string& data, const std::string& key);
			static size_t curlWriteCustomCallback(char *ptr, size_t size, size_t nmemb, void *data);

		private:
			static const std::string GET;
			static const std::string POST;
			static const std::string DELETE;
	};
}

#endif /* LIBMSG_WEBCLIENT_HPP */
