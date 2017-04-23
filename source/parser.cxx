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

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <map>

#include "MolecularModel.h"
#include "parser.h"

Frame parse_pdb(std::string file_path){
    /*
     * Parse a PDB file into a Frame object
     * 
     * Receives: The path to a PDB file as a string type
     * Returns:  A Frame object
     */
    // Creates a new Frame object
    Frame pdb_frame;
    // Put all the lines from the file into a list of strings
    std::ifstream my_file (file_path);
    std::vector<std::string> file_list;
    std::string line;
    if (my_file.is_open()){
        while (getline(my_file,line))
            file_list.push_back(line);
        my_file.close();
    }
    // Create variables to use in the parse process
    std::vector<Atom> pdb_atoms;
    std::map<char, Chain> f_chains;
    // Position of the atoms
    float pos_x, pos_y, pos_z;
    // Index of the atom and the residue at which belongs
    int at_index, at_res_index;
    // The name of the chain, the atom and the residue
    char at_chain;
    std::string at_name, at_res_name;
    // We use the 'stringstream ss' to convert string to float or int
    std::stringstream ss;
    // Begin to parse every atom, for now only a single frame pdb is contemplated
    for (int i = 0; i < file_list.size(); i++){
        if (file_list[i].compare(0, 4, "ATOM") == 0 || file_list[i].compare(0, 6, "HETATM") == 0){
            // Parse the atom index, name, residue name, residue index and coordinates, in that order
            ss << file_list[i].substr(6, 5); ss >> at_index; ss.clear();
            at_name = file_list[i].substr(12, 4);
            at_res_name = file_list[i].substr(17, 3);
            ss << file_list[i].substr(22, 4); ss >> at_res_index; ss.clear();
            ss << file_list[i].substr(30, 8); ss >> pos_x; ss.clear();
            ss << file_list[i].substr(38, 8); ss >> pos_y; ss.clear();
            ss << file_list[i].substr(46, 8); ss >> pos_z; ss.clear();
            std::array<float, 3> pos_xyz = {pos_x, pos_y, pos_z};
            Atom new_atom(at_name, at_index, pos_xyz);
            at_chain = file_list[i][21];
            // Search for a chain. if there is no chain, the default is 'A'
            if (at_chain == ' ')
                at_chain = 'A';
            if (f_chains.find(at_chain) == f_chains.end()){
            /*
             * If the chain does not exist, then we don't have to worry about asking 
             * if the other elements exist, because they don't.
             */
                Chain new_chain;
                new_chain.name = at_chain;
                Residue new_resid;
                new_resid.name = at_res_name;
                new_resid.index = at_res_index;
                new_resid.atoms[at_index] = new_atom;
                new_chain.residues[at_res_index] = new_resid;
                f_chains[at_chain] = new_chain;
                pdb_frame.chains[at_chain] = new_chain;
            }
            else{
                /*
                 * IMPORTANT: When there is a chain already in our Frame, we use the '&'
                 * operator for accesing the memory of that object. If we didn't used the
                 * memory access and only wrote: 
                 * 	Chain temp_chain = pdb_frame.chains[at_chain];
                 * we would made a copy of the object and all the changes made it in the 
                 * chain would been lost.
                 */
                Chain & temp_chain = pdb_frame.chains[at_chain];
                if (temp_chain.residues.find(at_res_index) == temp_chain.residues.end()){
                    Residue new_resid;
                    new_resid.name = at_res_name;
                    new_resid.index = at_res_index;
                    new_resid.atoms[at_index] = new_atom;
                    temp_chain.residues[at_res_index] = new_resid;
                }
                else{
                    /*
                    * Same as IMPORTANT as above, only for residue.
                    */
                    Residue & temp_resid = temp_chain.residues[at_res_index];
                    temp_resid.atoms[at_index] = new_atom;
                }
            }
            pdb_atoms.push_back(new_atom);
        }
        else{
            if (file_list[i].compare(0, 4, "ENDMDL") == 0 || file_list[i].compare(0, 3, "END") == 0){
            i = file_list.size();
            }
        }
    }
    // We add all the atoms created in the Frame object just for later purposes
    pdb_frame.atoms = pdb_atoms;
    return pdb_frame;
}



