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
#include <sstream>
#include <vector>

#include "parser.h"
#include "MolecularModel.h"

//std::vector<float> parse_pdb(std::string file_path){
Frame parse_pdb(std::string file_path){
    /*
     * 
     */
    Frame pdb_frame;
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
    int at_index, at_res_index;
    std::stringstream ss;
    std::string at_name, at_res_name;
    Chain pdb_chain;
    std::vector<Residue> chain_resids;
    Residue resid;
    
    for (int i = 0; i < file_list.size(); i++){
	if (file_list[i].compare(0, 4, "ATOM")==0 || file_list[i].compare(0, 6, "HETATM")==0){
	    // Parse the atom index, name, residue name, residue index and coordinates, in that order
	    ss << file_list[i].substr(6, 5); ss >> at_index; ss.clear();
	    at_name = file_list[i].substr(12, 4);
	    at_res_name = file_list[i].substr(17, 3);
	    ss << file_list[i].substr(22, 4); ss >> at_res_index; ss.clear();
	    ss << file_list[i].substr(30, 8); ss >> pos_x; ss.clear();
	    ss << file_list[i].substr(38, 8); ss >> pos_y; ss.clear();
	    ss << file_list[i].substr(46, 8); ss >> pos_z; ss.clear();
	    Atom atomo = Atom(at_name, at_index, at_pos, at_res_index);
	    
	    
	    pdb_atoms.push_back(pos_x);
	    pdb_atoms.push_back(pos_y);
	    pdb_atoms.push_back(pos_z);
	    
	}
	else{
	    if (file_list[i].compare(0, 4, "ENDMDL")==0){
		i = file_list.size();
	    }
	    
	}
	
    }
    
    //for (int i = 0; i < pdb_atoms.size(); i++){
	//std::cout << pdb_atoms[i] << std::endl;
    //}
    
    return pdb_frame;
}



