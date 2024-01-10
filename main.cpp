/*
 * Project made for the course Podstawy Programowania 2023/2024
 * author: Szymon Różycki
 */

#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}


// SCREEN RESOLUTION

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	650

const int maximum_jump = SCREEN_HEIGHT / 8;
const int maximum_barrels = 500;
const int barrel_jump_value = 300;
const int treasure_value = 2000;
const int listCapacity = 100;
const int visibleList = 15;


// STRUCTURES

struct Player {
	double x, y;
	bool isJumping, isFalling;
	int lifes;
	int score;
	bool playing;
	bool canLadder;
	bool onLadder = false;
	bool topLadder = false;
	bool botLadder = false;
	bool endedGame = false;
	bool pause = false;
	char nickname[30];
	bool moveLeft = false;
	bool moveRight = false;
};


struct Platform {
	int xBegin;
	int xEnd;
	int length;
	int y;
};


struct Ladder {
	int x, y;
	int xBegin = x - 25;
	int xEnd = x + 25;
	int yBegin = y - 50;
	int yEnd = y + 50;
};


struct Scores {
	char nickname[20];
	int score;
};


struct gameGraphics {
	SDL_Surface* charset;
	SDL_Surface* gorilla, * main_character;
	SDL_Surface* main_left, * main_right, * main_jump_left, * main_jump_right, * main_ladder_left, * main_ladder_right;;
	SDL_Surface* platform, * ladder;
	SDL_Surface* logo, * highscore, * no_option, * select_stage, * exit, * cursor;
	SDL_Surface* heart, * no_heart;
	SDL_Surface* treasure;
	SDL_Surface* barrel;
	SDL_Surface* barrel_two, * barrel_four;
	SDL_Surface* gorilla_prepare, * gorilla_barrel;
	SDL_Surface* main_left_two, * main_right_two;
	SDL_Surface* gorilla_head;
	SDL_Surface* reward;
};


struct Barrel {
	int id;
	double x, y;
	bool isFalling;
	bool end;
	bool jumpedOver = false;
};


struct Treasure {
	double x, y;
	bool collected = false;
};


struct Colors {
	int black, green, red, blue, gray, dark_red, dark_purple, purple, brown, dark_brown, green_two, light_green;
};



// FUNCTIONS - DECLARATIONS

void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void jump(int* jump, bool* jumping, int delta);
void DrawPlatforms(SDL_Surface* screen, SDL_Surface* platform, Platform* platformarray);
void DrawLadders(SDL_Surface* screen, SDL_Surface* ladder, Ladder* ladderarray);
void FreeMemory(SDL_Surface* charset, SDL_Surface* screen, SDL_Texture* scrtex,
	SDL_Renderer* renderer, SDL_Window* window);
void RenderingScreen(SDL_Texture* scrtex, SDL_Renderer* renderer, SDL_Surface* screen);
void CheckInMap(Player* player);
void HandleMovement(Player* player);
void InitializeGraphics(gameGraphics* graphics);
void FreeGraphics(gameGraphics* graphics);
void CheckIfAtLadder(Player* player, Ladder* ladder, int numberOfLadders);
void LadderAnimation(SDL_Surface** main_character, gameGraphics graphic, Player player);
bool CheckPlatformPlayer(Player* player, Platform* platform, int numberOfPlatforms, int jumping);
void ShowLifes(SDL_Surface* screen, gameGraphics graphics, Player player);
void InitializeInitialPlayerParameters(Player* player);
void InitializeTreasure(Treasure* treasure);
void HandleJump(Player* player, double* jumping);
void SpawnBarrels(Barrel* barrelsArray, double Time, int* barrelNumber);
void PrintBarrels(Barrel* barrelsArray, SDL_Surface* skin, int barrelNumber, SDL_Surface* screen);
bool CheckPlatformBarrel(Barrel* barrel, Platform* platform, int numberOfPlatforms);
void ChangeBarrelDirection(Barrel* barrelArray, int index);
void BarrelMovement(Barrel* barrelsArray, int barrelNumber, int numberOfPlatforms, Platform* platform, SDL_Surface** skin, gameGraphics* graphics, int n);
void BarrelAnimation(Barrel barrel, SDL_Surface** skin, gameGraphics* graphics, int n, int id);
void AntagonistAnimation(SDL_Surface** skin, double Time, gameGraphics* graphics);
void CollectTreasure(Player* player, Treasure* treasure);
void CheckEnd(Player* player, double time);
char* WriteNickname(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics, Player* player, Colors colors);
void CollisionWithBarrel(Barrel* barrelArray, Player* player, int* barrelNumber, bool* jumpedOver);
void SaveScoreToFile(Player* player, char* nickname);
void ReadHighscores(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics);
void Death(Player* player, Barrel* barrels, int* barrelNumber);
void UnspawnBarrels(Barrel* barrels, int* barrelNumber);
void LifeGonePause(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics, Platform* platformArray, Ladder* ladderArray, Player* player, Colors colors);
void InitializeColors(Colors* colorPanel, SDL_Surface* screen);
void ShowHeader(SDL_Surface* screen, gameGraphics graphics, Player player, double worldTime, Colors color);
void PlayerAnimation(SDL_Surface** skin, gameGraphics graphics, Player player);


// FUNCTIONS - BODIES

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


void jump(int* jump, bool* jumping, int delta) {

	int maximum_height = 20;

	if (*jumping) {
		*jump = 1000 * delta;
	}

	if (*jumping > maximum_height)
		*jumping = false;

}


void DrawPlatforms(SDL_Surface* screen, SDL_Surface* platform, Platform* platformarray) {
	int lengthPlatform = 0;

	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < platformarray[i].length; ++j) {
			DrawSurface(screen, platform, platformarray[i].xBegin + lengthPlatform, platformarray[i].y);
			lengthPlatform += 100;
		}
		lengthPlatform = 0;
	}

}


void DrawLadders(SDL_Surface* screen, SDL_Surface* ladder, Ladder* ladderarray) {

	for (int i = 0; i < 6; ++i) {
		DrawSurface(screen, ladder, ladderarray[i].x, ladderarray[i].y);
	}

}


void FreeMemory(SDL_Surface* charset, SDL_Surface* screen, SDL_Texture* scrtex,
	SDL_Renderer* renderer, SDL_Window* window) {

	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}


void CheckInMap(Player* player) {
	if (player->x > SCREEN_WIDTH)
		player->x -= 10;
	else if (player->x < 0)
		player->x += 10;
}


void RenderingScreen(SDL_Texture* scrtex, SDL_Renderer* renderer, SDL_Surface* screen) {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void InitializeGraphics(gameGraphics* graphics) {
	graphics->charset = SDL_LoadBMP("./graphics/cs8x8.bmp");
	graphics->main_character = SDL_LoadBMP("./graphics/pik.bmp");
	graphics->gorilla = SDL_LoadBMP("./graphics/gorilla.bmp");
	graphics->main_left = SDL_LoadBMP("./graphics/character_left.bmp");
	graphics->main_right = SDL_LoadBMP("./graphics/character_right.bmp");
	graphics->main_jump_left = SDL_LoadBMP("./graphics/character_jump_left.bmp");
	graphics->main_jump_right = SDL_LoadBMP("./graphics/character_jump_right.bmp");
	graphics->platform = SDL_LoadBMP("./graphics/platform.bmp");
	graphics->ladder = SDL_LoadBMP("./graphics/ladder.bmp");
	graphics->logo = SDL_LoadBMP("./graphics/Logo.bmp");
	graphics->no_option = SDL_LoadBMP("./graphics/no_option.bmp");
	graphics->select_stage = SDL_LoadBMP("./graphics/select_stage.bmp");
	graphics->highscore = SDL_LoadBMP("./graphics/highscore.bmp");
	graphics->exit = SDL_LoadBMP("./graphics/exit.bmp");
	graphics->cursor = SDL_LoadBMP("./graphics/cursor.bmp");
	graphics->main_ladder_left = SDL_LoadBMP("./graphics/character_ladder_left.bmp");
	graphics->main_ladder_right = SDL_LoadBMP("./graphics/character_ladder_right.bmp");
	graphics->heart = SDL_LoadBMP("./graphics/heart.bmp");
	graphics->no_heart = SDL_LoadBMP("./graphics/no_heart.bmp");
	graphics->treasure = SDL_LoadBMP("./graphics/treasure.bmp");
	graphics->barrel = SDL_LoadBMP("./graphics/barrel_one.bmp");
	graphics->barrel_two = SDL_LoadBMP("./graphics/barrel_left.bmp");
	graphics->barrel_four = SDL_LoadBMP("./graphics/barrel_left3.bmp");
	graphics->gorilla_prepare = SDL_LoadBMP("./graphics/gorilla_prepare.bmp");
	graphics->gorilla_barrel = SDL_LoadBMP("./graphics/gorilla_barrel.bmp");
	graphics->main_left_two = SDL_LoadBMP("./graphics/character_left_two.bmp");
	graphics->main_right_two = SDL_LoadBMP("./graphics/character_right_two.bmp");
	graphics->gorilla_head = SDL_LoadBMP("./graphics/gorilla_head.bmp");
	graphics->reward = SDL_LoadBMP("./graphics/cash.bmp");
}


void FreeGraphics(gameGraphics* graphics) {
	SDL_FreeSurface(graphics->charset);
	SDL_FreeSurface(graphics->main_character);
	SDL_FreeSurface(graphics->gorilla);
	SDL_FreeSurface(graphics->main_left);
	SDL_FreeSurface(graphics->main_right);
	SDL_FreeSurface(graphics->main_jump_left);
	SDL_FreeSurface(graphics->main_jump_right);
	SDL_FreeSurface(graphics->platform);
	SDL_FreeSurface(graphics->ladder);
	SDL_FreeSurface(graphics->logo);
	SDL_FreeSurface(graphics->no_option);
	SDL_FreeSurface(graphics->select_stage);
	SDL_FreeSurface(graphics->highscore);
	SDL_FreeSurface(graphics->exit);
	SDL_FreeSurface(graphics->cursor);
	SDL_FreeSurface(graphics->main_ladder_left);
	SDL_FreeSurface(graphics->main_ladder_right);
	SDL_FreeSurface(graphics->heart);
	SDL_FreeSurface(graphics->no_heart);
	SDL_FreeSurface(graphics->treasure);
	SDL_FreeSurface(graphics->barrel);
	SDL_FreeSurface(graphics->barrel_two);
	SDL_FreeSurface(graphics->barrel_four);
	SDL_FreeSurface(graphics->gorilla_prepare);
	SDL_FreeSurface(graphics->gorilla_barrel);
	SDL_FreeSurface(graphics->main_left_two);
	SDL_FreeSurface(graphics->main_right_two);
	SDL_FreeSurface(graphics->gorilla_head);
	SDL_FreeSurface(graphics->reward);
}


void CheckIfAtLadder(Player* player, Ladder* ladder, int numberOfLadders) {

	if (player->isFalling || player->isJumping)
		return;


	for (int i = 0; i < numberOfLadders; ++i) {
		if ((player->y + 50) == ladder[i].yEnd && player->x > ladder[i].xBegin && player->x < ladder[i].xEnd)
		{
			player->botLadder = true;
			player->canLadder = false;
			player->onLadder = false;
			return;
		}
		else if (player->y + 150 == ladder[i].yEnd && player->x > ladder[i].xBegin && player->x < ladder[i].xEnd)
		{
			player->topLadder = true;
			player->canLadder = false;
			player->onLadder = false;
			return;
		}
		else if ((player->x > ladder[i].xBegin && player->x < ladder[i].xEnd) && (player->y + 150 > ladder[i].yEnd) && (player->y + 50) <= ladder[i].yEnd) {
			player->canLadder = true;
			player->botLadder = false;
			player->topLadder = false;
			return;
		}
	}
	player->canLadder = false;
	player->topLadder = false;
	player->botLadder = false;
}


bool CheckPlatformPlayer(Player* player, Platform* platform, int numberOfPlatforms, int jumping) {

	for (int i = 0; i < numberOfPlatforms; ++i) {
		if (player->x + 30 < platform[i].xEnd && player->x + 70 > platform[i].xBegin)
			if ((player->y + 60 + jumping) == platform[i].y && (player->y + jumping < platform[i].y))
				return true;
	}

	return false;
}


bool CheckPlatformBarrel(Barrel* barrel, Platform* platform, int numberOfPlatforms) {

	for (int i = 0; i < numberOfPlatforms; ++i) {
		if (barrel->x + 45 < platform[i].xEnd && barrel->x + 50 > platform[i].xBegin)
			if ((barrel->y + 25) >= platform[i].y && barrel->y < platform[i].y)
				return true;
	}

	return false;
}


void LadderAnimation(SDL_Surface** main_character, gameGraphics graphic, Player player) {

	if (int(player.y) % 2 == 0)
		*main_character = graphic.main_ladder_left;
	else
		*main_character = graphic.main_ladder_right;

}


void ShowLifes(SDL_Surface* screen, gameGraphics graphics, Player player) {

	const int posY = 40;
	int posX = 80;

	SDL_Surface* life = NULL;

	for (int i = 0; i < 3; ++i) {

		if (player.lifes > i)
			life = graphics.heart;
		else
			life = graphics.no_heart;

		DrawSurface(screen, life, posX, posY);
		posX += 60;
	}

}


void InitializeInitialPlayerParameters(Player* player) {
	player->x = SCREEN_WIDTH / 2;
	player->y = SCREEN_HEIGHT - 75;
	player->playing = false;
	player->isFalling = false;
	player->isJumping = false;
	player->lifes = 3;
	player->score = 0;
	player->onLadder = false;
}


void HandleJump(Player* player, double* jumping) {
	if (player->isJumping) {
		player->y -= 0.5;
		*jumping -= 0.5;
	}

	if (fabs(*jumping) >= maximum_jump) {
		player->isJumping = false;
		player->isFalling = true;
		*jumping = 0;
	}
}


void HandleMovement(Player* player) {
	
	if (player->moveLeft) {
		player->x -= 0.4;
	}
	else if (player->moveRight) {
		player->x += 0.4;
	}

}


void SpawnBarrels(Barrel* barrelsArray, double Time, int* barrelNumber) {
	if (int(Time) == 0 && *barrelNumber <= maximum_barrels)
		return;


	if (int(Time) % 8 == 0 && fabs(floor(Time) - Time) < 0.01) {
		barrelsArray[*barrelNumber].id = 1;
		barrelsArray[*barrelNumber].x = SCREEN_WIDTH / 2 + 170;
		barrelsArray[*barrelNumber].y = SCREEN_HEIGHT - 390;
		barrelsArray[*barrelNumber].isFalling = false;
		barrelsArray[*barrelNumber].end = false;
		barrelsArray[*barrelNumber].jumpedOver = false;
		*barrelNumber += 1;
	}
	else if (int(Time) % 4 == 0 && fabs(floor(Time) - Time) < 0.01) {
		barrelsArray[*barrelNumber].id = 0;
		barrelsArray[*barrelNumber].x = SCREEN_WIDTH / 2 - 170;
		barrelsArray[*barrelNumber].y = SCREEN_HEIGHT - 390;
		barrelsArray[*barrelNumber].isFalling = false;
		barrelsArray[*barrelNumber].end = false;
		barrelsArray[*barrelNumber].jumpedOver = false;
		*barrelNumber += 1;
	}



}


void PrintBarrels(Barrel* barrelsArray, SDL_Surface* skin, int barrelNumber, SDL_Surface* screen) {
	for (int i = 0; i < barrelNumber; ++i) {
		DrawSurface(screen, skin, barrelsArray[i].x, barrelsArray[i].y);
	}
}


void BarrelMovement(Barrel* barrelsArray, int barrelNumber, int numberOfPlatforms, Platform* platform, SDL_Surface** skin, gameGraphics* graphics, int n) {

	bool change = 0;

	for (int i = 0; i < barrelNumber; ++i) {
		if (!barrelsArray[i].end) {
			if (CheckPlatformBarrel(&barrelsArray[i], platform, 7)) {
				barrelsArray[i].isFalling = false;

				if (barrelsArray[i].id == 0) {
					barrelsArray[i].x -= 0.3;

					BarrelAnimation(barrelsArray[i], skin, graphics, n, barrelsArray[i].id);
				}
				else {
					barrelsArray[i].x += 0.3;

					BarrelAnimation(barrelsArray[i], skin, graphics, n, barrelsArray[i].id);
				}
			}

			else {
				barrelsArray[i].y += 0.25;
				barrelsArray[i].isFalling = true;
				if (CheckPlatformBarrel(&barrelsArray[i], platform, 7))
					ChangeBarrelDirection(barrelsArray, i);
			}
		}

	}

}


void BarrelAnimation(Barrel barrel, SDL_Surface** skin, gameGraphics* graphics, int n, int id) {
	if (!barrel.isFalling) {
		if (id == 0) {
			if (n % 2 == 0) {
				*skin = graphics->barrel_two;
			}
			else {
				*skin = graphics->barrel_four;
			}
		}
		else {
			if (n % 2 == 0) {
				*skin = graphics->barrel_four;
			}
			else {
				*skin = graphics->barrel_two;
			}
		}
	}
}


void ChangeBarrelDirection(Barrel* barrelArray, int index) {
	if (barrelArray[index].id == 0)
		barrelArray[index].id = 1;
	else
		barrelArray[index].id = 0;
}


void AntagonistAnimation(SDL_Surface** skin, double Time, gameGraphics* graphics) {
	double timeWarning = Time + 1.5;
	double timeAfter = Time - 1.5;

	if (int(timeWarning) % 4 == 0 && fabs(floor(timeWarning) - timeWarning) < 0.01)
		*skin = graphics->gorilla_prepare;
	else if (int(Time) % 4 == 0 && fabs(floor(Time) - Time) < 0.01)
		*skin = graphics->gorilla_barrel;
	else if ((int(timeAfter) % 4 == 0 && fabs(floor(timeAfter) - timeAfter) < 0.01))
		*skin = graphics->gorilla;
}


void InitializeTreasure(Treasure* treasure) {
	treasure->x = 50;
	treasure->y = 260;
}


void CollectTreasure(Player* player, Treasure* treasure) {
	if (player->x + 10 >= treasure->x && player->x - 10 <= treasure->x && player->y - 50 <= treasure->y) {
		player->score += 2000;
		treasure->x = -50;
		treasure->y = -50;
		treasure->collected = true;
	}
}


void CheckEnd(Player* player, double time) {
	if (player->x > 480 && player->x < 520 && player->y == 225)
	{
		player->playing = false;
		if (!player->endedGame)
			player->score += 5000 - (time * 30.0);
		player->endedGame = true;

	}
}


void SaveScoreToFile(Player* player, char* nickname) {
	FILE* file = fopen("Highscores.txt", "a");

	if (file == NULL) {
		return;
	}

	fprintf(file, "%s %d", nickname, player->score);
	fputc('\n', file);

	fclose(file);
}


void ReadHighscores(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics) {
	FILE* file = fopen("Highscores.txt", "r");

	Scores* scoreArray = new Scores[listCapacity];

	int numberOfScores = 0;
	SDL_Event event;

	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

	while (fscanf(file, "%19s %d", scoreArray[numberOfScores].nickname, &scoreArray[numberOfScores].score) == 2) {
		numberOfScores++;
	}

	int temporaryValue = 0;
	char test[20];

	for (int i = 0; i < numberOfScores; ++i) {
		for (int j = 0; j < numberOfScores - 1; ++j) {
			if (scoreArray[j + 1].score > scoreArray[j].score) {
				temporaryValue = scoreArray[j].score;
				scoreArray[j].score = scoreArray[j + 1].score;
				scoreArray[j + 1].score = temporaryValue;

				strcpy(test, scoreArray[j].nickname);
				strcpy(scoreArray[j].nickname, scoreArray[j + 1].nickname);
				strcpy(scoreArray[j + 1].nickname, test);
			}
		}
	}

	int positionY = 120;
	int q = 0;

	int currentPosition = 0;
	int visible = 15;

	while (q != 1) {
		SDL_FillRect(screen, NULL, black);
		char txt[128];

		DrawSurface(screen, graphics.highscore, SCREEN_WIDTH / 2, 50);

		for (int i = currentPosition; i <= visible; ++i) {
			sprintf(txt, "%s", scoreArray[i].nickname);
			DrawString(screen, SCREEN_WIDTH / 2 - 120, positionY, txt, graphics.charset);
			sprintf(txt, "%d", scoreArray[i].score);
			DrawString(screen, SCREEN_WIDTH / 2 + 80, positionY, txt, graphics.charset);
			positionY += 30;
		}
		positionY = 120;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					q = 1;
				else if (event.key.keysym.sym == SDLK_DOWN) {
					currentPosition++;
					visible++;
					if (visible >= numberOfScores) {
						visible--;
						currentPosition--;
					}
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					currentPosition--;
					visible--;
					if (currentPosition < 0) {
						currentPosition++;
						visible++;
					}
				}
				break;
			}
			RenderingScreen(scrtex, renderer, screen);
		}
	}

	fclose(file);
}


char* WriteNickname(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics, Player* player, Colors colors) {

	char* nickname = (char*)malloc(20 * sizeof(char));
	memset(nickname, 0, 20 * sizeof(char));
	int index = 0;
	bool written = false;
	SDL_Event event;
	char txt[128];

	while (!written) {
		SDL_FillRect(screen, NULL, colors.black);

		DrawRectangle(screen, 340, 255, 400, 100, colors.dark_purple, colors.purple);
		DrawSurface(screen, graphics.gorilla_head, SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 - 105);
		sprintf(txt, "Congratulations! You have scored: %d", player->score);
		DrawString(screen, 390, 280, txt, graphics.charset);
		sprintf(txt, "Enter your nickname to save the score: ");
		DrawString(screen, 400, 300, txt, graphics.charset);
		DrawString(screen, screen->w / 2 - strlen(nickname) - 60, 320, nickname, graphics.charset);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN && index != 0) {
					written = true;
					nickname[index] = '\0';
				}
				else if (event.key.keysym.sym == SDLK_BACKSPACE && index != 0) {
					index -= 1;
					nickname[index] = '\0';
				}
				break;
			case SDL_TEXTINPUT:
				if (index < 19 && event.text.text[0] != '\0' && event.text.text[0] != ' ') {
					nickname[index] = event.text.text[0];
					index++;
					nickname[index] = '\0';
				}
				break;
			}
			RenderingScreen(scrtex, renderer, screen);
		}
	}

	return nickname;
}


void Death(Player* player, Barrel* barrels, int* barrelNumber) {
	if (player->lifes > 1) {
		player->lifes -= 1;
		player->x = SCREEN_WIDTH / 2;
		player->y = SCREEN_HEIGHT - 75;
		player->isFalling = false;
		player->isJumping = false;
		player->onLadder = false;
		player->pause = true;
	}
	else if (player->lifes == 1)
	{
		player->lifes -= 1;
		player->endedGame = true;
		player->playing = false;
	}

	UnspawnBarrels(barrels, barrelNumber);
	player->moveLeft = false;
	player->moveRight = false;
}


void UnspawnBarrels(Barrel* barrels, int* barrelNumber) {
	*barrelNumber = 0;
}


void CollisionWithBarrel(Barrel* barrelArray, Player* player, int* barrelNumber, bool* jumpedOver) {
	for (int i = 0; i < *barrelNumber; ++i) {
		if ((player->x - 30 <= barrelArray[i].x && player->x - 15 >= barrelArray[i].x) ||
			(player->x + 30 >= barrelArray[i].x && player->x + 15 <= barrelArray[i].x)) {
			if (player->y + 38 >= barrelArray[i].y && player->y - 45 <= barrelArray[i].y) {
				Death(player, barrelArray, barrelNumber);
			}
			else if (player->y + 80 >= barrelArray[i].y && !(player->y + 30 >= barrelArray[i].y) && !barrelArray[i].jumpedOver && !barrelArray[i].isFalling) {
				barrelArray[i].jumpedOver = true;
				player->score += 50;
				*jumpedOver = true;
			}
		}
		else {

		}
	}
}


void LifeGonePause(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer, gameGraphics graphics, Platform* platformArray, Ladder* ladderArray, Player* player, Colors colors) {
	int q = 0;
	SDL_Event event;



	double dist = 0;

	while (q != 1) {
		SDL_FillRect(screen, NULL, colors.black);
		char txt[128];

		DrawPlatforms(screen, graphics.platform, platformArray);
		DrawLadders(screen, graphics.ladder, ladderArray);

		DrawRectangle(screen, 330, 120, 355, 100, colors.purple, colors.dark_purple);
		sprintf(txt, "You have just been hit by the Barrel!");
		DrawString(screen, 360, 135, txt, graphics.charset);
		sprintf(txt, "You've scored %d points so far", player->score);
		DrawString(screen, 375, 155, txt, graphics.charset);
		sprintf(txt, "Would you like to continue playing?");
		DrawString(screen, 375, 175, txt, graphics.charset);
		sprintf(txt, "Enter - continue \t ESC - quit");
		DrawString(screen, 375, 195, txt, graphics.charset);

		DrawSurface(screen, graphics.gorilla_head, SCREEN_WIDTH / 6 + sin(dist) * SCREEN_HEIGHT / 8,
			SCREEN_WIDTH / 6 + cos(dist) * SCREEN_HEIGHT / 8);
		DrawSurface(screen, graphics.gorilla_head, SCREEN_WIDTH / 1.175 + sin(dist) * SCREEN_HEIGHT / 8,
			SCREEN_WIDTH / 6 + cos(dist) * SCREEN_HEIGHT / 8);
		dist += 0.01;


		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					q = 1;
					char* nickname = WriteNickname(screen, scrtex, renderer, graphics, player, colors);
					SaveScoreToFile(player, nickname);
					player->lifes = 0;
					player->playing = false;
					player->pause = false;
				}
				else if (event.key.keysym.sym == SDLK_RETURN) {
					q = 1;
					player->pause = false;
				}
				break;
			}
		}
		RenderingScreen(scrtex, renderer, screen);
	}
}


void ShowHeader(SDL_Surface* screen, gameGraphics graphics, Player player, double worldTime, Colors color) {

	char text[128];

	DrawRectangle(screen, 30, 4, SCREEN_WIDTH - 75, 70, color.purple, color.dark_purple);
	//sprintf(text, "KING DONKEY - 193390");
	//DrawString(screen, screen->w / 2 - strlen(text) - 60, 20, text, graphics.charset);

	DrawLine(screen, 50, 4, 70, 1, 1, color.purple);
	DrawLine(screen, 40, 4, 70, 1, 1, color.purple);
	DrawLine(screen, 30, 4, 70, 1, 1, color.purple);

	DrawLine(screen, 240, 4, 70, 0, 1, color.purple);
	DrawLine(screen, 235, 4, 70, 0, 1, color.purple);

	DrawLine(screen, 435, 4, 70, 0, 1, color.purple);
	DrawLine(screen, 440, 4, 70, 0, 1, color.purple);

	DrawLine(screen, 735, 4, 70, 0, 1, color.purple);
	DrawLine(screen, 730, 4, 70, 0, 1, color.purple);

	DrawLine(screen, SCREEN_WIDTH - 70, 4, 70, -1, 1, color.purple);
	DrawLine(screen, SCREEN_WIDTH - 60, 4, 70, -1, 1, color.purple);
	DrawLine(screen, SCREEN_WIDTH - 50, 4, 70, -1, 1, color.purple);

	sprintf(text, "Stage of the game: 1");
	DrawString(screen, screen->w / 2 - strlen(text) - 220, 15, text, graphics.charset);
	sprintf(text, "Score : %d", player.score);
	DrawString(screen, screen->w / 2 - strlen(text) - 232, 35, text, graphics.charset);
	sprintf(text, "Time = %.1lf s", worldTime);
	DrawString(screen, screen->w / 2 - strlen(text) - 228, 55, text, graphics.charset);
	sprintf(text, "Position X: %f", player.x);
	DrawString(screen, screen->w / 2 - strlen(text) + 280, 25, text, graphics.charset);
	sprintf(text, "Position Y: %f", player.y);
	DrawString(screen, screen->w / 2 - strlen(text) + 280, 45, text, graphics.charset);
	sprintf(text, "Walking: <-, ->");
	DrawString(screen, screen->w / 2 - strlen(text) - 30 , 25, text, graphics.charset);
	sprintf(text, "Ladder walk: v, ^");
	DrawString(screen, screen->w / 2 - strlen(text) - 30, 45, text, graphics.charset);
	sprintf(text, "Jump: space");
	DrawString(screen, screen->w / 2 - strlen(text) + 120, 25, text, graphics.charset);
	sprintf(text, "New game: n");
	DrawString(screen, screen->w / 2 - strlen(text) + 120, 45, text, graphics.charset);

}


void InitializeColors(Colors* colorPanel, SDL_Surface *screen) {
	colorPanel->black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	colorPanel->green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	colorPanel->red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	colorPanel->blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	colorPanel->gray = SDL_MapRGB(screen->format, 0x80, 0x80, 0x80);
	colorPanel->dark_red = SDL_MapRGB(screen->format, 0xA4, 0x19, 0x19);
	colorPanel->dark_purple = SDL_MapRGB(screen->format, 0x05, 0x17, 0x24);
	colorPanel->purple = SDL_MapRGB(screen->format, 0x19, 0x42, 0x5E);
	colorPanel->brown = SDL_MapRGB(screen->format, 0x4D, 0x0F, 0x0F);
	colorPanel->dark_brown = SDL_MapRGB(screen->format, 0x5F, 0x34, 0x34);
	colorPanel->green_two = SDL_MapRGB(screen->format, 0x4D, 0x0F, 0x0F);
	colorPanel->light_green = SDL_MapRGB(screen->format, 0x5F, 0x34, 0x34);
}


void PlayerAnimation(SDL_Surface** skin, gameGraphics graphics, Player player) {
	if (player.moveLeft) {
		if (*skin == graphics.main_left)
			*skin = graphics.main_left_two;
		else if (*skin = graphics.main_left_two)
			*skin = graphics.main_left;

		if (player.isJumping || player.isFalling)
			*skin = graphics.main_jump_left;
	}
	else if (player.moveRight) {
		if (*skin == graphics.main_right)
			*skin = graphics.main_right_two;
		else if (*skin = graphics.main_right_two)
			*skin = graphics.main_right;

		if (player.isJumping || player.isFalling)
			*skin = graphics.main_jump_right;
	}
	else if (player.isJumping) {
		if (*skin == graphics.main_left)
			*skin = graphics.main_jump_left;
		else *skin = graphics.main_jump_right;
	}
	else if (player.onLadder) {
		LadderAnimation(skin, graphics, player);
	}


}


#ifdef __cplusplus
extern "C"
#endif


// MAIN

int main(int argc, char** argv) {
	int t1, t2, quit = 0, renderCheck;
	double delta, worldTime = 0;
	SDL_Event event;
	SDL_Surface* screen;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	gameGraphics graphics;
	InitializeGraphics(&graphics);


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	renderCheck = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "King Donkey - 193390");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetColorKey(graphics.charset, true, 0x000000);


	Colors color_panel;
	InitializeColors(&color_panel, screen);

	char text[128];
	double jumping = 0;
	double moving = 0;

	SDL_Surface* characterGraphic = graphics.main_left;
	SDL_Surface* barrelSkin = graphics.barrel;
	SDL_Surface* antagonistSkin = graphics.gorilla;

	Platform platformsArray[7];
	platformsArray[0] = { 0, 1050, 11, SCREEN_HEIGHT - 15 };
	platformsArray[1] = { 150, 350, 2, SCREEN_HEIGHT - 115 };
	platformsArray[2] = { 750 ,950, 2, SCREEN_HEIGHT - 115 };
	platformsArray[3] = { 400, 700, 3, SCREEN_HEIGHT - 165 };
	platformsArray[4] = { 200, 500, 3, SCREEN_HEIGHT - 265 };
	platformsArray[5] = { 600, 900, 3, SCREEN_HEIGHT - 265 };
	platformsArray[6] = { 300, 800, 5, SCREEN_HEIGHT - 365 };

	Ladder ladderArray[6];
	ladderArray[0] = { 200, SCREEN_HEIGHT - 75 };
	ladderArray[1] = { SCREEN_WIDTH - 200, SCREEN_HEIGHT - 75 };
	ladderArray[2] = { 400, SCREEN_HEIGHT - 225 };
	ladderArray[3] = { 600, SCREEN_HEIGHT - 225 };
	ladderArray[4] = { 300, SCREEN_HEIGHT - 325 };
	ladderArray[5] = { 700, SCREEN_HEIGHT - 325 };

	Player player;
	InitializeInitialPlayerParameters(&player);
	int cursorPos = 0;

	Barrel barrels[maximum_barrels];
	int barrelNumbers = 0;

	Treasure treasure;
	InitializeTreasure(&treasure);


	Scores* scoreArray[listCapacity];
	int n = 0, m = 0;
	bool jumpedOver = false;

	while (!quit) {

		if (!player.playing) {

			SDL_FillRect(screen, NULL, color_panel.black);

			if (player.endedGame) {
				char* nickname = WriteNickname(screen, scrtex, renderer, graphics, &player, color_panel);
				SaveScoreToFile(&player, nickname);
				UnspawnBarrels(barrels, &barrelNumbers);
				player.endedGame = false;
			}


			SDL_FillRect(screen, NULL, color_panel.black);
			DrawSurface(screen, graphics.logo, SCREEN_WIDTH / 2, 125);
			DrawSurface(screen, graphics.select_stage, SCREEN_WIDTH / 2, 300);
			DrawSurface(screen, graphics.highscore, SCREEN_WIDTH / 2, 400);
			DrawSurface(screen, graphics.exit, SCREEN_WIDTH / 2, 500);
			DrawSurface(screen, graphics.cursor, SCREEN_WIDTH / 3.5, 300 + cursorPos);


			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_UP) {
						cursorPos -= 100;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						cursorPos += 100;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) {
						if (cursorPos == 0) {
							InitializeInitialPlayerParameters(&player);
							worldTime = 0;
							player.lifes = 3;
							player.playing = true;
							t1 = SDL_GetTicks();
						}
						else if (cursorPos == 100) {
							ReadHighscores(screen, scrtex, renderer, graphics);
						}
						else if (cursorPos == 200) {
							player.playing = true;
							quit = 1;
						}
					}
					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					break;
				};
			};

			if (cursorPos < 0)
				cursorPos += 100;
			else if (cursorPos > 200)
				cursorPos -= 100;

		}
		else if (player.playing) {

			t2 = SDL_GetTicks();
			delta = (t2 - t1) * 0.001;
			t1 = t2;
			worldTime += delta;

			PlayerAnimation(&characterGraphic, graphics, player);

			CheckEnd(&player, worldTime);

			AntagonistAnimation(&antagonistSkin, worldTime, &graphics);

			SpawnBarrels(barrels, worldTime, &barrelNumbers);

			HandleJump(&player, &jumping);
			HandleMovement(&player);

			CollisionWithBarrel(barrels, &player, &barrelNumbers, &jumpedOver);

			if (player.pause) {
				LifeGonePause(screen, scrtex, renderer, graphics, platformsArray, ladderArray, &player, color_panel);
			}

			if (CheckPlatformPlayer(&player, platformsArray, 7, 0) && !player.onLadder && !player.isJumping) {
				player.isFalling = false;
			}

			if (!CheckPlatformPlayer(&player, platformsArray, 7, 0) && !player.onLadder && !player.isJumping) {
				player.isFalling = true;
			}

			if (player.isFalling) {
				player.y += 0.5;
			}

			BarrelMovement(barrels, barrelNumbers, 7, platformsArray, &barrelSkin, &graphics, n);
			CollectTreasure(&player, &treasure);

			CheckIfAtLadder(&player, ladderArray, 6);

			SDL_FillRect(screen, NULL, color_panel.black);


			if (treasure.collected) {
				sprintf(text, "%d", treasure_value);
				DrawString(screen, player.x - 15, player.y - 90, text, graphics.charset);
				m++;
				if (m > 300) {
					treasure.collected = false;
					m = 0;
				}
			}

			if (jumpedOver) {
				sprintf(text, "++++");
				DrawString(screen, player.x - 15, player.y - 75, text, graphics.charset);
				m++;
				if (m > 300) {
					jumpedOver = false;
					m = 0;
				}
			}



			DrawPlatforms(screen, graphics.platform, platformsArray);
			DrawLadders(screen, graphics.ladder, ladderArray);
			DrawSurface(screen, graphics.treasure, treasure.x, treasure.y);
			DrawSurface(screen, antagonistSkin, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 440);
			DrawSurface(screen, graphics.reward, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 385);
			DrawSurface(screen, characterGraphic, player.x, player.y);

			ShowHeader(screen, graphics, player, worldTime, color_panel);
			ShowLifes(screen, graphics, player);
			PrintBarrels(barrels, barrelSkin, barrelNumbers, screen);



			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_LEFT && !player.onLadder) {
						player.moveLeft = true;
					}
					else if (event.key.keysym.sym == SDLK_RIGHT && !player.onLadder) {
						player.moveRight = true;
					}
					else if ((event.key.keysym.sym == SDLK_UP && player.canLadder && !player.topLadder) || player.botLadder) {
						player.y -= 5;
						player.onLadder = true;
					}
					else if ((event.key.keysym.sym == SDLK_DOWN && player.canLadder && !player.botLadder) || player.topLadder) {
						player.y += 5;
						player.onLadder = true;
					}
					else if (event.key.keysym.sym == SDLK_SPACE && !player.isJumping && !player.isFalling && !player.onLadder) {
						player.isJumping = true;

					}
					else if (event.key.keysym.sym == SDLK_n) {
						InitializeInitialPlayerParameters(&player);
						UnspawnBarrels(barrels, &barrelNumbers);
						player.playing = true;
						worldTime = 0;
					}
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_LEFT)
						player.moveLeft = false;
					else if (event.key.keysym.sym == SDLK_RIGHT)
						player.moveRight = false;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				default:
					break;
				};

			};
			
			CheckInMap(&player);
			n++;
		}

		RenderingScreen(scrtex, renderer, screen);
	};


	FreeMemory(graphics.charset, screen, scrtex, renderer, window);
	FreeGraphics(&graphics);
	free(scoreArray);

	SDL_Quit();
	return 0;
};
