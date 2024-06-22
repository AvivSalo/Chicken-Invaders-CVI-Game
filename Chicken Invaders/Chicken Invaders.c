//includes
#include "bass.h"
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include "Chicken Invaders.h"


//DEFINE
#define FIREAMOUNT 1000

#define MAX_COLS 20
#define MAX_ROWS 20
#define CHIK_COLS 5//10 
#define CHIK_ROWS 1//5

#define CHICKEN_COL_WIDTH 50	//chicken[row][col].x
#define CHICKEN_ROW_HEIGHT 50

//Function Prototype
void UpdateMenu();
void DrawMenu();
void updateBoss();
void drawBoss();

//struct
typedef struct
{
	int x;
	int y;
	int hit;
	int bitmap;
	int height;
	int width;
	int tick;

} Enemy;

typedef struct
{
	int top;
	int left;
	int height;
	int width;
	int Bitmap;
	int life;
	int state; //state=0 ==>regular. state=1 hit enemy .state=2 in defence
	//last hit location
	int hitX;
	int hitY;
} Warior;

typedef struct
{
	int top;
	int left;
	int height;
	int width;
	int Bitmap;
	int state; //state=0 ==>not shoot yet, state=1 ==>fire in the way to target. state=2 ==>hit the target. state=3 ==> done animation of Spark(Boom!)
	int fireTick;
} FireBall;


typedef struct
{
	int top;
	int left;
	int height;
	int width;
	int Bitmap;
	int tick;
} pictues;



//Global Varibels
static int panelHandle;

int StateInGame;
int leftClick=0, tablePress;
int ForkCoursorUP_bitmap ;
double vol;

Enemy boss, chicken[MAX_ROWS][MAX_COLS]= {{0}} ;  //0 no Chicken info in the cell
Warior SpaceShip;
pictues backpic, Chicken_pic,LogoMenu,About,ScoreTable,StartGame,Spark,ShipSpark,Bar;
FireBall fire[FIREAMOUNT];

int i=0,fireindex=0,ImageBackcounter=0;
int shot=0;
int AnimationTick=0;
int	ChikenAnimationTick;
double ch_vx=80.0;
int DeadChicken;

HSTREAM MainTheme_SND;
HSTREAM Boss_SND;
HSTREAM Fire_SND; //Fire.mp3
HSTREAM Eat_SND; //Eat.mp3
HSTREAM	Crash_SND; //Crash.mp3


//function

void draw()
{

	AnimationTick++;

	//Canvas
	CanvasStartBatchDraw(panelHandle, PANEL_CANVAS);

	CanvasClear (panelHandle, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
	SetPanelAttribute (panelHandle, ATTR_MOUSE_CURSOR, VAL_HIDDEN_CURSOR);

	//Loop BackPic  ImageBackcounter
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(backpic.top+ImageBackcounter,backpic.left,backpic.height ,backpic.width));
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(-backpic.height+ImageBackcounter,backpic.left,backpic.height ,backpic.width));


	//Chicken
	for (int row=0 ; row<CHIK_ROWS ; row++)
	{
		for (int col=0 ; col<CHIK_COLS ; col++)
		{
			//CanvasDrawBitmap (panelHandle, PANEL_CANVAS, chicken[row][col].Bitmap, MakeRect ((AnimationTick%5)*row*chicken[row][col].height, 0, chicken[row][col].height, chicken[row][col].width),MakeRect (chicken[row][col].top, chicken[row][col].left, chicken[row][col].height, chicken[row][col].width) );
			//CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Chicken_pic.Bitmap, MakeRect (row*CHICKEN_ROW_HEIGHT, 0, CHICKEN_ROW_HEIGHT, CHICKEN_COL_WIDTH), MakeRect (chicken[row][col].y, chicken[row][col].y, CHICKEN_ROW_HEIGHT, CHICKEN_COL_WIDTH)); //chicken[row][col].x  or chicken[row][col].y is the  hight left cornert of the pic
			if ( chicken[row][col].hit==0)
				CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Chicken_pic.Bitmap, VAL_ENTIRE_OBJECT,MakeRect(chicken[row][col].y,chicken[row][col].x,CHICKEN_ROW_HEIGHT ,CHICKEN_COL_WIDTH) );

		}
	}

	//spaceship Animation
	if (SpaceShip.state==0 || SpaceShip.state==2) //state=0 ==>regular. state=1 hit enemy .state=2 in defence
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, SpaceShip.Bitmap, MakeRect (0, (AnimationTick%25)*100, SpaceShip.height, SpaceShip.width),MakeRect(SpaceShip.top,SpaceShip.left-(SpaceShip.width/2),SpaceShip.height ,SpaceShip.width) );
	if (SpaceShip.state==1) //hit
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, ShipSpark.Bitmap, MakeRect (0, (ShipSpark.tick%35)*ShipSpark.width, ShipSpark.height, ShipSpark.width),MakeRect( SpaceShip.hitY ,SpaceShip.hitX ,ShipSpark.height ,ShipSpark.width) );  //35 frames
		ShipSpark.tick++;
		if (ShipSpark.tick==35)
		{
			SpaceShip.state=0;// done animation of the Boom for the SpaceShip
			ShipSpark.tick=0;
		}
	}

	//FireBalls
	for(i=0; i<FIREAMOUNT; i++)
	{
		if(fire[i].state==1) //fire is in the way to target
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, fire[i].Bitmap, VAL_ENTIRE_OBJECT,MakeRect(fire[i].top,fire[i].left-(fire[i].width/2),fire[i].height ,fire[i].width) );
		if(fire[i].state==2) //hit the target and animate the boom
		{
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Spark.Bitmap, MakeRect (0, (fire[i].fireTick%11)*Spark.width, Spark.height, Spark.width),MakeRect(fire[i].top-10,fire[i].left-(fire[i].width/2),Spark.height, Spark.width) );  //11 is the number of frames
			fire[i].fireTick++;
			if (fire[i].fireTick==11)
				fire[i].state=3;// done animation of the Boom
		}
	}
	i=0;

	//Bar
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Bar.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(backpic.height-Bar.height,0,Bar.height, Bar.width));
	SetCtrlVal (panelHandle, PANEL_LIFE, SpaceShip.life);

	CanvasEndBatchDraw(panelHandle, PANEL_CANVAS);

}

void update()
{
	//counter for rolling the BackPicture
	ImageBackcounter++;
	ImageBackcounter=ImageBackcounter%backpic.height;

	//get mouse location
	GetRelativeMouseState (panelHandle, PANEL_CANVAS, &SpaceShip.left, &SpaceShip.top, 0, 0, 0);

	//Bar
	//(all data in the Draw Function)

	//SpaceShip logics
	for (int row=0 ; row<CHIK_ROWS ; row++)
	{
		for (int col=0 ; col<CHIK_COLS ; col++)
		{
			/*if (SpaceShip.top+10+SpaceShip.height/5<=(chicken[row][col].y+CHICKEN_ROW_HEIGHT-5)
					&&
					SpaceShip.top+10>=(chicken[row][col].y)
					&&
					SpaceShip.left-(SpaceShip.width/5) >= (chicken[row][col].x)
					&&
					SpaceShip.left+SpaceShip.width/5 <= (chicken[row][col].x+CHICKEN_COL_WIDTH)
					&& SpaceShip.state==0
					&& chicken[row][col].hit==0)*/
			if (abs((SpaceShip.top+10+SpaceShip.height/5)-(chicken[row][col].y+CHICKEN_ROW_HEIGHT-5))<=10
					&&
					abs((SpaceShip.left-(SpaceShip.width/5))-(chicken[row][col].x))<= 25
					&&
					SpaceShip.state==0
					&&
					chicken[row][col].hit==0) //SpaceShip collaps into chicken ,while Spaceship & Chicken is alive

			{
				SpaceShip.state=1; //state=0 ==>regular. state=1 hit enemy .state=2 in defence
				SpaceShip.hitX =  SpaceShip.left-(SpaceShip.width/5)-15;
				SpaceShip.hitY =  SpaceShip.top+10;
				SpaceShip.life--;
				BASS_ChannelPlay(Crash_SND,TRUE);

				if(SpaceShip.life==0)//Warior killed
				{
					StateInGame=1;//menu 
					SetCtrlAttribute (panelHandle, PANEL_VOLUME, ATTR_VISIBLE, 1); //show volume Control
					SetCtrlAttribute (panelHandle, PANEL_POLKE_NUMERIC, ATTR_VISIBLE, 0); //Stop show collected Point for the game
					SetCtrlAttribute (panelHandle, PANEL_LIFE, ATTR_VISIBLE, 0); //Stop show life points for the game

					//update the Table
					//
					//
					//
					//
				}


			}

		}
	}


	//Chicken moving calc
	//Chicken Horizintal moving										    	  //or
	if(  ((chicken[0][0].x+CHIK_COLS*CHICKEN_COL_WIDTH+100) >= backpic.width)  ||  ((chicken[0][0].x-100) <= 0))
		ch_vx = -ch_vx;
	//move right or left
	for (int row=0 ; row<CHIK_ROWS ; row++)
	{
		for (int col=0 ; col<CHIK_COLS ; col++)
		{
			chicken[row][col].x = chicken[row][col].x + 0.05*ch_vx;
		}
	}
	//Chicken move verticly as a sin(y)
	//Note: sin() in that case is not symetric function,due to INTEGERS VALUE so it is always get a small amout of values from the possitive side of
	//the sin() or on the other hande it have the negative values. Thats is the reason for the UP and DOWN cases below in the code.
	//UP
	if( (chicken[0][0].y >= 20)  ||  ((chicken[0][0].y+CHIK_ROWS*CHICKEN_ROW_HEIGHT) <= 500) )

	{
		for (int row=0 ; row<CHIK_ROWS ; row++)
		{
			for (int col=0 ; col<CHIK_COLS ; col++)
			{
				chicken[row][col].y = chicken[row][col].y + (8.0)*sin((double)(ChikenAnimationTick/3.0));
			}
		}
	}
	//DOWN
	if(     (chicken[0][0].y <= 20)  ||  ((chicken[0][0].y+CHIK_ROWS*CHICKEN_ROW_HEIGHT) >= 500)    )
	{

		for (int row=0 ; row<CHIK_ROWS ; row++)
		{
			for (int col=0 ; col<CHIK_COLS ; col++)
			{
				chicken[row][col].y = chicken[row][col].y - (8.0)*sin((double)(ChikenAnimationTick/3.0));
			}
		}
	}
	ChikenAnimationTick++;



	//Shoots Logics
	if (shot == 1) //Left Click pressed - user want for fire the chicken //and not in the menu
	{
		shot=0;
		BASS_ChannelPlay(Fire_SND,TRUE);
		fire[fireindex].state=1;  //state=0 ==>not shoot yet, state=1 ==>fire in the way to target. state=2 ==>hit the target. state=3 ==> done animation of Spark(Boom!)
		fire[fireindex].top =  SpaceShip.top;
		fire[fireindex].left = SpaceShip.left;
		fireindex++;
	}
	for (i=0 ; i<FIREAMOUNT ; i++)
	{
		if(fire[i].state==1)
		{
			//1
			fire[i].top = fire[i].top - 5;   //the Fire ball is been shooted ==> (Walk Throw the Target) move forward the fireball to the chiken

			//2
			for (int row=0 ; row<CHIK_ROWS ; row++)
			{
				for (int col=0 ; col<CHIK_COLS ; col++)
				{
					if (
						chicken[row][col].hit==0
						&&
						fire[i].top <= (chicken[row][col].y+CHICKEN_ROW_HEIGHT)
						&&
						fire[i].top >= chicken[row][col].y
						&&
						fire[i].left >= chicken[row][col].x
						&&
						fire[i].left <= (chicken[row][col].x+CHICKEN_COL_WIDTH)
					)
					{
						fire[i].state=2;   // state=2 ==> hit the target
						chicken[row][col].hit=1;
						DeadChicken--;
						if( DeadChicken==0)//stage 1 complete ==>move to Boss
						{
							StateInGame=3; //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level

							//music for Boss
							BASS_ChannelStop(MainTheme_SND);
							BASS_ChannelPlay(Boss_SND,TRUE);  //Boss Fight.mp3

						}

					}
				}
			}
		}
	}


}


void initilize()
{
	//Menu
	StateInGame=1;  //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level

	GetBitmapFromFile ("Images\\ForkCoursorUP.png", &ForkCoursorUP_bitmap);
	//GetBitmapFromFile ("Images\\ForkCoursorDOWN.png", &ForkCoursorDOWN_bitmap);
	GetBitmapFromFile ("Images\\MainLOGO.png", &LogoMenu.Bitmap);
	LogoMenu.width=500;
	LogoMenu.height=236;
	GetBitmapFromFile ("Images\\Credits.png", &About.Bitmap);
	About.width = 260;
	About.height = 54;
	GetBitmapFromFile ("Images\\ScoreTable.png", &ScoreTable.Bitmap);
	ScoreTable.width=260;
	ScoreTable.height=54;
	GetBitmapFromFile ("Images\\SaveTheWorld.png", &StartGame.Bitmap);   //SaveTheWorld.png
	StartGame.width=260;
	StartGame.height=54;
	GetBitmapFromFile ("Images\\Spark.png", &Spark.Bitmap);  //Spark.png
	Spark.height=50;
	Spark.width=50;
	GetBitmapFromFile ("Images\\ShipSpark.png", &ShipSpark.Bitmap); //ShipSpark.png
	ShipSpark.height=70;
	ShipSpark.width=70;
	GetBitmapFromFile ("Images\\Bar.png", &Bar.Bitmap); //Bar.png
	Bar.height=52;
	Bar.width=162;


	//srand (time(0));		 //for randomize features

	//MUSIC initilize
	BASS_Init( -1,44100, 0,0,NULL);
	MainTheme_SND = BASS_StreamCreateFile(FALSE,"Sounds\\Main Theme.mp3",0,0,0);
	Boss_SND = BASS_StreamCreateFile(FALSE,"Sounds\\Boss Fight.mp3",0,0,0);
	Fire_SND = BASS_StreamCreateFile(FALSE,"Sounds\\Fire.mp3",0,0,0);		
	Eat_SND = BASS_StreamCreateFile(FALSE,"Sounds\\Eat.mp3",0,0,0);
	Crash_SND = BASS_StreamCreateFile(FALSE,"Sounds\\Crash.mp3",0,0,0);


	//backPicture parameters
	GetBitmapFromFile ("Images\\backImage.jpg", &backpic.Bitmap);
	backpic.top=0;
	backpic.left = 0;
	GetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_HEIGHT, &backpic.height)  ;
	GetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_WIDTH, &backpic.width);


	//Spacecraft animation
	GetBitmapFromFile ("Images\\craf25fps.png", &SpaceShip.Bitmap);
	SpaceShip.height = 170;
	SpaceShip.width = 100 ;
	SpaceShip.top = (backpic.height)-(SpaceShip.height);
	SpaceShip.left = (backpic.width/2)-(SpaceShip.width/2);
	SpaceShip.life =3;
	SpaceShip.state=0; //state=0 ==>regular. state=1 hit enemy .state=2 in defence

	//Chicken
	DeadChicken= CHIK_ROWS*CHIK_COLS;
	GetBitmapFromFile ("Images\\chickenRed.png",&Chicken_pic.Bitmap);   //100 pixel X 100 pixel
	for (int row=0 ; row<CHIK_ROWS ; row++)
	{
		for (int col=0 ; col<CHIK_COLS ; col++)
		{
			chicken[row][col].hit=0;
			chicken[row][col].x= 150+CHICKEN_COL_WIDTH*col;
			chicken[row][col].y= 50+CHICKEN_ROW_HEIGHT*row;
		}
	}

	//Boss
	GetBitmapFromFile ("Images\\Boss.png",&boss.bitmap);   //100 pixel X 100 pixel
	boss.hit=15;	//Boss.png
	boss.width = 180;
	boss.height = 230;
	boss.x = backpic.width/2 - boss.width/2;
	boss.y = 10;

	//FireBall
	for (i=0 ; i<FIREAMOUNT ; i++)
	{
		GetBitmapFromFile ("Images\\Fire1.png", &fire[i].Bitmap);
		fire[i].height=37;
		fire[i].width=33;
	}
	i=0;

	//Sounds
	vol=0.2; //Volume first state
	BASS_ChannelSetAttribute(MainTheme_SND, BASS_ATTRIB_VOL, vol/10.0);
	BASS_ChannelSetAttribute(Boss_SND, BASS_ATTRIB_VOL, vol/10.0);
	BASS_ChannelSetAttribute(Fire_SND, BASS_ATTRIB_VOL, vol);
	BASS_ChannelSetAttribute(Eat_SND, BASS_ATTRIB_VOL, vol);
	BASS_ChannelSetAttribute(Crash_SND, BASS_ATTRIB_VOL, vol);
	BASS_ChannelPlay(MainTheme_SND,TRUE); 

}


void terminate()
{
	//Free Bitmap
	DiscardBitmap(backpic.Bitmap);
	DiscardBitmap(SpaceShip.Bitmap);
	for  (i=0 ; i<FIREAMOUNT ; i++)
	{
		DiscardBitmap(fire[i].Bitmap);
	}
	DiscardBitmap(Chicken_pic.Bitmap);
	DiscardBitmap(ForkCoursorUP_bitmap);
	DiscardBitmap(LogoMenu.Bitmap);
	DiscardBitmap(About.Bitmap );
	DiscardBitmap(ScoreTable.Bitmap);
	DiscardBitmap(StartGame.Bitmap);
	DiscardBitmap(Spark.Bitmap);
	DiscardBitmap(ShipSpark.Bitmap);

	//Free Sounds
	BASS_StreamFree(MainTheme_SND);
	BASS_StreamFree(Boss_SND);
	BASS_StreamFree(Fire_SND);
	BASS_StreamFree(Eat_SND);
	BASS_StreamFree(Crash_SND);

}


void UpdateMenu()
{
	GetRelativeMouseState (panelHandle, PANEL_CANVAS, &SpaceShip.left, &SpaceShip.top, 0, 0, 0);

	//Select option on the menu
	if (leftClick == 1) //Left Click pressed and in the menu
	{
		leftClick = 0;

		//Cursor on Credis Button
		if (SpaceShip.top>=(backpic.height/3*2)
				&&
				SpaceShip.top<=(backpic.height/3*2+About.height)
				&&
				SpaceShip.left>= ((backpic.width/2)-( About.width/2))
				&&
				SpaceShip.left <= ((backpic.width/2)+( About.width/2))
				&& tablePress==0)
			MessagePopup ("Abouut", "Aviv Salomon\n 206282576");

		//  Cursor on Hall of Fame (score table) Button
		if (SpaceShip.top>=(backpic.height/3*2+ScoreTable.height)
				&&
				SpaceShip.top<=(backpic.height/3*2+ScoreTable.height+ScoreTable.height)
				&&
				SpaceShip.left>= ((backpic.width/2)-( ScoreTable.width/2))
				&&
				SpaceShip.left <= ((backpic.width/2)+( ScoreTable.width/2)) )
		{
			tablePress++;
			tablePress = tablePress%2;

			if(tablePress==1)
				SetCtrlAttribute (panelHandle, PANEL_TABLE, ATTR_VISIBLE, tablePress); //show table
			if(tablePress==0)
				SetCtrlAttribute (panelHandle, PANEL_TABLE, ATTR_VISIBLE, tablePress); //hide table
		}
		//user click "StartGame"
		if (SpaceShip.top>=(StartGame.height/3*2-StartGame.height)
				&&
				SpaceShip.top<=(backpic.height/3*2)
				&&
				SpaceShip.left>= ((backpic.width/2)-( StartGame.width/2))
				&&
				SpaceShip.left <= ((backpic.width/2)+( StartGame.width/2))
				&& tablePress==0)
		{
			//Reset SpaceShip Life, Chicken hits,fire[counter] 
			SpaceShip.life =3;
			SpaceShip.state=0; //state=0 ==>regular. state=1 hit enemy .state=2 in defence
			
			//Reset boss hits and location
			boss.hit=15;
			boss.x = backpic.width/2 - boss.width/2;
			boss.y = 10;
			
			//Reset the Fires Arry
			for (i=0 ; i<FIREAMOUNT ; i++)	  
			{
			 fire[i].fireTick=0;
			 fire[i].state=0;
			}
			i=0;
			fireindex =0; 
			
			//Reset Chicken
			DeadChicken = CHIK_ROWS*CHIK_COLS; //reset the countdown to the first amount

			for (int row=0 ; row<CHIK_ROWS ; row++)
			{
				for (int col=0 ; col<CHIK_COLS ; col++)
				{
					chicken[row][col].hit=0;
					chicken[row][col].x= 150+CHICKEN_COL_WIDTH*col;
					chicken[row][col].y= 50+CHICKEN_ROW_HEIGHT*row;
				}
			}


			SetCtrlAttribute (panelHandle, PANEL_VOLUME, ATTR_VISIBLE, 0); //Stop show Volume Control
			SetCtrlAttribute (panelHandle, PANEL_POLKE_NUMERIC, ATTR_VISIBLE, 1); //Show collected Point for the game
			SetCtrlAttribute (panelHandle, PANEL_LIFE, ATTR_VISIBLE, 1); //Show life points for the game

			StateInGame=2; //Start The game  //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level
		}

	}

}
void DrawMenu()
{
	AnimationTick++;

	//Canvas
	CanvasStartBatchDraw(panelHandle, PANEL_CANVAS);

	CanvasClear (panelHandle, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
	SetPanelAttribute (panelHandle, ATTR_MOUSE_CURSOR, VAL_HIDDEN_CURSOR);

	//Loop BackPic  ImageBackcounter
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(backpic.top+((AnimationTick)%backpic.height),backpic.left,backpic.height ,backpic.width));
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(-backpic.height+((AnimationTick)%backpic.height),backpic.left,backpic.height ,backpic.width));

	if(tablePress==0)
	{
		//about
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS,  About.Bitmap, VAL_ENTIRE_OBJECT,MakeRect(backpic.height/3*2,(backpic.width/2)-( About.width/2), About.height, About.width));
		//StartGame botton  //StartGame
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS,  StartGame.Bitmap, VAL_ENTIRE_OBJECT,MakeRect(backpic.height/3*2-StartGame.height,(backpic.width/2)-( StartGame.width/2), StartGame.height, StartGame.width));
	}

	//Score Table - Hall of fame	 //ScoreTable
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS,  ScoreTable.Bitmap, VAL_ENTIRE_OBJECT,MakeRect(backpic.height/3*2+ScoreTable.height,(backpic.width/2)-( ScoreTable.width/2), ScoreTable.height, ScoreTable.width));

	//Cursor Fork
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, ForkCoursorUP_bitmap, VAL_ENTIRE_OBJECT,MakeRect(SpaceShip.top,SpaceShip.left-(44/5),55,44)); //for ForkCoursor:(width 44,height 55)

	//Logo
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, LogoMenu.Bitmap, VAL_ENTIRE_OBJECT,MakeRect(20,(backpic.width/2)-(LogoMenu.width/2),LogoMenu.height,LogoMenu.width));

	CanvasEndBatchDraw(panelHandle, PANEL_CANVAS);
}



void updateBoss()
{


	//get mouse location
	GetRelativeMouseState (panelHandle, PANEL_CANVAS, &SpaceShip.left, &SpaceShip.top, 0, 0, 0);

	//Bar
	//(all data in the Draw Function)

	//SpaceShip logics
	if (abs((SpaceShip.top+10+SpaceShip.height/5)-(boss.y))<=10
			&&
			abs((SpaceShip.left-(SpaceShip.width/5))-(boss.x))<= 25
			&&
			SpaceShip.state==0
			&&
			boss.hit ==0) //SpaceShip collaps into Boss ,while Spaceship & Boss is alive

	{
		SpaceShip.state=1; //state=0 ==>regular. state=1 hit enemy .state=2 in defence
		SpaceShip.hitX =  SpaceShip.left-(SpaceShip.width/5)-15;
		SpaceShip.hitY =  SpaceShip.top+10;
		SpaceShip.life--;
		BASS_ChannelPlay(Crash_SND,TRUE);
		
		if(SpaceShip.life==0)//Warior killed
		{
			StateInGame=1;//menu
			SetCtrlAttribute (panelHandle, PANEL_VOLUME, ATTR_VISIBLE, 1); //show volume Control
			SetCtrlAttribute (panelHandle, PANEL_POLKE_NUMERIC, ATTR_VISIBLE, 0); //Stop show collected Point for the game
			SetCtrlAttribute (panelHandle, PANEL_LIFE, ATTR_VISIBLE, 0); //Stop show life points for the game

			//update the Table
			//
			//
			//
			//
		}


	}


	//Boss logic moving algorithm
	//Explanation: X = X + (Velocity Factor)*(unit Vector) ==> unit Vector = (Delta of X)/(Size of the Vector) ==> Size of the Vector is Pythagoras equation 
	boss.x= boss.x +(int)((25)*(((SpaceShip.left-(SpaceShip.width/2))-boss.x)/sqrt(pow(boss.x,2)+pow((SpaceShip.left-(SpaceShip.width/2)),2))));
	boss.y= boss.y +(int)((25)*((SpaceShip.top-boss.y)/sqrt(pow(boss.y,2)+pow(SpaceShip.top,2))));


	//Shoots Logics
	if (shot == 1) //Left Click pressed - user want to fire the chicken //and not in the menu
	{
		shot=0;
		BASS_ChannelPlay(Fire_SND,TRUE);
		fire[fireindex].state=1;  //state=0 ==>not shoot yet, state=1 ==>fire in the way to target. state=2 ==>hit the target. state=3 ==> done animation of Spark(Boom!)
		fire[fireindex].top =  SpaceShip.top;
		fire[fireindex].left = SpaceShip.left;
		fireindex++;
	}
	for (i=0 ; i<FIREAMOUNT ; i++)
	{
		if(fire[i].state==1)//state=1 ==>fire in the way to target
		{
			//1
			fire[i].top = fire[i].top - 5;   //the Fire ball is been shooted ==> (Walk Throw the Target) move forward the fireball to the chiken

			//2
			if (boss.hit > 0
				&&
				fire[i].top <= (boss.y+boss.height-70)
				&&
				fire[i].top >= boss.y
				&&
				fire[i].left >= boss.x
				&&
				fire[i].left <= (boss.x+boss.width)
				)
					{
					 fire[i].state=2;   // state=2 ==> hit the target
					 boss.hit--;
					 if(boss.hit==0)//Boss stage complete ==> move to menu
					   {
						//music for Menu
						BASS_ChannelPlay(Crash_SND,TRUE); 
						BASS_ChannelStop(Boss_SND); 
						BASS_ChannelPlay(MainTheme_SND,TRUE); 
						SetCtrlAttribute (panelHandle, PANEL_VOLUME, ATTR_VISIBLE, 1); //show volume Control
						SetCtrlAttribute (panelHandle, PANEL_POLKE_NUMERIC, ATTR_VISIBLE, 0); //Stop show collected Point for the game
						SetCtrlAttribute (panelHandle, PANEL_LIFE, ATTR_VISIBLE, 0); //Stop show life points for the game
						StateInGame=1; //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level
					   }
					}
		}
	}


}


void drawBoss()
{

	//counter for rolling the BackPicture
	ImageBackcounter++;
	ImageBackcounter=ImageBackcounter%backpic.height;
	
	AnimationTick++;

	CanvasStartBatchDraw(panelHandle, PANEL_CANVAS);

	CanvasClear (panelHandle, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
	SetPanelAttribute (panelHandle, ATTR_MOUSE_CURSOR, VAL_HIDDEN_CURSOR);

	//Loop BackPic  ImageBackcounter
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(backpic.top+ImageBackcounter,backpic.left,backpic.height ,backpic.width));
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, backpic.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(-backpic.height+ImageBackcounter,backpic.left,backpic.height ,backpic.width));


	//Boss
	boss.tick++;
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, boss.bitmap, MakeRect (0, (boss.tick%17)*boss.width, boss.height, boss.width),MakeRect(boss.y,boss.x,boss.height ,boss.width) ); //17 frames
		  

	//spaceship Animation
	if (SpaceShip.state==0 || SpaceShip.state==2) //state=0 ==>regular. state=1 hit enemy .state=2 in defence
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, SpaceShip.Bitmap, MakeRect (0, (AnimationTick%25)*100, SpaceShip.height, SpaceShip.width),MakeRect(SpaceShip.top,SpaceShip.left-(SpaceShip.width/2),SpaceShip.height ,SpaceShip.width) );
	if (SpaceShip.state==1) //hit
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, ShipSpark.Bitmap, MakeRect (0, (ShipSpark.tick%35)*ShipSpark.width, ShipSpark.height, ShipSpark.width),MakeRect( SpaceShip.hitY ,SpaceShip.hitX ,ShipSpark.height ,ShipSpark.width) );  //35 frames
		ShipSpark.tick++;
		if (ShipSpark.tick==35)
		{
			SpaceShip.state=0;// done animation of the Boom for the SpaceShip
			ShipSpark.tick=0;
		}
	}

	//FireBalls
	for(i=0; i<FIREAMOUNT; i++)
	{
		if(fire[i].state==1) //fire is in the way to target
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, fire[i].Bitmap, VAL_ENTIRE_OBJECT,MakeRect(fire[i].top,fire[i].left-(fire[i].width/2),fire[i].height ,fire[i].width) );
		if(fire[i].state==2) //hit the target and animate the boom
		{
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Spark.Bitmap, MakeRect (0, (fire[i].fireTick%11)*Spark.width, Spark.height, Spark.width),MakeRect(fire[i].top-10,fire[i].left-(fire[i].width/2),Spark.height, Spark.width) );  //11 is the number of frames
			fire[i].fireTick++;
			if (fire[i].fireTick==11)
				fire[i].state=3;// done animation of the Boom
		}
	}
	i=0;

	//Bar
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Bar.Bitmap, VAL_ENTIRE_OBJECT, MakeRect(backpic.height-Bar.height,0,Bar.height, Bar.width));
	SetCtrlVal (panelHandle, PANEL_LIFE, SpaceShip.life);

	CanvasEndBatchDraw(panelHandle, PANEL_CANVAS);
}

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Chicken Invaders.uir", PANEL)) < 0)
		return -1;
	initilize();
	DisplayPanel (panelHandle);
	RunUserInterface ();
	terminate();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK exitFunc (int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK CanvasClickFunc (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{

	switch(event)
	{
		case EVENT_LEFT_CLICK:
		{
			//if in the game ,and not the menu
			if(StateInGame==2 || StateInGame==3)  //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level
				shot=1;

			//if in the menu
			if(StateInGame == 1)  //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level
				leftClick=1;
		}
		break;


	}

	return 0;
}

int CVICALLBACK timerFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:

			switch (StateInGame)  //StateInGame==> 1= Menu. 2=play the first level.  3=Boss level
			{
				case 1: //Menu
					UpdateMenu();
					DrawMenu();
					break;

				case 2: //play the first level
					update();
					draw();
					break;
				case 3: //Boss level
					updateBoss();
					drawBoss();
					break;
			}

			break;
	}
	return 0;
}

int CVICALLBACK VolumeFunc (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:

			GetCtrlVal (panelHandle, PANEL_VOLUME, &vol);
			BASS_ChannelSetAttribute(MainTheme_SND, BASS_ATTRIB_VOL, vol/10.0);
			BASS_ChannelSetAttribute(Boss_SND, BASS_ATTRIB_VOL, vol/10.0);
			BASS_ChannelSetAttribute(Fire_SND, BASS_ATTRIB_VOL, vol);
			BASS_ChannelSetAttribute(Eat_SND, BASS_ATTRIB_VOL, vol);
			BASS_ChannelSetAttribute(Crash_SND, BASS_ATTRIB_VOL, vol);
				
			break;
	}
	return 0;
}
