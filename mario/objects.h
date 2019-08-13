


struct player
{
	int x;
	int y;
	int lives;
	int velx,vely;
	int boundx;
	int boundy;
	int score;

	int maxframe;
	int curframe;
	int framecount;
	int framedelay;
	int framewidth;
	int frameheight;
	int animationcolumns;
	int animationdirection;

	int animationrow;

	ALLEGRO_BITMAP *image;
};

struct nonplayer
{
	int x;
	int y;
	int velx;
	bool live;	
	ALLEGRO_BITMAP *image;
};

struct wall
{
	int x;
	int y;

	ALLEGRO_BITMAP *image;
};

struct coinbox
{
	int x;
	int y;
	int coins;

	ALLEGRO_BITMAP *image;
	
};

