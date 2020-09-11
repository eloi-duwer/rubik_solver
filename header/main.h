/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:51 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/12 00:09:02 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MAIN_H__
# define __MAIN_H__

# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>

# define M_U 0
# define M_U2 1
# define M_UP 2

# define M_D 3
# define M_D2 4
# define M_DP 5

# define M_L 6
# define M_L2 7
# define M_LP 8

# define M_R 9
# define M_R2 10
# define M_RP 11

# define M_F 12
# define M_F2 13
# define M_FP 14

# define M_B 15
# define M_B2 16
# define M_BP 17

/*
 * Corner pos order: UFL, UBL, UBR, UFR, DFL, DBL, DBR, DFR
 * Edge pos order: UF, UL, UB, UR, FL, BL, BR, FR, DF, DL, DB, DR
 * Edge orientation: 0 if good, 1 if bad.
 * good = the piece can be brought back to it's position / orientation
 * without using up or down move,
 * so each up and down moves flip the edge orientation
 * Corner orientation: 0 if the U or D corner facelet is
 * on the U or D side of the Cube, 2 or 1 if the corner
 * needs to be rotated cw or ccw respectivley
 */
typedef struct	s_cube {
	uint8_t		corner_pos[8];
	uint8_t		edge_pos[12];
	uint8_t		corner_orientation[8];
	uint8_t		edge_orientation[12];

} 				t_cube;

void print_cube(t_cube *cube);
t_cube *create_base_cube();
t_cube *rotation_cube(t_cube *cube, int move, bool free_old_cube);
t_cube	*duplicate_cube(const t_cube *in);
uint8_t get_next_pos(int move, int step);

#endif
