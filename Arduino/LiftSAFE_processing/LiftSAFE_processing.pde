import processing.serial.*;
Serial port;               //Declare XBEE
String message = ""; //hold entier string that come in over serial
String MapVal = "";  // hold value for mapedval
String Color = "";   //hold value for color
String Red = "RED";
String Green = "GREEN";
PrintWriter output;
 
//String[] save = split(message, ' ');
 
int index = 0;       //find break in string
PFont font;
 
void setup(){
 
   size(400,400); //size of window
 
  //FIND PORT
  for (int i=0; i< Serial.list().length; i++)
  {
    //Check console and search for your USB-Port
    println(i+": "+Serial.list()[i]);   
  }
 
  //SELECT PORT
  port = new Serial(this, Serial.list()[0], 9600);
 
  port.bufferUntil('.');   //keep looking till find period and move on
  font = loadFont("ArialMT-48.vlw"); // set up font
  textFont(font, 45);
 
  output = createWriter( "data.txt" );
}
 
 
void draw()
{
 
  if ( port.available() > 0) {  // If data is available,
      message = port.readStringUntil('\n');         // read it and store it in val
  }
     if (message != null) {                      
       if(Red.equals(Color) == true){
         background(255,0,0); //RGB
         fill(0, 0, 0);
         text(MapVal, 100, 200); // text(string, x-corr, y- corr)
         //text("STOP", 100, 100);
         text("Back fault", 100, 100);
           println(MapVal);
            output.println(message);
 
       } else if(Green.equals(Color) == true){
         background(0,255,0);
         fill(0,0,0);
         text(MapVal, 100, 200);
           println(MapVal);
             output.println(message);
       } else {
         println("Error");
     }
  }
 
 // saveStrings("FlexReadings.txt", save);
}
 
 
void serialEvent(Serial port) {
  
  message = port.readStringUntil('.');   // read incommung String '.'
  message = message.substring(0, message.length() - 1); //get rids of last chr
 
  //look for comma
  index = message.indexOf(",");
  //fetch MapVal
  MapVal = message.substring(0, index);
  //get color value
  Color = message.substring(index+1, message.length());
 
}
 
void keyPressed() {
    output.flush();  // Writes the remaining data to the file
    output.close();  // Finishes the file
    exit();  // Stops the program
}