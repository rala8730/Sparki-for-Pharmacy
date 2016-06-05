#include <Sparki.h> // include the sparki library
#include <SoftwareSerial.h>
#include <math.h> 
#define NO_LCD // for memory!!!!!

int degree_baseline_number = 0;
int degree_increment = 30;
int desired_bottle[10];
int angle=-15;
int distance_from_sparki [30];
int goal = 3;
//String q2;
//int bottle;
//int rfidtag[28];
const int idLen = 13;
const int kTags = 5;

// Put your known tags here!
char knownTags[kTags][idLen] = {
             "7F000ADC9A33",      // this is bottle 1, but is named as tag 2
             "7F0007F4EE62",    //2nd bottle: tag 3
             "7F000ABEBB70",    //3rd bottle: tag 4
             "7F000AD86BC6",  //4th bottle: tag 5
             "7C0055E2569D" //5th bottle: tag 6
};
int counter=0;
//Serial.begin(19200);
//Serial1.begin(9600);


//#define S_IDLE 0
//#define S_MFE 1
//
//#define S_MTB 2 // MOVE TO BOTTLE
//#define S_US 3 // use ultrasound sensor
//#define S_GB 4 // grab bottle
//
//int state = S_IDLE;

/* ***************************************************************************************************
 *                              FUNCTION: Move from entrance
 *   ***************************************************************************************************
 *  
 */
int move_from_entrance()

{
  boolean start_line_reached = false; 
  int threshold = 700;
  
  int lineLeft   = sparki.lineLeft();   // measure the left IR sensor
  int lineCenter = sparki.lineCenter(); // measure the center IR sensor
  int lineRight  = sparki.lineRight();  // measure the right IR sensor

  if ( lineCenter < threshold ) // if line is below left line sensor
  {  
    sparki.moveForward(); // move forward
    
  }
  else
  {
    if ( lineLeft < threshold ) // if line is below left line sensor
    {  
      sparki.moveLeft(); // turn left
    }
  
    if ( lineRight < threshold ) // if line is below right line sensor
    {  
      sparki.moveRight(); // turn right
    }
  }

  int start_line = lineCenter < threshold  && lineLeft < threshold && lineRight < threshold;
  if (start_line)
  {
    sparki.moveForward(2.5);
    sparki.moveStop();
    start_line_reached = true;
    //Serial.println("move to to 0,0");
  }

  else if (!start_line)
  {
  start_line_reached = false;
  }
  else 
  {
    sparki.beep();
  }
  
  return start_line_reached;
}


/*  *********************************
 *   FUNCTION: USE ULTRASOUND SENSOR ONCE
 *   *********************************
 *  use_ultrasound_sensor adapted from Professor Nikolas Correll 
 *  of University of Colorado at Boulder 
 *  makes the ultrasonic sensor  
 *  pivot from -30 to 30 degrees
 *  in order to detect items in front of it
 */
 
void use_ultrasound_sensor_once()
{
  sparki.servo(angle); // tells sparki to look at that angle
  angle=angle+1; 
  
  
  if(angle>30) 
  { 
    angle=-30; 
    //Serial.println(); 
    // end of a scan
  } 
  int cm = sparki.ping(); // measures the distance with Sparki's eyes 
  //Serial.print(cm); 
  //Serial.print(" "); 

  // an array where the index is the angle measurement, and the value is the distance.
  distance_from_sparki[angle+60] = cm ; // save the measured distances to an array
  //Serial.print("Finding values for distance array");
  
//  for(int i = 0; i< sizeof(distance_from_sparki);i++)
//  {
//    //Serial.println(" distance from sparki at i:");
//    //Serial.print(distance_from_sparki[i]);
//    //Serial.print(" ");
//    
//  }
  
  }
/* ***************************************************************************************************
 *                              FUNCTION: PROCESS ULTRASOUND
 *   ***************************************************************************************************
 *  
 */
 int process_ultrasound(){
  
 int angle_at;
 int dist = 99 ;
  for( int i; i< sizeof(distance_from_sparki);i++)
  {
    if(distance_from_sparki[i] > 0)
    {
      if ( distance_from_sparki[i] < dist ) // traverse through each element in the array, if any of the vals are smaller than the first element
      {
        angle_at = i;
        dist = distance_from_sparki[i] ;    // update that to be the new min.
      }
      
      //Serial.println("distance from sparki in process_ultrasound");
      //Serial.print(distance_from_sparki[i]);
      //Serial.print(" ");
      //Serial.println("At angle");
      //Serial.print(angle_at);

    }
  }
  return angle_at, dist;
 }
 


 /* ***************************************************************************************************
 *                              FUNCTION: Move to bottle
 *   ***************************************************************************************************
 *  should be a value from 1-5 
 *  moves to a space 10 cm in front of the object to then use the ultrasound sensor
 */
int move_to_bottle()
{ 
  
  //int arrived = 0;
  
  if( goal == 1 )
  {
    sparki.moveForward(20);
    sparki.moveStop();// move forward to the first pill bottle 
    //use_ultrasound_sensor(desired_bottle);    
  }
  else if ( goal == 2)
  {
    sparki.moveRight(45);
    sparki.moveForward(30);
    sparki.moveStop();    
  }

  else if ( goal == 3)
  {
    sparki.moveRight(90);
    sparki.moveForward(17);
    //while (Serial1.available() != true) sparki.moveForward();
    sparki.moveStop();
    //arrived = 1;
  }

  else if (goal== 4 )
  {
    sparki.moveRight(-45);
    sparki.moveForward(30);
    sparki.moveStop();
    //grab_the_bottle(smallest);
  }

  else if (goal == 5)
  {
    sparki.moveLeft(90);
    sparki.moveForward(17);
    sparki.moveStop();
    //grab_the_bottle(smallest);
  }
    
  //return arrived;
}

/* ***************************************************************************************************
 *                              FUNCTION: READ AND MAKE TAGS
 *   ***************************************************************************************************
 *  uses the smallest value found from ultrasound sensor to travel directly in that path
 */
//int read_tag(){ 
//  counter = 0;
//  while (counter<28){
//    if(Serial1.available()){
//      rfidtag[counter] = Serial1.read();
//      Serial.print(rfidtag[counter],HEX);
//      counter++;
//    }
//   }
//   if (counter>27) counter = 0;
// int q2 = rfidtag[24];
// int bottle = q2;
// return bottle; 
//} 
int compare()
{
  //Serial.begin(9600);
  //Serial1.begin(9600);
  //tag read
  char my_array[idLen];
  counter=0;
  while(counter<idLen)
  {
    if ( Serial1.available() )
    {
      // if data is unread from the radio
      int readbyte = Serial1.read();
      my_array[counter] = (char) readbyte;
      //Serial.print(my_array[counter]);
      if ((int) my_array[counter] > 20)
      {
        counter++;
      }
      //if(counter=idLen-1) {
        //counter = 0;
      //}
    }
  }
  my_array[idLen-1] = '\0';
  /*for(int i=0;i<idLen;i++) 
  {
    //string s = i ":" my_array[i]
    Serial.print(i);
    Serial.print(":");
    Serial.println((int)my_array[i]);
  }*/
  Serial.println(my_array);
  for(int i=0;i<kTags;i++)
  {
    if(strcmp(my_array,knownTags[i])==0)
    {
      //Serial.println(i+1);
      return (i+1);
    }
  }
  return 0;
}

/* ***************************************************************************************************
 *                              FUNCTION: GRAB THE BOTTLE
 *   ***************************************************************************************************
 *  uses the smallest value found from ultrasound sensor to travel directly in that path
 */
 
void grab_the_bottle(int at_angle, int dist){

 // Serial.println("Beginning grasping PVC pipe routine");
  //delay(5000);
  // first it would be nice to see if there the pill bottle is indeed there
  // so use ultrasound sensors
  //Serial.println("Scanning for pill bottles");
  //Serial.println("closing grippers");
//  int tag;
//  if (goal==1) tag = 3; //USING SCANNERS 2-6
//  else if (goal==2) tag = 2;
//  else if (goal==3) tag = 0;
//  else if (goal==4) tag = 6;
//  else if (goal==5) tag = 4;
//  
  sparki.gripperClose();
  delay(7000);

  //sparki.moveRight(at_angle);// focus sparki's center on the pill bottle
  
  //Serial.println("opening grippers ");
  sparki.gripperOpen();// open gripper claws 5 cm 
  delay(6000);

  //Serial.println("stopping grippers");
  sparki.gripperStop();
  //delay(9000);
  
  //Serial.println("moving forward");
  sparki.moveForward(4); // move sparki forward
  sparki.moveBackward(4); // move sparki forward
  sparki.moveForward(4); // move sparki forward
  int x = compare();
  if( x == goal)
  {
    Serial.println("This is the correct pill bottle!!!!!");
    sparki.gripperClose();
    delay(5000);

  // Serial.println("locking grippers");
    sparki.gripperStop();
    delay(1000);
  }
  else{
    Serial.println("This is NOT the correct pill bottle!!!!!");
    sparki.RGB(100,0,0);
    
  }
  //delay(9000);
//  if(bottle == tag) sparki.RGB(0,0,100);
//  else if(bottle != tag) sparki.beep();
  //Serial.println("grasping bottle");
  

  //Serial.println("Returning to mainline");
  
  //sparki.moveStop();
  //sparki.RGB(0,0,100);
  //delay(1000);
  //sparki.moveRight(180);
  //sparki.RGB(100,0,0);
//return 1;
}

 /* ***************************************************************************************************
 *                              FUNCTION: Move back to start
 *   ***************************************************************************************************
 *  should be a value from 1-5 
 *  moves to a space 10 cm in front of the object to then use the ultrasound sensor
 */
int move_back()
{ 
  
  //int arrived = 0;
  
  if( goal == 1 )
  {
    sparki.moveBackward(24);
    sparki.moveLeft(180);
    sparki.moveForward(35);
    sparki.gripperOpen();
    sparki.moveBackward(4);    
  }
  else if ( goal == 2)
  {
  sparki.moveBackward(34);
  sparki.moveRight(135);
  sparki.moveForward(35);
  sparki.gripperOpen();
  sparki.moveBackward(4);    
  }

  else if ( goal == 3)
  {
    sparki.moveBackward(21);
    sparki.moveRight(90);
    sparki.moveForward(35);
    sparki.gripperOpen();
    sparki.moveBackward(4);
    //arrived = 1;   
  }

  else if (goal== 4 )
  {
    sparki.moveBackward(34);
    sparki.moveLeft(135);
    sparki.moveForward(35);
    sparki.gripperOpen();
    sparki.moveBackward(4);
  }

  else if (goal == 5)
  {
    sparki.moveBackward(21);
    sparki.moveLeft(90);
    sparki.moveForward(35);
    sparki.gripperOpen();
    sparki.moveBackward(4);
  }
}


void setup()
{
  Serial1.begin(9600);
}
void loop()
{
 // int arrived=move_to_bottle();
  int at_angle, dist = process_ultrasound();
  boolean  start_line_reached = move_from_entrance();
  int arrived_at_bottle = 0;
//  int bottle = read_tag();
//  int bottle = 
  move_from_entrance();
  sparki.RGB(100,100,0); // Make the LED maximum yellow?
  if( start_line_reached == true){
    sparki.RGB(0,100,0); // Make the LED maximum Green
    move_to_bottle();
    delay(2000);
    //Serial.print("move to bottle completed");
    arrived_at_bottle=1;
    if (arrived_at_bottle=1)
    {
     use_ultrasound_sensor_once();
     process_ultrasound();
     arrived_at_bottle=2;
//     read_tag();
     //compare();
     
    grab_the_bottle(at_angle, dist);
     move_back();   
      }
     // else if (arrived_at_bottle=2)
        
    }

    /*
  //boolean arrived = move_to_bottle( );
  //delay(3000);
  else{
    arrived_at_bottle = 0;
    sparki.stop();
  }
  
  
  if (arrived_at_bottle = 1)
  {
  sparki.gripperOpen(20);
   
   use_ultrasound_sensor_once();
   process_ultrasound();
  }
*/
  //grab_the_bottle(at_angle, dist);

  //if(found_bottle = true)
/*
  switch(state) {
    
    case S_IDLE  :
      
      {state = S_MFE; // move from the entrance first.
      //state = S_US;
      
      }break; 
      
    case S_MFE  :
     { move_from_entrance();
      
      if (start_line_reached = true) // then continue to move to bottle.
      {
        state = S_MTB;
      }
      //state = S_MTB;
    //if(goal is reached) state = 
      //break; 
     }

      break;
  
    case S_MTB :
      {move_to_bottle(3);
      }
      //break;
      //state = S_US;

    /*case S_US :
      use_ultrasound_sensor_once();
      if(angle>14) state = S_GB;  // if the angle is greater than 14, then move onto grabbing the bottle.
      break;


    case S_GB :
      grab_the_bottle(at_angle);
      break;
    
   
   default : /* Optional */
    //delay(1); 
}


