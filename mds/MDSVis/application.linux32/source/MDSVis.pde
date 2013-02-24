/**
 * Show a 3D (or 2D) visualization of MDS of the image set 
 */

import java.io.*;
import java.util.regex.*;
import java.util.*;
import peasy.*;
import javax.swing.*;

int numPoints;
PVector[] points;
String[] tags;
PImage[] imgs;
boolean[] class_visible;
PeasyCam cam;
PImage bgImage;


HashMap class_map;
int[] class_idx;
String[] class_names;

float text_size = 2.0;
float quadsize = 2.00;
float stretch = 4.0;
float stroke_weight = 1.0;
int class_page = 0;
int num_classes = 0;
boolean point_mode = false;
boolean show_tags = false;
boolean do_lights = false;
boolean cam_hud = false;
boolean show_bg_img = false;
boolean show_hud = false;
boolean show_instructions = true;

String tag_font_name = "Arial";

int MAX_POINTS = 500;
int MAX_CLASSES = 100;
float TEXT_PADDING = 0.3;
int HUD_TEXT_SIZE = 16;
int HUD_SPACE_SIZE = 2;
String NAMES_AND_POINTS_3D_FILE = "names_and_points_3d.txt";

color[] colors;

String getClass(String name)
{
  Matcher matcher = Pattern.compile("\\d+").matcher(name);

  if (matcher.find())
  {
    int start = matcher.start();
    print(start);
    print("\n");
    return name.substring(0,start);
  }
  else
  {
    return null;
  }
}

void setup()
{
  size(1024, 768, P3D);
  //fill(204);
  
  
  String bg_file = dataPath("northern_lights_1024x768.jpg");
  bgImage = loadImage(bg_file);
  
  PFont tag_font;
  // The font "AndaleMono-48.vlw"" must be located in the 
  // current sketch's "data" directory to load successfully
  tag_font = loadFont("SimplifiedArabicFixed-48.vlw");
  textFont(tag_font);
 
  
  points = new PVector[MAX_POINTS];
  tags = new String[MAX_POINTS];
  imgs = new PImage[MAX_POINTS];
  class_idx = new int[MAX_POINTS];
  
  colors = new color[MAX_CLASSES];
  class_visible = new boolean[MAX_CLASSES];
  class_names = new String[MAX_CLASSES];
  
  cam = new PeasyCam(this, 200);
  // Use right-click drag for panning cause touchpad has no middle drag
  cam.setRightDragHandler(cam.getPanDragHandler()); 
  
  // set system look and feel 
  try { 
    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName()); 
  } catch (Exception e) { 
    e.printStackTrace();  
  } 
  // prompt for db directory
  final JFileChooser fc = new JFileChooser();
  //fc.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY); 
  int returnVal = fc.showOpenDialog(this);
  String img_path=null;
  String filename = null;
  if (returnVal == JFileChooser.APPROVE_OPTION) {
    filename = fc.getSelectedFile().getPath();
    img_path = fc.getSelectedFile().getParent();
    //img_path = fc.getSelectedFile().getPath();  
  }
  else
  {
    // default path?
    //img_path = dataPath("C:\\Users\\jonotron\\Documents\\image_banks\\shapes216\\");
    exit();
  }
  
//  String filename = dataPath(img_path + "\\" + NAMES_AND_POINTS_3D_FILE);
  print(filename);
  
  BufferedReader r = null;
  try
  {
    r = new BufferedReader (new FileReader(filename));
  } catch (FileNotFoundException e)
  {
  }
  
  String line;
  numPoints = 0;
  //classes = new Set<String>();
  //class_map =  new HashMap<String, int>();
  class_map =  new HashMap();
  boolean mode_2d = false;
  try {
    while ((line = r.readLine()) != null) {
      
      String[] parts = line.split(" ");
      tags[numPoints] = parts[0];
      float x = Float.parseFloat(parts[1]);
      float y = Float.parseFloat(parts[2]);
      float z;
      if (parts.length > 3) 
        z = Float.parseFloat(parts[3]);
      else {
        z = 0;
        mode_2d = true;
      }
      PVector p = new PVector(x,y,z);
      points[numPoints] = p;
      //String img_name = dataPath(img_path + "\\" + tags[numPoints]);
      String img_name = dataPath(img_path + "/" + tags[numPoints]);
      String classname = getClass(tags[numPoints]);
      if (class_map.containsKey(classname))
      {
        
      }
      else
      {
        class_map.put(classname, num_classes);
        colors[num_classes] = color((int)random(255),(int)random(255),(int)random(255));
        class_visible[num_classes] = true;
        class_names[num_classes] = classname;
        num_classes += 1;
      }
      class_idx[numPoints] = (Integer)class_map.get(classname);
      //classes.add(classname);
      
      print(img_name + "\n");
      imgs[numPoints] = loadImage(img_name);
      numPoints++;
    }
  } catch (IOException e)
  {
  }
  
  if (mode_2d)
    cam.setLeftDragHandler(cam.getPanDragHandler()); // No rotation in 2d mode
  
  // pick colors
  colorMode(HSB, 100);
  for (int i=0; i<num_classes; i++)
  {
    colors[i] = color(100.0/ (float) num_classes * (float)i, 100, 100);
  }
  
//  for (int i=0; i<colors.size(); i++) {
//    class_idx[numPoints] = classes.indexOf(classname);
//  
//  colors = new color[classes.size()];
//  for (int i=0; i<colors.size(); i++) {
//    colors[i] = color((int)random(255),(int)random(255),(int)random(255));
//  }
}

void draw() 
{
  if (do_lights)
    lights();
    
  if (show_bg_img)
    background(bgImage);
  else
    background(0);
    
    
  
  float[] rotations = cam.getRotations();
  // face the cam

  
  strokeWeight(stroke_weight);
  for (int i=0; i<numPoints; i++)
  {
    
    PVector p = points[i];
    PImage img = imgs[i];
    if (img == null) continue;
    int class_num = class_idx[i];
    if (!class_visible[class_num])
      continue;
    
    pushMatrix();
    
    translate(p.x*stretch,p.y*stretch,p.z*stretch);
    
    rotateX(rotations[0]);
  rotateY(rotations[1]);
  rotateZ(rotations[2]);
    
    //point(p.x, p.y, p.z);
    //stroke(255);
    textSize(text_size);
    fill(0,0,100);
    if (show_tags)
        text(tags[i], quadsize + TEXT_PADDING, 0, 0);
        
    stroke(colors[class_num]);
    if (point_mode)
      point(0,0,0);
    else
    {
      
      beginShape();
      
     // print('0');
      texture(img);
      //print( '1');
      //noStroke();
      vertex(-quadsize, -quadsize, 0, 0, 0);
      vertex(quadsize, -quadsize, 0, img.width, 0);
      vertex(quadsize, quadsize, 0, img.width, img.height);
      vertex(-quadsize, quadsize, 0, 0, img.height);
      //print('2');
      endShape();
      
    }
    popMatrix();
  }
  
  
  if (show_hud)
  {
    noLights();
    textSize(HUD_TEXT_SIZE);
    textAlign(LEFT, TOP);
    fill(0,0,100);
    cam.beginHUD();
    text("Page " + class_page, 10, 10);
    for (Integer i= 0 ; i< 10; i++)
    {
      int class_num = i + (class_page * 10);
      if (class_num > num_classes || class_num < 0)
        break;
      fill(colors[class_num]);
      String class_text = i.toString() + ". " + class_names[class_num];
      if (!class_visible[class_num])
       class_text = class_text.concat("-x"); 
      text(class_text, 10, 10 + (HUD_TEXT_SIZE + HUD_SPACE_SIZE) * (i+1));
    }
    //text("test",10,10);
    cam.endHUD();
    textAlign(LEFT, BASELINE);
  }
  else if (show_instructions)
  {
   showInstructions();
  }
}

String[] instructions = { "CONTROLS",
                          "********",
                          "i: Show/hide these instructions",
                          "q/w: Shrink/stretch the coordinate scale", 
                          "a/s: Smaller/larger class tags",
                          "z/x: Smaller/larger images",
                          "t: Toggle tag visibility",
                          "p: Toggle point mode",
                          "-/=: Smaller/larger points or image borders",
                          "h: Show/hide HUD with class list",
                          ",/.: Previous/next class page",
                          "0-9: Toggle visibility of classes",
                          "c/v: Show none/all classes",
                          "b: Toggle background image",
                          "Mouse drag left: rotate",
                          "Mouse drag right: pan",
                          "Mouse scroll: zoom" };

void showInstructions()
{
  noLights();
  cam.beginHUD();
  textSize(HUD_TEXT_SIZE);
  textAlign(LEFT, TOP);
  fill(0,0,100);
  
  for (int i=0; i<instructions.length; i++)
  {
    text(instructions[i], 10, 10 + (HUD_TEXT_SIZE + HUD_SPACE_SIZE) * i);
  }
  
  cam.endHUD();
  textAlign(LEFT, BASELINE);
}

void keyPressed() {
//  print(key);
//  print("" + (int)key);
//  print("\n");

  if (key == 's') {
    text_size *= 2;
  }
  else if (key == 'a')
  {
    text_size *= .5;
  }
  else if (key == 'z')
  {
    quadsize *= .5;
  }
  else if (key == 'x')
  {
    quadsize *= 2;
  }
  else if (key == 'q')
  {
    stretch *= .5;
  }
  else if (key == 'w')
  {
    stretch *= 2;
  }
  else if (key == 'p')
  {
    point_mode = !point_mode;
  }
  else if (key == 't')
  {
    show_tags = !show_tags;
  }
  else if (key == 'l')
  {
    do_lights = !do_lights;
  }
  else if (key == 'h')
   {
     show_hud = !show_hud;

   }
   else if (key == 'b')
   {
     show_bg_img = !show_bg_img;
   }
   else if (key == '=')
   {
     stroke_weight *= 2;
   }
   else if (key == '-')
   {
     stroke_weight *= 0.5;
   }
   else if (key == ',')
   {
     if (class_page > 0)
     class_page--;
   } 
   else if (key == '.')
   {
     if (class_page < num_classes / 10)
     class_page++;
   }
   else if ((int)key >= 48 && (int)key <=57)
   {
     int class_num = (int)key - 48 + class_page * 10;
     if (class_num >= 0 && class_num < num_classes)
       class_visible[class_num] = !class_visible[class_num];
   }
   else if (key == 'c')
   {
     for (int i=0; i<num_classes; i++)
       class_visible[i] = false;
   }
   else if (key == 'v')
   {
     for (int i=0; i<num_classes; i++)
       class_visible[i] = true;
   }
   else if (key == 'i')
   {
     show_instructions = !show_instructions;
     println(instructions[0]);
   }
       

}

