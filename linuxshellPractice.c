#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Cameron Courtney (2/2/21)
// This is a rough outline for the Project 0 Linux Shell Assignment.
// I have created a skeleton of how I believe my partner and I should
// create our structs, tokenize input, and execute commands. Since
// the actual project codebase is more expansive than this single file
// I have created, the code I have created is not yet complete. When
// my partner and I begin, we can start to fill in the gaps. This is
// just a starting point I have created to help us get a better 
// understanding of the assignemnt.


// These are dummy constants for testing purposes.
// I believe the actual project has defined these values.
#define MAX_CHARS 32
#define MAX_WORDS 256
#define MAX_COMMANDS 256

// This is a constant that represents the ampersand
// in the command line. This will be used to break up
// commands.
const char AMP = '&';

// This is a struct that represents that of a Command on
// the command line.
typedef struct Command
{   
    // This is a char array of the arguments sent in to the
    // command.
    char **args;

    // This is the file that any ouput is redirected to 
    // (if necessary).
    char *outputFile;
} cmd;

char **tokenize(char *cmdLine);
void printTokens(char **tokens);
void printCommandInfo(cmd *command);
cmd* makeCommand(char **tokens);
void ampTest(char *s);
void eval(struct Command *cmd);
int try_exec_builtin(struct Command *cmd);
int exec_builtin(struct Command *cmd);
int count_args (struct Command *cmd);
cmd **list(char **tokens);
void printList(cmd** commandList);
void foo(char *c);
void exec(cmd **command, int index);

// This loop does basic tokenization, creates a command
// struct, and list of commands. This main method can be
// better structured and broken down into helper functions,
// but this is just for testing. I will clean it up for the 
// actual project.
int main()
{
    size_t size = MAX_CHARS;
    size_t characters = 0;
    int index = 0;
    char *amp = "&";
    char *test = "cameron";
    bool comp = strncmp(amp, test, 1);
    //printf("%d\n", comp);
    char *c = (char *)(malloc((sizeof(char)) * (size + 1)));
    if (c == NULL)
    {
        printf("Could not allocate");
    }

    // This is just to interact with the user.
    printf("Enter Lines: ");
    characters = getline(&c, &size, stdin);
    printf("\n");
    printf("%s\n", c);
   // printf("%zu\n", characters);
    printf("\n");

    // The command line is tokenized.
    // It is then printed just to make sure everything is
    // tokenized properly.
    char **tokens = tokenize(c);
    printTokens(tokens);

    // These create the command struct and print them
    // out for debugging purposes.
    //cmd *command = makeCommand(tokens);
    //printCommandInfo(command);

    // This evaluates the command.
    //eval(command);

    // This creates a list of all of the commands.
    cmd **commandList = list(tokens);

    printf("\nI have made a list of commands: \n");
    printf("\n");

    // This prints the commands.
    printList(commandList);

    // This executes a given command.
    //exec(commandList, 0);

    // Deallacoates any allocated memory.
    free(commandList);
    free(tokens);

    return 0;
}

// This is a dummy exec function.
// The real project will much more effectively
// make use of concurrent commands. This is just
// for basic testing to make sure the structs are
// set up properly/
void exec(cmd **list, int index) {
    cmd *com = list[index];
        if (com == NULL) {
        printf("END OF COMMANDS\n");
        exit(0);
    }

    int c = fork();

    if (c == 0) {
        printf("CHILD %s\n", com->args[0]);
        printf("index: %d\n", index);
        eval(com);
        exec(list, (index + 1));
        exit(0);
    }

    else {
        wait(NULL);
    }

}

// This prints the list of commands that are to 
// be executed.
void printList(cmd **commandList) {
    cmd* cur = NULL;
    int index = 0;
    cur = commandList[index];

    while (cur != NULL) {
        printf("COMMAND #%d\n", index + 1);
        printCommandInfo(cur);
        eval(cur);
        index++;
        cur = commandList[index];
        printf("\n");
    }

    printf("print list end\n");
}

// This runs through the list of commands and executes them.
// This is not a complete function, it is just for testing
// right now.
cmd** list(char **tokens) {
    cmd **list = malloc(sizeof(cmd *) * (MAX_COMMANDS + 1));
    char **startToken = NULL;
    int commandIndex = 0;
    int tokenIndex = 0;
    bool prevAmp = true;
    char *cur = NULL;
    cur = tokens[tokenIndex];

    while (cur != NULL && tokenIndex < MAX_WORDS) {
        if (strcmp(cur, "&") == 0) {
            tokens[tokenIndex] = NULL;

            if (startToken != NULL && commandIndex < MAX_COMMANDS) {
                list[commandIndex] = makeCommand(startToken);
                //printCommandInfo(list[commandIndex]);
                //eval(list[commandIndex]);
                commandIndex++;
            }

            startToken = NULL;
            prevAmp = true;
        }

        else {
            if (prevAmp) {
                startToken = &tokens[tokenIndex];
            }

            prevAmp = false;
        }

        tokenIndex++;
        cur = tokens[tokenIndex];
    }

    if (startToken != NULL && commandIndex < MAX_COMMANDS) {
        list[commandIndex] = makeCommand(startToken);
        //eval(list[commandIndex]);
        //printCommandInfo(list[commandIndex]);
    }

    list[MAX_COMMANDS] = NULL;
    return list;
}

// This ecvaluates a given command.
// This is just for testing purposes.
// This function will be much more expansive
// in the completed version.
void eval(struct Command *cmd) {
    int status;
    
    // This checks if there are no commands.
    if (cmd == NULL) {
        printf("This is an invlaid commands\n.");
    }

    status = try_exec_builtin(cmd);
    if (!status) {
        //exec_external_cmd(cmd);
        printf("not a built in command!\n");
    }
}

// This is a messy recreation of the project's exec
// function. The function in the final version will
// be much cleaner. This is just for simple testing.
// I want to make sure it recognizes the basic commands.
int try_exec_builtin (struct Command *cmd) {
    // check if internal command
    int c = count_args(cmd);

    // Possibly return early if cmd is NULL!

    char* temp_cmd = cmd->args[0]; // maybe use strncpy
    bool status = false;

    if(strcmp(temp_cmd, "cd") == 0){
        // check for incorrect num of args
        printf("CD!!\n");

        if(c != 2){
            printf("cd error\n");
            // error
        }
        int stat = chdir(cmd->args[1]);

        // check if chdir returned error
        if(stat == -1){
            // error
        }
        status = true;
    }else if(strcmp(temp_cmd, "exit") == 0){
        printf("EXIT\n");
        if(c != 1){
            printf("Exit error\n");
            // error
        }else{
            //exit(0);
        }
        status = true;
    }else if(strcmp(temp_cmd, "path") == 0){
        printf("path!!\n");

        char **prac;
        prac = cmd->args;

        if (c == 1) {
            cmd->args[1] = "/bin";
        }


        // ADD THIS!!
        //printTokens(&(cmd->args[1]));

        // loop through args
        // place each into shell_paths array
        //int stat = set_shell_path((cmd->args) + (sizeof(char*)));
        //if(stat == 0){
            // error
        //}
        status = true;
    }

    return status;
}

// This counts the number of arguments in a command.
// Helps for debugging.
int count_args(struct Command* cmd){
    int c = 0;

    // If command is NULL, return 0.
    if (cmd == NULL) {
        return 0;
    } 

    // Otherwise grab the arguments and count them.
    char* curr = cmd->args[0]; // maybe use strncpy
    while(curr != NULL){
        c++;
        curr = cmd->args[c];
    }

    // The count is returned.
    return c;
}

// This prints the elements of a given command struct.
void printCommandInfo(cmd *com)
{
    int index = 0;
    char *cur = com->args[index];
    //printf("print command....\n");

    while (cur != NULL)
    {
        printf("ARG #%d is : %s\n", index, com->args[index]);
        index++;
        cur = com->args[index];
    }

    if (com->outputFile == NULL)
    {
        printf("NO REDIRECT!\n");
    }

    else
    {
        printf("REDIRECT IS %s\n", com->outputFile);
    }
}

// This makes command structs and returns an array of all of
// the commands.
cmd* makeCommand(char **tokens)
{
    const char *terminate = ">";
    size_t size = 1;
    int index = 0;
    bool redirect = false;
    cmd *commands = malloc(sizeof(cmd));
    commands->args = malloc(sizeof(char *) * (MAX_WORDS + 1));
    commands->args[MAX_WORDS] = NULL;
    commands->outputFile = NULL;
    printf("\n");
    //printf("COMMAND: \n");

    // MUST MAKE SURE THAT FIRST COMMAND IS NOT A REDIRECT

    char *cur;
    cur = tokens[index];

    // MAKE SURE THE FIRST COMMAND IS NOT A REDIRECT
    if (strncmp(cur, terminate, 1) == 0) {
        printf("FIRST COMMAND IS A REDIRECT!! THIS IS AN ERROR\n");
        return commands;
    }


    redirect = false;

    while (cur != NULL && !redirect)
    {   
        if (strncmp(cur, terminate, 1) == 0) {
            //printf("FOUND A REDIRECT\n");
            redirect = true;
        }

        else {
            //printf("ADDING ARGS\n");
            commands->args[index] = cur;
        }

        index++;
        cur = tokens[index];
    }

    if (redirect) {
        //printf("redirect time!\n");
        if (cur == NULL) {
         printf("ERROR NO FILE TO REDIRECT TO!");
        }
        else{
            commands->outputFile = cur;
            cur = tokens[index++];
        }
    }

    // Also check if multiple redirects
    //if (cur != NULL || THIS GIVES SEGFAULT --> strncmp(cur, "&", 1) != 0) {
      // printf("MULTIPLE REDIRECTS THIS IS AN ERROR\n");
    //}
    //printf("exit the making commands\n");
    return commands;
}

// This function tokenizes the command line.
char **tokenize(char *cmdLine) {
    char *delimiters = "\n\t ";
    int index = 0;
    int otherDex = 0;
    char **tokens = malloc((sizeof(char *) * (MAX_WORDS + 1)));

    // maybe change the size
    char *tokPoint = malloc(sizeof(char *));
    char *other;

    bool prevAmp = false;
    bool prevLet = false;
    tokPoint = strtok(cmdLine, delimiters);

    while (tokPoint != NULL && index < MAX_WORDS)
    {
        // Another pointer to the tokPoint is stored.
        other = tokPoint;

        // This loops through the tokPoint, searching for '&' chars.
        // It loops through all of tokPoint.
        while (other[otherDex] != '\0') {

            // If the current character is an '&', work must be done.
            if (other[otherDex] == '&') {

                // The location in tokPoint where '&' existed is
                // made into a terminating character.
                other[otherDex] = '\0';

                // If the previous character was not a '&', 
                // this is the first occurrence of a '&'. A pointer
                // in 'tokens' is updated to reflect this.
                if (!prevAmp) {
                    tokens[index] = "&";
                    index++;
                }

                prevAmp = true;
                prevLet = false;
            }

            else {

                // Otherwise, the character is not a '&'. If the 
                // previous character was not a letter, this the 
                // beginning of a new String. A pointer to it is
                // stored in 'tokens'.
                if (!prevLet) {
                    tokens[index] = other + otherDex;
                    index++;
                }

                prevLet = true;
                prevAmp = false;
            }

            otherDex++;
        }

        // A new tokPoint is grabbed and varaibles are reset for the next
        // loop.
        tokPoint = strtok(NULL, delimiters);
        prevAmp = prevLet = false;
        otherDex = 0;
    }

    // The last index in 'tokens' is set to NULL and 'tokens' is then returned.
    tokens[index] = NULL;
    return tokens;
}

// This prints the array of sent-in tokens.
void printTokens(char **tokens)
{
    char *cur;
    int index = 0;
    cur = tokens[index];
    printf("TOKENS: \n");

    // Basic while loop that prints each token.
    while (cur != NULL)
    {
        printf("%s\n", tokens[index]);
        index++;
        cur = tokens[index];
    }

    printf("\n");
}