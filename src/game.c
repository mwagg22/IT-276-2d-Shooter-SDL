#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "simple_logger.h"
#include "g_gungirl.h"
#include "g_camera.h"
#include "g_level.h"
#include "g_collision.h"
#include "g_shooterenemy.h"
#include "g_rollerenemy.h"
#include "g_fireman.h"
#include "g_lookerenemy.h"
#include "g_hazards.h"
#include "g_menu.h"
#include "g_game.h"
#include "simple_json_object.h"
//change based on states
game_controller *game;

void init_game(){
	game->menu=init_menu_ent();
	game->data = (game_data*)malloc(sizeof(game_data));
	game->data->bossdata = (int*)gfc_allocate_array(sizeof(int), 4);
	game->data->weapondata = (int*)gfc_allocate_array(sizeof(int), 4);
	memset(game->data->bossdata, 0, sizeof(int)* 4);
	memset(game->data->weapondata, 0, sizeof(int)* 4);
	game->cam=init_camera(NULL, 256, 240, vector2d(0, 0), vector2d(256, 240));
	game->PreviousState = G_Menu;
	game->CurrentState = G_Menu;
	game->controllerEntity = game->menu->cursor;
	game->transition = false;
	save_game();
}

void set_game_state(game_state state,int level){
	game->PreviousState = game->CurrentState;
	game->transition = true;
	gf2d_clear_entity_manager();
	switch (state){
	case(G_Menu) : {
					   game->menu = init_menu_ent();
	}break;
	case(G_BossSelect) : {
					   game->selectScreen = init_select_screen();
	}break;
	case(G_Level) : {
						//load level file n stuff n level entity
						switch (level){
						case(0) : {
								//fireman
									  game->currentLevel = load_level("../levels/test.json");
						}break;
						case(1) : {
								//test
						}break;
						case(2) : {
								//metalman
						}break;
						case(3) : {
									  //bubbleman
						}break;

						}
						game->controllerEntity = get_player_entity();
						set_target(game->cam, game->controllerEntity, 1);
						set_bounds(game->cam, vector2d(0, 0), vector2d(2000, 750));
	}break;
	}
	game->CurrentState = state;
	game->transition = false;
}

void load_save(){
	//check if file if not return
	//else read json data and fill in data of game_data
	SJson *file,*value;
	file = sj_load("../gamedata.sav");
	if (file == NULL){
		slog("file not found");
		return;
	}
	//get boss data and player weapon data
	value = sj_object_get_value(file, "bossdata");

	game->data->bossdata = sj_array_return(value);
	sj_echo(value);
	value = sj_object_get_value(file, "weapondata");
	sj_echo(value);
	game->data->weapondata = sj_array_return(value);
}

void save_game(){
	SJson *file,*array,*array2,*data;
	file = sj_object_new();
	array=sj_array_new();
	array2 = sj_array_new();
	
	for (int i = 0; i < 4; i++){
		data = sj_new_int(game->data->bossdata[i]);
		sj_array_append(array, data);
	}
	
	for (int i = 0; i < 4; i++){
		data = sj_new_int(game->data->weapondata[i]);
		sj_array_append(array2, data);
	}

	sj_object_insert(file, "bossdata", array);
	sj_object_insert(file, "weapondata", array2);
	sj_save(file, "../gamedata.sav");
}

game_state return_game_state(){
	return game->CurrentState;
}
void update_game(const Uint8 *keys){
	switch (game->CurrentState){
	case(G_Menu) : {
					   get_menu_inputs(game->menu, keys);
	}break;
	case(G_BossSelect) : {
					   get_select_inputs(game->selectScreen, keys);
	}break;
	case(G_Level) : {
						gungirl_get_inputs(game->controllerEntity,keys);
						entity_tile_collision(game->currentLevel->tiles);
						draw_tiles(game->currentLevel, game->cam);
	}break;
	}
	if (!game->transition){
		update_camera(game->cam);
		draw_entities(game->cam);
	}
}


int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;	
	SDL_Rect rect;
	game = (game_controller*)malloc(sizeof(game_controller));
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        256,
        240,
        256,
        240,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
	gf2d_entity_manager_init(20);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
	init_game();
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        gf2d_graphics_clear_screen();// clears drawing buffers
		Vector2D pos = vector2d(0, 0);
            
		update_game(keys);

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
		
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
