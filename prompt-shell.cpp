#include <algorithm>

#include <ctime>
#include <cstdlib>
#include <conio.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>  
#include <Windows.h>  

/* --------------------
PREPROCESSOR DIRECTIVES
----------------------- */
#ifdef _WIN32
#define OS_NAME "Windows"
#elif defined(__APPLE__)
#define OS_NAME "Mac OS"
#elif defined(__linux__)
#define OS_NAME "Linux"
#else
#define OS_NAME "Unknown"
#endif

/* -------------
GLOBAL VARIABLES 
---------------- */
std::string APPLICATION_VERSION = "[Version 0.1]";
std::string APPLICATION_DATE_VERSION = "2023.04";

std::string USER_INPUT = "";
std::string USER_LOGGED_DIRECTORY = "";
std::string USER_GUEST_DIRECTORY = "";

std::string OS = OS_NAME;

bool STATUS_EXIT_ON = false;
bool TEXT_DELAY_ON = true;


/* ---------------
USER-DEFINED TYPES 
------------------ */
enum TT_Input
{
	BACKSPACE = 8,
	RETURN = 32
};

/* --------------------------
THREAD FUNCTIONS DECLARATIONS 
----------------------------- */
void promptShellIntroduction();
void promptShellLoginSignIn();

/* ------------------
FUNCTION DECLARATIONS 
--------------------- */
void printStatic(std::string text, int newline);
void printTypewriter(std::string text, int newline, int low_delay = 10, int high_delay = 20);


int main()
{
    while (!STATUS_EXIT_ON)
    {
        promptShellIntroduction();
        promptShellLoginSignIn();
    }
    
}

/* -------------------------
THREAD FUNCTIONS DEFINITIONS 
---------------------------- */
void promptShellIntroduction()
{
    if (OS == "Windows")
        system("cls");
    else
        system("clear");

    printTypewriter("PromptShell - Terminal Emulator " + APPLICATION_VERSION, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2);
}

void promptShellLoginSignIn()
{
    USER_INPUT = "";
    std::string input;

    do
    {
        // CL-interface for the default user..
        printTypewriter(USER_GUEST_DIRECTORY + "> ", 0);

        // Getting the user's input..
        std::getline(std::cin, input);

        // If the user's input is empty or whitespace, continue..
        if (!(input.find_first_not_of(' ') != std::string::npos))
			continue;
        
        USER_INPUT = input;

        // Transforming the user's input to lowercase for easier case matching..
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "help")
        {
            printTypewriter("delay           Allow the user to turn the text delay ", 0, 5, 15); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("login           Allow the user to login onto their account.", 1, 5, 15);
            printTypewriter("signup          Allow the user to create their account.", 2, 5, 15);
            

        }
        else if (input == "delay") 
		{
			TEXT_DELAY_ON = !TEXT_DELAY_ON;
			printTypewriter("Text delay has been turned ", 0); 
			printTypewriter((TEXT_DELAY_ON) ? "on." : "off.", 2);
        }
        else if (input == "exit" || input == "quit") 
        {
            STATUS_EXIT_ON = true;
            return;
        }
        else if (input == "cls" || input == "clear")
        {

            if (OS == "Windows")
                system("cls");
            else
                system("clear");
        }
        else 
        {
            printTypewriter("'" + USER_INPUT + "'" + " is not a recognized command.", 2);
        }

    } while (true);
}

/* -----------------
FUNCTION DEFINITIONS  
-------------------- */
void printStatic(std::string text, int newline)
{
    std::cout << text;

    switch (newline)
    {
    case 1:
        std::cout << std::endl;
        break;
    case 2:
        std::cout << std::endl << std::endl;
        break;
    case 3:
        std::cout << std::endl << std::endl << std::endl;
        break;
    }
}
void printTypewriter(std::string text, int newline, int low_delay, int high_delay)
{
    srand(time(NULL));
    
    int rand_sleep(low_delay + (rand() % high_delay));

    if (TEXT_DELAY_ON == false)
    {
        printStatic(text, newline);
        return; 
    }

    for (std::string::iterator letter = text.begin(); letter != text.end(); letter++)
	{
		std::cout << *letter;
		Sleep(rand_sleep);
	}

    switch (newline)
    {
    case 1:
        std::cout << std::endl;
        break;
    case 2:
        std::cout << std::endl << std::endl;
        break;
    case 3:
        std::cout << std::endl << std::endl << std::endl;
        break;
    }
}