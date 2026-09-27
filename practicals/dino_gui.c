#define _WIN32_WINNT 0x0600
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <windowsx.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 450
#define GROUND_Y 340

#define MAX_PARTICLES 60
#define MAX_CLOUDS 6
#define MAX_OBSTACLES 4

typedef enum
{
  STATE_PLAYING,
  STATE_PAUSED,
  STATE_GAMEOVER
} GameState;

// Dust Particle
typedef struct
{
  float x, y;
  float vx, vy;
  float alpha;
  float size;
  bool active;
} Particle;

// Cloud
typedef struct
{
  float x, y;
  float speed;
  float width;
} Cloud;

// Obstacle
typedef struct
{
  float x, y;
  float width, height;
  int type; // 0 = Small Cactus, 1 = Large Cactus, 2 = Pterodactyl
  bool active;
} Obstacle;

// Global Game Variables
GameState g_state = STATE_PLAYING;
int g_score = 0;
int g_highScore = 0;
float g_distance = 0;
float g_gameSpeed = 7.0f;

// Dino Physics State
float g_dinoY = GROUND_Y - 50;
float g_dinoVY = 0.0f;
bool g_isGrounded = true;
bool g_isDucking = false;
int g_animFrame = 0;

// Entities
Obstacle g_obstacles[MAX_OBSTACLES];
Cloud g_clouds[MAX_CLOUDS];
Particle g_particles[MAX_PARTICLES];
float g_groundOffset = 0.0f;

void SpawnDust(float x, float y, int count)
{
  for (int k = 0; k < count; k++)
  {
    for (int p = 0; p < MAX_PARTICLES; p++)
    {
      if (!g_particles[p].active)
      {
        g_particles[p].active = true;
        g_particles[p].x = x + (rand() % 12 - 6);
        g_particles[p].y = y + (rand() % 6 - 3);
        g_particles[p].vx = -((rand() % 25) / 10.0f + 1.0f);
        g_particles[p].vy = -((rand() % 15) / 10.0f + 0.3f);
        g_particles[p].size = (float)((rand() % 5) + 3);
        g_particles[p].alpha = 1.0f;
        break;
      }
    }
  }
}

void InitGame()
{
  g_state = STATE_PLAYING;
  g_score = 0;
  g_distance = 0;
  g_gameSpeed = 7.0f;
  g_dinoY = GROUND_Y - 50;
  g_dinoVY = 0.0f;
  g_isGrounded = true;
  g_isDucking = false;

  for (int i = 0; i < MAX_OBSTACLES; i++)
    g_obstacles[i].active = false;
  for (int p = 0; p < MAX_PARTICLES; p++)
    g_particles[p].active = false;

  // Initialize Clouds
  for (int i = 0; i < MAX_CLOUDS; i++)
  {
    g_clouds[i].x = (float)(rand() % WINDOW_WIDTH);
    g_clouds[i].y = (float)(40 + rand() % 120);
    g_clouds[i].speed = 0.8f + (rand() % 10) / 10.0f;
    g_clouds[i].width = (float)(50 + rand() % 40);
  }
}

void UpdatePhysics()
{
  if (g_state != STATE_PLAYING)
    return;

  g_animFrame++;
  g_distance += g_gameSpeed * 0.1f;
  g_score = (int)g_distance;
  if (g_score > g_highScore)
    g_highScore = g_score;

  g_gameSpeed += 0.0008f; // Gradually increase speed
  g_groundOffset += g_gameSpeed;
  if (g_groundOffset >= 40.0f)
    g_groundOffset -= 40.0f;

  // Dino Jump / Gravity
  g_dinoVY += 0.95f; // Gravity
  g_dinoY += g_dinoVY;

  if (g_dinoY >= GROUND_Y - 50)
  {
    if (!g_isGrounded)
    {
      // Just landed! Spawn landing dust
      SpawnDust(90, GROUND_Y - 5, 8);
    }
    g_dinoY = GROUND_Y - 50;
    g_dinoVY = 0.0f;
    g_isGrounded = true;
  }

  // Spawn running dust
  if (g_isGrounded && g_animFrame % 4 == 0)
  {
    SpawnDust(80, GROUND_Y - 8, 2);
  }

  // Move & Update Dust Particles
  for (int p = 0; p < MAX_PARTICLES; p++)
  {
    if (g_particles[p].active)
    {
      g_particles[p].x += g_particles[p].vx;
      g_particles[p].y += g_particles[p].vy;
      g_particles[p].alpha -= 0.04f;
      if (g_particles[p].alpha <= 0.0f)
      {
        g_particles[p].active = false;
      }
    }
  }

  // Move Clouds
  for (int i = 0; i < MAX_CLOUDS; i++)
  {
    g_clouds[i].x -= g_clouds[i].speed;
    if (g_clouds[i].x < -100)
    {
      g_clouds[i].x = WINDOW_WIDTH + (rand() % 100);
      g_clouds[i].y = (float)(40 + rand() % 120);
    }
  }

  // Obstacle Spawning
  bool canSpawn = true;
  for (int j = 0; j < MAX_OBSTACLES; j++)
  {
    if (g_obstacles[j].active && g_obstacles[j].x > (WINDOW_WIDTH - 320))
    {
      canSpawn = false;
      break;
    }
  }

  if (canSpawn && (rand() % 35 == 0))
  {
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
      if (!g_obstacles[i].active)
      {
        g_obstacles[i].active = true;
        g_obstacles[i].x = (float)WINDOW_WIDTH;
        g_obstacles[i].type = rand() % 3; // 0=Small, 1=Large, 2=Pterodactyl
        if (g_obstacles[i].type == 0)
        {
          g_obstacles[i].width = 30;
          g_obstacles[i].height = 45;
          g_obstacles[i].y = GROUND_Y - 45;
        }
        else if (g_obstacles[i].type == 1)
        {
          g_obstacles[i].width = 48;
          g_obstacles[i].height = 65;
          g_obstacles[i].y = GROUND_Y - 65;
        }
        else
        {
          g_obstacles[i].width = 48;
          g_obstacles[i].height = 35;
          g_obstacles[i].y =
              GROUND_Y - 95 - (rand() % 40); // Varying bird height
        }
        break;
      }
    }
  }

  // Move Obstacles & Collision Check
  float dinoX = 80;
  float dinoW = g_isDucking ? 60 : 40;
  float dinoH = g_isDucking ? 28 : 45;
  float curDinoY = g_dinoY + (g_isDucking ? 22 : 5);

  for (int i = 0; i < MAX_OBSTACLES; i++)
  {
    if (g_obstacles[i].active)
    {
      g_obstacles[i].x -= g_gameSpeed;
      if (g_obstacles[i].x < -80)
      {
        g_obstacles[i].active = false;
      }

      // AABB Collision Detection with forgiving margin
      float ox = g_obstacles[i].x + 4;
      float oy = g_obstacles[i].y + 4;
      float ow = g_obstacles[i].width - 8;
      float oh = g_obstacles[i].height - 8;

      if (dinoX < ox + ow && dinoX + dinoW > ox && curDinoY < oy + oh &&
          curDinoY + dinoH > oy)
      {
        g_state = STATE_GAMEOVER;
        SpawnDust(dinoX + 20, curDinoY + 20, 25);
      }
    }
  }
}

// Drawing Helper Functions
void DrawDinoSprite(HDC hdc, float x, float y, bool isDucking, int animFrame,
                    bool isGrounded)
{
  HBRUSH bodyBrush = CreateSolidBrush(RGB(55, 65, 81)); // Modern Slate Gray
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bodyBrush);
  HPEN nullPen = (HPEN)GetStockObject(NULL_PEN);
  HPEN oldPen = (HPEN)SelectObject(hdc, nullPen);

  if (isDucking && isGrounded)
  {
    // Ducking Dino
    RoundRect(hdc, (int)x, (int)y + 20, (int)x + 58, (int)y + 45, 12, 12);
    RoundRect(hdc, (int)x + 42, (int)y + 15, (int)x + 68, (int)y + 34, 8, 8);

    // Eye
    HBRUSH eyeBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, eyeBrush);
    Ellipse(hdc, (int)x + 58, (int)y + 18, (int)x + 64, (int)y + 24);
    DeleteObject(eyeBrush);

    // Running Legs
    SelectObject(hdc, bodyBrush);
    if ((animFrame / 3) % 2 == 0)
    {
      Rectangle(hdc, (int)x + 14, (int)y + 42, (int)x + 22, (int)y + 50);
      Rectangle(hdc, (int)x + 38, (int)y + 42, (int)x + 46, (int)y + 47);
    }
    else
    {
      Rectangle(hdc, (int)x + 14, (int)y + 42, (int)x + 22, (int)y + 47);
      Rectangle(hdc, (int)x + 38, (int)y + 42, (int)x + 46, (int)y + 50);
    }
  }
  else
  {
    // Standing / Jumping Dino
    // Body
    RoundRect(hdc, (int)x + 10, (int)y + 14, (int)x + 42, (int)y + 44, 14, 14);
    // Head
    RoundRect(hdc, (int)x + 24, (int)y, (int)x + 50, (int)y + 24, 10, 10);
    // Snout
    Rectangle(hdc, (int)x + 36, (int)y, (int)x + 54, (int)y + 15);

    // Eye
    HBRUSH eyeBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, eyeBrush);
    Ellipse(hdc, (int)x + 42, (int)y + 4, (int)x + 48, (int)y + 10);
    DeleteObject(eyeBrush);

    SelectObject(hdc, bodyBrush);
    // Arms
    Rectangle(hdc, (int)x + 40, (int)y + 22, (int)x + 48, (int)y + 27);

    // Tail
    POINT tailPts[3] = {{(int)x + 12, (int)y + 22},
                        {(int)x - 10, (int)y + 16},
                        {(int)x + 12, (int)y + 35}};
    Polygon(hdc, tailPts, 3);

    // Legs
    if (!isGrounded)
    {
      // Legs in air
      Rectangle(hdc, (int)x + 14, (int)y + 42, (int)x + 23, (int)y + 49);
      Rectangle(hdc, (int)x + 30, (int)y + 42, (int)x + 39, (int)y + 49);
    }
    else
    {
      // Running leg animation
      if ((animFrame / 3) % 2 == 0)
      {
        Rectangle(hdc, (int)x + 14, (int)y + 42, (int)x + 23, (int)y + 52);
        Rectangle(hdc, (int)x + 30, (int)y + 42, (int)x + 39, (int)y + 46);
      }
      else
      {
        Rectangle(hdc, (int)x + 14, (int)y + 42, (int)x + 23, (int)y + 46);
        Rectangle(hdc, (int)x + 30, (int)y + 42, (int)x + 39, (int)y + 52);
      }
    }
  }

  SelectObject(hdc, oldBrush);
  SelectObject(hdc, oldPen);
  DeleteObject(bodyBrush);
}

void DrawCactusSprite(HDC hdc, float x, float y, float w, float h, int type)
{
  HBRUSH cactusBrush = CreateSolidBrush(RGB(34, 139, 34)); // Emerald Green
  HBRUSH darkBrush = CreateSolidBrush(RGB(16, 95, 30));
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, cactusBrush);
  HPEN nullPen = (HPEN)GetStockObject(NULL_PEN);
  HPEN oldPen = (HPEN)SelectObject(hdc, nullPen);

  int ix = (int)x;
  int iy = (int)y;
  int iw = (int)w;
  int ih = (int)h;

  // Main Stem
  RoundRect(hdc, ix + iw / 3, iy, ix + (2 * iw) / 3, iy + ih, 8, 8);

  // Left Arm
  RoundRect(hdc, ix, iy + ih / 3, ix + iw / 3 + 2, iy + (int)(ih / 1.8f), 6, 6);
  RoundRect(hdc, ix, iy + ih / 5, ix + iw / 4 + 2, iy + (int)(ih / 1.8f), 6, 6);

  // Right Arm
  RoundRect(hdc, ix + (2 * iw) / 3 - 2, iy + ih / 2, ix + iw,
            iy + (int)(ih / 1.5f), 6, 6);
  RoundRect(hdc, ix + iw - iw / 4, iy + ih / 3, ix + iw, iy + (int)(ih / 1.5f),
            6, 6);

  // Shadow detailing
  SelectObject(hdc, darkBrush);
  Rectangle(hdc, ix + iw / 3 + 2, iy + 4, ix + iw / 3 + 5, iy + ih - 4);

  SelectObject(hdc, oldBrush);
  SelectObject(hdc, oldPen);
  DeleteObject(cactusBrush);
  DeleteObject(darkBrush);
}

void DrawPterodactylSprite(HDC hdc, float x, float y, int animFrame)
{
  HBRUSH birdBrush = CreateSolidBrush(RGB(180, 83, 9)); // Crimson Brown
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, birdBrush);
  HPEN nullPen = (HPEN)GetStockObject(NULL_PEN);
  HPEN oldPen = (HPEN)SelectObject(hdc, nullPen);

  int ix = (int)x;
  int iy = (int)y;

  // Body
  RoundRect(hdc, ix + 10, iy + 10, ix + 36, iy + 24, 8, 8);
  // Beak
  POINT headPts[3] = {
      {ix + 36, iy + 10}, {ix + 52, iy + 16}, {ix + 36, iy + 22}};
  Polygon(hdc, headPts, 3);

  // Wing flapping
  if ((animFrame / 5) % 2 == 0)
  {
    // Wings Up
    POINT wing[3] = {
        {ix + 15, iy + 12}, {ix + 25, iy - 16}, {ix + 32, iy + 12}};
    Polygon(hdc, wing, 3);
  }
  else
  {
    // Wings Down
    POINT wing[3] = {
        {ix + 15, iy + 18}, {ix + 25, iy + 38}, {ix + 32, iy + 18}};
    Polygon(hdc, wing, 3);
  }

  SelectObject(hdc, oldBrush);
  SelectObject(hdc, oldPen);
  DeleteObject(birdBrush);
}

void RenderWindow(HDC hdcBuffer)
{
  // 1. Sky Gradient Background (Transitions from Day Blue to Dusk/Night
  // depending on score)
  RECT rcSky = {0, 0, WINDOW_WIDTH, GROUND_Y};
  HBRUSH skyBrush =
      CreateSolidBrush(RGB(235, 243, 250)); // Bright crisp day sky
  FillRect(hdcBuffer, &rcSky, skyBrush);
  DeleteObject(skyBrush);

  // 2. Draw Sun
  HBRUSH sunBrush = CreateSolidBrush(RGB(253, 224, 71)); // Warm sunny yellow
  HPEN nullPen = (HPEN)GetStockObject(NULL_PEN);
  HPEN oldPen = (HPEN)SelectObject(hdcBuffer, nullPen);
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdcBuffer, sunBrush);
  Ellipse(hdcBuffer, WINDOW_WIDTH - 120, 40, WINDOW_WIDTH - 65, 95);
  DeleteObject(sunBrush);

  // 3. Draw Fluffy Clouds
  HBRUSH cloudBrush = CreateSolidBrush(RGB(255, 255, 255));
  SelectObject(hdcBuffer, cloudBrush);
  for (int i = 0; i < MAX_CLOUDS; i++)
  {
    int cx = (int)g_clouds[i].x;
    int cy = (int)g_clouds[i].y;
    int cw = (int)g_clouds[i].width;
    RoundRect(hdcBuffer, cx, cy, cx + cw, cy + 24, 12, 12);
    RoundRect(hdcBuffer, cx + 10, cy - 10, cx + cw - 10, cy + 14, 14, 14);
  }
  DeleteObject(cloudBrush);

  // 4. Ground Surface (Desert Sand Color with Scrolling Gravel Texture)
  RECT rcGround = {0, GROUND_Y, WINDOW_WIDTH, WINDOW_HEIGHT};
  HBRUSH groundBrush = CreateSolidBrush(RGB(217, 119, 6)); // Rich Warm Earth
  FillRect(hdcBuffer, &rcGround, groundBrush);
  DeleteObject(groundBrush);

  // Ground Line
  HPEN linePen = CreatePen(PS_SOLID, 3, RGB(180, 83, 9));
  SelectObject(hdcBuffer, linePen);
  MoveToEx(hdcBuffer, 0, GROUND_Y, NULL);
  LineTo(hdcBuffer, WINDOW_WIDTH, GROUND_Y);
  DeleteObject(linePen);

  // Scrolling Gravel / Pebbles Texture
  HPEN pebblePen = CreatePen(PS_SOLID, 2, RGB(251, 191, 36));
  SelectObject(hdcBuffer, pebblePen);
  for (int x = -40 + (int)(-g_groundOffset); x < WINDOW_WIDTH + 40; x += 35)
  {
    MoveToEx(hdcBuffer, x, GROUND_Y + 12, NULL);
    LineTo(hdcBuffer, x + 10, GROUND_Y + 12);

    MoveToEx(hdcBuffer, x + 18, GROUND_Y + 28, NULL);
    LineTo(hdcBuffer, x + 24, GROUND_Y + 28);
  }
  DeleteObject(pebblePen);

  // 5. Dust Particles
  for (int p = 0; p < MAX_PARTICLES; p++)
  {
    if (g_particles[p].active)
    {
      HBRUSH pBrush = CreateSolidBrush(RGB(180, 150, 100));
      SelectObject(hdcBuffer, pBrush);
      int px = (int)g_particles[p].x;
      int py = (int)g_particles[p].y;
      int ps = (int)g_particles[p].size;
      Ellipse(hdcBuffer, px, py, px + ps, py + ps);
      DeleteObject(pBrush);
    }
  }

  // 6. Draw Obstacles
  for (int i = 0; i < MAX_OBSTACLES; i++)
  {
    if (g_obstacles[i].active)
    {
      if (g_obstacles[i].type == 2)
      {
        DrawPterodactylSprite(hdcBuffer, g_obstacles[i].x, g_obstacles[i].y,
                              g_animFrame);
      }
      else
      {
        DrawCactusSprite(hdcBuffer, g_obstacles[i].x, g_obstacles[i].y,
                         g_obstacles[i].width, g_obstacles[i].height,
                         g_obstacles[i].type);
      }
    }
  }

  // 7. Draw Dino
  DrawDinoSprite(hdcBuffer, 80, g_dinoY, g_isDucking, g_animFrame,
                 g_isGrounded);

  // 8. Draw HUD (Score Counter)
  SetBkMode(hdcBuffer, TRANSPARENT);
  HFONT hFontScore =
      CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_SWISS, "Segoe UI");
  SelectObject(hdcBuffer, hFontScore);
  SetTextColor(hdcBuffer, RGB(55, 65, 81));

  char scoreStr[60];
  sprintf(scoreStr, "HI  %05d    %05d", g_highScore, g_score);
  TextOutA(hdcBuffer, WINDOW_WIDTH - 220, 25, scoreStr, (int)strlen(scoreStr));
  DeleteObject(hFontScore);

  // 9. Overlay for PAUSED or GAMEOVER state
  if (g_state == STATE_PAUSED)
  {
    HFONT hFontMsg =
        CreateFontA(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    SelectObject(hdcBuffer, hFontMsg);
    SetTextColor(hdcBuffer, RGB(31, 41, 55));
    TextOutA(hdcBuffer, WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2 - 40,
             "PAUSED", 6);
    DeleteObject(hFontMsg);
  }
  else if (g_state == STATE_GAMEOVER)
  {
    // Game Over Dialog Box
    RECT rcModal = {WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 80,
                    WINDOW_WIDTH / 2 + 180, WINDOW_HEIGHT / 2 + 60};
    HBRUSH modalBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdcBuffer, &rcModal, modalBrush);
    DeleteObject(modalBrush);

    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(229, 231, 235));
    SelectObject(hdcBuffer, borderPen);
    Rectangle(hdcBuffer, rcModal.left, rcModal.top, rcModal.right,
              rcModal.bottom);
    DeleteObject(borderPen);

    HFONT hFontGO =
        CreateFontA(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    SelectObject(hdcBuffer, hFontGO);
    SetTextColor(hdcBuffer, RGB(220, 38, 38));
    TextOutA(hdcBuffer, WINDOW_WIDTH / 2 - 95, WINDOW_HEIGHT / 2 - 65,
             "GAME OVER", 9);
    DeleteObject(hFontGO);

    HFONT hFontSub =
        CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    SelectObject(hdcBuffer, hFontSub);
    SetTextColor(hdcBuffer, RGB(75, 85, 99));
    TextOutA(hdcBuffer, WINDOW_WIDTH / 2 - 145, WINDOW_HEIGHT / 2 - 15,
             "Press SPACE or R to Restart", 27);
    TextOutA(hdcBuffer, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 15,
             "Press ESC to Exit", 17);
    DeleteObject(hFontSub);
  }

  SelectObject(hdcBuffer, oldBrush);
  SelectObject(hdcBuffer, oldPen);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CREATE:
    SetTimer(hwnd, 1, 16, NULL); // ~60 FPS Timer
    InitGame();
    break;

  case WM_TIMER:
    UpdatePhysics();
    InvalidateRect(hwnd, NULL, FALSE);
    break;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE)
    {
      PostQuitMessage(0);
    }
    else if (wParam == 'P' || wParam == 'p')
    {
      if (g_state != STATE_GAMEOVER)
      {
        g_state = (g_state == STATE_PLAYING) ? STATE_PAUSED : STATE_PLAYING;
      }
    }
    else if (wParam == VK_SPACE || wParam == VK_UP || wParam == 'R' ||
             wParam == 'r')
    {
      if (g_state == STATE_GAMEOVER)
      {
        InitGame();
      }
      else if (g_state == STATE_PLAYING && g_isGrounded)
      {
        g_dinoVY = -14.5f; // Responsive 60 FPS jump force
        g_isGrounded = false;
        SpawnDust(80, GROUND_Y - 5, 6);
      }
    }
    else if (wParam == VK_DOWN)
    {
      if (g_state == STATE_PLAYING && g_isGrounded)
      {
        g_isDucking = true;
      }
    }
    break;

  case WM_KEYUP:
    if (wParam == VK_DOWN)
    {
      g_isDucking = false;
    }
    else if ((wParam == VK_SPACE || wParam == VK_UP) && g_dinoVY < -4.0f)
    {
      // Short hop variable jump control
      g_dinoVY += 4.0f;
    }
    break;

  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Double Buffering for 100% flicker-free rendering
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer =
        CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

    RenderWindow(hdcBuffer);

    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcBuffer, hbmOld);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcBuffer);

    EndPaint(hwnd, &ps);
    break;
  }

  case WM_DESTROY:
    KillTimer(hwnd, 1);
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
  srand((unsigned int)time(NULL));

  WNDCLASSEXA wc = {0};
  wc.cbSize = sizeof(WNDCLASSEXA);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = "DinoHDGameClass";

  if (!RegisterClassExA(&wc))
  {
    MessageBoxA(NULL, "Window Registration Failed!", "Error",
                MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  // Adjust window size for client area
  RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
                   FALSE);

  HWND hwnd = CreateWindowExA(
      0, "DinoHDGameClass", "Chrome Dino Runner HD - Native Edition",
      (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
      NULL, NULL, hInstance, NULL);

  if (!hwnd)
  {
    MessageBoxA(NULL, "Window Creation Failed!", "Error",
                MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}
