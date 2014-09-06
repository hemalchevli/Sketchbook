/*

 ----------------------------------------------------------------
  Copyright (C) 2005 Ricard Marxer Pi??n

  email (at) ricardmarxer.com
  http://www.ricardmarxer.com/
 ----------------------------------------------------------------

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 ----------------------------------------------------------------
  Built with Processing (Beta) v0118
        uses Geomerative (Alpha) v007
 ----------------------------------------------------------------

  Created 17 January 2006

 ---------------------------------------------------------------- 
  fractalfabet
 ---------------------------------------------------------------- 

*/

import processing.opengl.*;
import geomerative.*;

RFont f;
RMatrix mtx;
RGroup basura;
RGroup adptdGrp;
RGroup txtGrp;
RShape shp;
int shapeIndex = 0;

int[] wordOffsets;
int textIndex = 0;
int charIndex = 0;

final int numChars = 100;

//------------------------ Variables for OSD -----------------------------------
boolean osd = true;
boolean forceOSD = true;
PFont fnt;
final String explanation = "Click to get focus. Type and press ENTER.  Press ESC to exit.";
final int explanationFg = #000000;
final String renderTextLabel = "Text entered:  ";
final int renderTextFg = #000000;
final int osdBg = #ffffcc;
final int margin = 20;
final int pad = 5;
float textHeight;
//------------------------------------------------------------------------------

String[] calculateRenderText(String txt){
 // String[] renderText = split(txt);
  String[] renderText = 'A';
  if(renderText == null){
    return null;
  }
  
  for(int i=0;i<renderText.length;i++){
    if(renderText[i].length() != 0){
      for(int j=0;j<numChars;j++){
        renderText[i] = renderText[i] + renderText[i].charAt(j % renderText[i].length());
      }
    }
  }
  
  return renderText;
}

void initOSD(){
  fnt = loadFont("sansserif-12.vlw");
  textFont(fnt,12);
}

void drawOSD(PGraphics gfx){
  float textHeight = textAscent() + textDescent() + 2*pad;
  if(newString != "" || forceOSD){
    fill(osdBg);
    noStroke();
    rect(0,height-3*textHeight,width, 3*textHeight);
    
    // Draw explanation
    fill(explanationFg);
    text(explanation, pad, height - 2*textHeight - pad);
    
    if(newString != ""){
      String textRendered = renderTextLabel + newString;
      
      // Draw text to be rendered
      fill(renderTextFg);
      text(textRendered, pad, height - 1*textHeight - pad);
    }
  }
}

long framesPassed = 0;

int w,h;
String r;
boolean allowSaving = true;


final int APPLI = 0;
final int EXHIBIT = 1;
final int APPLET = 2;
final int VIDEO = 3;
final int PRINT = 4;

//------------------------ Runtime properties ----------------------------------
// Save each frame
int mode = APPLET;

String DEFAULTAPPLETRENDERER = JAVA2D;
int DEFAULTAPPLETWIDTH = 680;
int DEFAULTAPPLETHEIGHT = 480;

String DEFAULTAPPLIRENDERER = OPENGL;
int DEFAULTAPPLIWIDTH = 800;
int DEFAULTAPPLIHEIGHT = 600;

String DEFAULTEXHIBITRENDERER = OPENGL;
int DEFAULTEXHIBITWIDTH = screen.width;
int DEFAULTEXHIBITHEIGHT = screen.height;

//------------------------------------------------------------------------------

// The text to write in depth and around each character
String[] STRNGS= { 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
};
String[] words= { 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};
/*
String[] STRNGS= { 
  "999999999999999999999999999999999999999999999999999999999999",
  "888888888888888888888888888888888888888888888888888888888888",
  "777777777777777777777777777777777777777777777777777777777777",
  "666666666666666666666666666666666666666666666666666666666666",
  "555555555555555555555555555555555555555555555555555555555555",
  "444444444444444444444444444444444444444444444444444444444444",
  "333333333333333333333333333333333333333333333333333333333333",
  "222222222222222222222222222222222222222222222222222222222222",
  "111111111111111111111111111111111111111111111111111111111111",
  "000000000000000000000000000000000000000000000000000000000000"
};

String[] STRNGS= { 
  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
};*/

// The font to be used
String FONT = "MONO___.ttf";
static String CUSTOMFONT = "";

String newString = "";

static public void main(String args[]) {
  try{
    if(args != null){
      CUSTOMFONT = args[0];
    }
  }catch(Exception e){}
  PApplet.main(new String[] { "--present", "--bgcolor=#ffffff", "--hide-stop", "fractalfabet" });
}

void setup(){
  
  w = 320;
  h = 240;
  
  switch(mode){
    case EXHIBIT:
      w = DEFAULTEXHIBITWIDTH;
      h = DEFAULTEXHIBITHEIGHT;
      r = DEFAULTEXHIBITRENDERER;
      break;
      
    case APPLI:
      w = DEFAULTAPPLIWIDTH;
      h = DEFAULTAPPLIHEIGHT;
      r = DEFAULTAPPLIRENDERER;
      break;
      
    case APPLET:
      // Specify the widtha and height at runtime
      w = int(param("width"));
      h = int(param("height"));
      r = (String)param("renderer");
      
      // (String) will return null if param("renderer") doesn't exist
      if (r != OPENGL && r != P3D && r != JAVA2D && r != P2D) {
        r = DEFAULTAPPLETRENDERER;
      }
      // int() will return 0 if param("width") doesn't exist
      if (w <= 0) {
        w = DEFAULTAPPLETWIDTH;
      }
      // int() will return 0 if param("height") doesn't exist
      if (h <= 0) {
        h = DEFAULTAPPLETHEIGHT;
      }
      break;
      
    default:
      break;
  }
  
  size(w,h,r);
  frameRate(25);
  
  try{
    smooth();
    ((PGraphicsOpenGL)g).gl.setSwapInterval(1);
  }catch(Exception e){}

  noCursor();
  
  background(255);
  noFill();
  stroke(0);

  if(CUSTOMFONT.equals("") || CUSTOMFONT.equals("\"\"")){
    f = new RFont(this,FONT,372,RFont.LEFT);
  }else{
    try{
      f = new RFont(this,CUSTOMFONT,372,RFont.LEFT);
    }catch(Exception e){
      f = new RFont(this,FONT,372,RFont.LEFT);
    }
  }
  
  initialize();
  initOSD();
}

void initialize(){
  charIndex = 0;
  shapeIndex = 0;
  textIndex = 0;
  wordOffsets = new int[words.length];
  for(int i=0;i<wordOffsets.length;i++)
    wordOffsets[i] = 0;
  
  RCommand.setSegmentator(RCommand.UNIFORMSTEP);
  RCommand.setSegmentStep(3);
  
  RGroup.setAdaptor(RGroup.BYELEMENTINDEX);
  RGroup.setAdaptorScale(0.000055);

  if(charIndex >= words[textIndex].length()){
    charIndex=0;
    wordOffsets[textIndex]++;
    textIndex = (textIndex + 1) % STRNGS.length;
    shapeIndex = (wordOffsets[textIndex] * words[textIndex].length()) % STRNGS[textIndex].length();
  }
  
  txtGrp = f.toGroup(STRNGS[textIndex]);

  basura = new RGroup();
  adptdGrp = new RGroup(txtGrp);
  shp = (RShape)(adptdGrp.elements[shapeIndex]);

  adptdGrp = new RGroup();
  /* Reduce the number of characters per subshape depending on the length of the subshape */
  float maxlength = 0F;
  for(int i=0;i<shp.countSubshapes();i++){
    RSubshape subshp = shp.subshapes[i];
    maxlength = max(subshp.getCurveLength(),maxlength);
  }
  int charsinsubshape;
  for(int i=0;i<shp.countSubshapes();i++){
    RSubshape subshp = shp.subshapes[i];
    charsinsubshape = floor(STRNGS[textIndex].length() * subshp.getCurveLength() / maxlength);
    txtGrp = f.toGroup(STRNGS[textIndex].substring(0,charsinsubshape));
    adptdGrp.addGroup(txtGrp.adaptTo(subshp));
  }
  /* End of adapting the group */
    
  shapeIndex = (shapeIndex + 1) % txtGrp.countElements();
  charIndex ++;
  adptdGrp.removeElement(shapeIndex);

  RMatrix makeSmall = new RMatrix();
  makeSmall.scale(0.01);
  adptdGrp.transform(makeSmall);
  basura.transform(makeSmall);
  shp.transform(makeSmall);
}

void draw(){
  background(255);
  pushMatrix();
  translate(width/2,height/2);

  mtx = shp.getCenteringTransf(g, 0, 0, 0.04);
  mtx.scale(1.04);
  mtx.rotate(PI/300);

  shp.transform(mtx);
  basura.transform(mtx);
  adptdGrp.transform(mtx);

  if(isBig(shp,3)){
    basura = adptdGrp;
    adptdGrp = new RGroup();
    
    if(charIndex >= words[textIndex].length()){
      charIndex = 0;
      wordOffsets[textIndex]++;
      textIndex = (textIndex + 1) % STRNGS.length;
      shapeIndex = (wordOffsets[textIndex] * words[textIndex].length()) % STRNGS[textIndex].length();
    }
    
    
    /* Reduce the number of characters per subshape depending on the length of the subshape */
    float maxlength = 0F;
    for(int i=0;i<shp.countSubshapes();i++){
      RSubshape subshp = shp.subshapes[i];
      maxlength = max(subshp.getCurveLength(),maxlength);
    }
    int charsinsubshape;
    for(int i=0;i<shp.countSubshapes();i++){
      RSubshape subshp = shp.subshapes[i];
      charsinsubshape = floor(STRNGS[textIndex].length() * subshp.getCurveLength() / maxlength);
      txtGrp = f.toGroup(STRNGS[textIndex].substring(0,charsinsubshape));
      adptdGrp.addGroup(txtGrp.adaptTo(subshp));
    }
    /* End of adapting the group */
    
    //adptdGrp = txtGrp.adaptTo(shp);

    shp = (RShape)(adptdGrp.elements[shapeIndex]);
    adptdGrp.removeElement(shapeIndex);
    shapeIndex = (shapeIndex + 1) % txtGrp.countElements();
    charIndex++;
  }

  noFill();
  stroke(0);
  
  basura.draw(g);
  adptdGrp.draw(g);
  shp.draw(g);
  
  popMatrix();
  
  try{
    if(mode == VIDEO) saveFrame("fractalfabet-#####.tga");
  }catch(Exception e){}
  
  drawOSD(g);
}

void keyReleased(){
  if(keyCode==ENTER){
    String[] newWords = split(newString);
    String[] newText = calculateRenderText(newString);
    if(newText != null){
      //println("FRACTALFABET: "+day()+"-"+month()+"-"+year()+" "+hour()+":"+minute()+":"+second()+ " "+newString);
      //STRNGS[0] = newText;
      //STRNGS = split(newText);
      STRNGS = newText;
      words = newWords;
        
      newString = "";
      forceOSD = false;
      initialize();
    }else{
      forceOSD = true;
    }
  }else if(keyCode==BACKSPACE){
    if(newString.length() !=0 ){
      newString = newString.substring(0,newString.length()-1);
    }
  }else if(keyCode == CONTROL){
  try{
    if(allowSaving)
      saveFrame(STRNGS[0]+"-####.tga");
  }catch(Exception e){}
  }else if(key != CODED){
    newString += key;
  }else{
    forceOSD = true;
  }
}

void addGroup(RGroup grp, RGroup addedgrp){
  for(int i=0;i<addedgrp.countElements();i++){
    grp.addElement(addedgrp.elements[i]);
  }
}


boolean isBig(RGeomElem e,float minSize){
  boolean result = false;

  RContour c = e.getBounds();

  float ewidth = abs(c.points[0].x-c.points[2].x);
  float eheight = abs(c.points[0].y-c.points[2].y);

  if(ewidth >= minSize || eheight >= minSize){
    result = true;
  }

  return result;
}

String removeSpaces(String s) {
  StringTokenizer st = new StringTokenizer(s," ",false);
  String t="";
  while (st.hasMoreElements()) t += st.nextElement();
  return t;
}

