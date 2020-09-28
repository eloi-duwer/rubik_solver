/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/24 19:22:39 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/29 00:11:06 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

int	rubik_hash(void *key) {
	int 			i = 0;
	int 			ret = 0;
	unsigned char	*pt = (unsigned char *)key;

	while (i < pt[0]) {
		ret += pt[i];
		ret *= 12;
		++i;
	}
	return (ret);
}

bool rubik_equals(void *a, void *b) {
	return (memcmp(a, b, sizeof(uint_fast8_t) * 41) == 0);
}

t_cube_list	*init_cube_list(int step, t_cube *cube, int_fast8_t *moves, int nb_moves, bool is_forward) {
	t_cube_list	*ret;

	if (cube == NULL)
		return (NULL);
	if ((ret = (t_cube_list *)malloc(sizeof(t_cube_list) * 1)) == NULL)
		return (NULL);
	ret->cube = cube;
	ret->moves = moves;
	ret->next = NULL;
	ret->nb_moves = nb_moves;
	ret->is_forward = is_forward;
	get_cube_id(step, cube, ret->id);
	return (ret);
}

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
uint_fast32_t	slice_state(t_cube *cube) {
	uint_fast32_t	ret = 0;
	int				i = 0;
	char			slice;

	while (i < 12) {
		slice = cube->edge_pos[i] / 4;
		ret |= (slice & 0b1) << 2 * i;
		ret |= (slice & 0b10) << 2 * i; 
		++i;
	}
	return (ret);
}

//same as slice_state, but just for the M layer (used in the second state of the solving)
uint_fast16_t	m_slice_state(t_cube *cube) {
	uint_fast16_t	ret = 0;
	int				i = 0;

	while (i < 12) {
		ret |= (cube->edge_pos[i] / 4 > 0) << i;
		++i;
	}
	return (ret);
}

uint_fast8_t	*get_cube_id(int step, t_cube *cube, uint_fast8_t *ret) {

	if (ret == NULL)
		return (NULL);
	memset(ret, 0, sizeof(uint_fast8_t) * 41);
	if (step == 0) {
		ret[0] = 13 * sizeof(uint_fast8_t);
		memcpy(&ret[1], cube->edge_orientation, sizeof(uint_fast8_t) * 12);
	} else if (step == 1) {
		ret[0] = 9 * sizeof(uint_fast8_t) + sizeof(uint_fast16_t);
		memcpy(&ret[1], cube->corner_orientation, sizeof(uint_fast8_t) * 8);
		uint_fast16_t	slice = m_slice_state(cube);
		memcpy(&ret[9], &slice, sizeof(uint_fast16_t));
	} else if (step == 2) {
		ret[0] = 2 * sizeof(uint_fast8_t) + sizeof(uint_fast32_t);
		ret[1] = orbit_state(cube);
		uint_fast32_t slice = slice_state(cube);
		memcpy(&ret[2], &slice, sizeof(uint_fast32_t));
	} else {
		ret[0] = 41 * sizeof(uint_fast8_t);
		memcpy(&ret[1], cube, sizeof(uint_fast8_t) * 40);
	}
	return (ret);
}

void			free_one_cube_list(t_cube_list *one_cube) {
	free(one_cube->cube);
	free(one_cube->moves);
	free(one_cube);
}

void			free_cube_list(t_cube_list *list) {
	t_cube_list *next;
	
	while (list != NULL) {
		next = list->next;
		free_one_cube_list(list);
		list = next;
	}
}
