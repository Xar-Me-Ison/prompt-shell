#include <algorithm>

#include <direct.h>

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
std::string APPLICATION_VERSION = "[Version 0.3]";
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
    promptShellUser(); // Default constructor, to sign up ..
    promptShellUser(int); // Overloaded constructor, to login ..
    ~promptShellUser();
private:
    // Invoked by the default constuctor (signing up) .. 
    void createUsername();
    void createPassword();
    void createUserData();

    std::string getPassword();

    // Invoked by the overloaded constructor (logging in) ..
    bool validateUser();
    void loggedIn();

	/* Class variables */
public:
private:
    std::string username, password, email;

    bool LOGGED_IN = false;
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
        createUserData();
    }
    else 
    {
        validateUser();
    }

}
promptShellUser::promptShellUser(int code) 
{
    std::string input;

    if (validateUser())
    {
        printTypewriter("\nSUCCESS: You are now logged in as \033[1m" + username + "\033[0m.", 1, 30, 40);

        LOGGED_IN = true;
        loggedIn();
    }
    else
    {
        printTypewriter("\nERROR: Your credentials could not be verified.", 1, 30, 40);

        promptShellIntroduction();
        return;
    }
}
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
            printTypewriter("\nERROR: Password do not match.", 2);

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

void promptShellUser::createUserData()
{
    const char* path_userdata = ".userdata"; 
    const char* path_database = ".database";

    _mkdir(path_userdata);
    _mkdir(path_database);

    if (SetFileAttributes(path_userdata, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
    if (SetFileAttributes(path_database, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
 
    const char* fn_userdatabase = ".database/.usernames.txt";
    std::ofstream outputfile(fn_userdatabase, std::ios::app);

    // In the case of a file error ..
    if (!outputfile) { std::cout << "Error creating file" << std::endl; }
    outputfile << username << std::endl;
    outputfile.close();

    std::string fl_userdata = ".userdata/" + username + ".txt";
    const char* fn_userdata = fl_userdata.c_str();
    std::ofstream outfile(fn_userdata);

    // In the case of a file error ..
    if (!outfile) { std::cout << "Error creating file" << std::endl; }

    outfile << username << std::endl;
    outfile << password << std::endl;
    // outfile << email << std::endl;
    outfile.close();
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

/* Invoked by the overloaded constructor */
bool promptShellUser::validateUser()
{
    std::ifstream username_infile(".database/.usernames.txt");
    std::fstream user_filename;
    std::string usrname, passwd;
    std::string usr_line, pwd_line;
    std::string data_line;
    bool validated = false;
    int line_num = 1;

    // In the case of a file error ..
    if (!username_infile) { return 0; }

    printTypewriter("Username: ", 0); getline(std::cin, usrname);

    while (std::getline(username_infile, usr_line))
    {
        if (usr_line.find(usrname) != std::string::npos || true)
        {
            passwd = getPassword();

            user_filename.open(".userdata/" + usrname + ".txt");

            // In the case of a file error ..
            if (!user_filename) { return 0; }

            while (std::getline(user_filename, pwd_line) && passwd.size() >= 1)
            {
                if (line_num == 2 && pwd_line.compare(passwd) == 0)
                {
                    validated = true;
                    user_filename.close();
                    break;
                }
                line_num++;
            }

            break;
        }

        if (validated)
        {
            user_filename.open(".userdata/" + usrname + ".txt");
            line_num = 1;

            while (std::getline(user_filename, data_line))
            {
                switch (line_num)
                {
                    case 1:
                        username = data_line;
                        break;
                    case 2:
                        password = data_line;
                        break;
                    // case 3: 
                    //     email = data_line;
                    //     break;
                }
                line_num++;
            }
        }
    }

    username_infile.close();

    return validated;
}

void promptShellUser::loggedIn()
{
systemClear();
    printTypewriter(LINE_SEPARATOR, 1, 1, 1);
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION + "  \033[1m<" + username + ">\033[0m" , 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);

    USER_INPUT = "";
	std::string input;

	do 
	{
		// CL-interface for the logged in user..
		printTypewriter(USER_GUEST_DIRECTORY + "> ", 0); 

		// Getting the logged in user's input..
		getline(std::cin, input);

		// If the user's input is empty or whitespace, continue..
		if (!(input.find_first_not_of(' ') != std::string::npos))
			continue;

		USER_INPUT = input;

		// Transforming the user's input to lowercase for easier case matching..
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		if (input == "help")
		{
			printTypewriter("\nDELAY           Allow the user to turn the text delay ", 0, 5, 15); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 5, 15);
            printTypewriter("CLEAR           Allow the user to clear up the terminnal.", 1, 5, 15);
			printTypewriter("LOGOUT          Allows the logged in user to logout of their account.", 1, 5, 15);
			printTypewriter("EXIT            Allows the logged in user to exit from the application.", 2, 5, 15);
            printTypewriter("VERSION         Allows the logged in user to see the current running version.", 1, 5, 15);
			
		}
		else if (input == "delay") 
		{
			TEXT_DELAY_ON = !TEXT_DELAY_ON;
			printTypewriter("Text delay has been turned ", 0); 
			printTypewriter((TEXT_DELAY_ON) ? "on." : "off.", 2);
		}
		else if (input == "logout" || input == "signout")	
		{
			printTypewriter("\nSUCCESS: You have been logged out as \033[1m" + username + "\033[0m.", 1, 30, 40);
            promptShellIntroduction();
			return;
		}
		else if (input == "version" || input == "ver")
		{
            systemClear();
			printTypewriter(LINE_SEPARATOR, 0, 1, 1);
			printTypewriter("\n" + APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.  \033[1m<" + username + ">\033[0m", 1);
			printTypewriter("Copyright (C) GNB Corporation. All rights reserved.", 2);
		}
		else if (input == "cls" || input == "clear" || input == "clean")
		{
			system("cls");
		}
		else if (input == "exit" || input == "quit")
		{
			return;
		}
		else
		{

			printTypewriter("'" + USER_INPUT + "'" + " is not a recognized command.", 2); 
		}
	} while (LOGGED_IN);

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
            printTypewriter("CLEAR           Allow the user to clear up the terminnal.", 1, 5, 15);
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
            systemClear();
            printTypewriter(LINE_SEPARATOR, 0, 1, 1);
            printTypewriter("\n" + APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.", 1);
            printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
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