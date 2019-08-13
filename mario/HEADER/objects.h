
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
	bool live;
	int velx;
	int boundx;
	int boundy;
	int framewidth;
	int frameheight;
	ALLEGRO_BITMAP *image;
};