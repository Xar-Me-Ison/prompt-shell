#include <algorithm>
#include <dirent.h>
#include <ctime>
#include <cstdlib>

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>

#include <regex>
#include <vector>
#include <unistd.h>  
#include <sys/stat.h>
/* --------------------
PREPROCESSOR DIRECTIVES
----------------------- */
#ifdef _WIN32
#define OS_NAME "Windows"
#include <windows.h>
#include <conio.h>
#include <direct.h>
#define mkdir _mkdir
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
std::string OS = OS_NAME;
std::string APPLICATION_VERSION = "[Version 2.0]";
std::string APPLICATION_DATE_VERSION = "2023.04";

std::string USER_INPUT = "";
std::string USER_LOGGED_DIRECTORY = "";
std::string USER_GUEST_DIRECTORY = "";

bool PS_MERGE_ON = false;
bool STATUS_EXIT_ON = false;
bool USER_LOGGED_IN = false;
bool TEXT_DELAY_ON = false;
bool LINE_SEPERATOR_ON = false;
bool DIRECTORY_SHOW_ON = false;
bool ACCOUNT_CREATED_SUCCESSFULLY = false;

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

    std::string getterUsername();
    
    // Invoked by the user itself when changing username or password ..
    void changeUsername();
    void changePassword();
private:
    // Invoked by the default constuctor (signing up) .. 
    void createUsername();
    void createPassword();
    void createUserData();
    bool isUsernameAvailable(std::string);
    
    std::string getPasswordFromUser();
    
    // Invoked by the overloaded constructor (logging in) ..
    bool validateUser();
    
    // Invoked by the 'changeUsername()' and 'changePassword()' ..
    void changeUserData(std::string, std::string);
    bool verifyUser();
    
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
void promptShellIntroduction(std::string);

/* ------------------
FUNCTION DECLARATIONS 
--------------------- */
void printStatic(std::string text, int newline);
void printTypewriter(std::string text, int newline, int low_delay = 10, int high_delay = 20);
void cdCommand(const std::vector<std::string>& arguments);
void mvCommand(const std::vector<std::string>& arguments);
void cpCommand(const std::vector<std::string>& arguments);
void lsCommand(const std::vector<std::string>& arguments);
void pwdCommand(const std::vector<std::string>& arguments);
void rmCommand(const std::vector<std::string>& file_names);
void rmdirCommand(const std::vector<std::string>& dir_names);
void mkdirCommand(const std::vector<std::string>& dir_names);
void touchCommand(const std::vector<std::string>& file_names);
void echoCommand(const std::vector<std::string>& texts);
void catCommand(const std::vector<std::string>& file_names);
void gitCommand(const std::vector<std::string>& arguments);
void wingetCommand(const std::vector<std::string>& arguments);
void psmergeCommand(const std::vector<std::string>& arguments);
void updateDirectory();

/* -------------------------
INLINE FUNCTION DECLARATIONS
---------------------------- */
inline void systemClear();
inline void lineSeparator();
inline void helpCommand(bool flag);
inline void delayCommand();
inline void lineCommand();
inline void dirCommand();
inline void psmergCommand();
inline void versionCommand();
inline void versionCommand(std::string);

int main()
{
    if (OS != "Windows")
    {
        printTypewriter("\033[1mPromptShell is not yet supported in " + OS + ".\033[0m", 1, 30, 40);
        exit(EXIT_SUCCESS);
    }

    promptShellIntroduction();
    promptShellLoginSignIn();
}

/* --------------
CLASS DEFINITIONS 
----------------- */
promptShellUser::promptShellUser()
{
    createUsername();
    createPassword();

    if (ACCOUNT_CREATED_SUCCESSFULLY)
    {
        createUserData();
        printTypewriter("\033[1m<> Your account has been created successfully, you can now login.\033[0m", 2, 20, 30);
    }
}
promptShellUser::promptShellUser(int code) 
{
    std::cout << std::endl;

    if (validateUser())
    {
        printTypewriter("\n<> You are now logged in as \033[1m" + username + "\033[0m.", 1, 30, 40);

        LOGGED_IN = true;
        USER_LOGGED_IN = true;
        promptShellIntroduction(username);
        return;
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
    USER_LOGGED_IN = false;
    LOGGED_IN = false;
}

/* Public methods */
std::string promptShellUser::getterUsername()
{
    return username;
}
void promptShellUser::changeUsername()
{
    std::string input;

    // You will call the 'verifyUser()' function ..
    printTypewriter("\033[1m<> First you have to verify yourself.\033[0m", 2); 
    if (!verifyUser())
    {
        printTypewriter("\033[1mERROR: Verification failed, please try again later.\033[0m", 2);
        return;
    }

    printTypewriter("\nNew Username: ", 0); getline(std::cin, input);

    while ((input.size() > 60 || input.size() == 0 || !isUsernameAvailable(input)) && input != username)
    {
        if (input == username)
        {
            printTypewriter("Username can't be the same as before, please try again.", 2);
            break;
        }
        printTypewriter("Username is taken, please try again.", 2);
        printTypewriter("New Username: ", 0); getline(std::cin, input);
    }

    changeUserData(input, password);
    printTypewriter("\033[1mUsername was changed sucessfully to '" + input + "'.\033[0m", 2, 30, 40);
}
void promptShellUser::changePassword()
{
    std::string input1, input2;
    bool password_match;
    int password_counter = 0;

    // You will call the overloaded 'validateUser()' function ..
    printTypewriter("\033[1m<> First you have to verify yourself.\033[0m", 2); 
    if (!verifyUser())
    {
        printTypewriter("\033[1mERROR: Verification failed, please try again later.\033[0m", 2);
        return;
    }

    do
    {

        printTypewriter("\nNew Password: ", 0); input1 = getPasswordFromUser();
        while (input1.size() <= 1 && input1 != password)
        {
            if (input1 == password) 
            {
                printTypewriter("Password can't be the same as before, please try again.", 2);
                continue;
            }
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("New Password: ", 0); input1 = getPasswordFromUser();
        }

        printTypewriter("Retype password: ", 0); input2 = getPasswordFromUser();

        while (input2.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Retype Password: ", 0); input2 = getPasswordFromUser();
        }

        if (input1 != input2)
        {
            if (password_counter >= 2)
            {
                printTypewriter("\033[1mERROR: Too many attempts have been made, try again later.\033[0m", 2);
                ACCOUNT_CREATED_SUCCESSFULLY = false;
                break;
            }
            printTypewriter("\033[1mERROR: Password do not match.\033[0m", 2);
            password_match = false;
        }
        else
        {
            ACCOUNT_CREATED_SUCCESSFULLY = true; password_match = true;
            changeUserData(username, input1);

            printTypewriter("\033[1mPassword was changed sucessfully.", 2, 30, 40);
        }

        password_counter++;
    } while (!password_match);
    
}

void promptShellUser::createUsername()
{
    std::string input;

    printTypewriter("\nUsername: ", 0); getline(std::cin, input);

    while (input.size() > 60 || input.size() == 0 || !isUsernameAvailable(input))
    {
        printTypewriter("Username is taken, please try again.", 2);
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

        printTypewriter("Password: ", 0); input1 = getPasswordFromUser();
        while (input1.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Password: ", 0); input1 = getPasswordFromUser();
        }

        printTypewriter("Retype password: ", 0); input2 = getPasswordFromUser();

        while (input2.size() <= 1)
        {
            printTypewriter("Weak password, try again.", 2);
            printTypewriter("Retype Password: ", 0); input2 = getPasswordFromUser();
        }

        if (input1 != input2)
        {
            if (password_counter >= 2)
            {
                printTypewriter("\033[1mERROR: Too many attempts have been made, try again.\033[0m", 2);
                ACCOUNT_CREATED_SUCCESSFULLY = false;
                break;
            }
            printTypewriter("\033[1mERROR: Password do not match.\033[0m", 2);
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

    mkdir(path_userdata);
    mkdir(path_database);

    if (SetFileAttributes(path_userdata, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
    if (SetFileAttributes(path_database, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
    
    const char* fn_userdatabase = ".database/.usernames.txt";
    std::ofstream outputfile(fn_userdatabase, std::ios::app);

    // In the case of a file error ..
    if (!outputfile) { std::cout << "Error creating database file" << std::endl; }
    outputfile << username << std::endl;
    outputfile.close();

    std::string fl_userdata = ".userdata/" + username + ".txt";
    const char* fn_userdata = fl_userdata.c_str();
    std::ofstream outfile(fn_userdata);

    // In the case of a file error ..
    if (!outfile) { std::cout << "Error creating user file" << std::endl; }

    outfile << username << std::endl;
    outfile << password << std::endl;
    // outfile << email << std::endl;
    outfile.close();
}
bool promptShellUser::isUsernameAvailable(std::string usrname)
{
   	std::ifstream infile(".database/.usernames.txt");
    std::string search_string = usrname;
	std::string line;

    if (!infile.is_open()) { printTypewriter("ERROR: Failed to open database file", 2); return false;} 

    std::regex regex("\\b" + search_string + "\\b");

	while (std::getline(infile, line))
	{
		// If username is not available return false ..
		if (std::regex_search(line, regex))
		{
            infile.close();
			return false;
		}
	}

    infile.close();
	return true;
}

std::string promptShellUser::getPasswordFromUser()
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
	int line_number = 1;
	
	
	// In case of a file error ..
	if (!username_infile) { return 0; }

	printTypewriter("Username: ", 0); getline(std::cin, usrname);
	
	while(std::getline(username_infile, usr_line))
	{
		if (usr_line.find(usrname) != std::string::npos || true)
		{
			printTypewriter("Password: ", 0); passwd = getPasswordFromUser();
			// Here you will look for the corresponding user file and validate the password
			
			// Look in the corresponding userdata file ..
			user_filename.open(".userdata/" + usrname + ".txt");

			// In case of a file error ..
			if (!user_filename) { return 0; }

			while (std::getline(user_filename, pwd_line) && passwd.size() >= 1)
			{
				if (line_number == 2 && pwd_line.compare(passwd) == 0)
				{
					validated = true;
					user_filename.close();
					break;
				}	
				line_number++;
			}

			break;
		}
	}

	if (validated)
	{
		// Assign usernames and so on by reading from the file
		user_filename.open(".userdata/" + usrname + ".txt");
		line_number = 1;

		while (std::getline(user_filename, data_line))
		{
			switch (line_number)
			{
				case 1:
					username = data_line;
					break;
				case 2:
					password = data_line;
					break;
			}
			line_number++;
		}
	}

	username_infile.close();
	

	return validated; 
}

/* Invoked by the user when changing username or password */
void promptShellUser::changeUserData(std::string usrname, std::string passwd)
{
    const char* path_userdata = ".userdata";
    const char* path_database = ".database";

    mkdir(path_userdata);
    mkdir(path_database);

    if (SetFileAttributes(path_userdata, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }
    if (SetFileAttributes(path_database, FILE_ATTRIBUTE_HIDDEN) == 0) { std::cout << "Failed to hide directory" << std::endl; }


    std::ifstream infile(".database/.usernames.txt");
    // std::ofstream outfile(".database/temp.txt");
    
    std::string targetString;
    std::string replacementString;

    // if (usrname != username)
    // {
    //     targetString = username;
    //     replacementString = usrname;
    // }
    // else
    // {
    //     targetString = password;
    //     replacementString = passwd;
    // }

    std::string line;
    std::regex regex("\\b" + username + "\\b");
    while (std::getline(infile, line))
    {
        if (std::regex_search(line, regex))  
            std::regex_replace(line, regex, usrname); 
        // else if (std::regex_search(line, username))
        //     continue;
    }

    infile.close();
    // outfile.close();

    std::remove(".database/.usernames.txt");
    std::rename(".database/temp.txt", ".database/.usernames.txt");
    
    // std::rename((".database/" + username + ".txt").c_str(), (".database/" + usrname + ".txt").c_str());
    
    std::remove((".userdata/" + username + ".txt").c_str());
    username = usrname;
    password = passwd;
    createUserData();
}
bool promptShellUser::verifyUser()
{
    std::fstream users_file((".userdata/" + username + ".txt").c_str());
    std::string usrname, passwd;
    std::string line;
    int line_number = 0;
    bool verified = false;

    printTypewriter("Username: ", 0); getline(std::cin, usrname);
    while (usrname != username)
    {
        if (line_number > 2)
        {
            return verified;
        }
        printTypewriter("\033[1mERROR: Credentials mismatch, please try again.\033[0m", 2);
        printTypewriter("Username: ", 0); getline(std::cin, usrname);
        line_number++;
    }
    line_number = 0;
    printTypewriter("Password: ", 0); passwd = getPasswordFromUser();

    while(std::getline(users_file, line))
    {
        if (line_number == 0 && line == usrname)
        {
            line_number++;
        }
        else if (line_number == 1 && line == passwd)
        {
            verified = true;
        }
    }

    return verified;
}


/* -------------------------
THREAD FUNCTIONS DEFINITIONS 
---------------------------- */
void promptShellIntroduction()
{
    systemClear();
    lineSeparator();
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION, 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);
}
void promptShellLoginSignIn()
{
    // Required for the command parser ..
    std::string input;
    std::string command;
    std::istringstream iss;
    std::vector<std::string> tokens;

    promptShellUser *user_ptr;
    user_ptr = nullptr;

    do
    {
        if (DIRECTORY_SHOW_ON)
        {
            // To update the 'USER_LOGGED_DIRECTORY' beforehand ..
            updateDirectory(); 

            // CL-interface for the logged in user..
            printTypewriter(USER_LOGGED_DIRECTORY + "> ", 0); 
        }
        else
        {
            printTypewriter("> ", 0); 
        }

        // Getting the user's input..
        std::getline(std::cin, input);

        iss.clear(); // Clear the state of istringstream ..
        iss.str(input); // Set the string to parse in istringstream ..

        tokens.clear(); // Clear the vector of tokens ..

        // Split input into tokens 
        tokens = {std::istream_iterator<std::string>{iss},
              std::istream_iterator<std::string>{}};

        // If the user's input is empty or whitespace, continue..
        if (tokens.empty())
            continue;

        command = tokens[0];
    
        if (command == "help")
        {
            if (USER_LOGGED_IN)
            {
                helpCommand(true);
            }
            else
            {
                helpCommand(false);
            }
        }
        else if (command == "ps")
        {
            if (USER_LOGGED_IN)
            {
                promptShellIntroduction(user_ptr->getterUsername());
            }
            else
            {
                promptShellIntroduction();
            }
        }
        else if (command == "delay") 
		{
			delayCommand();
        }
        else if (command == "line")
        {
            lineCommand();
        }
        else if (command == "dr")
        {
            dirCommand();
        }
        else if (command == "cls" || command == "clear" || command == "clean")
		{
			systemClear();
		}
        else if (command == "cd")
        {
            cdCommand(tokens);
        }
        else if (command == "mv")
        {
            mvCommand(tokens);
        }
        else if (command == "cp")
        {
            cpCommand(tokens);
        }
        else if (command == "ls")
        {
            lsCommand(tokens);
        }
        else if (command == "pwd")
        {
            pwdCommand(tokens);
        }
        else if (command == "rm")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "rm" itself ..
            rmCommand(tokens);
        }
        else if (command == "rmdir")
        {
            tokens.erase(tokens.begin());   //  Remove the first element which is the command "rmdir" itself ..
            rmdirCommand(tokens);
        }
        else if (command == "mkdir")
        {
            tokens.erase(tokens.begin());   //  Remove the first element which is the command "mkdir" itself ..
            mkdirCommand(tokens);
        }
        else if (command == "touch")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "touch" itself ..
            touchCommand(tokens);
        }
        else if (command == "echo")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "echo" itself ..
            echoCommand(tokens);
        }
        else if (command == "cat")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "cat" itself ..
            catCommand(tokens);
        }
        else if (command == "git")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "git" itself ..
            gitCommand(tokens);
        }
        else if (command == "winget" && OS == "Windows")
        {
            tokens.erase(tokens.begin());   // Remove the first element which is the command "winget" itself ..
            wingetCommand(tokens);
        }
        else if (command == "logout" || command == "signout")
        {
            if (USER_LOGGED_IN && user_ptr != nullptr)
            {
                printTypewriter("\nYou have now been logged out as \033[1m" + user_ptr->getterUsername() + "\033[0m.", 1, 30, 40);
                promptShellIntroduction();

                delete user_ptr; user_ptr = nullptr;
            }
            else
            {
                printTypewriter("ERROR: To sign out, you must be first logged in.", 2, 30, 40);
            }            
        }
        else if (command == "chguser" || command == "changeuser" || command == "changeusername")
        {
            if (!USER_LOGGED_IN && user_ptr == nullptr)
            {
                printTypewriter("ERROR: To change username, you must first be logged in.", 2, 30, 40);
            }
            else
            {
                user_ptr->changeUsername();
            }
        }
        else if (command == "chgpass" || command == "changepass" || command == "changepassword")
        {
            if (!USER_LOGGED_IN && user_ptr == nullptr)
            {
                printTypewriter("ERROR: To change password, you must first be logged in.", 2, 30, 40);
            }
            else
            {
                user_ptr->changePassword();
            }
        }
        else if (command == "login" || command == "signin")
        {
            if (USER_LOGGED_IN && user_ptr != nullptr)
            {
                printTypewriter("ERROR: You are already logged in as \033[1m" + user_ptr->getterUsername() + "\033[0m.", 2, 30, 40);
            }
            else
            {
                user_ptr = new promptShellUser(69);

                if (user_ptr == nullptr) { printTypewriter("\033[1mERROR: Unable to login at the moment.\033[0m", 1); }
            }
        }
        else if (command == "sign" || command == "signup")
        {
            if (USER_LOGGED_IN && user_ptr != nullptr)
            {
                printTypewriter("ERROR: To sign up, you must first logout.", 2, 30, 40);
            }
            else
            {
                user_ptr = new promptShellUser;

                if (user_ptr == nullptr) { printTypewriter("\033[1mERROR: Unable to create an account at this time.\033[0m", 2); }
            }
        }
        else if (command == "psmerge" || command == "merge")
        {
            psmergCommand();
        }
        else if (command == "version" || command == "ver")
        {
            if (USER_LOGGED_IN && user_ptr != nullptr)
            {
                versionCommand(user_ptr->getterUsername());
                continue;
            }
            
            versionCommand();
        }
        else if (command == "terminate" || command == "term" || command == "exit" || input == "quit") 
        {
            STATUS_EXIT_ON = true; exit(EXIT_SUCCESS);
        }
        else 
        {
            if (PS_MERGE_ON)
            {
                psmergeCommand(tokens);
            }
            else
            {
                printTypewriter("'" + input + "'" + " is not a recognized command.", 2);
            }
        }
    } while (true);
}
void promptShellIntroduction(std::string username)
{
    systemClear();
    lineSeparator();
    printTypewriter("PromptShell - Terminal Application " + APPLICATION_VERSION + "  \033[1m<" + username + ">\033[0m" , 1, 1, 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
    printTypewriter("Install the latest PromptShell for new features and improvements! https://bit.ly/3mvnEc2", 2, 1, 1);
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
void cdCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() < 2)
    {
        printTypewriter("\033[1mUsage: cd <directory>\033[0m", 2, 5, 10);
    }
    else
    {
        if (chdir(arguments[1].c_str()) == 0)
        {
            if (!DIRECTORY_SHOW_ON && arguments[1] != "." && arguments[1] != "..")
                printTypewriter("\033[1mDirectory changed to '" + arguments[1] + "'.\033[0m", 2, 5, 10);
            else
                std::cout << std::endl;
        }
        else
        {
            printTypewriter("\033[1mDirectory not found: " + arguments[1] + "'.\033[1m", 1, 5, 10);
        }
    }
}
void mvCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() != 3)
    {
        printTypewriter("\033[1mUsage: mv <source_file> <destination_file>\033[0m", 2, 5, 10);
        return;
    }

    // Extract source and destination file paths from arguments ..
    const std::string& source_file = arguments[1];
    const std::string& destination_file = arguments[2];

    // Move file using system call ..
    int result = std::rename(source_file.c_str(), destination_file.c_str());

    // Check if move operation succeeded ..
    if (result == 0)
    {
        printTypewriter("File moved: '" + source_file + "' -> '" + destination_file + "'.", 2, 5, 10);
    }
    else 
    {
        printTypewriter("Failed to move file: '" + source_file + "' -> '" + destination_file + ".", 2, 5, 10);
    }
}
void cpCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() != 3)
    {
        printTypewriter("\033[1mUsage: cp <source_file> <destination_file>\033[0m", 2, 5, 10);
        return;
    }

    // Extract source and destination file paths from arguments ..
    const std::string source_file = arguments[1];
    const std::string destination_file = arguments[2];

    // Open source file for binary input ..
    std::ifstream src(source_file, std::ios::binary);
    if (!src.is_open()) { printTypewriter("Failed to open source file '" + source_file + "'.", 2, 5, 10); return; }

    // Open destination file for binary output ..
    std::ofstream dest(destination_file, std::ios::binary);
    if (!dest.is_open()) { printTypewriter("Failed to open destination file '" + destination_file + "'.", 2, 5, 10); src.close(); return; }

    // Copy contents from source to destination ..
    dest << src.rdbuf();

    // Close file streams ..
    src.close();
    dest.close();

    printTypewriter("File copied: '" + source_file + "' -> '" + destination_file + "'.", 2, 5, 10);
}
void lsCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() > 1)
    {
        printTypewriter("\033[1mUsage: ls\033[0m", 2, 5, 10);
        return;
    }

    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(".")) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            printTypewriter(entry->d_name, 1, 5, 10);
        }
        std::cout << std::endl;
        closedir(dir);
    }
    else
    {
        std::cerr << "Failed to open directory" << std::endl;
    }

}
void pwdCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() > 1)
    {
        printTypewriter("\033[1mUsage: pwd\033[0m", 2, 5, 10);
        return;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printTypewriter("\nPath", 1, 5, 10);
        printTypewriter("---", 1, 5, 10);
        printTypewriter(cwd, 3, 5, 10);
    }
    else
    {
        std::cerr << "\033[1mFailed to get current working directory\033[0m" << std::endl;
    }
}
void rmCommand(const std::vector<std::string>& file_names)
{
    if (file_names.empty()) { printTypewriter("\033[1mUsage: rm <filename(s)>\033[0m", 2, 5, 10); return; }

    for (const std::string& file_name : file_names)
    {
        if (remove(file_name.c_str()) != 0)
        {
            printTypewriter("\033[1mERROR: Failed to remove file '" + file_name + "'.\033[0m", 1, 5, 10);
        }
        else
        {
            printTypewriter("\033[1mRemoved file '" + file_name + "'.\033[0m", 1, 5, 10);
        }
    }
    std::cout << std::endl;
}
void rmdirCommand(const std::vector<std::string>& dir_names)
{
    if (dir_names.empty()) { printTypewriter("\033[1mUsage: rmdir <directory>\033[0m", 2, 5, 10); return; }

   for (const std::string& dir_name : dir_names)
   {
        if (rmdir(dir_name.c_str()) != 0)
        {
            printTypewriter("\033[1mERROR: Failed to remove directory '" + dir_name+ "'.\033[0m", 1, 5, 10);
        }
        else
        {
            printTypewriter("\033[1mRemoved directory '" + dir_name + "'.\033[0m", 1, 5, 10);
        }
   }
   std::cout << std::endl;
}
void mkdirCommand(const std::vector<std::string>& dir_names)
{
    if (dir_names.empty()) { printTypewriter("\033[1mUsage: mkdir <directory>\033[0m", 2, 5, 10); return; }

    for (const std::string& dir_name : dir_names)
    {
        if (mkdir(dir_name.c_str()) != 0)
        {
            printTypewriter("\033[1mERROR: Failed to create directory'" + dir_name + "'.\033[0m", 1, 5, 10);
        }
        else
        {
            printTypewriter("\033[1mCreated directory '" + dir_name + "'.\033[0m", 1, 5, 10);
        }
    }
    std::cout << std::endl;
}
void touchCommand(const std::vector<std::string>& file_names)
{
    if (file_names.empty()) { printTypewriter("\033[1mUsage: touch <filename(s)>\033[0m", 2, 5, 10); return; }
    
    for (const auto& file_name : file_names)
    {
        std::ofstream file(file_name);
        if (!file.is_open())
        {
            printTypewriter("\033[1mERROR: Failed to create file '" + file_name + "'.\033[0m", 1, 5, 10);
        }
        else
        {
            printTypewriter("\033[1mCreated file '" + file_name + "'.\033[0m", 1, 5, 10);
        }
    }
    std::cout << std::endl;
}
void echoCommand(const std::vector<std::string>& texts)
{
    if (texts.empty()) { printTypewriter("\033[1mUsage: echo <text>\033[0m", 2, 5, 10); return; }

    for (const std::string& text : texts)
    {
        printTypewriter(text + " ", 0, 5, 10);
    }
    std::cout << std::endl << std::endl;
}
void catCommand(const std::vector<std::string>& file_names)
{
    if (file_names.size() < 1) { printTypewriter("\033[1mUsage: cat <filename(s)>\033[0m", 2, 5, 10); return; }

    for (const std::string& file_name : file_names)
    {
        std::ifstream file(file_name);

        if (!file.is_open())
        {
            printTypewriter("\033[1mERROR: Failed to open file '" + file_name + "'.\033[0m", 1, 5, 10);
            continue;
        }

        printTypewriter("\n\033[1m=== Contents of " + file_name + " ===\033[0m", 1, 5, 10);

        std::string line;
        while (std::getline(file, line))
        {
            printTypewriter(line, 1, 5, 10);
        }
        printTypewriter("\033[1m=== Contents of " + file_name + " ===\033[0m", 1, 5, 10);

        file.close();
    }
    
    std::cout << std::endl;
}
void gitCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() < 1) { printTypewriter("\033[1mUsage: git <command(s)>\033[0m", 2, 5, 10); return; }
    
    std::string git_command = "git";
    for (const std::string& argument : arguments)
    {
        git_command += " " + argument;
    }

    int result = system(git_command.c_str());

    if (result == 0)
    {
        printTypewriter("\033[1mGit command executed successfully.\033[0m", 1, 5, 10);
    }
    else
    {
        printTypewriter("\033[1mERROR: Git command exited with status '" + std::to_string(result) + "'.\033[0m", 1, 5, 10);
    }

    std::cout << std::endl;
}
void wingetCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() < 1) { printTypewriter("\033[1mUsage: winget <command(s)>\033[0m", 2, 5, 10); return; }

    std::string winget_command = "winget";
        for (const std::string& argument : arguments)
    {
        winget_command += " " + argument;
    }

    int result = system(winget_command.c_str());
    if (result == 0)
    {
        printTypewriter("\033[1mwinget command executed successfully.\033[0m", 1, 5, 10);
    }
    else    
    {
        printTypewriter("\033[1mERROR: winget command exited with status '" + std::to_string(result) + "'.\033[0m", 1, 5, 10);
    }

    std::cout << std::endl;
}
void psmergeCommand(const std::vector<std::string>& arguments)
{
    if (arguments.size() < 1) { return; }

    std::string _command = "";
    for (const std::string& argument : arguments)
    {
        _command += argument + " ";
    }

    int result = system(_command.c_str());
    
    if (result == 0)
    {
        printTypewriter("\033[1m'" + arguments[0] + "' command executed successfully.\033[0m", 1, 5, 10);
    }
    else    
    {
        printTypewriter("\033[1mERROR: '" + arguments[0] + "' command exited with status '" + std::to_string(result) + "'.\033[0m", 1, 5, 10);
    }

    std::cout << std::endl;
}
void updateDirectory()
{
    // Solely to update the 'USER_GUEST_DIRECTORY' & 'USER_LOGGED_DIRECTORY' ..
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        USER_GUEST_DIRECTORY = cwd;
        USER_LOGGED_DIRECTORY = cwd;
    }
}

/* ------------------------
INLINE FUNCTION DEFINITIONS
--------------------------- */
inline void systemClear()
{
    std::string OS = OS_NAME;

    if (OS == "Windows")
        system("cls");
    else
        system("clear");
}
inline void lineSeparator()
{
    std::string LINE_SEPARATOR = 
"---------------------------------------------------------------------------------";

    if (LINE_SEPERATOR_ON)
    {
        printTypewriter(LINE_SEPARATOR, 1, 1, 1);
    }
    else 
    {
        // Print nothing ..
    }
}
inline void helpCommand(bool flag)
{
    printTypewriter("\n\033[1mSETTINGS\033[0m", 1, 0, 10);
    printTypewriter("    DELAY           Allow the user to turn the text delay ", 0, 0, 10); printTypewriter((!TEXT_DELAY_ON) ? "on." : "off.", 1, 0, 10);
    printTypewriter("    LINE            Allow the user to turn the line seperator ", 0, 0, 10); printTypewriter((!LINE_SEPERATOR_ON) ? "on." : "off.", 1, 0, 10);
    printTypewriter("    DR              Allow the user to show the current directory ", 0, 0, 10); printTypewriter((!DIRECTORY_SHOW_ON) ? "on." : "off.", 1, 0, 10);
    printTypewriter("\n\033[1mCOMMANDS\033[0m", 1, 0, 10);
    printTypewriter("    CLS             Allow the user to clear up the terminal.", 1, 0, 10);
    printTypewriter("    PS              Allow the user to reset the terminal.", 1, 0, 10);
    printTypewriter("    MV              Allow the user to move files from one directory to another.", 1, 0, 10);
    printTypewriter("    CP              Allow the user to copy files from one directory to another.", 1, 0, 10);
    printTypewriter("    CD              Allow the user to change the current working directory.", 1, 0, 10);
    printTypewriter("    LS              Allow the user to list the current directory contents.", 1, 0, 10);
    printTypewriter("    PWD             Allow the user to print the working directory.", 1, 0, 10);
    printTypewriter("    RM              Allow the user to remove files within a directory.", 1, 0, 10);
    printTypewriter("    RMDIR           Allow the user to remove directories.", 1, 0, 10);
    printTypewriter("    MKDIR           Allow the user to create directories.", 1, 0, 10);
    printTypewriter("    TOUCH           Allow the user to create empty files.", 1, 0, 10);
    printTypewriter("    ECHO            Allow the user to print text to the terminal.", 1, 0, 10);
    printTypewriter("    CAT             Allow the user to print the contents of a file to the terminal.", 1, 0, 10);
    printTypewriter("    GIT             Allow the user to use git functionality from the terminal.", 1, 0, 10);
    if (OS == "Windows")
    {
    printTypewriter("    WINGET          Allow the user to use the Windows Package Manager from the terminal.", 1, 0, 10);
    }
    printTypewriter("\n\033[1mFUNCTIONS\033[0m", 1, 0, 10);
    if (flag) 
    {
    printTypewriter("    LOGOUT          Allow the user to logout out of their account.", 1, 0, 10);
    //printTypewriter("    CHGUSER         Allow the user to change their username.", 1, 0, 10);
    //printTypewriter("    CHGPASS         Allow the user to change their password.", 1, 0, 10);
    }
    else
    {
    printTypewriter("    LOGIN           Allow the user to login onto their account.", 1, 0, 10);
    printTypewriter("    SIGNUP          Allow the user to create their account.", 1, 0, 10);
    }
    printTypewriter("    PSMERGE         Allow the user to use commands that are recognized PowerShell. ", 1, 0, 10);
    printTypewriter("    VERSION         Allow the user to see the current running version.", 1, 0, 10);
    printTypewriter("    TERMINATE       Allow the user to terminate the terminal application.", 2, 0, 10);
}
inline void delayCommand()
{
    TEXT_DELAY_ON = !TEXT_DELAY_ON;
    printTypewriter("\033[1mText delay has been turned ", 0); 
    printTypewriter((TEXT_DELAY_ON) ? "on.\033[0m" : "off.\033[0m", 2);
}
inline void lineCommand()
{
    LINE_SEPERATOR_ON = !LINE_SEPERATOR_ON;
    printTypewriter("\033[1mLine seperator has been turned ", 0); 
    printTypewriter((LINE_SEPERATOR_ON) ? "on.\033[0m" : "off.\033[0m", 2);
}
inline void dirCommand()
{
    DIRECTORY_SHOW_ON = !DIRECTORY_SHOW_ON;
    printTypewriter("\033[1mShow directory has been turned ", 0); 
    printTypewriter((DIRECTORY_SHOW_ON) ? "on.\033[0m" : "off.\033[0m", 2);
}
inline void psmergCommand()
{
    PS_MERGE_ON = !PS_MERGE_ON;
    printTypewriter("\033[1mPromptShell and PowerShell merge has been turned ", 0); 
    printTypewriter((PS_MERGE_ON) ? "on.\033[0m" : "off.\033[0m", 2);
}
inline void versionCommand()
{
    systemClear();
    lineSeparator();
    printTypewriter(APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.", 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2, 1, 1);
}
inline void versionCommand(std::string username)
{
    systemClear();
    lineSeparator();
    printTypewriter(APPLICATION_VERSION + " | " + APPLICATION_DATE_VERSION + " - Developed by Harrison L.  \033[1m<" + username + ">\033[0m", 1);
    printTypewriter("Copyright (C) PromptShell Corporation. All rights reserved.", 2);
}