
// Screen = 128x64
#define WIDTH 128
#define HEIGHT 64
void setup(){
    const int ballLocation[] = {WIDTH/2, HEIGHT/2}; // Start at center
    const int ballOrientation = 1; // 0 left, 1 right
    const int xSpeed = 1;
    const int ySpeed = 1;
    const int playerAScore = 0;
    const int playerBScore = 0;
    const int padLocation[] = {HEIGHT/2, HEIGHT/2}; // Player1, Player2
}

void moveBall(int x, int y){
    int newX = x;
    int newY = y;
    ballLocation[0] = ballLocation[0] + (newX * xSpeed);
    ballLocation[1] = ballLocation[1] + (newY * Yspeed);
}

void movePad(int player, int y){
    int pad = player;
    padLocation[pad] += y;
}
void loop(){
    
}
