// Pins en config
#define SDA_PIN A4
#define SCL_PIN A5
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define P1UP 2
#define P1DOWN 3
#define P2UP 4
#define P2DOWN 5
#define PAD_LENGTH 10

// Libs
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Vars
float ballLocation[] = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2}; // Start at center
int ballOrientationX = 1; // 0 left, 1 right
int ballOrientationY = 1; // 0 down, 1 up
float xSpeed = 2;
float ySpeed = 2;
int p1Score = 0;
int p2Score = 0;
int padLocation[] = {SCREEN_HEIGHT/2, SCREEN_HEIGHT/2}; // Player1, Player2
int p1botMode = 0;
int p2botMode = 0;

// Define screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(){
    // Start serial for debugging
    Serial.begin(9600);

    // Define input buttons
    pinMode(2, INPUT); //P1 L
    pinMode(3, INPUT); //P1 R
    pinMode(4, INPUT); //P2 L
    pinMode(5, INPUT); //P2 R

    //Start display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }else{
        Serial.println(F("BOOT"));
    }

    // Check for botMode
    if (digitalRead(4) == HIGH && digitalRead(5) == HIGH){
        p2botMode = 1;
    }
    if (digitalRead(2) == HIGH && digitalRead(3) == HIGH){
        p1botMode = 1;
    }
}



void moveBall(){
    // Mechanism to update ball location
    if(ballOrientationX == 0){
        ballLocation[0] = ballLocation[0] - (1 * xSpeed);
    }
    if(ballOrientationX == 1){
        ballLocation[0] = ballLocation[0] + (1 * xSpeed);
    }
    if(ballOrientationY == 0){
        ballLocation[1] = ballLocation[1] - (1 * ySpeed);
    }
    if(ballOrientationY == 1){
        ballLocation[1] = ballLocation[1] + (1 * ySpeed);
    }
}

void movePad(int player, int y){
    // Mechanism to update pad location
    int pad = player;
    padLocation[pad] += y;
}

void blinkScreen(){
    // Function to blink screen on goal
    display.invertDisplay(true);
    delay(500);
    display.invertDisplay(false);
    delay(500);
}

void drawBall(int x, int y){
    // Draw ball at x, y
    display.drawPixel(x+1, y, WHITE);
    display.drawPixel(x-1, y, WHITE);
    display.drawPixel(x, y+1, WHITE);
    display.drawPixel(x, y-1, WHITE);
    display.drawPixel(x, y, WHITE);
}

void drawPads(){
    //Player 1
    display.drawPixel(2, padLocation[0], WHITE);
    for (int i=0; i<PAD_LENGTH/2; i++){
        display.drawPixel(2, padLocation[0]+i, WHITE);
        display.drawPixel(2, padLocation[0]-i, WHITE);
    }
    //Player 2
    display.drawPixel(SCREEN_WIDTH-2, padLocation[1], WHITE);
    for (int i=0; i<PAD_LENGTH/2; i++){
        display.drawPixel(SCREEN_WIDTH-2, padLocation[1]+i, WHITE);
        display.drawPixel(SCREEN_WIDTH-2, padLocation[1]-i, WHITE);
    }
}

void drawScore(){
    // Always show score on screen
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(SCREEN_WIDTH/2-15, 0);
    display.print(p1Score);
    display.setCursor(SCREEN_WIDTH/2+10, 0);
    display.print(p2Score);
}

void drawLine(){
    // Center line
    for (int i=0; i<SCREEN_HEIGHT; i = i+2){
        display.drawPixel(SCREEN_WIDTH/2, i, WHITE);
    }
}

void checkButtons(){
    // Mechanism to define button behaviour
    if (digitalRead(P1UP) == HIGH){
        if (padLocation[0] > PAD_LENGTH/2){
            padLocation[0] = padLocation[0] - 2;
        }
    }
    if (digitalRead(P1DOWN) == HIGH){
        if (padLocation[0] < SCREEN_HEIGHT - PAD_LENGTH/2){
            padLocation[0] = padLocation[0] + 2;
        }
    }
    if (digitalRead(P2UP) == HIGH){
        if (padLocation[1] > PAD_LENGTH/2){
            padLocation[1] = padLocation[1] - 2;
        }
    }
    if (digitalRead(P2DOWN) == HIGH){
        if (padLocation[1] < SCREEN_HEIGHT - PAD_LENGTH/2){
            padLocation[1] = padLocation[1] + 2;
        }
    }
}

void checkBounce(){
    // P2 score
    if(ballLocation[0] <= 0+1){
        p2Score++;
        blinkScreen();
        resetBall();
    }
    // P1 score
    if(ballLocation[0] >= SCREEN_WIDTH-1){
        p1Score++;
        blinkScreen();
        resetBall();
    }
    // Bounce bottom
    if(ballLocation[1] <= 0+1){
        ballOrientationY = 1;
    }
    // Bounce top
    if(ballLocation[1] >= SCREEN_HEIGHT-1){
        ballOrientationY = 0;
    }
}

float iToSpeed(int i){
    // Array i map to ball angle
    float result = 0;
    if (i<2){
        result = 2;
        ballOrientationY = 1;
    }else if (i<4){
        ballOrientationY = 1;
        result = 1;
    }else if (i<7){
        result = 0;
    }else if (i<9){
        ballOrientationY = 0;
        result = 1;
    }else{
        ballOrientationY = 0;
        result = 2;
    }
    return result;
}

void checkPadBounce(){
    int p1pad[11];
    int p2pad[11];
    // Fill array with pad pixels
    for (int i=0; i<PAD_LENGTH/2; i++){
        p1pad[i] = padLocation[0] + i;
        p2pad[i] = padLocation[1] + i;
    }
    p1pad[PAD_LENGTH/2] = padLocation[0] + PAD_LENGTH/2;
    p2pad[PAD_LENGTH/2] = padLocation[1] + PAD_LENGTH/2;
    for (int i=0; i<PAD_LENGTH/2; i++){
        p1pad[i+PAD_LENGTH/2+1] = padLocation[0] - i;
        p2pad[i+PAD_LENGTH/2+1] = padLocation[1] - i;
    }
    for (int i=0; i<11; i++){
        if (int(ballLocation[1]) == p1pad[i] && int(ballLocation[0]) <= 3){
            // P1 Pad hit
            ballOrientationX = 1;
            ySpeed = iToSpeed(i);
            break;
        }
        if (int(ballLocation[1]) == p2pad[i] && int(ballLocation[0]) >= SCREEN_WIDTH - 4){
            // P2 Pad hit
            ballOrientationX = 0;
            ySpeed = iToSpeed(i);
            break;
        }
    }
}

void resetBall(){
    // Reset ball to center
    ballLocation[0] = SCREEN_WIDTH/2;
    ballLocation[1] = SCREEN_HEIGHT/2;
}

void botMove(){
    // Mechanism to control bot if enabled
    if (p1botMode == 1){
        if (padLocation[0] < int(ballLocation[1])){
            padLocation[0] = padLocation[0]+2;
        }else if (padLocation[0] > int(ballLocation[1])){
            padLocation[0] = padLocation[0]-2;
        }
    }
    if (p2botMode == 1){
        if (padLocation[1] < int(ballLocation[1])){
            padLocation[1] = padLocation[1]+2;
        }else if (padLocation[1] > int(ballLocation[1])){
            padLocation[1] = padLocation[1]-2;
        }
    }
}

void loop(){
    // Update buttonstate
    checkButtons();

    // Move bot if enabled
    botMove();

    // Move ball
    moveBall();

    // Check collisions
    checkBounce();
    checkPadBounce();

    // Update display
    display.clearDisplay();
    drawBall(ballLocation[0],ballLocation[1]);
    drawPads();
    drawScore();
    drawLine();
    display.display();
}
