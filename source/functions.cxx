/*
 * functions.cxx
 * 
 * Copyright 2017 Carlos Eduardo Sequeiros Borja <casebor@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <vector>
#include <map>

#include "functions.h"

template <typename T, typename T2>
std::vector<T> get_keys(std::map<T, T2> map_t){
    std::vector<T> keys;
    keys.reserve(map_t.size());
    for(auto const& elem: map_t)
	keys.push_back(elem.first);
    return keys;
    
}

template <typename T, typename T2>
std::vector<T2> get_values(std::map<T, T2> map_t){
    std::vector<T2> values;
    values.reserve(map_t.size());
    for(auto const& elem: map_t)
	values.push_back(elem.second);
    return values;
    
}

