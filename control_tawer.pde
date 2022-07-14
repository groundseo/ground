import processing.serial.*;

int x = 50;
int y = 100;
int small_x = 300;
int small_y = 30;


Serial port;
int[] rfidValues;

boolean[][] clicked;
boolean[][] clicked_1;
float [][] rectanglesX;
float [][] rectanglesY;
float [][] rectanglesX_1;
float [][] rectanglesY_1;
color [][] colors;
color [][] colors_1;
boolean [][] mouseClicked;
boolean [][] mouseClicked_1;

int w = 100, h = 100;
int small_w = 100, small_h = 50;

color darkYellow = color(100, 200, 200);
color black = color(0, 158, 0);
color red = color(255, 0, 0);
int idx = 0;
int stride = 100;
int stride_1 = -150;
int stride_2 = 220;
int val;
int val2;
int input;

void setup()
{
background(255);
size(1200, 900);
clicked = new boolean[3][5];
for (int k = 0; k < 3; k++) {
for (int j = 0; j < 5; j++)
clicked[k][j] = false;
}
mouseClicked = new boolean[3][5];
colors = new color[3][5];
rectanglesX = new float[3][5];
rectanglesY = new float[3][5];
for (int i = 0; i < 3; i++) {
for (int j = 0; j < 5; j++) {
rectanglesX[i][j] = x * (j + 1) + j * (w + stride);
rectanglesY[i][j] = y * (i + 1) + i * (h + stride);
colors[i][j] = darkYellow;
}
}

mouseClicked_1 = new boolean[3][5];
colors_1 = new color[3][5];
rectanglesX_1 = new float[3][3];
rectanglesY_1 = new float[3][3];
for (int i = 0; i < 3; i++) {
for (int j = 0; j < 3; j++) {
rectanglesX_1[i][j] = small_x * (j + 1) + j * (small_w + stride_1);
rectanglesY_1[i][j] = small_y * (i + 1) + i * (small_h + stride_2);
colors_1[i][j] = darkYellow;
}
}

String arduinoPort = Serial.list()[1];
port = new Serial(this, arduinoPort, 9600);
}



void clearClickedFlag()
{
for (int i = 0; i < 3; i++) {
for (int j = 0; j < 5; j++) {
clicked[i][j] = false;
}
}
}

void clearMouseClickedFlag()
{
for (int i = 0; i < 3; i++) {
for (int j = 0; j < 5; j++) {
mouseClicked[i][j] = false;
mouseClicked_1[i][j] = false;
}
}
}

void checkClickedFlag(int row, int col)
{
clearClickedFlag();
clicked[row][col] = false;
}

void setRfidValue(int rfidValue)
{
int row = rfidValue % 10 - 1;
int col = rfidValue / 10 - 1;
row = 2 - row;
clicked[row][col] = true;
}

void serialEvent (Serial port) {
val = port.read();
input= val;
println("processing getData", val, val2, input);
}

void update()
{
clearClickedFlag();

for (int i = 0; i < 3; i++) {
for (int j = 0; j < 5; j++) {
clicked[i][j] = mouseClicked[i][j];
}
}

if (11 <= input && input <= 35) {
setRfidValue(input);
}

for (int i = 0 ; i < 3; i++)
{
for (int j = 0; j < 5; j++)
{
if (mouseClicked[i][j]) {
colors[i][j] = red;
colors_1[i][j] = darkYellow;
}
else if (clicked[i][j]) {
colors[i][j] = black;
}
else if (mouseClicked_1[i][j]) {
colors_1[i][j] = red;
colors[i][j] = darkYellow;
}
else {
colors[i][j] = darkYellow;
colors_1[i][j] = darkYellow;
}
}
}
}

void render()
{
fill(darkYellow);

for (int i = 0 ; i < 3; i++)
{
for (int j = 0; j < 5; j++)
{
fill(colors[i][j]);
rect(rectanglesX[i][j], rectanglesY[i][j], w, h, 20);
}
}

for (int i = 0 ; i < 3; i++)
{
for (int j = 0; j < 3; j++)
{
fill(colors_1[i][j]);
rect(rectanglesX_1[i][j], rectanglesY_1[i][j], small_w, small_h, 10);
}
}


}

void draw()
{
update();
render();
delay(100);
}


void mousePressed()
{
clearMouseClickedFlag();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
if(mouseX >x && mouseX < x+w && mouseY > y && mouseY < y + w) {
mouseClicked[0][0] = true;
port.write("313");
}
else if(mouseX > x * 2 + w + stride && mouseX < x * 2 + 2 * w + stride && mouseY>y && mouseY < y+w) {
mouseClicked[0][1] = true;
port.write("323");

}
else if(mouseX > x * 3 + w + 3 * stride && mouseX < x * 3 + 3 * w + 2 * stride && mouseY > y && mouseY < y + w) {
mouseClicked[0][2] = true;
port.write("333");

}
else if(mouseX > x * 4 + w + 5 * stride && mouseX < x * 4 + 4 * w + 3 * stride && mouseY > y && mouseY < y + w) {
mouseClicked[0][3] = true;
port.write("343");

}
else if(mouseX> x * 5 + w + 7 * stride && mouseX < x * 5 + 5 * w + 4 * stride && mouseY > y && mouseY < y + w) {
mouseClicked[0][4] = true;
port.write("353");

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(mouseX >x && mouseX < x+w && mouseY > 2 * y + 1 * w + stride && mouseY < 2 * y + 2 * w + stride) {
mouseClicked[1][0] = true;
port.write("312");

}
else if(mouseX > x * 2 + w + stride && mouseX < x * 2 + 2 * w + stride && mouseY > 2 * y + 1 * w + stride && mouseY < 2 * y + 2 * w + stride) {
mouseClicked[1][1] = true;
port.write("322");

}
else if(mouseX > x * 3 + w + 3 * stride && mouseX < x * 3 + 3 * w + 2 * stride && mouseY > 2 * y + 1 * w + stride && mouseY < 2 * y + 2 * w + stride) {
mouseClicked[1][2] = true;
port.write("332");

}
else if(mouseX > x * 4 + w + 5 * stride && mouseX < x * 4 + 4 * w + 3 * stride && mouseY > 2 * y + 1 * w + stride && mouseY < 2 * y + 2 * w + stride) {
mouseClicked[1][3] = true;
port.write("342");

}
else if(mouseX> x * 5 + w + 7 * stride && mouseX < x * 5 + 5 * w + 4 * stride && mouseY > 2 * y + 1 * w + stride && mouseY < 2 * y + 2 * w + stride) {
mouseClicked[1][4] = true;
port.write("352");

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(mouseX >x && mouseX < x+w && mouseY > 2 * y + 1 * w + 4 * stride && mouseY < 2 * y + 2 * w + 4 * stride) {
mouseClicked[2][0] = true;
port.write("311");

}
else if(mouseX > x * 2 + w + stride && mouseX < x * 2 + 2 * w + stride &&  mouseY > 2 * y + 1 * w + 4 * stride && mouseY < 2 * y + 2 * w + 4 * stride) {
mouseClicked[2][1] = true;
port.write("321");

}
else if(mouseX > x * 3 + w + 3 * stride && mouseX < x * 3 + 3 * w + 2 * stride &&  mouseY > 2 * y + 1 * w + 4 * stride && mouseY < 2 * y + 2 * w + 4 * stride) {
mouseClicked[2][2] = true;
port.write("331");

}
else if(mouseX > x * 4 + w + 5 * stride && mouseX < x * 4 + 4 * w + 3 * stride &&  mouseY > 2 * y + 1 * w + 4 * stride && mouseY < 2 * y + 2 * w + 4 * stride) {
mouseClicked[2][3] = true;
port.write("341");

}
else if(mouseX> x * 5 + w + 7 * stride && mouseX < x * 5 + 5 * w + 4 * stride &&  mouseY > 2 * y + 1 * w + 4 * stride && mouseY < 2 * y + 2 * w + 4 * stride) {
mouseClicked[2][4] = true;
port.write("351");

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(mouseX > small_x * 1 && mouseX < small_x+small_w && mouseY > small_y && mouseY < small_y + small_h) {
mouseClicked_1[0][0] = true;
port.write("323");

}
else if(mouseX > small_x * 2 + small_w + stride_1 && mouseX < small_x * 2 + 2 * small_w + stride_1 && mouseY>small_y && mouseY < small_y + small_h) {
mouseClicked_1[0][1] = true;
port.write("233");

}
else if(mouseX > small_x * 3 + small_w + 3 * stride_1 && mouseX < small_x * 3 + 3 * small_w + 2 * stride_1 && mouseY > small_y && mouseY < small_y + small_h) {
mouseClicked_1[0][2] = true;
port.write("243");

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(mouseX > small_x * 1 && mouseX < small_x+small_w && 
        mouseY > 2 * small_y + 1 * small_h + stride_2 && mouseY < 2 * small_y + 2 * small_h + stride_2) {
mouseClicked_1[1][0] = true;
port.write("322");

}
else if(mouseX > small_x * 2 + small_w + stride_1 && mouseX < small_x * 2 + 2 * small_w + stride_1 && 
        mouseY > 2 * small_y + 1 * small_h + stride_2 && mouseY < 2 * small_y + 2 * small_h + stride_2) {
mouseClicked_1[1][1] = true;
port.write("232");

}
else if(mouseX > small_x * 3 + small_w + 3 * stride_1 && mouseX < small_x * 3 + 3 * small_w + 2 * stride_1 && 
        mouseY > 2 * small_y + 1 * small_h + stride_2 && mouseY < 2 * small_y + 2 * small_h + stride_2) {
mouseClicked_1[1][2] = true;
port.write("242");

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if(mouseX > small_x * 1 && mouseX < small_x+small_w && 
        mouseY > 3 * small_y + 2 * small_h + 2 * stride_2 && mouseY < 3 * small_y + 3 *small_h + 2 * stride_2) {
mouseClicked_1[2][0] = true;
port.write("321");

}
else if(mouseX > small_x * 2 + small_w + stride_1 && mouseX < small_x * 2 + 2 * small_w + stride_1 &&
        mouseY > 3 * small_y + 2 * small_h + 2 * stride_2 && mouseY < 3 * small_y + 3 *small_h + 2 * stride_2) {
mouseClicked_1[2][1] = true;
port.write("231");

}
else if(mouseX > small_x * 3 + small_w + 3 * stride_1 && mouseX < small_x * 3 + 3 * small_w + 2 * stride_1 &&
        mouseY > 3 * small_y + 2 * small_h + 2 * stride_2 && mouseY < 3 * small_y + 3 *small_h + 2 * stride_2) {
mouseClicked_1[2][2] = true;
port.write("241");

}

}
