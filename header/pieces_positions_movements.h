/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pieces_positions_movements.h                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eduwer <eduwer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/06 16:46:02 by eduwer            #+#    #+#             */
/*   Updated: 2020/09/16 23:58:04 by eduwer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PIECES_POSITIONS_H__
# define __PIECES_POSITIONS_H__

/**
 * Defines all the movements and positions possible of the cubies,
 * To simplify the rest of the code
 * (and not having random numbers all over the place)
 * U = UP
 * D = DOWN
 * L = LEFT
 * R = RIGHT
 * F = FRONT
 * B = BACK
 * cw = clockwise
 * ccw = counterclockwise
 */

//Possibe movemements for the cube:
//for each face respectively: 1/4 turn cw, 1/2 turn, 1/4 turn ccw
# define M_U 0
# define M_U2 1
# define M_UP 2

# define M_D 3
# define M_D2 4
# define M_DP 5

# define M_L 6
# define M_L2 7
# define M_LP 8

# define M_R 9
# define M_R2 10
# define M_RP 11

# define M_F 12
# define M_F2 13
# define M_FP 14

# define M_B 15
# define M_B2 16
# define M_BP 17

//Corner positions
# define P_CUFL 0
# define P_CUBL 1
# define P_CUBR 2
# define P_CUFR 3
# define P_CDFR 4
# define P_CDFL 5
# define P_CDBL 6
# define P_CDBR 7

//Edge positions: Middle layer (M)
# define P_EUF 0
# define P_EUB 1
# define P_EDB 2
# define P_EDF 3

//Standing layer (S)
# define P_EUL 4
# define P_EUR 5
# define P_EDR 6
# define P_EDL 7

//Equatorial layer (E)
# define P_EFL 8
# define P_EBL 9
# define P_EBR 10
# define P_EFR 11

#endif