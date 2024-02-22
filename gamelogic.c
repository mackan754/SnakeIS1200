#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */

int gameover = 0;
int snakeLength = 3; // Initial length of the snake

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

// Define an enum for the snake's direction
typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

Direction snakeDirection = RIGHT; // Initial direction

void gameinit(void)
{
    // Declare and initialize the initial position for the snake's head
    int initialX = 60; // Example initial x-coordinate for the snake's head
    int initialY = 25; // Example initial y-coordinate for the snake's head

    snakeLength = 25; // Set the initial length of the snake to 3 segments

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

    int i;
    // Move the rest of the snake
    for (i = 1; i < snakeLength; i++)
    {
        // Store current segment's position
        tempX = snake[i].x;
        tempY = snake[i].y;

        // Shift current segment to previous segment's old position
        snake[i].x = prevX;
        snake[i].y = prevY;

        // Update previous position to current segment's old position
        prevX = tempX;
        prevY = tempY;
    }

    // Optionally clear the tail segment from the display
    // clearSnakeSegment(prevX, prevY); // Uncomment if you have implemented this function
}

void displaySnake(void)
{
    int i;
    for (i = 0; i < snakeLength; i++)
    {
        displaySnakeSegment(snake[i].x, snake[i].y);
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
            displaySnake();      // Draw the snake at its current position
            updateGameDisplay(); // Optionally, update other parts of the display if needed
            IFSCLR(0) = 0x100;   // Reset the timer flag
        }
    }
}
