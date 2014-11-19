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

int main(int argc, char* argv[]) {
    libmsg::Webclient w;
    {
        try {
            libmsg::JsonPtr v = w.performHttpGet("https://dev3-api.mysmartgrid.de:8443/sensor/0d53f4b15ce1dfb0932e47c5f1751279?unit=watt&interval=hour", libmsg::Secret::fromKey("8793a87821ef852ac3d31033a7cea385"));
        } catch (const std::exception &e) {
            std::cout << "performHttpGet failed: " << e.what() << std::endl;
        }
    }
    {
        try {
            boost::shared_ptr<Json::Value> v = w.performHttpGet("https://api.mysmartgrid.de:8443/sensor/5715a57b0c8ec958727ab3bb206d8b6e?unit=watt&interval=hour", libmsg::Secret::fromToken("d3a16238d92456bb0f6727743275dce0"));
        } catch (const std::exception &e) {
            std::cout << "performHttpGetToken failed: " << e.what() << std::endl;
        }
    }
}
