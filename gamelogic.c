#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */

void gameloop(void);

int gameover = 0;
int snakeLength = 3; // Initial length of the snake
int growSnake = 0;
int score = 0;
int highScore = 0;

// Define a struct for the positions on the game board
typedef struct
{
    int x;
    int y;
} Position;

// Define the snake array and fruit position
#define MAX_SNAKE_LENGTH 1024 // Adjust based on your display/grid size
Position snake[MAX_SNAKE_LENGTH];
Position fruit;
Position enemy;

// Define an enum for the snake's direction
typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

Direction snakeDirection = RIGHT; // Initial direction

unsigned int generateSimpleRandom(unsigned int seed)
{
    static unsigned int randSeed = 0; // Static variable to hold the current seed/state

    if (seed != 0)
    {
        randSeed = seed; // Use the provided seed to initialize
    }

    // Check to ensure randSeed is not zero after initialization; if zero, set to a default value
    if (randSeed == 0)
    {
        randSeed = 123456789; // Default initialization if seed is 0 and no seed has been set before
    }

    // Linear Congruential Generator (LCG) formula
    randSeed = randSeed * 1103515245 + 12345;
    return (randSeed / 65536) % 32768;
}

void spawnFood(void)
{
    int foodPlaced = 0;

    while (!foodPlaced)
    {
        fruit.x = generateSimpleRandom(0) % 127; // Assuming rand() generates a random number and 128 is your grid width
        fruit.y = generateSimpleRandom(0) % 31;  // Assuming 32 is your grid height

        int i, isClear = 1;
        for (i = 0; i < snakeLength; i++)
        {
            if (snake[i].x == fruit.x && snake[i].y == fruit.y)
            {
                isClear = 0;
                break;
            }
        }
        if (isClear)
            foodPlaced = 1;
    }
}

void spawnEnemy(void) //Spawn an enemy as an plus sign (plus sign is made in func file)
{
    int enemyPlaced = 0;

    while (!enemyPlaced)
    {
        enemy.x = (generateSimpleRandom(0)) % 127; // Assuming rand() generates a random number and 128 is your grid width
        enemy.y = (generateSimpleRandom(0)) % 31;  // Assuming 32 is your grid height

        int i, isClear = 1;
        for (i = 0; i < snakeLength; i++)
        {
            if (snake[i].x == enemy.x && snake[i].y == enemy.y)
            {
                isClear = 0;
                break;
            }
        }
        if (isClear)
            enemyPlaced = 1;
    }
}

void gameinit(void)
{
    // Declare and initialize the initial position for the snake's head
    int initialX = 60; // Example initial x-coordinate for the snake's head
    int initialY = 25; // Example initial y-coordinate for the snake's head
    snakeDirection = RIGHT;

    snakeLength = 3; // Set the initial length of the snake to 3 segments
    growSnake = 0;   // Ensure the snake starts without needing to grow
    score = 0;
    generateSimpleRandom(TMR2);
    spawnFood(); // Place the first piece of food

    // Place the fruit in a random position, ensuring it's not on the snake
    // You'll need to implement or use an existing random function
    int i;
    for (i = 0; i < snakeLength; i++)
    {
        // Set the initial positions of the snake, making it 3 segments long
        // If the snake moves to the right, each segment should be to the left of the previous one
        snake[i].x = initialX - (i * 2);
        snake[i].y = initialY;
    }

    gameover = 0; // Ensure the gameover flag is reset
}

void handleInput(void)
{
    int buttonStatus = getbtns();

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
    // Temporary storage for the previous position
    int prevX = snake[0].x;
    int prevY = snake[0].y;
    int tempX, tempY;

    // Update the head position based on the current direction
    switch (snakeDirection)
    {
    case RIGHT:
        snake[0].x += 2; // Adjust movement speed as necessary
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
        growSnake = 0; // Reset the flag
        score++;

        if (score > highScore)
        {
            highScore = score;
        }
    }
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

    // Check for food consumption with both the snake head and fruit being 2x2 segments
    // Check if the snake head drives into the food
    //(snake[0].x == fruit.x || snake[0].x == fruit.x + 1) &&
    //  (snake[0].y == fruit.y || snake[0].y == fruit.y + 1)
    if ((snake[0].x < fruit.x + 2 && snake[0].x + 2 > fruit.x) &&
        (snake[0].y < fruit.y + 2 && snake[0].y + 2 > fruit.y))
    {
        growSnake = 1; // Indicate that the snake should grow
        spawnFood();   // Spawn new food at a different location
    }
}

void collisionWall()
{
    // Adjust these values based on your display's dimensions
    int MAX_X = 128; // Example maximum x-coordinate
    int MAX_Y = 32;  // Example maximum y-coordinate

    // Check if any part of the snake's head 2x2 segment is out of bounds
    if (snake[0].x < 0 || snake[0].x + 1 >= MAX_X || // +1 to check the right side of the 2x2 segment
        snake[0].y < 0 || snake[0].y + 1 >= MAX_Y)   // +1 to check the bottom side of the 2x2 segment
    {
        gameover = 1;
    }
}

void collisionSelf()
{
    int i;
    for (i = 1; i < snakeLength; i++)
    {
        // Check if any part of the snake's head 2x2 segment collides with any part of another segment's 2x2 area
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

void displaySnake(void)
{
    int i;
    for (i = 0; i < snakeLength; i++)
    {
        displaySnakeSegment(snake[i].x, snake[i].y);
    }
    displaySnakeSegment(fruit.x, fruit.y); // Display the food
    displayEnemy(enemy.x, enemy.y); //Display the enemy
}

void displayGameOverScreen() // Hampus
{
    // Clear the display
    clear_display();
    char *scoreStr;
    scoreStr = itoaconv(score);
    char *highScoreStr;
    highScoreStr = itoaconv(highScore);

    // Display "Game over" message
    // Adjust coordinates and font size as needed for your display
    display_string(0, "Score: ");
    display_string(1, scoreStr);
    display_string(2, "Highscore: ");
    display_string(3, highScoreStr);
    // display_string(2,"to play again :)");
    // display_string("Highscore" + snakeLength)
    display_update();
    // Wait for a button press to start a new game
    while (1)
    {
        // Poll the buttons or use interrupts to detect button presses
        int buttonStatus = getbtns();

        // Check if any button is pressed
        if (buttonStatus & 0x1)
        {
            // Start a new game
            gameloop(); // Assuming gameloop() restarts the game
            break;      // Exit the loop after starting a new game
        }
    }
}

void gameloop(void)
{
    gameinit();

    while (!gameover)
    {

        // Handle immediate input here
        // You might use polling or interrupts to check for user input
        // and update the snakeDirection accordingly

        handleInput(); // This is a placeholder. Implement this function based on your input method.

        // Use the timer for screen updates
        if (IFS(0) & 0x100)
        {
            clear_display(); // Clear the display for the next drawing cycle
            updatePosition();
            collisionWall();
            collisionSelf();
            displaySnake();      // Draw the snake at its current position
            updateGameDisplay(); // Optionally, update other parts of the display if needed

            if (gameover)
            {
                displayGameOverScreen();
            }

            IFSCLR(0) = 0x100; // Reset the timer flag
        }
    }
}

void gameloophard(void)
{
    gameinit();
    spawnEnemy();

    while (!gameover)
    {

        // Handle immediate input here
        // You might use polling or interrupts to check for user input
        // and update the snakeDirection accordingly

        handleInput(); // This is a placeholder. Implement this function based on your input method.

        // Use the timer for screen updates
        if (IFS(0) & 0x100)
        {
            clear_display(); // Clear the display for the next drawing cycle
            updatePosition();
            collisionWall();
            collisionSelf();
            displaySnake();      // Draw the snake at its current position
            updateGameDisplay(); // Optionally, update other parts of the display if needed

            if (gameover)
            {
                displayGameOverScreen();
            }

            IFSCLR(0) = 0x100; // Reset the timer flag
        }
    }
}
