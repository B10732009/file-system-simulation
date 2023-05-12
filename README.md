# File System Simulation #

## Objectives ##
    
`C programming` `pointers` `link-lists` `trees`

## Background (Unix File System Tree) ##

The logical organization of a Unix file system is a general tree, as shown by the following diagram.


                     ---------------- / ---------------
                     |                |               |
               ----  a ---         -- b ---           c
              |          |        |   |   |           |
              b      --- c ---    d   e   f           g
                     |   |   |   
                     h   i   j

For simplicity, we shall assume that the tree contains only `DIRs` and `FILEs`. The root directory is represented by the symbol `/`. A `DIR` node may have a variable number of children nodes. Children nodes of the same parent are called siblings. In a Unix file system, each node is represented by a unique pathname of the form `/a/b/c` or `a/b/c`. A pathname is absolute if it begins with `/`, indicating that it starts from the root. Otherwise, it is relative to the **Current Working Directory (CWD)**. 

A general tree can be implemented as a binary tree. For each node, let childPtr point to the oldest child, and let siblingPtr point to the oldest sibling. For convenience, each node also has a parentPtr pointing to its parent node. For the root node, both parentPtr and siblingPtr point to itself.


## Requirements ##

1. Write a C program to simulate the Unix file system tree.

2. Your program should work as follows
    (1). Start with a / node.
    (2). Prompt the user for a command: `mkdir`, `rmdir`, `cd`, `ls`, `pwd`, `creat`, `rm`, `save`, `reload`, `quit`.
    (3). Execute the command, with appropriate tracing messages.
    (4). Repeat (2) until the `quit` command.

3. Commands Specification

    |  Command   | Specification |
    |  ----  | ----  |
    | mkdir  pathname  | make a new directiry for a given pathname |
    | rmdir  pathname  | rm the directory, if it is empty |
    | cd [pathname]  | change CWD to pathname, or to / if no pathname |
    | ls [pathname] | list the directory contents of pathname or CWD |
    | pwd  | print the (absolute) pathname of CWD |
    | creat  pathname  | create a FILE node |
    | rm pathname | rm a FILE node |
    | save filename | save the current file system tree in a file |
    | reload filename | re-initalize the file system tree from a file |
    | quit | save the file system tree, then terminate the program |


4. Help Infos
    - NODE type : define a C struct for the NODE type:
        
        ```
            64 chars      : name string of the node;
            char          : node type : 'D' or 'F'
            node pointers : *childPtr, *siblingPtr, *parentPtr;
        ```

    - Global variables

        ```c
        NODE *root, *cwd;                             /* root and CWD pointers */
        char line[128];                               /* user input line */
        char command[16], pathname[64];               /* user inputs */
        char dirname[64], basename[64];               /* string holders */
        /* (Others as needed) */
        ```

    - Find command : return an index
        ```c
        char *cmd[] = {"menu", "mkdir", "rmdir", "ls", "cd", "pwd", "create", "rm",
                    "reload", "save", "quit", 0};

        int findCmd(char *command)
        {
            int i = 0;
            while (cmd[i]) {
                if (strcmp(command, cmd[i]) == 0)
                    return i;
                i++;
            }
            return -1;
        }

        int index = findCmd(cmdString);  
        /* e.g. cmdString = "creat" ==> index = 6 */
        ```

    - Main function
        ```c
        void main(void)
        {
            int id;
            initialize();
            printf("Enter menu for help menu\n");
            printf("Command : ");
            while (1) {
                fgets(line, 128, stdin);
                command[0] = pathname[0] = 0;
                sscanf(line, "%s %s", command, pathname);
                if (command[0] != 0) {
                    id = findCmd(command);
                    if (id < 0) {
                        printf("invalid command\n");
                        continue;
                    }
                
                    fptr[id](pathname);
                    printf("Command : ");		
                }
            }
        }
        ```

        If the number of commands is small, e.g. only a few, it is OK to use a switch table, which is much better than a lot of `if .. else if ...`. For large number of commands, a table of **FUNCTION POINTERs** is usually used.

    - Action functions

        - HOW TO mkdir pathname

            ```c
            mkdir(char *pathname)
            {
                (1). Break up pathname into dirname and basename, e.g.
                    ABSOLUTE : pathname=/a/b/c/d. Then dirname=/a/b/c, basename=d
                    RELATIVE : pathname= a/b/c/d. Then dirname=a/b/c,  basename=d
                    
                (2). Search for the dirname node :
                    ASSOLUTE pathname : search from /
                    RELATIVE pathname : search from CWD.

                            if nonexist ==> error messages and return
                            if exist but not DIR ==> errot message and return
                        
                (3). (dirname exists and is a DIR) :
                    Search for basename in (under) the dirname node :
                            if already exists ==> error message and return;
            
                    ADD a new DIR node under dirname.
            }
            ```

        - Similarly for other action functions

            - HOW TO rmdir pathname
                ```
                (1). if pathname is absolute, start = /
                    else                     start = CWD --> current DIR node
                (2). search for pathname node :
                        tokenize pathname into components;
                        beginning from start, search for each component;
                        return ERROR if fails
                (3). pathname exists : 
                        check it's a DIR type;
                        check DIR is empty; can't rmdir if NOT empty;
                (4). delete node from parent's child list;
                ```
      
            - HOW TO creat pathname
                ```
                SAME AS mkdir except that the node type is F
                ```

            - HOW TO rm pathname
                ```
                SAME AS rmdir except check it's a file, do not need to check for EMPTY.
                ```

            - HOW TO cd pathname
                ```
                (1). find pathname node;
                (2). check it's a DIR;
                (3). change CWD to point at DIR
                ```

            - HOW TO ls pathname
                ```
                (1). find pathname node
                (2). list all children nodes in the form  TYPE  NAME
                                                          TYEP  NAME
                                                          ..........  
                ```

            - HOW TO pwd
                ```c
                /***********************************************************************
                rpwd() actually prints CWD's pathname
                **********************************************************************/
                int rpwd(NODE *p)
                {
                    WRITE YOUR C CODE HERE
                } 
                /*************************************************************************
                pwd() prints the full pathname of CWD. call rpwd(cwd) to do it recursively.
                *************************************************************************/
                int pwd()
                {
                    rpwd(cwd);
                    printf("\n");
                } 
                ```

            - HOW TO save to a FILE
                ```c
                FILE *fp = fopen("myfile", "w+");           // open a FILE stream for WRITE
                fprintf(fp, "%c %s", 'D', "namestring\n");  // print a line to file by FORMAT 
                fclose(fp);                                 // close FILE stream when done
                ```

        - Save filename
            Assume the file system tree is          
                
            ```
                    /
                ------------
                |     |    |
                A     B    C
                ---    |    |
                | |    z    E
                x y
            ```
   
            where A,B,C,D are DIRs and lower case names are FILEs
 
            The tree can be represented by the (text) lines :
            
            ```
                type      path
                ----- --------------
                D     /A
                F     /A/x
                F     /A/y
                D     /B
                F     /B/z
                D     /C
                D     /C/E
            ```

            The paths are generated by PRE-ORDER traversal of a binary tree : 
                
            ```
                print node       name;
                print node.left  name;
                print node.right name;
            ```

        - Reload filename

            Read each line from a (saved) file and re-creates the file system tree.


5. More Helps
    - Learn how to read Linux man pages :
    
        ```
            /usr/man/------- man1 : commonly used commands: ls, cat, mkdir ....
                        |--- man2 : system calls
                        |--- man3 : library functions: strtok, strcat, basename, dirname
                            etc.
        ```
        
        Examples : 
            - man ls     ==> show man pages of ls in man1
            - man open   ==> show man page of open in man2
            - man strtok ==> show man page of strtok in man 3, etc.
            - man 3 dirname : show dirname in man3, NOT that of man1
 
    -  Getline
        ```c
        gets(char s[128]);         // input a string, hopefully NOT overrun 128 chars
        fgets(s[128], 128, stdin); // input at most 128 chars BUT has \r at end
        s[strlen(s)] = 0;          // kill the \r at end.
        ```
        


     - Assume : line[128] contains "mkdir /a/b/c/d", HOW TO extract token strings from line[ ] :
        ```c
        char cmd[32], pathname[64];
        sscanf(line, "%s %s", cmd, pathname);  // NOTE : NEED ADDRESSes
        ```
    
    - Assume char line[128], HOW TO write items to line[ ] :
        ```c
        sprintf(line, "%c %s %d", 'D',"namestring", 1234);
        ```

    - #include &lt;libgen.h&gt;
        ```c
        char pathname[128], temp[128];
        char dname[64], bname[32];

        gets(pathname);   // assume /a/b/c

        strcpy(temp, pathname);
        strcpy(dname, dirname(temp));   // dname="/a/b"

        strcpy(temp, pathname);
        strcpy(bname,basename(temp));   // bname="c"
        ```

    -  Assume pathname="/a/b/c/d"
        ```c
        s = strtok(pathname, "/");     // break up "a/b/c/d" into tokens
        
        printf("s = %s\n", s);         // print first token

        while (s = strtok(0, "/")) {     // keep calling strtok() with NULL string
            printf("s = %s\n", s);       // until it return 0 pointer
        }
        ```

    -  Function pointers
        Assume :
        ```c
        int mkdir(char *pathname){..........}
        int rmdir(char *pathname){..........}
        etc.
        ```
        are FUNCTIONS in your program.

        ```c
                                        /*   0     1      2      3   4   5    6       7   8     9       10 */
        int (*fptr[ ])(char *) = {(int (*)())menu, mkdir, rmdir, ls, cd, pwd, create, rm, save, reload, quit};
        ```
        defines a TABLE OF FUNCTION POINTERS, each corresponds to an index 0 to 10.

        ```c
        int r = fptr[index](pathname)
        ```
        invokes that function with pathname as parameter.
