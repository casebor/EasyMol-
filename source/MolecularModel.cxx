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


#include <iostream>
#include <string>
#include <vector>

#include "MolecularModel.h"


Atom::Atom(std::string at_name, int at_index, float at_pos[3], Residue* at_resid){
    /*
     * 
     */
    
    name = at_name;
    index = at_index;
    //symbol = at_symbol;
    pos[0] = at_pos[0]; pos[1] = at_pos[1]; pos[2] = at_pos[2];
    resid = at_resid;
    color[0] = 0; color[1] = 1; color[2] = 0;
    col_rgb[0] = 0; col_rgb[1] = 1; col_rgb[2] = 0;
    radius = 2.5;
    vdw_rad = 4.0;
    cov_rad = 3.5;
    ball_radius = 1.4;
    sphere = false;
    dot = true;
}

Residue::Residue(std::vector<Atom> r_atoms, std::string r_name, int r_index, Chain* r_chain){
    /*
     * 
     */
    atoms = r_atoms;
    name = r_name;
    index = r_index;
    chain = r_chain;
}

Chain::Chain(char ch_name, std::vector<Residue> ch_residues, Frame* ch_frame){
    /*
     * 
     */
    name = ch_name;
    residues = ch_residues;
    frame = ch_frame;
}

Frame::Frame(std::vector<Chain> f_chains, float f_mass_center[]){
    /*
     * 
     */
    
    chains = f_chains;
    mass_center[0] = f_mass_center[0]; mass_center[1] = f_mass_center[1]; mass_center[2] = f_mass_center[2]; 
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
