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
	
	Atom(std::string at_name, int at_index, std::array<float, 3> at_pos);
	Atom();
	
        std::string name;
        int index;
	std::array<float, 3> pos;
        std::string symbol;
	std::array<float, 3> color;
	std::array<float, 3> col_rgb;
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
	
	Residue(std::string r_name, std::map<int, Atom> r_atoms, int r_index);
	Residue();
	
        std::string name;
	std::map<int, Atom> atoms;
        int index;
};

class Chain{
    /*
     * Class description goes here
     */
    public:
	
	Chain(char ch_name, std::map<int, Residue> ch_residues);
	Chain();
	
        char name;
	std::map<int, Residue> residues;
};

struct bond{
    Atom atm;
    float length;
    float angle;
    std::array<float, 3> vec_ort;
    std::array<float, 3> midpoint;
};

class Frame{
    /*
     * Class description goes here
     */
    
    public:
	
	Frame(std::map<char, Chain> f_chains, std::array<float, 3> f_mass_center);
	Frame();
	
	std::map<char, Chain> chains;
        std::array<float, 3> mass_center;
	std::vector<Atom> atoms;
	std::vector<bond> bonds;
	
	void load_bonds();
	void load_ribbons();
};

#endif
