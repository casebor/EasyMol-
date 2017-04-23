/*
 * functions.h
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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define PI 3.14159265358979323846

template <typename T, typename T2>
std::vector<T> get_keys(std::map<T, T2> map_t);

template <typename T, typename T2>
std::vector<T2> get_values(std::map<T, T2> map_t);

std::array<float, 3> unit_vector(std::array<float, 3> vec);

float get_angle(std::array<float, 3> vec_a, std::array<float, 3> vec_b);

std::array<float, 3> cross_p(std::array<float, 3> vec_a, std::array<float, 3> vec_b);

std::vector<bond> generate_bonds(std::vector<Atom> atoms);

double get_euclidean(double vec1[3], double vec2[3]);

//void mat4_to_vec16(glm::mat4 *mat, GLdouble *vec);
//void vec16_to_mat4(GLdouble *vec, glm::mat4 *mat);

#endif
