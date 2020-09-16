/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:29:36 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/16 23:44:30 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

int allowed_moves[4] = {
	0b111111111111111111, //Allow every moves
	0b010010111111111111, //Only disallow U, U', D, D'
	0b010010111111010010, //Only allow half turns except for L and R faces
	0b010010010010010010 //Only allow half turns
};

int orbits[2][4] = {
	{0, 2, 5, 7},
	{1, 3, 4, 6}
};

int slice[3][4] = {
	{0, 2, 8, 10},
	{1, 3, 9, 11},
	{4, 5, 6, 7}
};

// The edges of the M slice must be on the slice (so edges 0, 2, 8, 10 must be in the position 0, 2, 8 or 10)
bool step2_edge_pos(t_cube *cube, int nb) {
	return (cube->edge_pos[nb] == 0
		|| cube->edge_pos[nb] == 2
		|| cube->edge_pos[nb] == 8
		|| cube->edge_pos[nb] == 10
	);
}

bool	check_cube_state(int step, t_cube *base, t_cube *cube) {
	if (step == 0) {
		//Only check edge orientation
		return (memcmp(base->edge_orientation, cube->edge_orientation, sizeof(uint8_t) * 12) == 0);
	} else if (step == 1) {
		//Only check corner orientation & LR-edges (M slice, between left and right)
		return (memcmp(base->corner_orientation, cube->corner_orientation, sizeof(uint8_t) * 8) == 0 \
			&& step2_edge_pos(cube, 0) && step2_edge_pos(cube, 2) \
			&& step2_edge_pos(cube, 8) && step2_edge_pos(cube, 10)
		);
	} else if (step == 2) {
		return (false);
	} else {
		return (memcmp(base, cube, sizeof(t_cube)));
	}
}

char	*solve(t_cube *cube) {
	int step = 0;
	return ("Hmmm... it's harder than it seems!!!");
}
