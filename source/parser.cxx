/*
 * parser.cxx
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


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "parser.h"
#include "MolecularModel.h"

std::vector<float> parse_pdb(std::string file_path){
    /*
     * 
     */
    std::vector<float> pdb_atoms;
    std::ifstream my_file (file_path);
    std::vector<std::string> file_list;
    std::string line;
    // Fill a list with al the file lines
    if (my_file.is_open()){
	while (getline(my_file,line))
	    file_list.push_back(line);
	my_file.close();
    }
    // Parse the coordinates, assign them to the atoms and begin the parse
    float pos_x, pos_y, pos_z;
    for (int i = 0; i < file_list.size(); i++){
	if (file_list[i].compare(0, 4, "ATOM")==0 || file_list[i].compare(0, 6, "HETATM")==0){
	    pos_x = std::stof(file_list[i].substr(30, 8));
	    pos_y = std::stof(file_list[i].substr(38, 8));
	    pos_z = std::stof(file_list[i].substr(46, 8));
	    std::cout << pos_x << std::endl;
	    std::cout << file_list[i] << std::endl;
	    //std::cout << file_list[i].substr(30, 8) << std::endl;
	    //std::cout << file_list[i].substr(38, 8) << std::endl;
	    //std::cout << file_list[i].substr(46, 8) << std::endl;
	    pdb_atoms.push_back(pos_x);
	    pdb_atoms.push_back(pos_y);
	    pdb_atoms.push_back(pos_z);
	}
	
    }
    
    
    return pdb_atoms;
}



