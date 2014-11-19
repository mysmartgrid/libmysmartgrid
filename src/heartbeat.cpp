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

#include <iostream>
#include <libmysmartgrid/webclient.h>
#include <libmysmartgrid/error.h>

int main(int argc, char* argv[]) {
    Json::Value *value = new Json::Value(Json::objectValue);
    (*value)["key"] = "644bc984759564991ccabe7f9fcb801a";
    (*value)["uptime"] = 0;
    (*value)["version"] = 0;

    try {
        std::string url = libmsg::Webclient::composeDeviceUrl("https://dev3-api.mysmartgrid.de:8443/", "ae44fe656e58a43284995e5db583b378");
        libmsg::JsonPtr v = libmsg::Webclient::performHttpPost(url, libmsg::Secret::fromKey("644bc984759564991ccabe7f9fcb801a"), libmsg::JsonPtr(value));
        std::cout << "Result: " << *v << std::endl;
    } catch (const libmsg::GenericException& e) {
        std::cout << "Error during request: " << e.what();
    }
}
