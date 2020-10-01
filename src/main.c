/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:02 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/01 21:34:38 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

t_cube	*scramble_cube(char *str)
{
	char 			**strs = ft_strsplit(str, ' ');
	t_cube			*ret = create_base_cube();
	int				i = 0;
	int_fast8_t	rotation;

	if (strs == NULL)
		return (NULL);
	while (strs[i])
	{
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

void	print_moves(int_fast8_t *moves, bool verbose)
{
	int i = 0;

	while (moves[i] != -1)
	{
		printf("%s ", move_to_str(moves[i]));
		++i;
	}
	printf("\n");
	if (verbose == true)
		printf("(%d moves)\n", i);
}

void	print_help()
{
	printf("Usage: ./rubik [OPTIONS]... [SCRAMBLE_MOVES]\n\n");
	printf("Solves a 3x3x3 Rubik's cube, scrambled with the string given in argument, or a default scramble if no scramble is given\n\n");
	printf("Options are:\n\n  -v\tVerbose mode\n  -h\tprint this help\n\n");
	printf("Scramble moves must be a string, each moves must be separated by a space\n");
	printf("Available moves are: U, U', U2, D, D', D2, L, L', L2, R, R', R2, F, F', F2, B, B', B2\n\n");
	printf("Example: ./rubik \"U R2 B' L R U2 D' F L R'\"\n\n");
	exit(0);
}

int main(int argc, char **argv)
{
	t_cube 		*cube;
	bool		verbose = false;
	int			opt;
	int_fast8_t	*ret;
	

	while ((opt = getopt(argc, argv, "vh")) != -1)
	{
		switch(opt)
		{
			case 'h':
				print_help();
				break;
			case 'v':
				verbose = true;
				break;
			default:
				print_help();
				break;
		}
	}
	if (optind >= argc)
	{
		char *default_scramble = "D2 U' R2 U F2 D2 U' R2 U' B' L2 R' B' D2 U B2 L' D' R2";
		printf("No scramble is provided, going to solve for default scramble: %s\n", default_scramble);
		cube = scramble_cube(default_scramble);
	}
	else
		cube = scramble_cube(argv[optind]);
	if (cube == NULL) {
		printf("Error while scrambling the cube\n");
		return (-1);
	}
	ret = solve(cube, verbose);
	if (ret != NULL)
		print_moves(ret, verbose);
	else
		printf("Error while solving the cube\n");
	free(ret);
	free(cube);
	return (0);
}
