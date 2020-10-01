/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conv.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/14 00:13:16 by eduwer            #+#    #+#             */
/*   Updated: 2020/10/01 18:12:07 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <rubik.h>

int_fast8_t	str_to_move(char *str)
{
	size_t len = strlen(str);

	if (len == 1) //Quarter turn: UDLRFB
	{
		if (str[0] == 'U')
			return (M_U);
		else if (str[0] == 'D')
			return (M_D);
		else if (str[0] == 'L')
			return (M_L);
		else if (str[0] == 'R')
			return (M_R);
		else if (str[0] == 'F')
			return (M_F);
		else if (str[0] == 'B')
			return (M_B);
	}
	else if (len == 2)
	{
		if (str[1] == '2') //Double turn
		{ 
			if (str[0] == 'U')
				return (M_U2);
			else if (str[0] == 'D')
				return (M_D2);
			else if (str[0] == 'L')
				return (M_L2);
			else if (str[0] == 'R')
				return (M_R2);
			else if (str[0] == 'F')
				return (M_F2);
			else if (str[0] == 'B')
				return (M_B2);
		}
		else if (str[1] == '\'') //Reverse turn
		{ 
			if (str[0] == 'U')
				return (M_UP);
			else if (str[0] == 'D')
				return (M_DP);
			else if (str[0] == 'L')
				return (M_LP);
			else if (str[0] == 'R')
				return (M_RP);
			else if (str[0] == 'F')
				return (M_FP);
			else if (str[0] == 'B')
				return (M_BP);
		}
	}
	printf("Move %s is unrecognized\n", str);
	return (-1);
}

char	*move_to_str(int_fast8_t move)
{
	switch(move) {
		case M_U:
			return ("U");
		case M_U2:
			return ("U2");
		case M_UP:
			return ("U'");
		case M_D:
			return ("D");
		case M_D2:
			return ("D2");
		case M_DP:
			return ("D'");
		case M_L:
			return ("L");
		case M_L2:
			return ("L2");
		case M_LP:
			return ("L'");
		case M_R:
			return ("R");
		case M_R2:
			return ("R2");
		case M_RP:
			return ("R'");
		case M_F:
			return ("F");
		case M_F2:
			return ("F2");
		case M_FP:
			return ("F'");
		case M_B:
			return ("B");
		case M_B2:
			return ("B2");
		case M_BP:
			return ("B'");
		default:
			return (NULL);
	}
}

/**
 * Gets the inverse move: U to U', U2 to U2...
 */
int_fast8_t inverse_move(int_fast8_t move)
{
	return (move + 2 - 2 * (move % 3));
}
