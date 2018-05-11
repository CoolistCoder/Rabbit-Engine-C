//Engine made by Alec Roberts.
//I am using SDL2 under the zlib license and am not the creator of SDL2, SDL2_image, or SDL2_mixer.  I did not program SDL2
//or any of the other zlib library modules.

#include "Engine.h"

//initialization variables
static int isrunning=0;

//window variables
static int renderer_w;
static int renderer_h;
static char *name = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int isfullscreen = 0;

static Uint32 first_frame;

//camera variables
static rect_t camera_view;
static vector2_t camera_offset;
static int drawborder=0;

//sound variables
static int music_volume = 120;
static int sound_volume = 120;

//joystick variables
int savednum;
int firstconnect = 0;

static SDL_Joystick **joystick;

//define structs
rect_t define_rect(int rect_x, int rect_y, int rect_w, int rect_h)
{
	rect_t temp; //temporary rect to store values
	temp.x=rect_x;
	temp.y=rect_y;
	temp.w=rect_w;
	temp.h=rect_h;
	return temp; //return rect_t
}

color_t define_color(Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	color_t temp; //temporary color to store values
	temp.r=r;
	temp.g=g;
	temp.b=b;
	temp.a=a;
	return temp; //return color_t
}

vector2_t define_point(int vec_x, int vec_y)
{
	vector2_t temp; //temporary vector2 to store values
	temp.x=vec_x;
	temp.y=vec_y;
	return temp; //return vector2_t
}



//init functions
void engine_init_all()
{
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0) //init SDL
	{
		printf("Unable to init SDL2");
		isrunning=0;
	}
	else
	{
		isrunning=1;
	}
	if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG )) //init SDL_image
	{
		printf("Unable to init IMG");
		isrunning=0;
	}
	else
	{
		isrunning=1;
	}
    if (!Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG | MIX_INIT_MP3)) //init SDL_mixer
    {
    	printf("Unable to init MIXER");
    	isrunning=0;
    }
    else
    {
    	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
    	isrunning=1;
    }

    joystick = malloc(SDL_NumJoysticks()*sizeof(SDL_Joystick**));

    for (int i=0;i<SDL_NumJoysticks();i++)
    {
    	*(joystick+i) = SDL_JoystickOpen(i);
    }
}

int engine_get_running()
{
	return isrunning;
}

void end_engine()
{
	isrunning=0;
}

//video functions
void create_window(char* window_name, int window_w, int window_h)
{
	int len; //the length of the window name;
	len = strlen(window_name);
	name = (char*) malloc(len*sizeof(char)); //initializing the window name
	strcpy(name, window_name);

	//set the width and height
	renderer_w=window_w;
	renderer_h=window_h;

	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, renderer_w, renderer_h, SDL_WINDOW_SHOWN); //init window

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //init renderer
	SDL_RenderSetLogicalSize(renderer, renderer_w, renderer_h); //set the logical size of the renderer
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //set the blend mode

	//setup camera
	camera_view.x=0;
	camera_view.y=0;
	camera_view.w=renderer_w;
	camera_view.h=renderer_h;
	camera_offset.x=0;
	camera_offset.y=0;
	enable_border_cam();
}

void resize_window(int window_w, int window_h)
{
	SDL_SetWindowSize(window, window_w, window_h); //resize window based on width and height arguments
}

void resize_renderer(int newr_w, int newr_h)
{
	//adjust renderer and readjust camera to fit renderer, then reset camera view
	renderer_w=newr_w;
	renderer_h=newr_h;
	camera_view.w=renderer_w;
	camera_view.h=renderer_h;
	SDL_RenderSetLogicalSize(renderer, renderer_w, renderer_h);
}

void rename_window(char* window_name)
{
	//reallocate memory to char *name to set its value to window_name
	if (name)
	{
		free(name);
	}
	int len;
	len = strlen(window_name);
	name = (char*) malloc(len*sizeof(char));
	name = window_name;
	SDL_SetWindowTitle(window, name);
}

void fullscreen_window()
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); //simply fullscreen window
	isfullscreen=1;
}

void restore_window()
{
	SDL_SetWindowFullscreen(window, 0); //simply restore window
	isfullscreen=0;
}

int get_fullscreen_window()
{
	return isfullscreen;
}

void handle_window()
{
	//allow user to interact with window
	first_frame = SDL_GetTicks();
	SDL_Event event;

	while(SDL_PollEvent(&event)) //get the event and poll it
	{
		if (event.type==SDL_QUIT)
		{
			isrunning=0;
		}
	}
}

void clear_screen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //set the color of the renderer
	SDL_RenderClear(renderer); //clear said renderer
}

void update_screen()
{
	if (drawborder)
	{
		SDL_Rect croprect;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		croprect.x=0; croprect.y=0; croprect.w=camera_view.x, croprect.h=renderer_h;
		SDL_RenderFillRect(renderer, &croprect);

		croprect.w=renderer_w, croprect.h=camera_view.y;
		SDL_RenderFillRect(renderer, &croprect);

		croprect.x=camera_view.x+camera_view.w; croprect.w=camera_view.x+1, croprect.h=renderer_h;
		SDL_RenderFillRect(renderer, &croprect);

		croprect.x=0; croprect.y=camera_view.y+camera_view.h; croprect.w=renderer_w, croprect.h=camera_view.y+1;
		SDL_RenderFillRect(renderer, &croprect);
	}

	SDL_RenderPresent(renderer); //simply update the renderer
}

//camera functions
void center_view_cam()
{
	//center the cam's view on the renderer
	camera_view.x = (renderer_w/2)-(camera_view.w/2);
	camera_view.y = (renderer_h/2)-(camera_view.h/2);
}

void set_view_cam(rect_t new_view)
{
	camera_view = new_view; //simply change the camera_view variable to the new_view variable
	center_view_cam();
}

void focus_cam(rect_t f_rect)
{
	//focus the camera_offset on f_rect
	camera_offset.x = (f_rect.x-(f_rect.w/2))+(camera_view.w/2);
	camera_offset.y = (f_rect.y-(f_rect.h/2))+(camera_view.h/2);
}

void enable_border_cam()
{
	drawborder=1;
}

void hide_border_cam()
{
	drawborder=0;
}

rect_t get_cam_view()
{
	return camera_view; //returns the camera_view
}

//drawing functions
void draw_rect(rect_t rect,color_t color)
{
	SDL_Rect temprect; //init a pointer to an SDL rect
	temprect.x=rect.x+camera_view.x+camera_offset.x;
	temprect.y=rect.y+camera_view.y+camera_offset.y;
	temprect.w=rect.w;
	temprect.h=rect.h;

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); //set the rect's color
	SDL_RenderFillRect(renderer, &temprect); //draw the rect
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //reset the renderer's color to black
}

//loading/drawing images
static SDL_Texture* load_image(char* filename, color_t* key) //a simple method of loading in images for sprites
{
	SDL_Surface* temp_surface;
	SDL_Texture* temp_texture;

	temp_surface = IMG_Load(filename);
	if (temp_surface!=NULL)
	{
		if (key!=NULL)
		{
			Uint32 clkey = SDL_MapRGBA(temp_surface->format, key->r, key->g, key->b, key->a);
			SDL_SetColorKey(temp_surface, SDL_TRUE, clkey);
		}
		temp_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
		if (temp_texture!=NULL)
		{

			SDL_FreeSurface(temp_surface);
			return temp_texture;
		}
	}
	return NULL;
}

//drawing sprites
void assign_sprite(sprite_t *sprite, char *filename, color_t *key, rect_t dest)
{
	//simply assign the values to a sprite
	SDL_Texture *temp;
	temp = load_image(filename, key);
	sprite->tex=temp;
	sprite->modcol=define_color(255, 255, 255, 255);
	sprite->dest=dest;

	sprite->source=dest;
	sprite->fliptype=SDL_FLIP_NONE;
	sprite->angle=0.0;
	sprite->tileflag=0;
	sprite->frame=0;
	sprite->camrect = sprite->dest;
	sprite->drawrect = sprite->dest;
	sprite->visible = 1;
}

static int get_if_sprite_in(SDL_Rect *copyrect)
{
	if ((copyrect->y+copyrect->h)<=camera_view.y)
	{
		return 0;
	}
	if (copyrect->y>=(camera_view.y+camera_view.h))
	{
		return 0;
	}
	if ((copyrect->x+copyrect->w)<=camera_view.x)
	{
		return 0;
	}
	if (copyrect->x>=(camera_view.x+camera_view.w))
	{
		return 0;
	}
	return 1;
}

void draw_sprite(sprite_t sprite)
{
	SDL_Rect temp;
	temp.x=sprite.drawrect.x+camera_view.x+camera_offset.x;
	temp.y=sprite.drawrect.y+camera_view.y+camera_offset.y;
	temp.w=sprite.drawrect.w;
	temp.h=sprite.drawrect.h;

	SDL_Point tpont;
	tpont.x=temp.w/2; tpont.y=temp.h/2;

	if(sprite.tex!=NULL)
	{
		SDL_SetTextureColorMod(sprite.tex, sprite.modcol.r, sprite.modcol.g, sprite.modcol.b);
		SDL_SetTextureAlphaMod(sprite.tex, sprite.modcol.a);
		if(!sprite.tileflag)
		{
			if (sprite.visible)
			{
				if (get_if_sprite_in(&temp))
				{
					SDL_RenderCopyEx(renderer, sprite.tex, NULL, &temp, sprite.angle, &tpont, sprite.fliptype);
				}
			}
		}
		else
		{
			int w,h;
			SDL_QueryTexture(sprite.tex, NULL, NULL, &w, &h);

			SDL_Rect srcrect;

			int xframes;
			xframes=w/sprite.source.w;

			int fsrcx,fsrcy;
			fsrcx = (sprite.frame % xframes) * sprite.source.w;
			fsrcy= (sprite.frame / xframes) * sprite.source.h;

			srcrect.x = fsrcx;
			srcrect.y = fsrcy;
			srcrect.w = sprite.source.w;
			srcrect.h = sprite.source.h;

			if (sprite.visible)
			{
				if (get_if_sprite_in(&temp))
				{
					SDL_RenderCopyEx(renderer, sprite.tex, &srcrect, &temp, sprite.angle, &tpont, sprite.fliptype);
				}
			}
		}
	}
}

void move_sprite(sprite_t *sprite, vector2_t pos)
{
	sprite->dest.x=pos.x;
	sprite->dest.y=pos.y;
}

void angle_sprite(sprite_t *sprite, float angle)
{
	sprite->angle=angle;
}

void mirror_sprite(sprite_t *sprite, SDL_RendererFlip flip)
{
	sprite->fliptype = flip;
}

void color_sprite(sprite_t *sprite, color_t color)
{
	sprite->modcol = color;
}

void hide_sprite(sprite_t *sprite)
{
	sprite->visible = 0;
}

void show_sprite(sprite_t *sprite)
{
	sprite->visible = 1;
}

void resize_sprite(sprite_t *sprite, rect_t rect)
{
	sprite->dest=rect;
}

void tile_sprite(sprite_t *sprite, rect_t src)
{
	sprite->tileflag = 1;
	sprite->source = src;
}

void frame_sprite(sprite_t *sprite, int frame)
{
	if (sprite->tileflag == 1)
	{
		sprite->frame=frame;
	}
	else
	{
		sprite->frame = 0;
	}
}

void focus_sprite(sprite_t sprite)
{
	focus_cam(sprite.camrect);
}

void reor_draw_sprite(sprite_t *sprite)
{
	sprite->drawrect = sprite->dest;
}

void reor_cam_sprite(sprite_t *sprite)
{
	sprite->camrect = sprite->dest;
}

void reor_sprite(sprite_t *sprite)
{
	reor_draw_sprite(sprite);
	reor_cam_sprite(sprite);
}

void free_sprite(sprite_t *sprite)
{
	SDL_DestroyTexture(sprite->tex);
}

//tilemap function
void draw_tilemap(sprite_t sprite, vector2_t mapsize, int *data)
{
	int totalframes = ((mapsize.x*sprite.source.w)/sprite.source.w)*((mapsize.y*sprite.source.h)/sprite.source.h);
	int xframes = (mapsize.x*sprite.source.w)/sprite.source.w;

	int tempx, tempy;
	tempx=sprite.dest.x;
	tempy=sprite.dest.y;

	for (int i=0;i<totalframes;i++)
	{
		sprite.dest.x=((i%xframes)*sprite.dest.w)+tempx;
		sprite.dest.y=((i/xframes)*sprite.dest.h)+tempy;

		reor_sprite(&sprite);
		frame_sprite(&sprite, data[i]);
		draw_sprite(sprite);
	}
}

int collide_sprite(sprite_t sprite1, sprite_t sprite2)
{
	if ((sprite1.dest.y+sprite1.dest.h)<=sprite2.dest.y)
	{
		return 0;
	}
	if (sprite1.dest.y>=(sprite2.dest.y+sprite2.dest.h))
	{
		return 0;
	}
	if ((sprite1.dest.x+sprite1.dest.w)<=sprite2.dest.x)
	{
		return 0;
	}
	if (sprite1.dest.x>=(sprite2.dest.x+sprite2.dest.w))
	{
		return 0;
	}
	return 1;
}

//text functions
void draw_text(sprite_t sprite, char *text)
{
	if (sprite.tileflag)
	{
		int templen = strlen(text); //gets the string length
		sprite.dest.x-=sprite.dest.w; //pushes the text back one tile
		sprite.dest.x-=camera_offset.x; //subtracts based on offset
		sprite.dest.y-=camera_offset.y; //subtracts based on offset
		for (int i=0;i<templen;i++) //for loop from 0 to templen
		{
			sprite.dest.x+=sprite.dest.w; //moves the character one tile forward
			reor_draw_sprite(&sprite);
			frame_sprite(&sprite, text[i]-32); //sets the frame for the text character
			draw_sprite(sprite); //draws the character
		}
	}
}

//input functions
int get_key(SDL_Scancode scode)
{
	const Uint8 *pos = SDL_GetKeyboardState(NULL);
	if (pos[scode])
	{
		free((Uint8*)pos);
		return 1;
	}
	else
	{
		free((Uint8*)pos);
		return 0;
	}
}

void redetect_joysticks()
{

	if (joystick!=NULL)
	{
		 free(joystick);
		 joystick = malloc(SDL_NumJoysticks()*sizeof(SDL_Joystick**));
	}

	for (int i=0;i<SDL_NumJoysticks();i++)
	{
		*(joystick+i) = SDL_JoystickOpen(i);
	}
}

int x_axis_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 0)/(32000/255));
		}
	}
	return 0;
}

int y_axis_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 1)/(32000/255));
		}
	}
	return 0;
}

int z_axis_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 2)/(32000/255));
		}
	}
	return 0;
}

int x_rotate_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 3)/(32000/255));
		}
	}
	return 0;
}

int y_rotate_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 4)/(32000/255));
		}
	}
	return 0;
}

int z_rotate_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return (SDL_JoystickGetAxis(joystick[joyid], 5)/(32000/255));
		}
	}
	return 0;
}

int hat_joystick(int joyid)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			int temp = (int)SDL_JoystickGetHat(joystick[joyid], 0);
			return temp;
		}
	}
	return 0;
}

int button_joystick(int joyid, int button)
{
	if (*(joystick+joyid)!=NULL)
	{
		if (joyid<SDL_NumJoysticks()&&joyid>=0)
		{
			return SDL_JoystickGetButton(joystick[joyid], button);
		}
	}
	return 0;
}


//sound/music functions
void assign_sound(sound_t *sound, char *filename, mixtype type)
{
	if (type==MUSIC)
	{
		sound->mfile = Mix_LoadMUS(filename);
		sound->filespec = type;
	}
	else if (type==SOUND)
	{
		sound->sfile = Mix_LoadWAV(filename);
		sound->channel = 0;
		sound->filespec = type;
	}
	free(filename);
}

void play_sound(sound_t sound)
{
	if (sound.filespec==MUSIC)
	{
		Mix_VolumeMusic(music_volume);
		if (!Mix_PlayingMusic())
		{
			Mix_PlayMusic(sound.mfile, 0);
		}
	}
	else if (sound.filespec==SOUND)
	{
		Mix_VolumeChunk(sound.sfile, sound_volume);
		if (!Mix_Playing(sound.channel))
		{
			Mix_PlayChannel(sound.channel,sound.sfile,0);
		}
	}

}

void stop_sound(sound_t sound)
{
	if (sound.filespec==MUSIC)
	{
		Mix_HaltMusic();
	}
	else if (sound.filespec==SOUND)
	{
		Mix_HaltChannel(sound.channel);
	}
}

void pause_sound(sound_t sound)
{
	if (sound.filespec==MUSIC)
	{
		Mix_PauseMusic();
	}
	else if (sound.filespec==SOUND)
	{
		Mix_Pause(sound.channel);
	}
}

void resume_sound(sound_t sound)
{
	if (sound.filespec==MUSIC)
	{
		Mix_ResumeMusic();
	}
	else if (sound.filespec==SOUND)
	{
		Mix_Resume(sound.channel);
	}
}

void free_sound(sound_t *sound)
{
	if (sound->filespec==MUSIC)
	{
		Mix_FreeMusic(sound->mfile);
	}
	else
	{
		Mix_FreeChunk(sound->sfile);
	}
}

//handle sound
void set_volume(int volume, mixtype type)
{
	if (type==MUSIC)
	{
		music_volume = volume;
	}
	else if (type==SOUND)
	{
		sound_volume = volume;
	}
}

//cleanup function
void engine_cleanup_all()
{
	// free EVERY STATIC VARIABLE and cleanup SDL2

	for (int i=0;i<SDL_NumJoysticks();i++)
	{
		SDL_JoystickClose(*(joystick+i));
		free(*(joystick+i));
	}
	free(joystick);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);

	free(name);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

//extra functions
void set_fps(float fps)
{
    float frame_delay = 1000/fps;
    float frame_time = SDL_GetTicks()-first_frame;
    if (frame_time<frame_delay)
    {
    	SDL_Delay(frame_delay-frame_time);
    }
}

char* concstr_2(char *str1, char *str2)
{
	int len1;
	int len2;
	len1=strlen(str1);
	len2=strlen(str2);

	char *wholestr;
	wholestr = (char*) malloc((len1+len2)*sizeof(char));

	strcpy(wholestr,str1);
	strcat(wholestr,str2);

	return wholestr;
}

char* concstr_3(char *str1, char *str2, char *str3)
{
	int len1;
	int len2;
	int len3;
	len1=strlen(str1);
	len2=strlen(str2);
	len3=strlen(str3);

	char *wholestr;
	wholestr = (char*) malloc((len1+len2+len3)*sizeof(char));

	strcpy(wholestr,str1);
	strcat(wholestr,str2);
	strcat(wholestr,str3);

	return wholestr;
}


