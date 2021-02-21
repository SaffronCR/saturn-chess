// Main program.

// Chess pieces: https://opengameart.org/content/chess-pieces-0

/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/***************************************************************************************************\
** Special Note : The 3D part on Jo Engine is still in development. So, some glitch may occur ;)
**                Btw, texture mapping for triangle base mesh (not quads) is experimental.
\***************************************************************************************************/

#include <jo/jo.h>

#include "micromax.h"

#include "board.h"
#include "king_black.h"
#include "king_white.h"
#include "queen_black.h"
#include "queen_white.h"
#include "rook_black.h"
#include "rook_white.h"
#include "bishop_black.h"
#include "bishop_white.h"
#include "knight_black.h"
#include "knight_white.h"
#include "pawn_black.h"
#include "pawn_white.h"

#define BOARD_INI_POS 385
#define BOARD_SPACE 110

static jo_camera cam;
static bool use_screen_doors = false;
static bool use_light = true;

static int cam_t_x = 0;
static int cam_t_y = 0;
static int cam_t_z = 40;

static int cam_vp_x = 0;
static int cam_vp_y = -1000;
static int cam_vp_z = -666;

int piece_x = 0;
int piece_y = 0;
int piece_z = 0;

char input[4];

jo_vertice cursor_vertices[] = JO_3D_PLANE_VERTICES(55);
jo_3d_quad cursor;

int cursor_tile_h = 5;
int cursor_tile_v = 5;
int cursor_y = -5;

void create_cursor(void)
{
    jo_3d_create_plane(&cursor, cursor_vertices);
    jo_3d_set_texture(&cursor, 0);
}

void draw_cursor(void)
{
    jo_3d_push_matrix();
    {
		jo_3d_translate_matrix(	-BOARD_INI_POS + (cursor_tile_h * BOARD_SPACE),
								cursor_y,
								BOARD_INI_POS - (cursor_tile_v * BOARD_SPACE));
		jo_3d_rotate_matrix_x(90);
        jo_3d_draw(&cursor);
    }
    jo_3d_pop_matrix();
}


void draw_board(void)
{
    jo_3d_push_matrix();
    {
		jo_3d_set_scale(10, 10, 10);
		display_board_mesh();
    }
    jo_3d_pop_matrix();
}

void draw_pieces(void)
{
	piece_z = BOARD_INI_POS;
	for(int i=0; i<8; i++)
	{
		piece_x = -BOARD_INI_POS;
		for(int j=0; j<8; j++)
		{
			if (sym[chess_board[16*i+j]&15] != 0)
			{
				jo_3d_push_matrix();
				jo_3d_translate_matrix(piece_x, piece_y, piece_z);
			}

			switch(sym[chess_board[16*i+j]&15])
			{
				// Black.
				case 'k':
					display_king_black_mesh();
					break;
				case 'q':
					display_queen_black_mesh();
					break;
				case 'r':
					display_rook_black_mesh();
					break;
				case 'b':
					display_bishop_black_mesh();
					break;
				case 'n':
					display_knight_black_mesh();
					break;
				case 'p':
					display_pawn_black_mesh();
					break;
				// White.
				case 'K':
					display_king_white_mesh();
					break;
				case 'Q':
					display_queen_white_mesh();
					break;
				case 'R':
					display_rook_white_mesh();
					break;
				case 'B':
					display_bishop_white_mesh();
					break;
				case 'N':
					display_knight_white_mesh();
					break;
				case 'P':
					display_pawn_white_mesh();
					break;
			}

			if (sym[chess_board[16*i+j]&15] != 0)
			{
				jo_3d_pop_matrix();
			}

			piece_x += BOARD_SPACE;
		}
		piece_z -= BOARD_SPACE;
	}
}

void update_draw(void)
{
	jo_set_printf_color_index(JO_COLOR_INDEX_Yellow);

	if(input[3] == 0)
	{
    	jo_printf(12, 1, "*Saturn Chess*");
	}
	else
	{
		jo_printf(14, 1, "*Thinking*");
	}

 	jo_3d_camera_set_target(&cam, cam_t_x, cam_t_y, cam_t_z);
	jo_3d_camera_set_viewpoint(&cam, cam_vp_x, cam_vp_y, cam_vp_z);
	jo_3d_camera_look_at(&cam);

	draw_board();
	draw_pieces();
	draw_cursor();

	//jo_set_printf_color_index(JO_COLOR_INDEX_Blue);
	//jo_printf(0, 26, "Camera Target:   %d,%d,%d           ", cam_t_x, cam_t_y, cam_t_z);
	//jo_printf(0, 27, "Camera Viewport: %d,%d,%d           ", cam_vp_x, cam_vp_y, cam_vp_z);
	//jo_printf(0, 28, "Board: %d", sym[chess_board[16*1+1]&15]);
	// jo_printf(0, 24, "%c", input[0]);
	// jo_printf(1, 24, "%c", input[1]);
	// jo_printf(0, 25, "%c", input[2]);
	// jo_printf(1, 25, "%c", input[3]);
	//jo_printf(0, 28, "Z: %d", cam_vp_z);
	//jo_printf(0, 0, "%d", jo_3d_get_displayed_polygon_count());

	if(input[3] != 0)
	{
		micromax_input(&input);
		micromax_update();
		memset(input, 0, sizeof input);
	}
}

void set_board_colors(void)
{
	jo_3d_set_mesh_color(&MeshPlane003, JO_COLOR_MediumGreen);
	jo_3d_set_mesh_color(&MeshPlane0032, JO_COLOR_Gray);

	jo_3d_set_mesh_color(&MeshKingWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshKingBlack, JO_COLOR_Black);

	jo_3d_set_mesh_color(&MeshQueenWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshQueenBlack, JO_COLOR_Black);

	jo_3d_set_mesh_color(&MeshRookWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshRookBlack, JO_COLOR_Black);

	jo_3d_set_mesh_color(&MeshBishopWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshBishopBlack, JO_COLOR_Black);

	jo_3d_set_mesh_color(&MeshKnightWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshKnightBlack, JO_COLOR_Black);

	jo_3d_set_mesh_color(&MeshPawnWhite, JO_COLOR_Gray);
	jo_3d_set_mesh_color(&MeshPawnBlack, JO_COLOR_Black);
}

void update_gamepad_camera(void)
{
	if (jo_is_pad1_key_pressed(JO_KEY_R))
	{
		if (jo_is_pad1_key_pressed(JO_KEY_A))
		{
			cam_vp_x--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_X))
		{
			cam_vp_x++;
		}

		if (jo_is_pad1_key_pressed(JO_KEY_B))
		{
			cam_vp_y--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_Y))
		{
			cam_vp_y++;
		}

		if (jo_is_pad1_key_pressed(JO_KEY_C))
		{
			cam_vp_z--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_Z))
		{
			cam_vp_z++;
		}
	}

	if (jo_is_pad1_key_pressed(JO_KEY_L))
	{
		if (jo_is_pad1_key_pressed(JO_KEY_A))
		{
			cam_t_x--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_X))
		{
			cam_t_x++;
		}

		if (jo_is_pad1_key_pressed(JO_KEY_B))
		{
			cam_t_y--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_Y))
		{
			cam_t_y++;
		}

		if (jo_is_pad1_key_pressed(JO_KEY_C))
		{
			cam_t_z--;
		}
		if (jo_is_pad1_key_pressed(JO_KEY_Z))
		{
			cam_t_z++;
		}
	}
}

void update_gamepad_cursor(void)
{
	if (jo_is_pad1_key_down(JO_KEY_UP))
	{
		if (cursor_tile_v > 0)
		{
			cursor_tile_v--;
		}
	}
    else if (jo_is_pad1_key_down(JO_KEY_DOWN))
	{
		if (cursor_tile_v < 7)
		{
			cursor_tile_v++;
		}
	}
    else if (jo_is_pad1_key_down(JO_KEY_LEFT))
	{
		if (cursor_tile_h > 0)
		{
			cursor_tile_h--;
		}
	}
    else if (jo_is_pad1_key_down(JO_KEY_RIGHT))
	{
		if (cursor_tile_h < 7)
		{
			cursor_tile_h++;
		}
	}

	if (jo_is_pad1_key_down(JO_KEY_A))
	{
		if (input[0] == 0)
		{
			input[0] = 'a' + cursor_tile_h;
			input[1] = '8' - cursor_tile_v;
		}
		else if (input[2] == 0)
		{

			input[2] = 'a' + cursor_tile_h;
			input[3] = '8' - cursor_tile_v;
		}
	}
}

void update_gamepad(void)
{
	update_gamepad_cursor();
	//update_gamepad_camera();
}

void jo_main(void)
{
	jo_core_init(JO_COLOR_Black);

	memset(input, 0, sizeof input);
	micromax_init();

	jo_sprite_add_tga(JO_ROOT_DIR, "CURSOR.TGA", JO_COLOR_Transparent);
	create_cursor();

	//jo_3d_light(1, 1, 1);

	jo_3d_perspective_angle(60);
	jo_3d_display_level(5);
    jo_3d_camera_init(&cam);

	set_board_colors();

	jo_core_add_callback(update_gamepad);
	jo_core_add_callback(update_draw);

	jo_core_run();
}
