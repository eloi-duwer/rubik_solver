/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:29:36 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/01 18:32:48 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>
#include <hashmap.h>

int allowed_moves[4] = {
	0b111111111111111111, //Allow every moves
	0b010010111111111111, //Only disallow U, U', D, D'
	0b010010111111010010, //Only allow half turns except for L and R faces
	0b010010010010010010 //Only allow half turns
};

void			error_and_exit(char *str)
{
	printf("%s\n", str);
	exit(1);
}

int_fast8_t		*merge_paths(t_cube_list *forward, t_cube_list *backward)
{
	int_fast8_t *ret = (int_fast8_t *)malloc(sizeof(int_fast8_t) * (forward->nb_moves + backward->nb_moves + 1));
	int			i = 0;

	if (ret == NULL)
		error_and_exit("Error during path merging");
	memcpy(ret, forward->moves, sizeof(int_fast8_t) * forward->nb_moves);
	while (i < backward->nb_moves)
	{
		ret[forward->nb_moves + i] = inverse_move(backward->moves[backward->nb_moves - i - 1]);
		++i;
	}
	ret[forward->nb_moves + backward->nb_moves] = -1;
	return (ret);
}

t_cube_list	*init_with_move(int step, t_cube_list *ptr, t_cube *cube, int_fast8_t move)
{
	t_cube_list *newCube = init_cube_list(step, cube, NULL, 0, ptr->is_forward);

	if (newCube == NULL)
		error_and_exit("Error during cube state initialization");
	newCube->nb_moves = ptr->nb_moves + 1;
	newCube->moves = (int_fast8_t *)malloc(sizeof(int_fast8_t) * newCube->nb_moves);
	if (newCube->moves == NULL)
		error_and_exit("Malloc error during move initialization");
	if (ptr->nb_moves != 0)
		memcpy(newCube->moves, ptr->moves, sizeof(int_fast8_t) * ptr->nb_moves);
	newCube->moves[newCube->nb_moves - 1] = move;
	return (newCube);
}

int_fast8_t		*generate_cubes(int step, t_cube_list *ptr, t_cube_list **last, Hashmap *rubik_map)
{
	t_cube			*new_cube;
	int_fast8_t 	i = 0;
	t_cube_list		*ret;
	t_cube_list		*new_list;

	while (i < 18)
	{
		if (allowed_moves[step] & 1 << (17 - i) && \
			(ptr->nb_moves == 0 || ptr->moves[ptr->nb_moves - 1] / 3 != i / 3))
		{
			if ((new_cube = rotation_cube(ptr->cube, i, false)) == NULL)
			{
				printf("Error during turn\n");
				exit(1);
			}
			new_list = init_with_move(step, ptr, new_cube, i);
			ret = (t_cube_list *)hashmapGet(rubik_map, new_list->id);
			if (ret == NULL) //We didn't found it in the map, so we should add it
			{ 
				(*last)->next = new_list;
				*last = new_list;
				hashmapPut(rubik_map, (*last)->id, *last);
			}
			else if (ret->is_forward != new_list->is_forward) //We found a connection between forward and backward
			{
				(*last)->next = new_list;
				*last = new_list;
				if (ret->is_forward)
					return (merge_paths(ret, *last));
				else
					return (merge_paths(*last, ret));
			}
			else //We already found this cube for this direction, free it
				free_one_cube_list(new_list);
		}
		++i;
	}
	return (NULL);
}

int_fast8_t		*thistlewaite_step(int step, t_cube *cube)
{
	t_cube_list		*first;
	t_cube_list		*ptr;
	t_cube_list		*last;
	Hashmap			*rubik_map = hashmapCreate(500000, rubik_hash, rubik_equals);
	int_fast8_t		*ret;

	first = init_cube_list(step, duplicate_cube(cube), NULL, 0, true);
	last = init_cube_list(step, create_base_cube(), NULL, 0, false);
	if (first == NULL || last == NULL || rubik_map == NULL)
		return (NULL);
	first->next = last;
	hashmapPut(rubik_map, first->id, first);
	if (hashmapPut(rubik_map, last->id, last) != NULL)
	{ //The cubes are equivalent for the step
		ret = merge_paths(first, last);
		free_cube_list(first);
		hashmapFree(rubik_map);
		return (ret);
	}
	ptr = first;
	while (ptr != NULL)
	{
		if ((ret = generate_cubes(step, ptr, &last, rubik_map)) != NULL)
		{
			free_cube_list(first);
			hashmapFree(rubik_map);
			return (ret);
		}
		ptr = ptr->next;
	}
	printf("Could not find any solution to the step %d\n", step);
	return (NULL);
}

char	*append_moves(char *str, int_fast8_t *moves, bool verbose)
{
	int		i = 0;
	int		ret_asprintf;
	char	*ret = str;
	char	*move;

	if (verbose == true)
		printf("Moves for this step: ");
	while (moves[i] != -1)
	{
		move = move_to_str(moves[i]);
		if (verbose == true) {
			if (i == 0)
				printf("%s", move);
			else
				printf(", %s", move);
		}
		if (str == NULL)
			ret_asprintf = asprintf(&ret, "%s", move);
		else
			ret_asprintf = asprintf(&ret, "%s, %s", str, move);
		if (ret_asprintf == -1)
			error_and_exit("Error during asprintf\n");
		free(str);
		str = ret;
		++i;
	}
	if (verbose == true)
		printf("\n");
	return (ret);
}

char	*solve(t_cube *cube, bool verbose)
{
	int 			step = 0;
	int_fast8_t		*moves;
	char			*ret = NULL;
	t_cube			*copy = duplicate_cube(cube);
	int				i;
	int				nb_moves = 0;

	if (copy == NULL)
		return (NULL);
	while (step < 4)
	{
		if (verbose == true)
		{
			printf("Step %d: ", step + 1);
			if (step == 0)
				printf("Solving edges orientations\n");
			else if (step == 1)
				printf("Solving corners orientations + M slice edges\n");
			else if (step == 2)
				printf("Solving corners orbit + all edges slices + corner parity\n");
			else
				printf("Final step, solving the whole cube\n");
		}
		moves = thistlewaite_step(step, copy);
		if (moves == NULL)
			return (NULL);
		ret = append_moves(ret, moves, verbose);
		i = 0;
		while (moves[i] != -1)
		{
			copy = rotation_cube(copy, moves[i], true);
			++nb_moves;
			++i;
		}
		free(moves);
		++step;
	}
	if (copy != NULL)
		free(copy);
	if (verbose == true) 
		printf("Number of moves: %d\n", nb_moves);
	return (ret);
}
