/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:29:36 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/09 15:45:22 by eduwer           ###   ########.fr       */
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

static int_fast8_t		*merge_paths(t_cube_list *forward, t_cube_list *backward)
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

static t_cube_list	*init_with_move(int step, t_cube_list *ptr, t_cube *cube, int_fast8_t move)
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

static int_fast8_t		*generate_cubes(int step, t_cube_list *ptr, t_cube_list **last, Hashmap *rubik_map)
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

static int_fast8_t		*thistlewaite_step(int step, t_cube *cube)
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

static int_fast8_t	*append_moves(int_fast8_t *old_moves, int_fast8_t *new_moves, bool verbose)
{
	int_fast8_t	*ret;
	int i = 0;
	int	j = 0;

	while (old_moves != NULL && old_moves[i] != -1)
		++i;

	if (verbose == true)
		printf("Moves for this step: ");
	while (new_moves != NULL && new_moves[j] != -1)
	{
		if (verbose == true)
			printf("%s ", move_to_str(new_moves[j]));
		++j;
	}
	if (verbose == true)
		printf("\n");
	ret = (int_fast8_t *)malloc(sizeof(int_fast8_t) * (i + j + 1));
	if (ret == NULL)
		error_and_exit("Error during move concatenation\n");
	memcpy(ret, old_moves, sizeof(int_fast8_t) * i);
	memcpy(&ret[i], new_moves, sizeof(int_fast8_t) * j);
	ret[i + j] = -1;
	free(old_moves);
	return (ret);
}

static int_fast8_t	*merge_moves(int_fast8_t *moves, int i, int j)
{
	int				delta;
	int_fast8_t		*ret;
	int				size_moves = 0;
	int				pt = 0;

	while (moves[size_moves] != -1)
		++size_moves;
	delta = ((moves[i] % 3) + (moves[j] % 3) + 2) % 4; //Number of quarter turns from a no rotation: 0 = this 2 moves cancels out, 1 = we can replace the two moves by a quarter turn cw...
	ret = (int_fast8_t *)malloc(sizeof(int_fast8_t) * size_moves - (delta == 0));
	if (ret == NULL)
		return (NULL);
	memcpy(ret, moves, sizeof(int_fast8_t) * i);
	pt = i;
	if (delta > 0)
	{
		ret[i] = moves[i] - (moves[i] % 3) + (delta - 1);
		++pt;
	}
	++i;
	while (i <= j - 1)
	{
		ret[pt] = moves[i];
		++i;
		++pt;
	}
	++j;
	while (moves[j] != -1)
	{
		ret[pt] = moves[j];
		++j;
		++pt;
	}
	ret[pt] = -1;
	free(moves);
	return (ret);
}

/**
 * Find and patch moves redundancies:
 * Between steps, you can have the some move repetition, eg U U2 or R L R' (L does not move any cubes in R, so it's like R R' L)
 * That can be simplified: U and U2 merges into U' for example
 */
static int_fast8_t	*optimize_moves(int_fast8_t *moves)
{
	int	i = 0;
	int_fast8_t	*ret;

	while (moves && moves[i] != -1)
	{
		if (i >= 1 && moves[i] / 3 == moves[i - 1] / 3) //The moves at index i - 1 and i moves the same face
		{
			ret = merge_moves(moves, i - 1, i);
			if (ret == NULL)
				return (moves);
			else
				return (optimize_moves(ret));
		}
		else if (i >= 2 && (moves[i] / 6 == moves[i - 1] / 6 && moves[i] / 3 == moves[i - 2] / 3)) //The moves at index i - 2 and i moves the same face, and the move at index i - 1 does not interfere with this face
		{
			ret = merge_moves(moves, i - 2, i);
			if (ret == NULL)
				return (moves);
			else
				return (optimize_moves(ret));
		}
		++i;
	}
	return (moves);
}

int_fast8_t *solve(t_cube *cube, bool verbose)
{
	int 			step = 0;
	int_fast8_t		*moves;
	int_fast8_t		*all_moves = NULL;
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
		i = 0;
		while (moves[i] != -1)
		{
			copy = rotation_cube(copy, moves[i], true);
			++nb_moves;
			++i;
		}
		all_moves = append_moves(all_moves, moves, verbose);
		free(moves);
		++step;
	}
	free(copy);
	return (optimize_moves(all_moves));
}
