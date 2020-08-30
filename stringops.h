// -*- grammar-ext: .cpp -*-
/*	
 *	This file is a part of ConfigurableAssemblyIntepreter.
 *	
 *	ConfigurableAssemblyIntepreter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ConfigurableAssemblyIntepreter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <string>
#include <vector>

using stringPair_t = std::pair<std::string,std::string>; 

std::string strip(std::string str, char c);
std::string stripends(std::string str, char c);

stringPair_t splitOnFirstChar(std::string str, char c);
std::vector<int> processArrayString(std::string str);
