/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:02 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/14 00:30:34 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

/**
 * Represents the pieces affected and order when doing a cw rotation on a face
 * for each face, the 4 first numbers are the corner,
 * and the 4 last are the edges
 * Explanation for Down move for exxample:
 * Corner 4 goes to 7, 7 goes to 6, 6 goes to 5, 5 goes to 4. Same for the edges
 * Corner order: UFL, UBL, UBR, UFR, DFL, DBL, DBR, DFR
 * Edge order: UF, UL, UB, UR, FL, BL, BR, FR, DF, DL, DB, DR
 */
const uint8_t cube_moves[6][8] = {
	{0, 1, 2, 3, 0, 1, 2, 3}, //UP
	{4, 7, 6, 5, 8, 11, 10, 9}, //DOWN
	{0, 4, 5, 1, 1, 4, 9, 5}, //LEFT
	{2, 6, 7, 3, 3, 6, 11, 7}, //RIGHT
	{0, 3, 7, 4, 0, 7, 8, 4}, //FRONT
	{1, 5, 6, 2, 2, 5, 10, 6} //BACK
};

uint8_t get_next_pos(int move, int step) {
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
	char 	**strs = ft_strsplit(str, ' ');
	t_cube	*ret = create_base_cube();
	int		i = 0;
	int		rotation;

	while (strs[i]) {
		rotation = str_to_move(strs[i]);
		if (rotation == -1)
			return (NULL);
		ret = rotation_cube(ret,  rotation, true);
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
t_cube *rotation_cube(t_cube *cube, int move, bool free_old_cube) {
	t_cube			*ret = duplicate_cube(cube);
	const uint8_t	*cubes = cube_moves[move / 3];
	uint8_t			next_pos;
	int				i = 0;

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
		//(move >= 6 && move % 3 != 1) * (1 + (i % 2)): Same stuff than the one for edge orientation, but more complicated
		//UP or DOWN don't change the orientation, Double neither. So the part before the * evaluates to 0 if the corner orientation doesn't change
		//The part after evaluates to 1 or 2 in function of the number of the step, 2 out of 4 cubes are rotated ccw, and the other cw
		ret->corner_orientation[cubes[next_pos]] = \
			(cube->corner_orientation[cubes[i]] \
			+ ((move >= 6 && move % 3 != 1) * (1 + (i % 2)))) % 3;
		ret->edge_pos[cubes[next_pos + 4]] = cube->edge_pos[cubes[i + 4]];
		//(move < 6 && move % 3 != 1): move is UP or DOWN and move is not a half turn (M_U2, M_D2)
		//it adds one to the orientation if it is the case, with % 2 it effectively swaps the orientation if the condition evaluates to true
		ret->edge_orientation[cubes[next_pos + 4]] = \
			(cube->edge_orientation[cubes[i + 4]] \
				+ (move < 6 && move % 3 != 1)) % 2;
		++i;
	}
	if (free_old_cube)
		free(cube);
	return (ret);
}

t_cube *create_base_cube() {
	int 	i = 0;
	t_cube	*ret = (t_cube *)malloc(sizeof(t_cube));

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

void print_cube(t_cube *cube) {
	int	i = 0;

	printf("Cube:\n");
	printf("Corner positions: ");
	while (i < 8) {
		printf("%d", (int)cube->corner_pos[i]);
		if (i < 7)
			printf(", ");
		else
			printf("\n");
		++i;
	}
	i = 0;
	printf("Corner orientations: ");
	while (i < 8) {
		printf("%d", (int)cube->corner_orientation[i]);
		if (i < 7)
			printf(", ");
		else
			printf("\n");
		++i;
	}
	i = 0;
	printf("Edge positions: ");
	while (i < 12) {
		printf("%d", (int)cube->edge_pos[i]);
		if (i < 11)
			printf(", ");
		else
			printf("\n");
		++i;
	}
	i = 0;
	printf("Edge orientations: ");
	while (i < 12) {
		printf("%d", (int)cube->edge_orientation[i]);
		if (i < 11)
			printf(", ");
		else
			printf("\n");
		++i;
	}
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
	print_cube(cube);
	int i = 0;
	while (i < 10000000) {
		cube = rotation_cube(cube, i % 18, true);
		++i;
	}
	print_cube(cube);
	free(cube);

	//Yes it works... Is it a good idea ? Probably not
	//printf("\e[1;34m⬛\e[m\e[0;31m⬛\e[m\e[0;32m⬛\e[m\e[1;33m⬛\e[m\n");

	return (0);
}
