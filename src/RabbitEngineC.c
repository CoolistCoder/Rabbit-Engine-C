//Engine made by Alec Roberts.
//I am using SDL2 under the zlib license and am not the creator of SDL2, SDL2_image, or SDL2_mixer.  I did not program SDL2
//or any of the other zlib library modules.

#include "Engine.h"

void toggle_fullscreen(SDL_Scancode scancode);
int fulldel=0;



int main(int argc, char**args)
{
	engine_init_all(); //init the engine
	create_window("", 1920/2, 1080/2); //create the window
	resize_renderer(1920/4, 1080/4); //resize the window's renderer
	set_view_cam(define_rect(0, 0, 320, 240)); //set the view of the camera (centered on the renderer)
	rename_window("RabbitEngineC Project"); //Rename's the window


	while(engine_get_running()) //main loop
	{
		handle_window(); //allows the user to close and move the window
		clear_screen(); //clears the renderer
		center_view_cam(); //sets the view of the camera
		redetect_joysticks(); //redetects the number of joysticks inserted


		if (get_key(SDL_SCANCODE_ESCAPE)) //closes the window and ends the engine
		{
			end_engine();
		}

		toggle_fullscreen(SDL_SCANCODE_F4); //toggles fullscreen

		update_screen(); //updates objects drawn to the renderer
		set_fps(60); //sets the frames per second
	}

	engine_cleanup_all(); // cleans up the engine

	return 0;
}

void toggle_fullscreen(SDL_Scancode scancode)
{
	if (get_key(scancode))
	{
		if(fulldel==0)
		{
			if (get_fullscreen_window())
			{
				SDL_ShowCursor(1);
				restore_window();
			}
			else
			{
				SDL_ShowCursor(0);
				fullscreen_window();
			}
		}
		fulldel++;
	}
	else
	{
		fulldel = 0;
	}
}

