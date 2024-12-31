#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef enum {
    LOGO=0,
    TITLE,
    GAMEPLAY,
    ENDING,
    LOSE,
    WIN,
    EXIT
} GameScreen;

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

float getRandomRangeFloat(float min, float max){
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

int getRandomRangeInt(int min, int max){
    return rand() % (max - min + 1) + min;
}

int main(void){

    // Seed the random number generator
    srand(time(NULL));

    bool exitWindowRequested = false;   // Flag to request window to exit
    bool exitWindow = false;            // Flag to set window to exit

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Game");
	SetTargetFPS(60);
	// enable sound
	InitAudioDevice();

	Music music = LoadMusicStream("resources/time_for_adventure.mp3");
	if (IsMusicValid(music)) {
		music.looping = true;
	}
	Music game_over_music = LoadMusicStream("resources/game_over.ogg");
	if (IsMusicValid(game_over_music)) {
		game_over_music.looping = false;
	}

	Sound jump_sound_ogg = LoadSound("resources/jump.ogg"); // Load OGG audio file
	Sound hurt_sound_wav = LoadSound("resources/hurt.wav"); // Load WAV audio file


	Texture title_tex = LoadTexture("resources/cat.png");
	Texture player_run_texture = LoadTexture("resources/scarfy.png");

	Vector2 player_vel;
	Vector2 player_pos;
	bool player_grounded = true;
	bool player_flip = false;
	bool player_moving = false;

	float player_run_width = (float)(player_run_texture.width);
	float player_run_height = (float)(player_run_texture.height);
	int player_run_num_frames = 6;
	float player_wh = 128;
	float player_width = player_run_width / (float)(player_run_num_frames);
	Rectangle player_rect;

	float player_run_frame_timer;
	int player_run_current_frame;
	float player_run_frame_length = 0.1;


	// ---- INIT ----
	float startTime = GetTime();
	GameScreen currentScreen = LOGO;
	float game_end_time;
	float game_time = 15;
	int time_left;
	int hp = 3;

	Vector2 falling_items_pool[10];
	Rectangle falling_items_rect_pool[10];
	float falling_items_times_pool[10];


	int fipRectLength = sizeof(falling_items_rect_pool)/sizeof(falling_items_rect_pool[0]);
	int fipTimesLength = sizeof(falling_items_times_pool)/sizeof(falling_items_times_pool[0]);


	// set init values
	player_pos = (Vector2){640, 320};
	player_rect = (Rectangle) {player_pos.x, player_pos.y, player_width, player_wh};
	int i = 0;

	for(i=0; i < fipRectLength; i++){
	    falling_items_rect_pool[i] = (Rectangle){0,-64, 64, 64};
	}
	float t = GetTime();
	for(i=0; i < fipTimesLength; i++){
	    falling_items_times_pool[i] = t + getRandomRangeFloat(0.1, 3.0);
	}

	// ---- RUN ----
	while(!exitWindow) {

		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) { currentScreen = EXIT; }

		switch (currentScreen) {
		case LOGO:

			if (GetTime() > startTime + 3) {
				currentScreen = TITLE;
			}
			break;
		case TITLE:

			// Press enter to change to GAMEPLAY screen
			if ( IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP) ) {
				game_end_time = GetTime() + game_time;
				PlayMusicStream(music);
				currentScreen = GAMEPLAY;
			}
			break;
		case GAMEPLAY:

			// play music
			UpdateMusicStream(music);

			// TODO: Update GAMEPLAY screen variables here!
			//countdown_time = GetTime() + game_time
			time_left = (int)(game_end_time - GetTime());
			//sprintf("%f, %f - %f\n", game_end_time, GetTime(), time_left)
			// Press enter to change to ENDING screen
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
				currentScreen = ENDING;
			}
			if (time_left <= 0) {
				currentScreen = WIN;
			}
			break;
		case WIN:
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
				currentScreen = ENDING;
			}
			break;
		case LOSE:
			UpdateMusicStream(game_over_music);
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
				currentScreen = ENDING;
			}
			break;
		case ENDING:

			// Press enter to return to TITLE screen
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
                // --- REINIT -------------------------------------------------------------------
				game_time = 15;
				hp = 3;
				player_pos = (Vector2){640, 320};
				player_rect = (Rectangle) {player_pos.x, player_pos.y, player_width, player_wh};
				int i = 0;

				for(i=0; i < fipRectLength; i++){
				    falling_items_rect_pool[i] = (Rectangle){0,-64, 64, 64};
				}
				float t = GetTime();
				for(i=0; i < fipTimesLength; i++){
				    falling_items_times_pool[i] = t + getRandomRangeFloat(0.1, 3.0);
				}
				currentScreen = TITLE;
			}
			break;
		case EXIT:
	    	if (IsKeyPressed(KEY_Y)) { exitWindow = true; }
            else if (IsKeyPressed(KEY_N)) { currentScreen = TITLE; }
			break;
		}

		// draw ------------------------------------------------------
		BeginDrawing();
		ClearBackground(WHITE);
		switch (currentScreen) {
		case LOGO:
			DrawText("LOGO PLACEHOLDER", 20, 20, 40, LIGHTGRAY);
			DrawText("Wait for 3 seconds...", 290, 220, 20, GRAY);
			DrawTexture(title_tex, SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5, WHITE);
			break;
		case TITLE:
			// TODO: Update TITLE screen variables here!
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
			DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
			DrawText(
				"PRESS ENTER or TAP to jump to GAMEPLAY SCREEN",
				120,
				220,
				20,
				DARKGREEN
			);
			break;
		case GAMEPLAY:
		    // TODO: Update GAMEPLAY screen variables here!
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
			if (IsKeyDown(KEY_LEFT)) {
				player_vel.x = -400;
				player_flip = true;
				player_moving = true;
			} else if (IsKeyDown(KEY_RIGHT)) {
				player_vel.x = 400;
				player_flip = false;
				player_moving = true;
			} else {
				player_vel.x = 0;
				player_moving = false;
			}
			Vector2 ppAddResult = Vector2Add(player_pos, Vector2Scale(player_vel, GetFrameTime()));
			player_pos.x = ppAddResult.x;
			player_pos.y = ppAddResult.y;

			player_vel.y += 2000 * GetFrameTime();
			if (player_grounded && IsKeyPressed(KEY_SPACE)) {
				player_vel.y = -600;
				player_grounded = false;

				PlaySound(jump_sound_ogg);
			}
			// keep on the ground
			if (player_pos.y > (float)GetScreenHeight() - player_wh) {
				player_pos.y = (float)GetScreenHeight() - player_wh;
				player_grounded = true;
			}
			// keep in bounds
			if (player_pos.x <= 0) {
				player_pos.x = 0;
			}
			if (player_pos.x >= (float)GetScreenWidth() - player_wh) {
				player_pos.x = (float)GetScreenWidth() - player_wh;
			}


			player_run_frame_timer += GetFrameTime();
			if (player_run_frame_timer > player_run_frame_length) {
				player_run_current_frame += 1;
				player_run_frame_timer = 0;

				if (player_run_current_frame == player_run_num_frames) {
					player_run_current_frame = 0;
				}
			}


			Rectangle draw_player_source = (Rectangle) {
				.x      = player_moving ? (float)player_run_current_frame * player_run_width / (float)player_run_num_frames : 0,
				.y      = 0,
				.width  = player_width,
				.height = player_run_height,
			};
			Rectangle draw_player_dest = (Rectangle) {
				player_pos.x,
				player_pos.y,
				player_width,
				player_run_height,
			};

			if (player_flip) {
				draw_player_source.width = -draw_player_source.width;
			}

			player_rect.x = player_pos.x;
			player_rect.y = player_pos.y;

			// update items
			int min = 32;

			for( int i = 0; i < fipTimesLength; i++) {
			    //printf("now: %f t: %f ||| item: %d, x: %f y: %f \n", GetTime(), falling_items_times_pool[i], i, falling_items_rect_pool[i].x, falling_items_rect_pool[i].y);
				if(GetTime() > falling_items_times_pool[i]){
					falling_items_rect_pool[i].y += 1000 * GetFrameTime();
					if (falling_items_rect_pool[i].y >= (float)(SCREEN_HEIGHT + 64)) {
						falling_items_rect_pool[i].y = -64;
						falling_items_rect_pool[i].x = getRandomRangeFloat(min, SCREEN_WIDTH - (min * 2));
						falling_items_rect_pool[i].width = getRandomRangeFloat(min, 64);
						falling_items_times_pool[i] = GetTime() + getRandomRangeFloat(1.0, 5.0);
					}

					//CHECK FOR COLLISION WITH PLAYER
					if (CheckCollisionRecs(player_rect, falling_items_rect_pool[i])) {

						// reset that item
						falling_items_rect_pool[i].y = -64;
						falling_items_rect_pool[i].x = getRandomRangeFloat(min, SCREEN_WIDTH - (min * 2));
						falling_items_rect_pool[i].width = getRandomRangeFloat(min, 64);
						falling_items_times_pool[i] = GetTime() + getRandomRangeFloat(1.0, 5.0);

						// HIT!
						PlaySound(hurt_sound_wav);
						hp -= 1;
						if(hp <= 0){
							// DED
							StopMusicStream(music);
							PlayMusicStream(game_over_music);
							currentScreen = LOSE;
						}
					}
				}
			}


			// draw items
			// blink_color = u8(100 + (math.sin(GetTime() * 100) * 100))
			// { blink_color, blink_color, blink_color, 255}
			float color_angle = (float)(120 + (sin(GetTime() * 10) * 120));
			for(int i=0; i < fipRectLength; i++){
			    printf("now: %f || item: %d, x: %f, y: %f, w: %f, h: %f, \n", GetTime(), i, falling_items_rect_pool[i].x, falling_items_rect_pool[i].y, falling_items_rect_pool[i].width, falling_items_rect_pool[i].height);
                DrawRectangleRec(falling_items_rect_pool[i], ColorFromHSV(color_angle, 1, 1));
			}


			//DrawRectangleV(player_pos, {64, 64}, GREEN)
			//DrawTextureV(player_run_texture, player_pos, WHITE)
			//DrawTextureEx(player_run_texture, player_pos, 0, 4, WHITE)
			//DrawTextureRec(player_run_texture, draw_player_source, player_pos, WHITE)
			DrawTexturePro(
				player_run_texture,
				draw_player_source,
				draw_player_dest,
				(Vector2){0.0,0.0},
				0.0,
				WHITE
			);

			char titl[24] = "Time Left: ";
			snprintf(titl + strlen(titl), sizeof(titl) - strlen(titl), "%d", time_left);
			DrawText(titl, (int)((float)SCREEN_WIDTH * 0.5), 10, 40, WHITE);
			DrawText(
				"Dodge the falling items! ",
				(int)((float)SCREEN_WIDTH * 0.5),
				48,
				24,
				WHITE
			);
			char hpt[12] = "HP: ";
			snprintf(hpt + strlen(hpt), sizeof(hpt) - strlen(hpt), "%d", hp);
			DrawText(hpt, 10, 10, 40, WHITE);

			break;
		case WIN:
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PURPLE);
			DrawText("YOU WIN!", 20, 20, 40, DARKPURPLE);
			DrawText(
				"PRESS ENTER",
				120,
				220,
				20,
				DARKPURPLE
			);
			break;

		case LOSE:
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RED);
			DrawText("YOU LOOSE", 20, 20, 40, MAROON);
			DrawText(
				"PRESS ENTER",
				120,
				220,
				20,
				MAROON
			);
			break;
		case ENDING:

			// TODO: Update ENDING screen variables here!
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
			DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
			DrawText(
				"PRESS ENTER or TAP to RETURN to TITLE SCREEN",
				120,
				220,
				20,
				DARKBLUE
			);
			break;
		case EXIT:

			DrawRectangle(0, (SCREEN_HEIGHT * 0.5) - 100, SCREEN_WIDTH, 200, BLACK);
            DrawText("Are you sure you want to exit program? [Y/N]", 40, (SCREEN_HEIGHT * 0.5) - 60, 30, WHITE);
			break;
		}


		EndDrawing();
	}

	// ---- SHUTDOWN ----
	//unload audio and close audio device access
	UnloadSound(jump_sound_ogg);
	UnloadSound(hurt_sound_wav);
	StopMusicStream(music);
	StopMusicStream(game_over_music);
	UnloadMusicStream(music);
	UnloadMusicStream(game_over_music);
	CloseAudioDevice();

	UnloadTexture(player_run_texture);
	UnloadTexture(title_tex);

	CloseWindow();

    return 0;
}
