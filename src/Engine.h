//Engine made by Alec Roberts.
//I am using SDL2 under the zlib license and am not the creator of SDL2, SDL2_image, or SDL2_mixer.  I did not program SDL2
//or any of the other zlib library modules.

#ifndef ENGINE_H_
#define ENGINE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#ifdef __cplusplus
extern "C"{
#endif
typedef enum
{
	MUSIC = 0,
	SOUND = 1
}mixtype;

typedef struct
{
	int x;
	int y;
	int w;
	int h;
}rect_t;

typedef struct
{
	int x;
	int y;
}vector2_t;

typedef struct
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
}color_t;

typedef struct
{
	SDL_Texture *tex;
	color_t modcol;
	rect_t source;
	rect_t dest;
	SDL_RendererFlip fliptype;
	float angle;
	int tileflag;
	int frame;
	rect_t camrect;
	rect_t drawrect;
	int visible;
}sprite_t;

typedef struct
{
	Mix_Music *mfile;
	Mix_Chunk *sfile;
	int channel;
	mixtype filespec;
}sound_t;

//definition functions
rect_t define_rect(int rect_x, int rect_y, int rect_w, int rect_h);
color_t define_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a);
vector2_t define_point(int vec_x, int vec_y);

//init functions
void engine_init_all();
int engine_get_running();
void end_engine();

//window/renderer functions
void create_window(char* window_name, int window_w, int window_h);
void resize_window(int window_w, int window_h);
void resize_renderer(int renderer_w, int renderer_h);
void rename_window(char* window_name);
void fullscreen_window();
void restore_window();
int get_fullscreen_window();

void handle_window();
void clear_screen();
void update_screen();

//camera functions
void center_view_cam(); //centers the cam on the screen
void set_view_cam(rect_t new_view);
void focus_cam(rect_t f_rect);
void enable_border_cam();
void hide_border_cam();

rect_t get_cam_view();

//simple drawing
void draw_rect(rect_t rect,color_t color);

//sprite drawing
void assign_sprite(sprite_t *sprite, char *filename, color_t *key, rect_t dest);
void draw_sprite(sprite_t sprite);
void move_sprite(sprite_t *sprite, vector2_t pos);
void angle_sprite(sprite_t *sprite, float angle);
void mirror_sprite(sprite_t *sprite, SDL_RendererFlip flip);
void color_sprite(sprite_t *sprite, color_t color);
void hide_sprite(sprite_t *sprite);
void show_sprite(sprite_t *sprite);
void resize_sprite(sprite_t *sprite, rect_t rect);
void tile_sprite(sprite_t *sprite, rect_t src);
void frame_sprite(sprite_t *sprite, int frame);
void focus_sprite(sprite_t sprite);
void reor_draw_sprite(sprite_t *sprite);
void reor_cam_sprite(sprite_t *sprite);
void reor_sprite(sprite_t *sprite);
void free_sprite(sprite_t *sprite);

//tilemap drawing
void draw_tilemap(sprite_t sprite, vector2_t mapsize, int *data);

//sprite handling
int collide_sprite(sprite_t sprite1, sprite_t sprite2);

//text display
void draw_text(sprite_t sprite, char *text);

//user input
int get_key(SDL_Scancode scode);

void redetect_joysticks();
int x_axis_joystick();
int y_axis_joystick();
int z_axis_joystick();
int x_rotate_joystick();
int y_rotate_joystick();
int z_rotate_joystick();
int hat_joystick();
int button_joystick(int button);

//sound loading/playing
void assign_sound(sound_t *sound, char *filename, mixtype type);
void play_sound(sound_t sound);
void stop_sound(sound_t sound);
void pause_sound(sound_t sound);
void resume_sound(sound_t sound);
void free_sound(sound_t *sound);

//sound handling
void set_volume(int volume, mixtype type);

//Cleanup function
void engine_cleanup_all();

//extra functions
void set_fps(float fps);
char* concstr_2(char *str1, char *str2); /// concatenate 2 strings
char* concstr_3(char *str1, char *str2, char *str3); ///concatenate 3 strings


#ifdef __cplusplus
}
#endif

#endif /* ENGINE_H_ */
