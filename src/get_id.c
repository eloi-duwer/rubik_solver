/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_id.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/01 19:10:29 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/01 19:10:40 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

/**
 * Corners at even positions have the same orbit, same for odd posisions 
 * Same orbit = you can use only half turn to move to any point of the orbit
 * Actually phase 3 thistlewaite is more complicated than that, so we restrain it
 * a little bit more, by considering only "half orbits". Each orbit is divided by 2, on
 * by face up and down. cornes 0 and 2 have the same half orbit, aswell as 4 and 6, etc
 */
static uint_fast16_t	half_orbit_state(t_cube *cube)
{
	uint_fast16_t	ret = 0;
	int				i = 0;

	while (i < 8)
	{
		ret |= (cube->corner_pos[i] % 2 + ((cube->corner_pos[i] / 4) * 2)) << (i * 2);
		++i;
	}
	return (ret);
}

/**
 * Counts the parity of the number of swaps needed to solve the corners
 * On a solved cube this number is 0 (all corners are on a ascending order)
 * And solving using only half turns can't change this parity
 */
static uint_fast8_t	corner_parity(t_cube *cube)
{
	int 			i = 0;
	int 			j;
	uint_fast8_t	ret = 0;

	while (i < 8)
	{
		j = i + 1;
		while (j < 8)
		{
			ret ^= cube->corner_pos[i] > cube->corner_pos[j];
			++j;
		}
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
static uint_fast32_t	slice_state(t_cube *cube)
{
	uint_fast32_t	ret = 0;
	int				i = 0;
	char			slice;

	while (i < 12)
	{
		slice = cube->edge_pos[i] / 4;
		ret |= (slice & 0b1) << 2 * i;
		ret |= (slice & 0b10) << 2 * i; 
		++i;
	}
	return (ret);
}

//same as slice_state, but just for the M layer (used in the second state of the solving)
static uint_fast16_t	m_slice_state(t_cube *cube)
{
	uint_fast16_t	ret = 0;
	int				i = 0;

	while (i < 12)
	{
		ret |= (cube->edge_pos[i] / 4 > 0) << i;
		++i;
	}
	return (ret);
}

uint_fast8_t	*get_cube_id(int step, t_cube *cube, uint_fast8_t *ret)
{

	if (ret == NULL)
		return (NULL);
	memset(ret, 0, sizeof(uint_fast8_t) * 21);
	if (step == 0)
	{
		ret[0] = 13 * sizeof(uint_fast8_t);
		memcpy(&ret[1], cube->edge_orientation, sizeof(uint_fast8_t) * 12);
	}
	else if (step == 1)
	{
		ret[0] = 9 * sizeof(uint_fast8_t) + sizeof(uint_fast16_t);
		memcpy(&ret[1], cube->corner_orientation, sizeof(uint_fast8_t) * 8);
		uint_fast16_t	slice = m_slice_state(cube);
		memcpy(&ret[9], &slice, sizeof(uint_fast16_t));
	}
	else if (step == 2)
	{
		ret[0] = 2 * sizeof(uint_fast8_t) + sizeof(uint_fast32_t) + sizeof(uint_fast16_t);
		ret[1] = corner_parity(cube);
		uint_fast32_t slice = slice_state(cube);
		memcpy(&ret[2], &slice, sizeof(uint_fast32_t));
		uint_fast16_t half_orbit = half_orbit_state(cube);
		memcpy(&ret[2 + sizeof(uint_fast32_t)], &half_orbit, sizeof(uint_fast16_t));
	}
	else
	{	/** We don't care about the orientations, at this step they are all at 0,
		* and it reduces tremendously the hashes collisions (Many multlipications by 0)
		*/
		ret[0] = 21 * sizeof(uint_fast8_t);
		memcpy(&ret[1], cube, sizeof(uint_fast8_t) * 20);
	}
	return (ret);
}
