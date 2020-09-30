/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:02 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/30 20:10:25 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

/**
 * Represents the pieces affected and order when doing a cw rotation on a face
 * for each face, the 4 first numbers are the corner,
 * and the 4 last are the edges
 * the first on the line goes to the second, second to third... the last one
 * loops back to the beginning
 */
const int_fast8_t cube_moves[6][8] = {
	{P_CUBL, P_CUBR, P_CUFR, P_CUFL, P_EUF, P_EUL, P_EUB, P_EUR}, //UP
	{P_CDFL, P_CDFR, P_CDBR, P_CDBL, P_EDF, P_EDR, P_EDB, P_EDL}, //DOWN
	{P_CUFL, P_CDFL, P_CDBL, P_CUBL, P_EUL, P_EFL, P_EDL, P_EBL}, //LEFT
	{P_CUBR, P_CDBR, P_CDFR, P_CUFR, P_EUR, P_EBR, P_EDR, P_EFR}, //RIGHT
	{P_CUFL, P_CUFR, P_CDFR, P_CDFL, P_EUF, P_EFR, P_EDF, P_EFL}, //FRONT
	{P_CDBL, P_CDBR, P_CUBR, P_CUBL, P_EUB, P_EBL, P_EDB, P_EBR}  //BACK
};

uint_fast8_t get_next_pos(int_fast8_t move, int step) {
	return ((step + (move % 3) + 1) % 4);
}

t_cube	*duplicate_cube(const t_cube *in) {
	t_cube	*ret = (t_cube *)malloc(sizeof(t_cube));

	if (ret == NULL)
		return (NULL);
	memcpy(ret, in, sizeof(t_cube));
	return (ret);
}

t_cube	*scramble_cube(char *str)
{
	char 			**strs = ft_strsplit(str, ' ');
	t_cube			*ret = create_base_cube();
	int				i = 0;
	int_fast8_t	rotation;

	if (strs == NULL)
		return (NULL);
	while (strs[i]) {
		rotation = str_to_move(strs[i]);
		if (rotation == -1)
			return (NULL);
		ret = rotation_cube(ret, rotation, true);
		if (ret == NULL)
			return (NULL);
		free(strs[i]);
		++i;
	}
	free(strs);
	return (ret);
}

/**
 * Creates and returns a new cube state in function of the rotation
 * Move numbers are defined in main.h
 * It uses the array cube_moves, to find which cubes to move and where do they go
 * The new cube is malloced, and the old one is not modified.
 * It is freed is free_old_cube is set to true (do not set it to true if it is not malloced!)
 */
t_cube *rotation_cube(t_cube *cube, int_fast8_t move, bool free_old_cube) {
	t_cube				*ret = duplicate_cube(cube);
	const int_fast8_t	*cubes = cube_moves[move / 3];
	uint_fast8_t		next_pos;
	int					i = 0;

	if (ret == NULL) {
		printf("Error during rotation: can't allocate the cube\n");
		return (NULL);
	}
	while (i < 4) {
		/**
		* (i + (move % 3) + 1) % 4: gets where the next cube will be
		* in function of the move.
		* cube_moves describe where the cube has to go in case a cw move,
		* whe have to adapt it in case of a double or an inverse move
		* If the move is a half turn (U2, B2...), move % 3 is going to be 1,
		* if the move is a reverse (U', B'...), move % 3 is going to be 2
		* It effectively gets the good index: instead of getting 1, 2, 3, 0
		* For a half turn it will be 2, 1, 3, 0 and reverse 3, 2, 1, 0
		*/
		next_pos = (i + (move % 3) + 1) % 4;
		ret->corner_pos[cubes[next_pos]] = cube->corner_pos[cubes[i]];
		//((move < M_L || move > M_RP) && move % 3 != 1) * (1 + (i % 2)): Same stuff than the one for edge orientation, but more complicated
		//LEFT or RIGHT don't change the orientation, half turn neither. So the part before the * evaluates to 0 if the corner orientation doesn't change
		//The part after evaluates to 1 or 2 in function of the number of the step, 2 out of 4 corners are rotated ccw, and the other cw
		ret->corner_orientation[cubes[next_pos]] = (cube->corner_orientation[cubes[i]] \
			//Move is not L or R AND move is not half turn     (1 + (i % 2)) => 2 / 4 corners are rotated ccw, the other cw
			+ (((move < M_L || move > M_RP) && move % 3 != 1) * (1 + (i % 2))) \
			//L and R swaps the corner orientation each turn for orientation 1 and 2, and does nothing if the orientation is 0
			/*+ ((move >= M_L && move <= M_RP && move % 3 != 1) * cube->corner_orientation[cubes[i]])*/) \
			% 3;
		ret->edge_pos[cubes[next_pos + 4]] = cube->edge_pos[cubes[i + 4]];
		//(move < M_L && move % 3 != 1): move is UP or DOWN and move is not a half turn (M_U2, M_D2)
		//it adds one to the orientation if it is the case, with % 2 it effectively swaps the orientation if the condition evaluates to true
		ret->edge_orientation[cubes[next_pos + 4]] = \
			(cube->edge_orientation[cubes[i + 4]] \
				+ (move < M_L && move % 3 != 1)) % 2;
		++i;
	}
	if (free_old_cube)
		free(cube);
	return (ret);
}

t_cube *create_base_cube() {
	uint_fast8_t	i = 0;
	t_cube			*ret = (t_cube *)malloc(sizeof(t_cube));

	if (ret == NULL)
		return (NULL);
	while (i < 12) {
		if (i < 8) {
			ret->corner_pos[i] = i;
			ret->corner_orientation[i] = 0;
		}
		ret->edge_pos[i] = i;
		ret->edge_orientation[i] = 0;
		++i;
	}
	return (ret);
}

int main(int argc, char **argv) {
	t_cube *cube;

	if (argc == 1) {
		char *default_scramble = "D2 U' R2 U F2 D2 U' R2 U' B' L2 R' B' D2 U B2 L' D' R2";
		printf("No scramble is provided, going to solve for default scramble: %s\n", default_scramble);
		cube = scramble_cube(default_scramble);
	}
	else
		cube = scramble_cube(argv[1]);
	if (cube == NULL) {
		printf("Error while scrambling the cube\n");
		return (-1);
	}
	
	char *ret = solve(cube);
	if (ret != NULL)
		printf("%s\n", ret);
	else
		printf("Error while solving the cube\n");
	free(ret);
	free(cube);
	return (0);
}
