#include<stdio.h>
#include<allegro5\allegro.h>
#include<allegro5\allegro_font.h>
#include<allegro5\allegro_ttf.h>
#include<allegro5\allegro_primitives.h>
#include<allegro5\allegro_image.h>
#include<allegro5\allegro_audio.h>
#include<allegro5\allegro_acodec.h>
#include "objects.h" //this is the header file I created to store the structure declarations for the objects in the game

//global variables 

bool collision;
int temp;
int code;

bool win = false;
const int width = 800,height = 600;
const int num_enemy= 15;

bool jump;
int level =1;
int jumpspeed = 15;
const int gravity = 1.0;

enum KEYS{UP,DOWN,LEFT,RIGHT,SPACE};

bool keys[5]={false,false,false,false,false};


//now the function prototypes

bool checkifground(int x); //checks if there is ground underneath

//now, mario functions
void init_mario(player &mario,ALLEGRO_BITMAP *image); //init the structure
void draw_mario(player &mario); //draws mario on the screen 
void move_mario_right(player &mario); //moves right
void move_mario_left(player &mario); //moves left


//now the stationary objects
void init_brick(wall brick[],ALLEGRO_BITMAP *image); //init the brick structures
void init_box(coinbox box[],ALLEGRO_BITMAP *image); //init the coinbox structures
bool collide_box(coinbox box[],player &mario); //checks collision of mario with a coinbox

void init_enemy(nonplayer enemy[],ALLEGRO_BITMAP *image); //init the enemy
void move_enemy(nonplayer enemy[]); //moves the enemy
void collide_enemy(nonplayer enemy[],player &mario); //checks SIDE COLLISION with the enemy
bool kill_enemy(nonplayer enemy[],player &mario); //checks if the player jumped on the enemy

int main()
{
	bool done=false;
	bool redraw=true; //renders only after timer is executed
	const int FPS = 60; //fps is given for timer for the game to be smooth
	bool gameover = false;

	//OBJECT VARIABLES OF TYPE STRUCT 
	player mario;
	nonplayer enemy[3];
	wall brick[8];
	coinbox box[5];

	al_init(); //initializes allegro


	//allegro initializing functions for the various operations in my game

	ALLEGRO_DISPLAY *display = al_create_display(width,height);
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer=al_create_timer(1.0/FPS); //creates a timer which sets off every one sixtieth of a second 
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *brickimage = NULL;
	ALLEGRO_BITMAP *boximage = NULL;
	ALLEGRO_BITMAP *cloud = NULL;
	ALLEGRO_BITMAP *bush = NULL;
	ALLEGRO_BITMAP *controls = NULL;
	ALLEGRO_BITMAP *castle = NULL;
	ALLEGRO_BITMAP *block = NULL;
	ALLEGRO_BITMAP *logo = NULL;
	ALLEGRO_BITMAP *name = NULL;
	ALLEGRO_BITMAP *enemyimage = NULL;
	ALLEGRO_FONT *arial = NULL;
	ALLEGRO_SAMPLE *theme = NULL;
	ALLEGRO_SAMPLE *die = NULL;
	ALLEGRO_SAMPLE *jumpsound = NULL;
	ALLEGRO_SAMPLE *enemy_kill = NULL;
	ALLEGRO_SAMPLE *coinsound = NULL;

	//file for highscore
	FILE *highscore = NULL;
	int previous_score;
	highscore = fopen("Scores/highscore.txt","r+");
	fscanf(highscore,"%d",&previous_score);
	
	//allegro initalizing functions for tasks
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_acodec_addon();
	al_install_audio();
	
	//registration of event source in the event queue 
	al_register_event_source(event_queue,al_get_keyboard_event_source());
	al_register_event_source(event_queue,al_get_timer_event_source(timer));
	al_register_event_source(event_queue,al_get_display_event_source(display));
	
	image = al_load_bitmap("Images/final_sprite_sheet.png"); //loads the mario sprite sheet
	al_convert_mask_to_alpha(image,al_map_rgb(255,255,255)); //removes white(255,255,255) background from the image 

	al_reserve_samples(5); //reserves memory for 5 audio files,which follow

	//audio files
	theme = al_load_sample("Sounds/theme.wav");
	die = al_load_sample("Sounds/jump-big.wav");
	jumpsound = al_load_sample("Sounds/jump-small.wav");
	coinsound = al_load_sample("Sounds/coin_sound.wav");
	enemy_kill = al_load_sample("Sounds/enemy_kill.wav");

	//now the images in the game are loaded
	cloud = al_load_bitmap("Images/cloud.png");
	bush = al_load_bitmap("Images/bush.png");
	block = al_load_bitmap("Images/block.png");
	brickimage = al_load_bitmap("Images/brick.png");
	boximage = al_load_bitmap("Images/box.png");
	enemyimage = al_load_bitmap("Images/enemy.png");
	castle = al_load_bitmap("Images/castle.png");
	logo = al_load_bitmap("Images/logo.png");
	name = al_load_bitmap("Images/name.png");
	controls = al_load_bitmap("Images/controls.png");

	//convertion of background for each image
	al_convert_mask_to_alpha(castle,al_map_rgb(255,255,255));
	al_convert_mask_to_alpha(name,al_map_rgb(190,40,1));
	al_convert_mask_to_alpha(enemyimage,al_map_rgb(255,255,255));
	al_convert_mask_to_alpha(controls,al_map_rgb(255,255,255));

	//structure initializations
	init_mario(mario,image);
	init_brick(brick,brickimage);
	init_box(box,boximage);
	init_enemy(enemy,enemyimage);

	//initial screen
	al_clear_to_color(al_map_rgb(0,150,215)); //clears background to the specified colour
	al_draw_bitmap(logo,200,128,0); //draws the bitmaps
	al_draw_bitmap(name,400,250,0);
	al_draw_bitmap(controls,110,260,0);
	al_flip_display(); //flips back buffer into front buffer(display)[everything was drawn in the backbuffer]
	al_play_sample(theme,1,0,1,ALLEGRO_PLAYMODE_LOOP,NULL); //plays sound - theme song
	al_rest(2.0); //rests the display
	
	

	arial = al_load_font("Fonts/arial.ttf",18,0); //loads the font for displaying score,lives etc.
	
	al_start_timer(timer); //starts the timer
	
	while(!done)
	{
		ALLEGRO_EVENT ev; //creates event 
		al_wait_for_event(event_queue,&ev); //WAITS UNTIL THERE IS AN EVENT
		
		//now coming to the events
		if(ev.type==ALLEGRO_EVENT_KEY_DOWN) //when a key is pressed / held down
		{

			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT]=true;
				code = 0;
				break;
			case ALLEGRO_KEY_LEFT:
				
				keys[LEFT]=true;
				code = 1;
				break;
			case ALLEGRO_KEY_ESCAPE:
				done=true;
				break;
			case ALLEGRO_KEY_SPACE:
				if(mario.y >= 480) //triggers jump only if the player is on the ground(y=480 is ground as there are tiles beneath), i.e. a player cant jump again while jumping
				{	
					al_play_sample(jumpsound,1.5,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
					jump = true;
					mario.curframe = 4; //for animation - refer the sprite sheet
					mario.vely = -15.5; //sets initial velocity when jumped
				}
				break;
			}
		}
		else if(ev.type==ALLEGRO_EVENT_KEY_UP) //when a key is released
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT]=false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT]=false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				done=true;
				break;
			
			}
		}
		
		
		if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE) //when close button is clicked
			done=true;

		if(ev.type==ALLEGRO_EVENT_TIMER)  //occurs every (1/60)th of a second
		{
			//NOW COMES THE MAIN PART
			//THE TIMER EXECUTES UNTIL THERE IS AN EVENT IN THE EVENT QUEUE, IF THERE IS AN EVENT IN THE EVENT QUEUE,
			//IT SETS OFF EVERY ONE SIXTIETH OF A SECOND FOR THE FRAMES TO BE SMOOTH
			//NOW ALL THE FUNCTIONS ARE CALLED
			move_enemy(enemy); //TO CHANGE THE X AND Y CO-ORDINATES OF ENEMY
			if(kill_enemy(enemy,mario)) //IF ENEMY IS KILLED
				al_play_sample(enemy_kill,1.5,0,1,ALLEGRO_PLAYMODE_ONCE,0);
			collide_enemy(enemy,mario); //CHECKS FOR COLLISION OF MARIO WITH ENEMY
			
			if(keys[LEFT])
				move_mario_left(mario);
			else if(keys[RIGHT])
				move_mario_right(mario);
			else
			{
				mario.curframe = 0; //STANDING FRAME
			}
			if(collide_box(box,mario)) //IF MARI0 COLLIDES WITH A COINBOX
			{
				printf("Mario collided with coinbox!\n");
				al_play_sample(coinsound,1,1,1,ALLEGRO_PLAYMODE_ONCE,0);
			}

			if(jump) //NOW COMES TO THE JUMPING LOGIC
			{
				mario.curframe = 4;
				mario.vely += gravity; //GRAVITY IS +1,SO AS INITIAL VEL Y = -15, IT IS INCREASED AS THE JUMP DESCENDS, I.E. SIMULATES GRAVITY
				mario.y += mario.vely; //UPDATING MARIO.Y AFTER UPDATING VEL Y
			}
				
			if(mario.y >=  480) //JUMP BECOMES FALSE WHEN MARIO TOUCHES GROUND(Y=480)
			{
				jump = false;
				if(checkifground(mario.x)) //CHECKS IF THERE IS GROUND UNDERNEATH MARI0
					mario.y = 480;
				else 
				{
					mario.y = 600; //TAKES MARIO TO THE BOTTOM(HE FALLS INTO THE EMPTY TILE - DYING IS IN THE REDRAW IF PART )
				}
			}
			redraw=true; //REDRAW IS TRUE EVERY TIME THE TIMER IS ON
			//detect collision and alter player positions 
		}

		if(redraw && al_is_event_queue_empty(event_queue)) //REDRAWS EVERY IMAGE,BACKGROUND ON THE SCREEN, WHENEVER TIMER IS TRUE
		{
			//BACKGROUND
			al_clear_to_color(al_map_rgb(0,150,215));
			al_draw_bitmap(cloud,200,50,0);
			al_draw_bitmap(cloud,400,50,0);			
			al_draw_bitmap(cloud,700,50,0);
			al_draw_bitmap(bush,50,height-103,0);
			al_draw_bitmap(bush,400,height-103,0);
			al_draw_bitmap(bush,750,height-103,0);
			
			if(level == 1) //DRAWS BRICKS,ENEMIES,BOXES IN LEVEL 1
			{
				for(int i=0;i<2;i++)
				{
					if(enemy[i].live == true)
						al_draw_bitmap(enemyimage,enemy[i].x,enemy[i].y,0);
				}
				for(int colnum=0;colnum<=24;colnum++)
				{
					for(int rownum = 16;rownum<=18;rownum++)
					{
						if(colnum!=14)
							al_draw_bitmap(block,colnum*32,rownum*32,0);
					}
				}

				for(int i=0;i<6;i++)
					al_draw_bitmap(brick[i].image,brick[i].x,brick[i].y,0);
				for(int i=0;i<5;i++)
					al_draw_bitmap(box[i].image,box[i].x,box[i].y,0);
			}
			else if(level == 2) //DRAWS BRICKS AND ENEMIES IN LEVEL 2
			{
				if(enemy[2].live) //ONLY IF THE ENEMY IS LIVE,IT DRAWS THE ENEMY
					al_draw_bitmap(enemy[2].image,enemy[2].x,enemy[2].y,0);
				for(int colnum=0;colnum<=24;colnum++)
				{
					for(int rownum = 16;rownum<=18;rownum++)
					{
						if(colnum!=4)
							al_draw_bitmap(block,colnum*32,rownum*32,0);
					}
				}
				for(int i=6;i<8;i++)
					al_draw_bitmap(brick[i].image,brick[i].x,brick[i].y,0);
				
				al_draw_bitmap(castle,11*32,11*32,0);
			}
			if(mario.lives == 0) 
				gameover = 1;
			
			if(!gameover)
			{
				if(level == 1)
				{
					for(int i=0;i<2;i++)
					{
						if(enemy[i].live == true)
							al_draw_bitmap(enemyimage,enemy[i].x,enemy[i].y,0);
					}
				}
				draw_mario(mario); //DRAWS MARIO ON THE SCREEN
				//DISPLAYS HIGHSCORE,WORLD NUMBER,LIVES LEFT AND CURRENT SCORE
				al_draw_textf(arial,al_map_rgb(0,0,0),width-550,20,0,"Highscore : %i || World : %i || Lives left : %i || Score : %i",previous_score,level,mario.lives,mario.score);
			}
			else if(gameover == 1 && win == false) //PLAYER LOST
			{
				al_draw_textf(arial,al_map_rgb(0,0,0),width/2,height/2,ALLEGRO_ALIGN_CENTRE,"Game over! Final score : %i.  Press escape key or close button to exit.",mario.score);
				al_stop_samples(); //STOPS THE SOUNDS
			}
			else if(gameover == 1 && win == true) //PLAYER WON
			{
				al_draw_textf(arial,al_map_rgb(0,0,0),width/2,height/2,ALLEGRO_ALIGN_CENTRE,"YOU WIN! Final score : %i.  Press escape key or close button to exit.",mario.score);
				al_stop_samples(); //STOPS SOUNDS
			}
			if(mario.y>=600 || collision) //IF IT HAS FALLEN BENEATH THE GROUND OR IF IT HAS COLLIDED WITH AN ENEMY
			{	
				al_play_sample(die,1.5,0,1,ALLEGRO_PLAYMODE_ONCE,NULL); //PLAYS DIE SOUND
				printf("Mario has fallen/collided!!!");
				//INITALIZES THE BOXES TO PUT COINS IN THEM
				init_box(box,boximage);
				al_rest(1.5); //RESTS FOR 1.5 s
				mario.lives--; //LIVES DECREMENTED
				mario.x = 05; // SETS MARIO POSITION TO BEGINNING OF THE SCREEN
				level = 1; //MOVES TO LEVEL1
				//SETS SCORE TO ZERO
				mario.score = 0;
				mario.y = height - 120; //INITIALIZES MARIO'S HEIGHT
				for(int i=0;i<3;i++) //AGAIN MAKES ALL ENEMIES TRUE
					enemy[i].live = true;
			}
			//bounds checking to change the level and move mario to the respective level
			if(level == 1)
			{
				if(mario.x <= 5)
					mario.x = 5;
				else if(mario.x > width -5)
				{
					level = 2;
					mario.x = 5;
				}
			}
			else if(level == 2)
			{

				if(mario.x < 5)
				{
					level = 1;
					mario.x = width - 5;
				}
				else if(mario.x >= 11*32)
					{
						mario.x = 11*32;
						win = true;
						gameover = 1;
					}
			}
			al_flip_display(); //flips back buffer to display
			redraw=false; //drawing is stopped,until another event is there
		}
	
	}
	//FILE UPDATION
	if(mario.score > previous_score)
	{
		fseek(highscore,0,SEEK_SET);
		fprintf(highscore,"%d",mario.score);
	}

	return 0;

}


void init_mario(player &mario,ALLEGRO_BITMAP *image) //initializes the mario with the values
{
	mario.x = 20;
	mario.y = height - 120;
	mario.lives = 3;
	mario.velx = 2;
	mario.vely = 0;
	mario.boundx = 4;
	mario.boundy = 1;
	mario.score = 0;

	//now for animation
	mario.maxframe = 4;
	mario.curframe = 0;
	mario.framecount = 0;
	mario.framedelay = 4; 

	mario.framewidth = 32;
	mario.frameheight = 64;
	mario.animationcolumns = 4;
	mario.animationdirection = 1;

	mario.image = image;
}



void draw_mario(player &mario) //draws mario on the screen
{
	int fx = mario.curframe * mario.framewidth; //MOVES TO A PARTICULAR FRAME IN THE SPRITE SHEET - e.g. if curframe is 2 it moves to 2*framewidth position of x in the sprite sheet
	int fy = code * mario.frameheight; //shifts row in the sprite sheet
	//draws the bitmap
	al_draw_bitmap_region(mario.image,fx,fy,mario.framewidth,mario.frameheight,mario.x - mario.framewidth/2,mario.y - mario.frameheight/2,0);

}
void move_mario_right(player &mario)
//this is the animation logic
{
	if(++mario.framecount >= mario.framedelay) //whenever framecount reaches framedelay, the frame is changed
			{
					mario.curframe++;

					if(mario.curframe >= 4)
						mario.curframe = 1;
					else if(mario.curframe <= 0) //goes from running -> standing -> running again as it looks better in the animation
						mario.curframe = 3;
					mario.framecount=0;	//again framecount starts from zero
			}
	
			mario.x += mario.velx; //mario x increases
			
		
	}

void move_mario_left(player &mario)
//same collision logic
{
	if(++mario.framecount >= mario.framedelay)
			{
					mario.curframe--;
					if(mario.curframe <= 0)
						mario.curframe = 3;
					else if(mario.curframe >= 4)
						mario.curframe = 1;
					mario.framecount=0;	
			}
	
			mario.x -= mario.velx; // mario x decreases as left movement

}



void init_brick(wall brick[],ALLEGRO_BITMAP *image) //initializes the bricks
{
	for(int i=0;i < 8;i++)
		brick[i].image = image;
	//these bricks are for level 1
	brick[0].x = 3*32;
	brick[0].y = 10*32;

	brick[1].x = 5*32;
	brick[1].y = 10*32;

	brick[2].x = 8*32;
	brick[2].y = 10*32;

	brick[3].x = 9*32;
	brick[3].y = 10*32;

	brick[4].x = 10*32;
	brick[4].y = 10*32;

	brick[5].x = 11*32;
	brick[5].y = 10*32;

	//now for level 2

	brick[6].y = 10*32;
	brick[6].x = 1*32;

	brick[7].x = 3*32;
	brick[7].y = 10*32;
}

bool checkifground(int x) //checks if ground is beneath as some portions dont have tiles
{
	if(level==1)
	{
		if(x >= 32 * 14 && x <= 32 * 15) //if mario is in this bounds, he has fallen into a tile
			return false;
		else
			return true;
	}
	else if(level == 2)
	{
		if(x >= 32 * 4 && x <= 32 * 5)
			return false;
		else
			return true;
	}
}

void init_box(coinbox box[],ALLEGRO_BITMAP *image) //initializes the coinbox
{
	for(int i=0;i<5;i++)
	{
		box[i].image = image;
		box[i].coins = 1;
	}
	box[0].x=4*32;
	box[0].y=10*32;
	box[1].x=9*32;
	box[1].y=10*32;
	box[2].x=18*32;
	box[2].y=10*32;
	box[3].x=22*32;
	box[3].y=10*32;

}


bool collide_box(coinbox box[],player &mario) //checkes collision with coinbox
{
	if(level == 1)
	{
	for(int i=0;i<4;i++)
	{
		if(mario.x >= box[i].x +10 && mario.x <= box[i].x + 32) //if mario is in bounds of that box
		{
			
			if(jump) //if mario is in the air
			{
			if(mario.y >= box[i].y +32) //if mario's y value is the same as the box's y value + 32: as box's height is 32
				{
					
					//now the mario has collided with the box
					if(box[i].coins > 0)
					{
						box[i].coins--; //decrement coins from the box
						mario.score++;
						return true;
					
					}
					else
						return false;
					
					}	
				}
			}
		}
	}
	return false;
}

void init_enemy(nonplayer enemy[],ALLEGRO_BITMAP *image) //initializes the enemy
{
	for(int i=0;i<3;i++)
	{
		enemy[i].image = image;
		enemy[i].velx = 2.5;
		enemy[i].y = 15*32;
		enemy[i].live = true;
	}
	enemy[0].x=5*32;
	enemy[1].x=19*32;
	enemy[2].x=5*32;
	enemy[2].velx = 3.5;
}

void move_enemy(nonplayer enemy[]) //alters x and y of enemy
{
	
	if (enemy[0].x < 5*32 || enemy[0].x >= 11*32) //as enemy is moving only between two points
		enemy[0].velx = -2.5;	//enemy's velocity is reversed
	if(enemy[0].x == 5*32) //again reversing the velocity
		enemy[0].velx = 2.5;
	enemy[0].x += enemy[0].velx; //changing enemy x

	//same for second enemy
	if (enemy[1].x < 17*32 || enemy[1].x >= 23*32)
		enemy[1].velx = -2.5;
	if(enemy[1].x == 17*32)
		enemy[1].velx = 2.5;
	enemy[1].x += enemy[1].velx;

	//same for third enemy
	if (enemy[2].x < 5*32 || enemy[2].x >= 11*32)
		enemy[2].velx = -2.5;
	if(enemy[2].x == 5*32)
		enemy[2].velx = 2.5;
	enemy[2].x += enemy[2].velx;
}


void collide_enemy(nonplayer enemy[],player &mario) //checks collision with enemy
{
	collision = false;
	if(level == 1)
	{
		for(int i=0;i<2;i++)
		{
			if(enemy[i].live == true) //if the enemy is true
			{
				if(mario.x >= enemy[i].x && mario.x <= enemy[i].x + 32 && mario.y == 15*32) //logic :if mario'x exceeds enemy's x AND is lesser than the enemy's with AND if mario is on the ground
					//*32 as i have taken every 32*32 pixel for easy mapping
					{
						printf("Collision between mario and enemy!\n");
						collision = true;
					}
			}
		}
		
	}
	else if(level == 2)
		{
			if(enemy[2].live == true)
			{
				if(mario.x >= enemy[2].x && mario.x <= enemy[2].x + 32 && mario.y == 15*32)
					{
						printf("Collision between mario and enemy!\n");
						collision = true;
					}
			}
		}
}

bool kill_enemy(nonplayer enemy[],player &mario) //checks if mario killed the enemy
{
	if(level == 1)
	{
		for(int i=0;i<2;i++)
		{
			if(enemy[i].live == true)
			{
				if(mario.y < 15*32)
				{
					if((mario.x + 16 >= enemy[i].x || mario.x >= enemy[i].x) && mario.x <= enemy[i].x + 32)	//if mario is in the enemy's bounds
					{
						if(mario.y + 32 >= enemy[i].y) //if mario's y co-ordinate + mario's height == enemy's y co-ordinate
						{
							printf("Enemy killed!\n");
							enemy[i].live = false;
							mario.score += 5;
							return true;
						}
					}
				}
			}
		}
	}
	else if(level == 2)
	{
		if(enemy[2].live == true)
			{
				if(mario.y < 15*32)
				{
					if((mario.x + 16 >= enemy[2].x || mario.x >= enemy[2].x) && mario.x <= enemy[2].x + 32)	
					{
						if(mario.y + 32 >= enemy[2].y) //SAME LOGIC
						{
							printf("Enemy killed!\n");
							enemy[2].live = false;
							mario.score += 5;
							return true;
						}
					}
				}
			}
	}
	return false;
}