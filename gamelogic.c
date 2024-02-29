/* gamelogic.c
   This file written 2024 by Marcus Jansson and Hampus Berglund

   For copyright and licensing, see file COPYING */

/*
29/2-2024
Koden är skriven av Marcus Jansson och Hampus Berglund.
Den skrevs tillsammans sittandes vid samma dator.
*/

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */

#define MAX_SNAKE_LENGTH 1024 // Anger det maximala längden som snaken kan ha.
#define MAX_ENEMIES 3 // Anger det maximala antalet fiender.

extern int gamemode; // Variabel som anger vilket spelläge vi är i.
void gameloop(void); // Definierar gameloop.

int gameover = 0;    // Flagga som indikerar om spelet är över.
int snakeLength = 3; // Initiala längden på snaken.
int growSnake = 0;   // Flagga som indikerar om snaken ska växa.
int score = 0;       // Score
int highScore = 0;   // Highscore

// Definierar ett struct för positionen på spelplanen.
typedef struct
{
    int x;
    int y;
} Position;

// Definierar array för snake och fiender, samt positionen för frukten.
Position enemies[MAX_ENEMIES]; 
Position snake[MAX_SNAKE_LENGTH];
Position food;

// Definierar enum för snakens riktning.
typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

Direction snakeDirection = RIGHT; // Riktning för snaken.

unsigned int generateSimpleRandom(unsigned int seed)
{
    static unsigned int randSeed = 0; // Variabel som innehåller våran seed.

    if (seed != 0)
    {
        randSeed = seed; // Använd den angivna seeden för att initialisera
    }

    // Kontrollera så att randSeed inte är noll efter initialisering; om noll, sätt till ett standardvärde
    if (randSeed == 0)
    {
        randSeed = 123456789;
    }

    // Linjär Kongruential Generator (LCG)-formel
    randSeed = randSeed * 1103515245 + 12345;
    return (randSeed / 65536) % 32768;
}

void spawnFood(void)
{
    int foodPlaced = 0; //Flagga om maten har placerats
    while (!foodPlaced)
    {
        food.x = generateSimpleRandom(0) % 127; // Justerat för 2x2 mat
        food.y = generateSimpleRandom(0) % 31;  // Justerat för 2x2 mat

        int i, isClear = 1;
        // Kontrollera kollisioner med snaken.
        for (i = 0; i < snakeLength; i++)
        {
            if (snake[i].x == food.x && snake[i].y == food.y)
            {
                isClear = 0;
                break;
            }
        }

        // Kontrollera kollisoner med enemies.
        for (i = 0; i < MAX_ENEMIES && isClear; i++)
        {
            // Kontrollera om någon del av maten (2x2) är inom enemies area (7x7).
            int fx;
            for (fx = food.x; fx <= food.x + 1 && isClear; fx++)
            {
                int fy;
                for (fy = food.y; fy <= food.y + 1 && isClear; fy++)
                {
                    // Check if within the plus shape of the enemy (3 units from center)
                    if (fx >= enemies[i].x - 3 && fx <= enemies[i].x + 3 &&
                        fy >= enemies[i].y - 3 && fy <= enemies[i].y + 3)
                    {
                        // Additional logic to specifically exclude positions that are not part of the plus shape
                        // assuming plus shape means there's a center line horizontally and vertically
                        if (fx == enemies[i].x || fy == enemies[i].y ||                                 // directly aligns with the plus shape
                            (fy >= enemies[i].y - 3 && fy <= enemies[i].y + 3 && fx == enemies[i].x) || // vertical part of plus
                            (fx >= enemies[i].x - 3 && fx <= enemies[i].x + 3 && fy == enemies[i].y))
                        { // horizontal part of plus
                            isClear = 0;
                        }
                    }
                }
            }
        }

        if (isClear)
        {
            foodPlaced = 1;
        }
    }
}

void spawnEnemy(void)
{
    int e;
    for (e = 0; e < MAX_ENEMIES; e++)
    {
        int enemyPlaced = 0;
        while (!enemyPlaced)
        {
            enemies[e].x = 3 + (generateSimpleRandom(0) % (124 - 3));
            enemies[e].y = 3 + (generateSimpleRandom(0) % (28 - 3));

            int i, isClear = 1;
            // Kontrollera kollision med snaken.
            for (i = 0; i < snakeLength; i++)
            {
                if (snake[i].x == enemies[e].x && snake[i].y == enemies[e].y)
                {
                    isClear = 0;
                    break;
                }
            }
            // Kontrollera kollision med maten.
            if (enemies[e].x == food.x && enemies[e].y == food.y)
            {
                isClear = 0;
            }
            // Kontrollera kollision med andra fiender.
            for (i = 0; i < e; i++)
            { // Kontrollera endast redan placerade fiender.
                // Kontrollera om "+" symbolerna överlappar.
                if ((enemies[i].x >= enemies[e].x - 6 && enemies[i].x <= enemies[e].x + 6) &&
                    (enemies[i].y >= enemies[e].y - 6 && enemies[i].y <= enemies[e].y + 6))
                {
                    isClear = 0;
                    break;
                }
            }
            if (isClear)
            {
                enemyPlaced = 1;
            }
        }
    }
}

void gameinit(void)
{
    // Ställ in startposition och riktning för snakens huvud.
    int initialX = 60; 
    int initialY = 25;
    snakeDirection = RIGHT;

    snakeLength = 3; // Ställer in snakens initiala längd.
    growSnake = 0;   // Säkerställer att snaken inte växer.
    score = 0; // Nollställer score.
    generateSimpleRandom(TMR2);
    spawnFood(); // Placera ut den första maten.

    // Skapar snaken.
    int i;
    for (i = 0; i < snakeLength; i++)
    {
        snake[i].x = initialX - (i * 2);
        snake[i].y = initialY;
    }

    gameover = 0; // Nollställer flaggan för gameover.
}

void handleInput(void)
{
    // Input hanteras och knapparna avgör vilken riktning snaken tar.
    int buttonStatus = getbtns();
    int sw = getsw();

    if (buttonStatus & 0x8 && snakeDirection != RIGHT)
        snakeDirection = LEFT;

    if (buttonStatus & 0x4 && snakeDirection != DOWN)
        snakeDirection = UP;

    if (buttonStatus & 0x2 && snakeDirection != UP)
        snakeDirection = DOWN;

    if (buttonStatus & 0x1 && snakeDirection != LEFT)
        snakeDirection = RIGHT;    
}

void updatePosition(void)
{
    // Temporär lagring av föregående position.
    int prevX = snake[0].x;
    int prevY = snake[0].y;
    int tempX, tempY;

    // Uppdatera huvudets position baserat på riktningen.
    switch (snakeDirection)
    {
    case RIGHT:
        snake[0].x += 2;
        break;
    case LEFT:
        snake[0].x -= 2;
        break;
    case UP:
        snake[0].y -= 2;
        break;
    case DOWN:
        snake[0].y += 2;
        break;
    }

    if (growSnake && snakeLength < MAX_SNAKE_LENGTH)
    {
        int i;
        for (i = 1; i < snakeLength; i++)
        {
            tempX = snake[i].x;
            tempY = snake[i].y;

            snake[i].x = prevX;
            snake[i].y = prevY;

            prevX = tempX;
            prevY = tempY;
        }

        snake[snakeLength].x = prevX;
        snake[snakeLength].y = prevY;
        snakeLength++;
        growSnake = 0; // Nollställer flagga för snakens tillväxt.
        score++;

        // Uppdaterar score
        if (score > highScore)
        {
            highScore = score;
        }
    }
    // Om snaken inte växer uppdatera bara positionerna.
    else
    {
        int i;
        for (i = 1; i < snakeLength; i++)
        {
            tempX = snake[i].x;
            tempY = snake[i].y;

            snake[i].x = prevX;
            snake[i].y = prevY;

            prevX = tempX;
            prevY = tempY;
        }
    }

    // Kontrollera om snaken kör in i maten med någon del av huvudet(2x2).
    if ((snake[0].x < food.x + 2 && snake[0].x + 2 > food.x) &&
        (snake[0].y < food.y + 2 && snake[0].y + 2 > food.y))
    {
        growSnake = 1; // Flagga att snaken ska växa.
        spawnFood();   // Placera maten på en ny position.
    }
}

void collisionWall(void)
{
    int MAX_X = 128; // Maximala x-kordinat.
    int MAX_Y = 32;  // Maximala y-kordinat.

    // Kontrollera om någon del av snakens huvud är utanför gränsen av skärmen.
    if (snake[0].x < 0 || snake[0].x + 1 >= MAX_X || 
        snake[0].y < 0 || snake[0].y + 1 >= MAX_Y) 
    {
        gameover = 1;
    }
}

void collisionSelf(void)
{
    int i;
    for (i = 1; i < snakeLength; i++)
    {
        // Kontrollera om någon del av sankens huvud kolliderar med något annat segment.
        if ((snake[0].x == snake[i].x || snake[0].x + 1 == snake[i].x) &&
                (snake[0].y == snake[i].y || snake[0].y + 1 == snake[i].y) ||
            (snake[0].x == snake[i].x + 1 && snake[0].y == snake[i].y) ||
            (snake[0].x == snake[i].x && snake[0].y == snake[i].y + 1))
        {
            gameover = 1;
            break;
        }
    }
}

void collisionWithEnemies(void)
{
    int i;
    for (i = 0; i < MAX_ENEMIES; i++)
    {
        // Mittpunkten av fienden.
        int ex = enemies[i].x;
        int ey = enemies[i].y;

        // Beräkna gränserna för "+" symbolen.
        int leftX = ex - 3;
        int rightX = ex + 3;
        int topY = ey - 3;
        int bottomY = ey + 3;

        // Kontrollera om någon del av snakens huvud korsar någon del av fiendens "+" symbol.
        if (((snake[0].x <= rightX && snake[0].x + 1 >= leftX) && (snake[0].y == ey)) ||
            ((snake[0].y <= bottomY && snake[0].y + 1 >= topY) && (snake[0].x == ex || snake[0].x + 1 == ex)))
        {
            gameover = 1;
            break;
        }
    }
}

void displaySnake(void)
{
    // Visa varje segment av snaken på skärmen.
    int i;
    for (i = 0; i < snakeLength; i++)
    {
        displaySnakeSegment(snake[i].x, snake[i].y);
    }
    displaySnakeSegment(food.x, food.y); // Visar maten på skärmen.

    if (gamemode == 1)
    {
        int i;
        for (i = 0; i < MAX_ENEMIES; i++)
        {
            displayEnemy(enemies[i].x, enemies[i].y); // Visar alla fiender på skärmen.
        }
    }
}

void displayGameOverScreen(void) 
{
    // Rensar skärmen och visar score.
    clearDisplay();
    char *scoreStr;
    scoreStr = itoaconv(score);
    char *highScoreStr;
    highScoreStr = itoaconv(highScore);

    display_string(0, "Score: ");
    display_string(1, scoreStr);
    display_string(2, "Highscore: ");
    display_string(3, highScoreStr);

    display_update();
    quicksleep(2000000);

    // Väntar på knapptryck för att starta nytt spel.
    while (1)
    {
        int buttonStatus = getbtns();

        // Kontrollera om högerknappen är nedtryckt.
        if (buttonStatus & 0x1)
        {
            gameloop(); // Kör gameloop() och startar ett nytt spel.
            break;     
        }
    }
}

// Loop som driver hela spelet och anropas från menu.c
void gameloop(void)
{
    gameinit(); // Förbered spelet
    if (gamemode == 1) // Om gamemode är hard, skapa fiender
    {
        spawnEnemy();
    }

    while (!gameover) // Så länge spelet inte är över.
    {
        handleInput(); // Användarens input kontrolleras konstant.

        // Timern används för att uppdatera skärmen och spellogiken.
        if (IFS(0) & 0x100)
        {
            clearDisplay(); // Rensa skärmen i förberedelse för nästa uppdatering.
            updatePosition(); // Uppdatera snakens position.
            collisionWall(); // Kontrollera om snaken kolliderar med en vägg.
            collisionSelf(); // Kontrollera om snaken kolliderar med sig själv.
            if (gamemode == 1)
            {
                collisionWithEnemies(); // Om gamemode är hard kontrollera kollisioner med fiender.
            }

            displaySnake();      // Rita snaken vid sin nuvarande position.
            updateGameDisplay(); // Uppdatera skärmen så den syns på skärmen.

            if (gameover)
            {
                displayGameOverScreen(); // Spelet är slut.
            }

            IFSCLR(0) = 0x100; // Nollställ timer flagga.
        }
    }
}
