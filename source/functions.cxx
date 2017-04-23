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

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <map>

#include "MolecularModel.h"
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

std::array<float, 3> unit_vector(std::array<float, 3> vec){
    /*
     * 
     */
    std::array<float, 3> u_vec;
    float module = std::sqrt(std::pow(vec[0], 2) + std::pow(vec[1], 2) + std::pow(vec[2], 2));
    u_vec[0] = vec[0]/module;
    u_vec[1] = vec[1]/module;
    u_vec[2] = vec[2]/module;
    return u_vec;
}

float get_angle(std::array<float, 3> vec_a, std::array<float, 3> vec_b){
    /*
     * 
     */
    std::array<float, 3> vec_ua = unit_vector(vec_a);
    std::array<float, 3> vec_ub = unit_vector(vec_b);
    float dot_product = vec_ua[0]*vec_ub[0] + vec_ua[1]*vec_ub[1] + vec_ua[2]*vec_ub[2];
    float angle = std::acos(dot_product);
    return angle * 180.0 / PI;
}

std::array<float, 3> cross_p(std::array<float, 3> vec_a, std::array<float, 3> vec_b){
    /*
     * 
     */
    std::array<float, 3> vec_o;
    vec_o[0] = vec_a[1]*vec_b[2] - vec_a[2]*vec_b[1];
    vec_o[0] = vec_a[2]*vec_b[0] - vec_a[0]*vec_b[2];
    vec_o[0] = vec_a[0]*vec_b[1] - vec_a[1]*vec_b[0];
    return unit_vector(vec_o);
}

std::vector<bond> generate_bonds(std::vector<Atom> atoms){
    /*
     * 
     */
    std::vector<bond> bonds;
    std::array<float, 3> array1 = {0.0, 0.0, 1.0};
    for (int i = 0; i < atoms.size()-1; i++){
        int limit;
        if (i+25 >= atoms.size())
            limit = atoms.size();
        else
            limit = i+25;
        for (int j = i+1; j < limit; j++){
            if (atoms[j].pos[0] - atoms[i].pos[0] >= 2.0 ||
                atoms[j].pos[1] - atoms[i].pos[1] >= 2.0 ||
                atoms[j].pos[2] - atoms[i].pos[2] >= 2.0 ){
                // Do nothing
            }
            else{
                std::array<float, 3> v_dist = {atoms[j].pos[0] - atoms[i].pos[0], atoms[j].pos[1] - atoms[i].pos[1], atoms[j].pos[2] - atoms[i].pos[2]};
                float dist2 = std::pow(v_dist[0], 2) + std::pow(v_dist[1], 2) + std::pow(v_dist[2], 2);
                if (dist2 <= (std::pow(atoms[j].cov_rad + atoms[i].cov_rad, 2)) * 1.1){
                    float sqrt_dist2 = std::sqrt(dist2);
                    std::array<float, 3> v_dir = unit_vector(v_dist);
                    bond p_bond, q_bond;
                    p_bond.atm = atoms[i];
                    p_bond.length = sqrt_dist2 / 2.0;
                    p_bond.angle = get_angle(array1, v_dir);
                    p_bond.vec_ort = cross_p(array1, v_dir);
                    for (short k = 0; k < 3; i++){
                        p_bond.midpoint[k] = (atoms[j].pos[k] + atoms[i].pos[k]) / 2.0;
                        q_bond.midpoint[k] = p_bond.midpoint[k];
                    }
                    q_bond.atm = atoms[i];
                    q_bond.length = sqrt_dist2 - p_bond.length;
                    q_bond.angle = p_bond.angle + 180.0;
                    q_bond.vec_ort = p_bond.vec_ort;
                    
                    bonds.push_back(p_bond);
                    bonds.push_back(q_bond);
                }
            }
        }
    }
    return bonds;
}

double get_euclidean(double vec1[3], double vec2[3]){
    /*
     * 
     */
    double dx, dy, dz;
    dx = std::pow(vec1[0] - vec2[0], 2);
    dy = std::pow(vec1[1] - vec2[1], 2);
    dz = std::pow(vec1[2] - vec2[2], 2);
    return std::sqrt(dx + dy + dz);
}

//void mat4_to_vec16(glm::mat4 *mat, GLdouble *vec){
    ///*
     //* 
     //*/
    ////vec[0] = mat[0][0]; vec[1] = mat[0][1]; vec[2] = mat[0][2]; vec[3] = mat[0][3];
    ////vec[4] = mat[1][0]; vec[5] = mat[1][1]; vec[6] = mat[1][2]; vec[7] = mat[1][3];
    ////vec[8] = mat[2][0]; vec[9] = mat[2][1]; vec[10] = mat[2][2]; vec[11] = mat[2][3];
    ////vec[12] = mat[3][0]; vec[13] = mat[2][1]; vec[14] = mat[3][2]; vec[15] = mat[3][3];
//}

//void vec16_to_mat4(GLdouble *vec, glm::mat4 *mat){
    ///*
     //* 
     //*/
    ////mat[0][0] = vec[0]; mat[0][1] = vec[1]; mat[0][2] = vec[2]; mat[0][3] = vec[3];
    ////mat[1][0] = vec[4]; mat[1][1] = vec[5]; mat[1][2] = vec[6]; mat[1][3] = vec[7];
    ////mat[2][0] = vec[8]; mat[2][1] = vec[9]; mat[2][2] = vec[10]; mat[2][3] = vec[11];
    ////mat[3][0] = vec[12]; mat[2][1] = vec[13]; mat[3][2] = vec[14]; mat[3][3] = vec[15];
    ////mat[0] = glm::vec4(vec[0], vec[1], vec[2], vec[3]);
    
    //std::cout << vec[0] << std::endl;
//}
