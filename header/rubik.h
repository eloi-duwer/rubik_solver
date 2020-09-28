/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubik.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:51 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/28 18:10:45 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RUBIK_H__
# define __RUBIK_H__
# define _GNU_SOURCE //asprintf
# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <pieces_positions_movements.h>
# include <search.h>

/*
 * corner pos: see pieces_positions_movements.h
 * Edge orientation: 0 if good, 1 if bad.
 * good = the piece can be brought back to it's position / orientation
 * without using up or down move,
 * so each up and down moves flip the edge orientation (u2/d2 does nothing)
 * Corner orientation: 0 if the L or R corner facelet is
 * on the L or R side of the Cube, 1 or 2 if the corner
 * needs to be rotated cw or ccw respectivley
 */
typedef struct	s_cube {
	uint_fast8_t	corner_pos[8];
	uint_fast8_t	edge_pos[12];
	uint_fast8_t	corner_orientation[8];
	uint_fast8_t	edge_orientation[12];
}				t_cube;

typedef struct s_cube_list {
	t_cube				*cube;
	int_fast8_t			*moves;
	int					nb_moves;
	bool				is_forward;
	uint_fast8_t		id[41];
	struct s_cube_list	*next;
}				t_cube_list;

void			print_cube(t_cube *cube);
t_cube			*create_base_cube();
t_cube			*rotation_cube(t_cube *cube, int_fast8_t move, bool free_old_cube);
t_cube			*duplicate_cube(const t_cube *in);
uint_fast8_t	get_next_pos(int_fast8_t move, int step);
char			**ft_strsplit(char const *s, char c);
int_fast8_t		str_to_move(char *str);
char			*move_to_str(int_fast8_t move);
int_fast8_t		inverse_move(int_fast8_t move);
char			*solve(t_cube *cube);
t_cube_list		*init_cube_list(int step, t_cube *cube, int_fast8_t *moves, int nb_moves, bool is_forward);
uint_fast16_t	slice_state(t_cube *cube);
uint_fast16_t	m_slice_state(t_cube *cube);
uint_fast8_t	*get_cube_id(int step, t_cube *cube, uint_fast8_t *ret);
void			free_one_cube_list(t_cube_list *one_cube);
void			free_cube_list(t_cube_list *list);
int				rubik_hash(void *key);
bool			rubik_equals(void *a, void *b);
uint_fast8_t	orbit_state(t_cube *cube);
void			print_id(uint_fast8_t *id);
void			print_cube_id(t_cube *cube, int step);

#endif
