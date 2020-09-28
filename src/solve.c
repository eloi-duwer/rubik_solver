/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   solve.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:29:36 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/29 00:46:12 by eduwer           ###   ########.fr       */
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

void			error_and_exit(char *str) {
	printf("%s\n", str);
	exit(1);
}

int_fast8_t		*merge_paths(t_cube_list *forward, t_cube_list *backward)
{
	int_fast8_t *ret = (int_fast8_t *)malloc(sizeof(int_fast8_t) * (forward->nb_moves + backward->nb_moves + 1));
	int			i = 0;

	if (ret == NULL)
		error_and_exit("Error during path merging");
	i = 0;
	printf("Moves forward: ");
	while (i < forward->nb_moves) {
		printf("%s, ", move_to_str(forward->moves[i]));
		++i;
	}
	printf("\nMoves backward: ");
	i = 0;
	while (i < backward->nb_moves) {
		printf("%s, ", move_to_str(backward->moves[i]));
		++i;
	}
	printf("\n");
	printf("Matching cubes are: ");
	print_cube(forward->cube);
	print_id(forward->id);
	print_cube(backward->cube);
	print_id(backward->id);
	i = 0;
	memcpy(ret, forward->moves, sizeof(int_fast8_t) * forward->nb_moves);
	while (i < backward->nb_moves) {
		ret[forward->nb_moves + i] = inverse_move(backward->moves[backward->nb_moves - i - 1]);
		++i;
	}
	ret[forward->nb_moves + backward->nb_moves] = -1;
	return (ret);
}

t_cube_list	*init_with_move(int step, t_cube_list *ptr, t_cube *cube, int_fast8_t move) {
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

void	print_list_length(t_cube_list *list, t_cube_list *ptr) {
	int i = 0;
	int ptr_pos = -1;

	while (list != NULL) {
		i++;
		if (list == ptr)
			ptr_pos = i;
		list = list->next;
	}
	printf("List length: %d, ptr is at %d\n", i, ptr_pos);
}

int_fast8_t		*generate_cubes(int step, t_cube_list *ptr, t_cube_list **last, Hashmap *rubik_map) {
	t_cube			*new_cube;
	int_fast8_t 	i = 0;
	t_cube_list		*ret;
	t_cube_list		*new_list;

	while (i < 18) {
		if (allowed_moves[step] & 1 << (17 - i) && \
			(ptr->nb_moves == 0 || ptr->moves[ptr->nb_moves - 1] / 3 != i / 3)) {
			if ((new_cube = rotation_cube(ptr->cube, i, false)) == NULL) {
				printf("Error during turn\n");
				exit(1);
			}
			new_list = init_with_move(step, ptr, new_cube, i);
			//append_cube_to_end(step, ptr, last, new_cube, i);
			ret = (t_cube_list *)hashmapGet(rubik_map, new_list->id);
			if (ret == NULL) { //We didn't found it in the map, so we should add it
				(*last)->next = new_list;
				*last = new_list;
				hashmapPut(rubik_map, (*last)->id, *last);
			}
			else if (ret->is_forward != new_list->is_forward) { //We found a connection between forward and backward
				(*last)->next = new_list;
				*last = new_list;
				if (ret->is_forward)
					return (merge_paths(ret, *last));
				else
					return (merge_paths(*last, ret));
			}
		}
		++i;
	}
	return (NULL);
}

int_fast8_t		*thistlewaite_step(int step, t_cube *cube) {
	t_cube_list		*first;
	t_cube_list		*ptr;
	t_cube_list		*last;
	Hashmap			*rubik_map = hashmapCreate(500000, rubik_hash, rubik_equals);
	int_fast8_t		*ret;

	first = init_cube_list(step, duplicate_cube(cube), NULL, 0, true);
	last = init_cube_list(step, create_base_cube(), NULL, 0, false);
	printf("Starting cube is: ");
	print_cube(first->cube);
	printf("Starting cube id is: ");
	print_cube_id(first->cube, step);
	printf("Goal cube id is:     ");
	print_cube_id(last->cube, step);
	if (first == NULL || last == NULL || rubik_map == NULL)
		return (NULL);
	hashmapPut(rubik_map, first->id, first);
	if (hashmapPut(rubik_map, last->id, last) != NULL) { //The cubes are equivalent for the step
		ret = merge_paths(first, last);
		free_cube_list(first);
		hashmapFree(rubik_map);
		return (ret);
	}
	first->next = last;
	ptr = first;
	while (ptr != NULL) {
		if ((ret = generate_cubes(step, ptr, &last, rubik_map)) != NULL) {
			free_cube_list(first);
			hashmapFree(rubik_map);
			return (ret);
		}
		ptr = ptr->next;
	}
	return (NULL);
}

char	*append_moves(char *str, int_fast8_t *moves) {
	int		i = 0;
	int		ret_asprintf;
	char	*ret = str;

	while (moves[i] != -1) {
		if (str == NULL)
			ret_asprintf = asprintf(&ret, "%s", move_to_str(moves[i]));
		else {
			ret_asprintf = asprintf(&ret, "%s, %s", str, move_to_str(moves[i]));
			free(str);
		}
		if (ret_asprintf == -1)
			error_and_exit("Error during asprintf\n");
		str = ret;
		++i;
	}
	return (ret);
}

char	*solve(t_cube *cube) {
	int 			step = 0;
	int_fast8_t		*moves;
	char			*ret = NULL;
	t_cube			*copy = duplicate_cube(cube);
	int				i;

	if (copy == NULL)
		return (NULL);
	while (step < 4) {
		printf("------------------------------- STEP %d\n", step);
		moves = thistlewaite_step(step, copy);
		if (moves == NULL)
			return (NULL);
		ret = append_moves(ret, moves);
		i = 0;
		while (moves[i] != -1) {
			copy = rotation_cube(copy, moves[i], true);
			++i;
		}
		printf("Resultant cube id is: ");
		print_cube_id(copy, step);
		printf("Current solution is %s\n", ret);
		free(moves);
		++step;
	}
	return (ret);
}
