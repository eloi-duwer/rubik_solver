/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubik.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:51 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/14 23:47:50 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RUBIK_H__
# define __RUBIK_H__

# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <pieces_positions_movements.h>

/*
 * corner pos: see pieces_positions_movements.h
 * Edge orientation: 0 if good, 1 if bad.
 * good = the piece can be brought back to it's position / orientation
 * without using up or down move,
 * so each up and down moves flip the edge orientation (u2/d2 does nothing)
 * Corner orientation: 0 if the L or R corner facelet is
 * on the L or R side of the Cube, 2 or 1 if the corner
 * needs to be rotated cw or ccw respectivley
 */
typedef struct	s_cube {
	uint8_t		corner_pos[8];
	uint8_t		edge_pos[12];
	uint8_t		corner_orientation[8];
	uint8_t		edge_orientation[12];

}				t_cube;

void	print_cube(t_cube *cube);
t_cube	*create_base_cube();
t_cube	*rotation_cube(t_cube *cube, int move, bool free_old_cube);
t_cube	*duplicate_cube(const t_cube *in);
uint8_t	get_next_pos(int move, int step);
char	**ft_strsplit(char const *s, char c);
int		str_to_move(char *str);
char	*move_to_str(int move);

#endif
