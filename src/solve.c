/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:29:36 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/22 00:34:35 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

int allowed_moves[4] = {
	0b111111111111111111, //Allow every moves
	0b010010111111111111, //Only disallow U, U', D, D'
	0b010010111111010010, //Only allow half turns except for L and R faces
	0b010010010010010010 //Only allow half turns
};

/**
 * Corners at even positions have the same orbit, same for odd posisions 
 * Same orbit = you can use only half turn to move to any point of the orbit
 */
uint_fast8_t	orbit_state(t_cube *cube) {
	uint_fast8_t	ret = 0;
	int				i = 0;

	while (i < 8) {
		ret |= (cube->corner_pos[i] % 2 == i % 2) << i;
		++i;
	}
	return (ret);
}

/**
 * Edges are on their slice
 * Same slice = you can use only half turn to move any piece of the slice
 * To another place on the slice
 * Edges pos are defined slice by slice: slice 0,1,2,3 = slice M,
 * 4,5,6,7 = slice S, 8,9,10,11 = slice E.
 */
uint_fast16_t	slice_state(t_cube *cube) {
	uint_fast16_t	ret = 0;
	int				i = 0;

	while (i < 12) {
		ret |= (cube->edge_pos[i] / 4 == i / 4) << i;
		++i;
	}
	return (ret);
}

//same as slice_state, but just for the M layer (used in the second state of the solving)
uint_fast8_t	m_slice_state(t_cube *cube) {
	uint_fast8_t	ret = 0;
	int				i = 0;

	while (i < 4) {
		ret |= (cube->edge_pos[i] / 4 == 0) << i;
		++i;
	}
	return (ret);
}

bool	check_cube_state(int step, t_cube *base, t_cube *cube) {
	if (step == 0) {
		//Only check edge orientation
		return (memcmp(base->edge_orientation, cube->edge_orientation, sizeof(uint_fast8_t) * 12) == 0);
	} else if (step == 1) {
		//Only check corner orientation & M slice edges (between left and right)
		return (memcmp(base->corner_orientation, cube->corner_orientation, sizeof(uint_fast8_t) * 8) == 0 \
			&& m_slice_state(base) == m_slice_state(cube));
	} else if (step == 2) {
		return (orbit_state(base) == orbit_state(cube) \
			&& slice_state(base) == slice_state(cube));
	} else {
		return (memcmp(base, cube, sizeof(t_cube)) == 0);
	}
}

t_cube_state	*init_cube_state(t_cube *cube) {
	t_cube_state	*ret;

	if (cube == NULL)
		return (NULL);
	if ((ret = (t_cube_state *)malloc(sizeof(t_cube_state) * 1)) == NULL)
		return (NULL);
	ret->cube = cube;
	ret->moves = NULL;
	ret->next = NULL;
	return (ret);
}

/** TODO
 * Merges the moves of the matching cubes, and frees everything
 */
int_fast8_t		*merge_paths(t_cube_state *forward,t_cube_state *backward)
{
	return (NULL);
}

void			free_cube_list(t_cube_state *list) {
	
}

int_fast8_t		*parkour_list(int step, t_cube_state **one, t_cube_state **last, t_cube_state *two, bool is_reverse) {
	int				current_depth = (*one)->nb_moves;
	t_cube			*new_cube;
	int_fast8_t 	i;
	t_cube_state	*ptr;
	
	while ((*one)->nb_moves == current_depth) {
		i = 0;
		while (i < 18) {
			if (allowed_moves[step] & 1 << (17 - i) && \
				((*one)->nb_moves == 0 || (*one)->moves[(*one)->nb_moves - 1] / 3 != i / 3)) {
				if ((new_cube = rotation_cube((*one)->cube, i, false)) == NULL) {
					printf("Error during turn\n");
					exit(1);
				}
				ptr = two;
				while (ptr != NULL) {
					if (check_cube_state(step, (*one)->cube, ptr->cube)) { //It's a match!
						if (is_reverse)
							return (merge_paths(*one, two));
						else
							return (merge_paths(two, *one));
					}
					append_cube_to_end_and_free_first(one, last, new_cube, i);
					ptr = ptr->next;
				}
			}
			++i;
		}
	}
	return (NULL);
}

int_fast8_t		*thistlewaite_step(int step, t_cube *cube) {
	int_fast8_t		i;
	t_cube_state	*forward = init_cube_state(cube);
	t_cube_state	*last_forward = forward;
	t_cube_state	*backward = init_cube_state(create_base_cube());
	t_cube_state	*last_backward = backward;
	int_fast8_t		*ret;

	if (forward == NULL || backward == NULL)
		return (NULL);
	if (check_cube_state(step, forward[0].cube, backward[0].cube)) {
		free_cube_list(forward);
		free_cube_list(backward);
		return (merge_paths(forward, forward, backward, backward));
	}
	while (true) {
		if ((ret = parkour_list(step, &forward, &last_forward, backward, false)) != NULL) {
			free_cube_list(forward);
			free_cube_list(backward);
			return (ret);
		}
		if ((ret = parkour_list(step, &backward, &last_backward, forward, true)) != NULL) {
			free_cube_list(forward);
			free_cube_list(backward);
			return (ret);
		}

	}
}

char	*append_moves(char *str, int_fast8_t *moves) {
	int		i = 0;
	int		ret_asprintf;
	char	*ret;

	while (moves[i] != -1) {
		if (str = NULL)
			ret_asprintf = asprintf(&ret, "%s", move_to_str(moves[i]));
		else {
			ret_asprintf = asprintf(&ret, "%s, %s", str, move_to_str(moves[i]));
			free(str);
		}
		if (ret_asprintf == -1)
			return (NULL);
		str = ret;
		++i;
	}
	return (ret);
}

char	*solve(t_cube *cube) {
	int 			step = 0;
	int_fast8_t		*moves;
	char			*ret = NULL;

	while (step < 4) {
		moves = thistlewaite_step(step, cube);
		if (moves == NULL)
			return (NULL);
		ret = append_moves(ret, moves);
		if (ret == NULL)
			return (NULL);
		++step;
	}
	return (ret);
}
