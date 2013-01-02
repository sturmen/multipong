#include <pspkernel.h>
#include <oslib/oslib.h>

#define MAXBALLS 10



PSP_MODULE_INFO("MULTI PONG", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(12*1024);

int i = 0;
int NumOfBalls = 1;
int hits = 0;

struct ball {
	int x;
	int y;
	int vx;
	int vy;
} lob[MAXBALLS];


int initOSLib(){
    oslInit(0);
    oslInitGfx(OSL_PF_8888, 1);
    oslInitAudio();
    oslSetQuitOnLoadFailure(1);
    oslSetKeyAutorepeatInit(40);
    oslSetKeyAutorepeatInterval(10);
    return 0;
}


int startSong(){
	OSL_SOUND *music;
	music = oslLoadSoundFileBGM("song.bgm", OSL_FMT_STREAM);
	oslAssert(music);
	oslPlaySound(music, 0);
	return 0;
}

int newBall(int index) {
	lob[NumOfBalls].x = lob[index].x - 5;
	lob[NumOfBalls].y = lob[index].y;
	lob[NumOfBalls].vx = ((rand() % 4) + 2) * -1;
	lob[NumOfBalls].vy = -1 * lob[index].vy;
	return ++NumOfBalls;

}

int readInput(int py){
	oslReadKeys();
	if (osl_pad.released.start)
		oslQuit();
	if (py >= 5 && osl_pad.held.up)
		py -= 5;
	if (py <= 227 && osl_pad.held.down)
		py += 5;
	if (osl_pad.pressed.cross && NumOfBalls < MAXBALLS) newBall(NumOfBalls - 1);
	return py;
}

int gameOver(int score) {
	char buffer [32];
	int skip = 0;
	for (i = 0; i < 360; i++) {
		if (!skip) {
			oslStartDrawing();
			oslClearScreen(RGB(0,0,0));
			oslDrawString(200,150, "Game Over");
			snprintf(buffer, 32, "Final Score: %d", score);
			oslDrawString(170, 130, buffer);
			oslEndDrawing();
		}
		oslEndFrame();
		skip = oslSyncFrame();
	}
	oslQuit();
	return score;
}

int moveBalls(int py, int score){
	for (i = 0; i < NumOfBalls; i++) {
		lob[i].x += lob[i].vx;
		lob[i].y += lob[i].vy;
		if (lob[i].x > 460) {
			if (lob[i].y > py - 5 && lob[i].y < py + 45) {
				lob[i].vx *= -1;
				if (NumOfBalls < MAXBALLS && hits >= (NumOfBalls * NumOfBalls)) {
					newBall(i);
					hits = 0;
				} else {
					hits++;
				}
			} else {
				gameOver(score);
			}
		}
		if (lob[i].x < 0) {
			lob[i].vx *= -1;
			if (lob[i].vx > 1) {
				lob[i].vx -= 1;
				lob[i].x = 3;
			}
		}
		if (lob[i].y > 260 || lob[i].y < 0)
			lob[i].vy *= -1;
			if (lob[i].vx > 1) {
				lob[i].vx -= 1;
			}
	}
	return 0;
}
int drawScreen(int skip, int py, int score){
	char buffer [32];
	 if (!skip){
            oslStartDrawing();
			oslClearScreen(RGB(0,0,0));
			oslDrawFillRect(470, py, 480, py + 40, RGB(54,204,204));
			for (i = 0; i < NumOfBalls; i++) {
				oslDrawRect(lob[i].x, lob[i].y, lob[i].x + 10, lob[i].y + 10, RGB(255,0,0));
			}
			snprintf(buffer, 32, "Score: %d", score);
			oslDrawString(5, 10, buffer);
            oslEndDrawing();
        }
        oslEndFrame();
        skip = oslSyncFrame();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
    int skip = 0;
	int py = 0;
	int score = 0;
	
    initOSLib();
    oslIntraFontInit(INTRAFONT_CACHE_MED);

    //Load font:
    OSL_FONT *pgfFont = oslLoadFontFile("flash0:/font/ltn0.pgf");
    oslIntraFontSetStyle(pgfFont, 1.0, RGBA(255,255,255,255), RGBA(0,0,0,0), INTRAFONT_ALIGN_LEFT);
    oslSetFont(pgfFont);

	startSong();
	
	lob[0].x = 0;
	lob[0].y = rand() % 250;
	lob[0].vx = 1;
	lob[0].vy = -1;
	
    while(!osl_quit){
		py = readInput(py);
		moveBalls(py, score);
		drawScreen(skip, py, score);
		score += NumOfBalls;
    }
    //Quit OSL:
    oslEndGfx();

    sceKernelExitGame();
    return 0;

}
