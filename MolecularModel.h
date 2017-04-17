/*
 * MolecularModel.h
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


#ifndef MOLECULAR_MODEL_H
#define MOLECULAR_MODEL_H

class Residue;
class Chain;
class Frame;

class Atom{
    /*
     * Class description goes here
     */
    public:
	
	Atom(std::string at_name, int at_index, std::string at_symbol, float at_pos[3], Residue* at_resid);
	
	float* pos;
        int index;
        std::string name;
        std::string symbol;
	Residue* resid;
	float color[];
	float col_rgb[];
	float radius;
	float vdw_rad;
	float cov_rad;
	float ball_radius;
	bool sphere;
	bool dot;
};

class Residue{
    /*
     * Class description goes here
     */
    public:
	
	Residue(std::vector<Atom> r_atoms, std::string r_name, int r_index, Chain* r_chain);
	
	std::vector<Atom> atoms;
        std::string name;
        int index;
        Chain* chain;
    
};

class Chain{
    /*
     * Class description goes here
     */
    public:
	
	Chain(char ch_name, std::vector<Residue> ch_residues, Frame* ch_frame);
	
        char name;
	std::vector<Residue> residues;
	Frame* frame;
	//backbone;
    
};

class Frame{
    /*
     * Class description goes here
     */
    
    public:
	
	Frame(std::vector<Chain> f_chains, float f_mass_center[]);
	
	std::vector<Chain> chains;
        float* mass_center;
	std::vector<Atom> atoms;
	
	void load_bonds();
	void load_ribbons();
};

#endif
