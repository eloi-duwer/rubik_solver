/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/24 19:22:39 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/01 19:16:17 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

int	rubik_hash(void *key)
{
	int 			i = 0;
	int 			ret = 0;
	unsigned char	*pt = (unsigned char *)key;

	while (i < pt[0])
	{
		ret += pt[i];
		ret *= 12;
		++i;
	}
	return (ret);
}

bool rubik_equals(void *a, void *b)
{
	return (memcmp(a, b, sizeof(uint_fast8_t) * 21) == 0);
}

t_cube	*create_base_cube()
{
	uint_fast8_t	i = 0;
	t_cube			*ret = (t_cube *)malloc(sizeof(t_cube));

	if (ret == NULL)
		return (NULL);
	while (i < 12)
	{
		if (i < 8)
		{
			ret->corner_pos[i] = i;
			ret->corner_orientation[i] = 0;
		}
		ret->edge_pos[i] = i;
		ret->edge_orientation[i] = 0;
		++i;
	}
	return (ret);
}

t_cube	*duplicate_cube(const t_cube *in)
{
	t_cube	*ret = (t_cube *)malloc(sizeof(t_cube));

	if (ret == NULL)
		return (NULL);
	memcpy(ret, in, sizeof(t_cube));
	return (ret);
}

t_cube_list	*init_cube_list(int step, t_cube *cube, int_fast8_t *moves, int nb_moves, bool is_forward)
{
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

void			free_one_cube_list(t_cube_list *one_cube) {
	free(one_cube->cube);
	free(one_cube->moves);
	free(one_cube);
}

void			free_cube_list(t_cube_list *list) {
	t_cube_list *next;
	
	while (list != NULL)
	{
		next = list->next;
		free_one_cube_list(list);
		list = next;
	}
}
