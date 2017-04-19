/*
 * MolecularModel.cxx
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

#include <string>
#include <vector>
#include <array>
#include <map>

#include "MolecularModel.h"

Atom::Atom(std::string at_name, int at_index, std::array<float, 3> at_pos){
    /*
     * 
     */
    
    name = at_name;
    index = at_index;
    pos = at_pos;
    color[0] = 0; color[1] = 1; color[2] = 0;
    col_rgb[0] = 0; col_rgb[1] = 1; col_rgb[2] = 0;
    radius = 2.5;
    vdw_rad = 2.0;
    cov_rad = 1.7;
    ball_radius = 1.4;
    sphere = false;
    dot = true;
}

Atom::Atom(){
    /*
     * 
     */
    
    color[0] = 0; color[1] = 1; color[2] = 0;
    col_rgb[0] = 0; col_rgb[1] = 1; col_rgb[2] = 0;
    radius = 2.5;
    vdw_rad = 4.0;
    cov_rad = 3.5;
    ball_radius = 1.4;
    sphere = false;
    dot = true;
}

Residue::Residue(std::string r_name, std::map<int, Atom> r_atoms, int r_index){
    /*
     * 
     */
    name = r_name;
    atoms = r_atoms;
    index = r_index;
    //chain = r_chain;
}

Residue::Residue(){
    /*
     * 
     */
    
}

Chain::Chain(char ch_name, std::map<int, Residue> ch_residues){
    /*
     * 
     */
    name = ch_name;
    residues = ch_residues;
    //frame = ch_frame;
}

Chain::Chain(){
    /*
     * 
     */
    //name = ch_name;
    //residues = ch_residues;
    //frame = ch_frame;
}

Frame::Frame(std::map<char, Chain> f_chains, std::array<float, 3> f_mass_center){
    /*
     * 
     */
    
    chains = f_chains;
    mass_center = f_mass_center;
    //atoms = {};
}

Frame::Frame(){
    /*
     * 
     */
    
    mass_center[0] = 0; mass_center[1] = 0; mass_center[2] = 0; 
    //atoms = {};
}

void Frame::load_bonds(){
    /*
     * 
     */
}

void Frame::load_ribbons(){
    /*
     * 
     */
    
}

