// ---- Hungry Caterpillar ----
/* Copyright 2019 Tom Millichamp
 *  A game based on the old snake game
 *  Tom Millichamp 29/12/2019
 *  Uses C++ Standard Library from https://github.com/maniacbug/StandardCplusplus
 *  download, unpack into your sketchbook \libraries folder
 *  also you need to downgrade "Arduino AVR Boards" in your IDE for the Std library to work:
 *  Arduino IDE: Tools > Board > Boards Manager
 *  Wait for downloads to finish.
 *  When you move the mouse pointer over "Arduino AVR Boards", you will see a "Select version" dropdown menu appear. 
 *  Select "1.6.20". Click "Install".
 *  Wait for installation to finish. Click "Close". Now you can compile this game.
*/
#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <vector>
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <ArduboyTonesPitches.h>
using namespace std; // need the standard C++ lib for Vectors

//-------------Global declarations----------------
Arduboy2 arduboy; //instantiate the arduboy object
ArduboyTones sound(arduboy.audio.enabled); //enable sound (Arduboy Tones lib)

// Constants:
// Playing Area (boundaries)
#define SCR_TOP 8
#define SCR_BOTTOM (HEIGHT-2)
#define SCR_LEFT 2
#define SCR_RIGHT (WIDTH-2)
//define number of cabbages for each game
#define num_of_cabbages 3

// Segment Class - these make up the caterpillar
class Segment
  {
    public:
        int x,y;
    public:
        Segment (int vx, int vy){
            x=vx; y=vy;
        }
  };

vector<Segment> Snake;  // This is our caterpillar object (a vector Array made up of segments)
Rect snakeRect;         // This is the arduboy Rect object used to identify the caterpillar head for collisions

//Array for our cabbages for caterpillar to eat
Rect cabbages[num_of_cabbages]; // Cabbages are Rect so that we can use the .collide method on them

// Lets have a Rect for the bonus that reduces the caterpillar by 1 segment & slows it down!
Rect bonus;

// Variables
int frame_rate = 16;         // starting frame rate
int frame_rate_normal = 16;  // used to adjust snake speed in the game
int frame_rate_easy = 14; 
int snakex; int snakey;       // These hold our head of the caterpillar position through the game
int score=0;
int hi_score=0;
int level=1;
int bonus_random;             // a random time for the bonus triangle to appear!
int counter = 0;              // just used for flashing text in title screen
enum gamestates {title, gameplay, lose, pause, level2} gamestate;
enum directions {UP, DOWN, RIGHT, LEFT} direction; 
bool del_segment;             // This is used to say whether to not delete a segment when moving (grow caterpillar)
String sounds="on";           // Toggle sound is on or off
String game_mode="normal";    // either 'normal' or 'easy' game mode

//block image for border when the game gets harder
const unsigned char PROGMEM block[] = {
  // width, height,
  8, 8,
  0x00, 0x4a, 0x20, 0x14, 0x40, 0x12, 0x48, 0x00,
  };
unsigned char const Hungry_Caterpillar[] PROGMEM =
{
  40, 40,
  // 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xb0, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 
  0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0x7e, 0x7f, 0x7f, 
  0x7f, 0x7f, 0x7f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x60, 0x30, 0x1c, 0x0e, 0x06, 0x0e, 0xa4, 
  0xe7, 0xc7, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xfd, 0xc9, 0xc1, 0xed, 0x8c, 0x0e, 0x06, 0x06, 
  0x06, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
  0xff, 0xcf, 0x0d, 0x18, 0x1e, 0x3e, 0x7d, 0x7f, 
  0x7f, 0x6f, 0x5f, 0x5f, 0x6f, 0x7f, 0x7f, 0x3d, 
  0x1e, 0x0e, 0x88, 0xfd, 0x3f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x03, 0x07, 0x0e, 0x0c, 0x1c, 0x18, 
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0e, 
  0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};


//-------------------Setup---------------------
void setup() {
      arduboy.begin();  // initialize Arduboy
      arduboy.clear(); // clear screen
      if (sounds=="on")
        arduboy.audio.on();
      arduboy.setFrameRate(frame_rate);
      score=0;counter=0; level=1;
      arduboy.initRandomSeed();
      snakex=random(SCR_LEFT + 8, SCR_RIGHT - 50); //start caterpillar in a random location
      snakey=random(SCR_TOP + 20, SCR_BOTTOM - 12);
      direction = RIGHT; // Start caterpillar moving right
      
      //create an intial caterpillar of length 3 segments:
      Snake.clear(); //This clears the entire caterpillar Vector Array for us!
      Snake.push_back(Segment(snakex,snakey)); //head of caterpillar
      Snake.push_back(Segment(snakex-4,snakey)); // 2nd segment
      Snake.push_back(Segment(snakex-8,snakey)); // 3rd segment
      
      // Setup cabbages for him to eat!
      for (int i = 0; i < num_of_cabbages; i++) {
        cabbages[i] = {random(SCR_LEFT + 8, SCR_RIGHT - 8), random(SCR_TOP + 8, SCR_BOTTOM - 8), 4, 4};
        }
  
      // Create the bonus initial position randomly:
      bonus = {random(SCR_LEFT + 8, SCR_RIGHT - 8), random(SCR_TOP + 8, SCR_BOTTOM - 8), 4, 4};

      //show splash screen graphics for 2 secs
        Sprites::drawOverwrite(44, 0, Hungry_Caterpillar, 0);
        arduboy.setCursor(42, 42);
        arduboy.print(F("Hungry"));
        arduboy.setCursor(32, 52);
        arduboy.print(F("Caterpillar!"));
        arduboy.display();
        delay(2000);
        
      // Start at Title Screen
      gamestate = title;
}


//-------------------------------------------------
//-------------------Main Loop---------------------
void loop() {
    del_segment = true; // set to true to make caterpillar move by removing tail segment & adding head
    
    // waits for the next frame to be ready before proceeding
    if (!arduboy.nextFrame())
      return;      
    arduboy.clear();
    arduboy.pollButtons();
    
    
    switch(gamestate) { 
      //-------------------Title Screen---------------------
      case title:
        arduboy.setCursor(0,0);
        arduboy.print("Game Mode: " + game_mode);
        arduboy.print(F("\nUP/DWN - change mode"));
        arduboy.print("\nB - Sound: " + sounds);
        //flash the next bit of text on & off
        counter++; // just counting number of frames that have passed
        if(counter < 25)
            arduboy.print(F("\nA - Start/Pause Game"));
        else
            arduboy.print(F("\n"));
        if(counter >= 35) 
            counter = 0; //If counter is greater than or less than 120, reset it back to 0
        arduboy.print(F("\nBonus cabbage shrinks"));
        arduboy.print(F("\nand slows Caterpillar"));
        
        // Change Game Difficuly Toggle - using up and down button.
        if(arduboy.justPressed(UP_BUTTON) || arduboy.justPressed(DOWN_BUTTON))
          game_mode = (game_mode=="normal") ? "easy" : "normal" ;

        // B Button Toggle Sound On/Off
        if (arduboy.justPressed(B_BUTTON)) {
          sounds = (sounds=="off") ? "on" : "off";
          arduboy.audio.toggle();
        }

        //Start the game if they press A
        if (arduboy.justPressed(A_BUTTON)) {
            if (game_mode=="normal"){
                frame_rate=frame_rate_normal;
                // random number for when bonus starts to appear (no. of cabbages)
                bonus_random = 14 + random(1,6); 
                }
              else{
                frame_rate=frame_rate_easy;
                bonus_random = 8 + random(1,6); //bonus appears quicker on easy mode
                Snake.pop_back(); // remove segment, just start with 2!
                }
          gamestate = gameplay;
        }
        break;

  
      //-------------------Main Game Loop---------------------
      case gameplay:
        arduboy.setFrameRate(frame_rate); //set here as changes through game to control speed of snake

        //update score:
        arduboy.setCursor(0,0);
        arduboy.print(F("Score:")); arduboy.print(score);
        arduboy.print(F("    High:")); arduboy.print(hi_score);
        if (level==2)  
            draw_border();  // shrink the play area to make it harder!
        //draw cabbages:
        for ( int i = 0; i < num_of_cabbages; i++ ) 
          arduboy.drawCircle (cabbages[i].x,cabbages[i].y, 2, WHITE);
                   
        //move the bonus position every x frames!
        if(arduboy.everyXFrames(93-(level*3))){
            bonus = {random(SCR_LEFT + 8, SCR_RIGHT - 8), random(SCR_TOP + 8, SCR_BOTTOM - 8), 4, 4};
            }
            
        //draw bonus, but only after the caterpillar is longer than the bonus_random
        if (Snake.size()>= bonus_random)
            arduboy.fillCircle (bonus.x,bonus.y, 2, WHITE);

        //check if they pressed a direction button:
        if(arduboy.justPressed(LEFT_BUTTON) || arduboy.pressed(LEFT_BUTTON))
            direction=LEFT;       
        if(arduboy.justPressed(RIGHT_BUTTON) || arduboy.pressed(RIGHT_BUTTON))
            direction=RIGHT;        
        if(arduboy.justPressed(UP_BUTTON) || arduboy.pressed(UP_BUTTON))
            direction=UP;       
        if(arduboy.justPressed(DOWN_BUTTON) || arduboy.pressed(DOWN_BUTTON))
            direction=DOWN;
            
        /************* Added this to slow the snake down 
         *  moving it every x frames seems to work better
         *  for the buttons responsiveness than using delay
         */
        if(arduboy.everyXFrames(3)){
  
              switch (direction){
                  case UP:
                    snakey -= 4; break;
                  case DOWN:
                    snakey += 4; break;
                  case RIGHT:
                    snakex += 4; break;
                  case LEFT:
                    snakex -= 4; break;
                }
                
              //check if we ate a cabbage:
              if (ate_cabbage()){
                  sound.tone(NOTE_C4,50, NOTE_E4,50, NOTE_G4,50);
                  score += 10;
                  del_segment=false;
                  // if the caterpillar is 10 or more in length, then speed up the caterpillar!
                  if (Snake.size() >= bonus_random){
                    if (game_mode=="normal")
                        //let's make progressively it harder if in normal mode!!
                        frame_rate += 2;
                    else
                        frame_rate = frame_rate_easy + 4;
                  }
              }
      
              //check if we ate bonus, but only valid if the caterpillar is longer than the bonus_random
              if (Snake.size()>= bonus_random && arduboy.collide(snakeRect, bonus)){
                  sound.tone(NOTE_C5,50, NOTE_E5,50, NOTE_G6,50);
                  Snake.pop_back();
                  //slow the caterpillar down again
                  if (game_mode=="normal" && frame_rate >= frame_rate_normal)
                      frame_rate -= (level==1) ? 2 : 1;             
                  else
                      frame_rate = frame_rate_easy;
              }             
              
              // The next bit does the moving along by one segment
              // insert a new head segment into the caterpillar:
              Snake.insert(Snake.begin(),Segment(snakex,snakey));
              snakeRect = {snakex,snakey,4,4}; // place a rect where the caterpillar head is for collision detection
              // remove the tail segment, but not if you just ate a cabbage, as then makes the caterpillar longer:
              if (del_segment)
                  Snake.pop_back();
      
        } // end of the every x frames condition


        // draw our caterpillar:
        for (int i=0; i<Snake.size(); i++)
            arduboy.fillCircle(Snake[i].x, Snake[i].y, 2, WHITE);
        
        // refresh the display
        arduboy.display();
  
        // check if you went off screen or crashed into ourselves (lose!)
        if (crashed()){
            sound.tone(NOTE_C4,500, NOTE_C3,500, NOTE_C1,1000);
            gamestate = lose;
        }

        //Change the gamestate to paused if you press A
        if (arduboy.justPressed(A_BUTTON))
            gamestate = pause;

        // check score, goto level 2 if high enough!
        if (score == 300){
            sound.tone(NOTE_C5,150, NOTE_C5,150, NOTE_C5,600);
            gamestate = level2;
        }

      break;


      case level2:         
          arduboy.setCursor(0, HEIGHT/4);
          arduboy.setTextSize(2);
          arduboy.print(F("Level 2 \n"));
          arduboy.setTextSize(1);
          arduboy.print(F("Bonus 50 points!"));
          //flash the next bit of text on & off
          counter++; // just counting number of frames that have passed
          if(counter < 25)
              arduboy.print(F("\n\nA - Start Level 2"));
          if(counter >= 35) 
              counter = 0; //If counter is greater than or less than 120, reset it back to 0
            //Change the gamestate
          if (arduboy.justPressed(A_BUTTON)) {
              //level 2 setup
              score += 50;
              level=2; 
              frame_rate_normal += 2; frame_rate_easy +=1; //increase speed for both modes              
              arduboy.initRandomSeed();
              snakex=random(SCR_LEFT + 32, SCR_RIGHT - 72); //start caterpillar in a random location
              snakey=random(SCR_TOP + 20, SCR_BOTTOM - 16);
              direction = RIGHT; // Start caterpillar moving right
              
              //create a new caterpillar of length 5 segments for level 2:
              Snake.clear(); //This clears the entire caterpillar Vector Array for us!
              Snake.push_back(Segment(snakex,snakey)); //head of caterpillar
              for (int i=1;i<=5;i++){
                Snake.push_back(Segment(snakex-(i*4),snakey));
                }
              if (game_mode=="normal")
                frame_rate=frame_rate_normal;
              else{
                frame_rate=frame_rate_easy;
                Snake.pop_back(); // remove segment
                }
              // back to game!  
              gamestate=gameplay;
            }
        break;

  
      //-------------------Game Over---------------------
      case lose:
          hi_score=(score>hi_score) ? score : hi_score;  // set the new hi-score if it is one
          arduboy.setCursor(0, HEIGHT/4);
          arduboy.setTextSize(2);
          arduboy.print(F("Game Over!\n"));
          arduboy.setTextSize(1);
          arduboy.print(F("You Scored: ")); arduboy.print(score);
          arduboy.print(F("\nHi-Score: ")); arduboy.print(hi_score);
          arduboy.print(F("\nPress A to re-start"));
          //Change the gamestate
          if (arduboy.justPressed(A_BUTTON)) {
            reset();
            }
        break;

  
      //-------------------Paused Game---------------------
      case pause:
          // just keep looping until they press A button
          arduboy.setCursor((WIDTH/2)-16, HEIGHT/2);
          arduboy.print(F("PAUSED"));
          //Change the gamestate
          if (arduboy.justPressed(A_BUTTON)) {
            gamestate = gameplay;
            }
        break;
        
    } //end of gamestate switch

    //refresh screen at end of every main loop
    arduboy.display();
}


//-------------------Extra Functions used by game---------------------

// function to check if we go off the screen or crash into our own body
bool crashed(){
    // check level we are on, as level 2 has a smaller paly area (8 pixels)
    int xtra = (level==1) ? 0 : 8;

    if (snakex<SCR_LEFT+xtra || snakex>SCR_RIGHT-xtra || snakey<SCR_TOP+xtra || snakey>SCR_BOTTOM-xtra){
          return true;
        }
    // loop through every segment of the caterpillar and see if the head has collided with it
    //start at 1 (segment after head which is always 0)
    for (int i=1; i<Snake.size(); i++){
          if (Snake[i].x == snakex && Snake[i].y == snakey){
            return true;
          }
        }
     return false;
}

//function to check if we ate a cabbage or not using arduboy .collide method
bool ate_cabbage(){
    for (int i=0; i<num_of_cabbages; i++) {
          if (arduboy.collide(snakeRect, cabbages[i])){
            cabbages[i] = {random(SCR_LEFT + 8, SCR_RIGHT - 8), random(SCR_TOP + 8, SCR_BOTTOM - 8), 4, 4};
            return true;
            }
        }
    return false;
}

void draw_border(){
  for (int i=SCR_TOP; i<=SCR_BOTTOM; i+=8){
    Sprites::drawOverwrite(0, i, block, 0);
    Sprites::drawOverwrite(WIDTH - 8, i, block, 0);
  }
  for (int i=0; i<=WIDTH-8; i+=8){
    Sprites::drawOverwrite(i, 8, block, 0);
    Sprites::drawOverwrite(i, HEIGHT-8, block, 0);
  }
}

//function to reset the game & go back to title screen
void reset(){
    setup(); //reset all variables
    loop(); 
}
