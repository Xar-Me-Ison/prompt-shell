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
std::string APPLICATION_VERSION = "[Version 0.2]";
std::string APPLICATION_DATE_VERSION = "2023.04";

std::string USER_INPUT = "";
std::string USER_LOGGED_DIRECTORY = "";
std::string USER_GUEST_DIRECTORY = "";

std::string OS = OS_NAME;

std::string LINE_SEPARATOR = 
"---------------------------------------------------------------------------------";

bool STATUS_EXIT_ON = false;
bool TEXT_DELAY_ON = true;
bool ACCOUNT_CREATED_SUCCESSFULLY = false;

/* ------------
INLINE FUNCTION
--------------- */
inline void systemClear()
{
    if (OS == "Windows")
        system("cls");
    else
        system("clear");
}

/* ---------------
USER-DEFINED TYPES 
------------------ */
enum TT_Input
{
	BACKSPACE = 8,
	RETURN = 32
};

/* ---------------
CLASS DECLARATIONS 
------------------ */
class promptShellUser
{
    /* Class methods */
public:
    promptShellUser(); // Default constructor, to sign up..
    promptShellUser(int); // Overloaded constructor, to login..
    ~promptShellUser();
private:
    void createUsername();
    void createPassword();

    std::string getPassword();
	/* Class variables */
public:
private:
    std::string username, password, email;
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

/* --------------
CLASS DEFINITIONS 
----------------- */
promptShellUser::promptShellUser()
{
    std::string input;

    createUsername();
    createPassword();

    if (ACCOUNT_CREATED_SUCCESSFULLY)
    {
        // create user data
    }

}
promptShellUser::promptShellUser(int code) { }
promptShellUser::~promptShellUser() 
{ 
    ACCOUNT_CREATED_SUCCESSFULLY = false;
}

void promptShellUser::createUsername()
{
    std::string input;

    printTypewriter("\nUsername: ", 0); getline(std::cin, input);

    while (input.size() > 60 || input.size() == 0)
    {
        // printTypewriter("Username is taken, please try again.", 2);
        printTypewriter("Username: ", 0); getline(std::cin, input);
    }

    username = input;
}

void promptShellUser::createPassword() 
{ 
    std::string input1, input2;
    bool password_match;
    int password_counter = 0;

    do
    {
        if (password_counter != 0)
        {
            printTypewriter("Password: ", 0);
        }

        printTypewriter("Password: ", 0); input1 = getPassword();
        while (input1.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Password: ", 0); input1 = getPassword();
        }

        printTypewriter("Retype password: ", 0); input2 = getPassword();

        while (input2.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Password: ", 0); input2 = getPassword();
        }

        if (input1 != input2)
        {
            printTypewriter("\nERROR: Password do not match.", 1);

            if (password_counter == 2)
            {
                ACCOUNT_CREATED_SUCCESSFULLY = false;
                break;
            }
            password_match = false;
        }
        else
        {
            ACCOUNT_CREATED_SUCCESSFULLY = true;
            password_match = true;
            password = input1;
        }

        password_counter++;
    } while (!password_match);
    
}

std::string promptShellUser::getPassword()
{
	std::string password;	
	char input_ch;

	while ((input_ch = getch()) != '\r')
	{
		if (input_ch == '\b' && password.length() > 0)
		{
			password.pop_back();
			std::cout << "\b \b";
		}
		else if (input_ch != '\b')
		{
			password.push_back(input_ch);
			std::cout << '*';
		}
	}
	
	std::cout << std::endl;
	return password;
}
/* -------------------------
THREAD FUNCTIONS DEFINITIONS 
---------------------------- */
void promptShellIntroduction()
{
    systemClear();
    printTypewriter(LINE_SEPARATOR, 1, 1, 1);
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION, 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);
}

void promptShellLoginSignIn()
{
    USER_INPUT = "";
    std::string input;

    promptShellUser *user_ptr;
    user_ptr = nullptr;


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
            // printTypewriter("Guest user's commands. For full functionality please login, or sign up.", 1, 5, 15);
            printTypewriter("\nDELAY           Allow the user to turn the text delay ", 0, 5, 15); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("LOGIN           Allow the user to login onto their account.", 1, 5, 15);
            printTypewriter("SIGNUP          Allow the user to create their account.", 1, 5, 15);
            printTypewriter("VERSION         Allow the user to see the current running version.", 2, 5, 15);
            

        }
        else if (input == "delay") 
		{
			TEXT_DELAY_ON = !TEXT_DELAY_ON;
			printTypewriter("Text delay has been turned ", 0); 
			printTypewriter((TEXT_DELAY_ON) ? "on." : "off.", 2);
        }
        else if (input == "log" || input == "login")
        {
            user_ptr = new promptShellUser(69);

            if (user_ptr == nullptr)
            {
                printTypewriter("\nERROR: Unable to login at the moment.", 1);
                continue;
            }

            delete user_ptr; user_ptr = nullptr;
        }
        else if (input == "sign" || input == "signup")
        {
            if (user_ptr == nullptr)
            {
                //printTypewriter("SUCCESS", 1);
                user_ptr = new promptShellUser;
            }
            else
            {
                printTypewriter("ERROR: Unable to create an account at this time.", 1);
                continue;
            }
            
        }
        else if (input == "cls" || input == "clear")
        {
            systemClear();
        }
        else if (input == "version" || input == "ver")
        {
            if (OS == "Windows")
                system("cls");
            else
                system("clear");
        }
        else if (input == "exit" || input == "quit") 
        {
            STATUS_EXIT_ON = true;
            return;
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