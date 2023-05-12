#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char name[64];         // node name
    char type;             // node type : d = directory, f = file
    struct node *parent;   // parent pointer
    struct node *sibling;  // sibling pointer
    struct node *isibling; // inverse sibling pointer
    struct node *child;    // child pointer
};

struct node *root; // root
struct node *cwd;  // current working directory

char ssBuffer[32][64]; // buffer for strSplit() function

int strSplit(char *str)
{
    int cnt = 0;
    char *p = strtok(str, "/\n"); // string read from file will also get the'\n', so add it to delim
    while (p)
    {
        strcpy(ssBuffer[cnt], p);
        cnt++;
        p = strtok(0, "/\n");
    }
    return cnt;
}

struct node *search(struct node *ptr, int ssi, int sss, char type)
{
    // error check
    if (!ptr || ssi == sss)
        return (struct node *)0;

    // iterate through the child list
    for (struct node *iptr = ptr->child; iptr; iptr = iptr->sibling)
    {
        // name found
        if (strcmp(iptr->name, ssBuffer[ssi]) == 0)
        {
            // check node type
            if (type != 'n' && iptr->type != type)
                return (struct node *)0;
            // check if the node is the last node in this search
            if (ssi == sss - 1)
                return iptr; // yes, return the address of this node

            // no, recursively search next layer
            return search(iptr, ssi + 1, sss, type);
        }
    }

    // node not found
    return (struct node *)0;
}

void add(char *pathname, char type)
{
    // error check
    if (!pathname)
    {
        printf("\n\t[%s] error: pathname is a NULL pointer.\n\n", (type == 'd' ? "mkdir" : "creat"));
        return;
    }

    // split pathname
    int sss = strSplit(pathname);

    // initialize directory pointer (where to start?)
    struct node *dptr = (pathname[0] == '/') ? root : cwd;

    // check the need of searching dirname
    if (sss > 1)
    {
        // search dirname
        dptr = search(dptr, 0, sss - 1, 'd');

        // error check
        if (!dptr)
        {
            printf("\n\t[%s] error: path not found or is a file name.\n\n", (type == 'd' ? "mkdir" : "creat"));
            return;
        }
    }

    // check basename existence
    struct node *bptr = search(dptr, sss - 1, sss, 'n');
    if (bptr)
    {
        printf("\n\t[%s] error: directory name already exists.\n\n", (type == 'd' ? "mkdir" : "creat"));
        return;
    }

    // add new node
    struct node *newnode = (struct node *)malloc(sizeof(struct node));
    strcpy(newnode->name, ssBuffer[sss - 1]);
    newnode->type = type;
    newnode->parent = dptr;
    newnode->child = (struct node *)0;
    newnode->sibling = dptr->child;
    newnode->isibling = (struct node *)0;

    // deal with the linked-list
    if (dptr->child)
        dptr->child->isibling = newnode;
    dptr->child = newnode;
}

void delete(char *pathname, char type)
{
    // error check
    if (!pathname)
    {
        printf("\n\t[%s] error: pathname is a NULL pointer.\n\n", (type == 'd' ? "rmdir" : "rm"));
        return;
    }

    // split pathname
    int sss = strSplit(pathname);

    // initialize directory pointer (where to start?)
    struct node *dptr = (pathname[0] == '/') ? root : cwd;

    // check the need of searching dirname
    if (sss > 1)
    {
        // search dirname
        dptr = search(dptr, 0, sss - 1, 'd');

        // error check
        if (!dptr)
        {
            printf("\n\t[%s] error: path not found or is a file name.\n\n", (type == 'd' ? "rmdir" : "rm"));
            return;
        }
    }

    // check basename existence
    struct node *bptr = search(dptr, sss - 1, sss, type);
    if (!bptr)
    {
        printf("\n\t[%s] error: path not found or is a file name.\n\n", (type == 'd' ? "rmdir" : "rm"));
        return;
    }

    // check if the directory is empty
    if (bptr->child)
    {
        printf("\n\t[%s] error: directory is not empty.\n\n", (type == 'd' ? "rmdir" : "rm"));
        return;
    }

    // deal with the linked-list
    if (bptr->isibling)
        bptr->isibling->sibling = bptr->sibling;
    else
        bptr->parent->child = bptr->sibling;

    if (bptr->sibling)
        bptr->sibling->isibling = bptr->isibling;

    // release memory
    free(bptr);
}

void cd(char *pathname)
{
    // error check
    if (!pathname)
    {
        printf("\n\t[cd] error: pathname is a NULL pointer.\n\n");
        return;
    }

    // no pathname assigned, jump to root directly
    if (pathname[0] == 0)
    {
        cwd = root;
        return;
    }

    // split pathname
    int sss = strSplit(pathname);

    // initialize directory pointer (where to start?)
    struct node *dptr = (pathname[0] == '/') ? root : cwd;

    // search dirname
    dptr = search(dptr, 0, sss, 'd');

    // error check
    if (!dptr)
    {
        printf("\n\t[cd] error: path not found or is a file name.\n\n");
        return;
    }

    // assign the pointer to cwd
    cwd = dptr;
}

char *pwd(struct node *ptr, char *buf)
{
    // error check
    if (!ptr)
    {
        printf("\n\t[pwd] error: ptr is a NULL pointer.\n\n");
        return (char *)0;
    }

    // error check
    if (!buf)
    {
        printf("\n\t[pwd] error: buf is a NULL pointer.\n\n");
        return (char *)0;
    }

    // initialize buffer
    buf[0] = 0;

    char tempbuf[256];

    // jump up to the root layer by layer
    for (struct node *iptr = ptr; iptr->parent; iptr = iptr->parent)
    {
        sprintf(tempbuf, "%s/%s", iptr->name, buf);
        sprintf(buf, "%s", tempbuf);
    }

    sprintf(tempbuf, "/%s", buf);
    sprintf(buf, "%s", tempbuf);
    return buf;
}

void ls(char *pathname)
{
    // error check
    if (!pathname)
    {
        printf("\n\t[ls] error: pathname is a NULL pointer.\n\n");
        return;
    }

    // split pathname
    int sss = strSplit(pathname);

    // initialize directory pointer (where to start?)
    // no pathname assigned, give cwd
    struct node *dptr = cwd;
    // pathname asigned, search the pathname
    if (pathname[0] != 0)
    {
        dptr = (pathname[0] == '/') ? root : cwd;
        dptr = search(dptr, 0, sss, 'd');
    }

    // error check
    if (!dptr)
    {
        printf("\n\t[ls] error: path not found.\n\n");
        return;
    }

    // iterate through the child list and print infos
    printf("\n");
    printf("\tname\ttype\n\n");
    for (struct node *iptr = dptr->child; iptr; iptr = iptr->sibling)
        printf("\t%s\t%c\n", iptr->name, iptr->type);
    printf("\n");
}

void save(struct node *ptr, FILE *fp)
{
    // for recursion
    if (!ptr)
        return;

    // error check
    if (!fp)
    {
        printf("\n\t[save] error: fail to get file pointer.\n\n");
        return;
    }

    // get the pwd of this node
    char pwdbuf[128];
    pwd(ptr, pwdbuf);

    // the pwd of root is "/", but don't want to store it, so restrict the length of pwd should longer than 1
    if (strlen(pwdbuf) > 1)
        fprintf(fp, "%c %s\n", ptr->type, pwdbuf);

    // recursively save the pwd of child list
    for (struct node *iptr = ptr->child; iptr; iptr = iptr->sibling)
        save(iptr, fp);
}

void clear(struct node *ptr)
{
    // for recursion
    if (!ptr)
        return;

    // recursively clean the child list
    for (struct node *iptr = ptr->child; iptr; iptr = iptr->sibling)
        clear(iptr);

    // deal with the linked-list
    if (ptr->parent)
        ptr->parent->child = (struct node *)0;

    // if this node isn't root, release it memory
    if (ptr != root)
        free(ptr);
}

void reload(FILE *fp)
{
    // error check
    if (!fp)
    {
        printf("\n\t[reload] error: fail to get file pointer.\n\n");
        return;
    }

    char type;
    char pathname[128];

    // read every line and add node
    while (fgets(pathname, 128, fp))
    {
        // end-of-file mark
        if (pathname[0] == 'x')
            break;

        // add node
        add(pathname + 2, pathname[0]);
    }
}

void print(struct node *ptr, int depth)
{
    // for recursion
    if (!ptr)
        return;

    // iterate through the child list
    for (struct node *iptr = ptr->child; iptr; iptr = iptr->sibling)
    {
        printf("\n");
        // print indent
        for (int i = 0; i < depth + 1; i++)
            printf("\t");

        // print node infos
        printf("|---%s(%c)\n", iptr->name, iptr->type);

        // if the node is directory, recursively print infos of the child list
        if (iptr->type == 'd')
            print(iptr, depth + 1);

        if (depth == 0)
            printf("\n");
    }
}

int main()
{
    // initialize root node
    root = (struct node *)malloc(sizeof(struct node));
    strcpy(root->name, "root");
    root->type = 'd';
    root->parent = (struct node *)0;
    root->child = (struct node *)0;
    root->sibling = (struct node *)0;
    root->isibling = (struct node *)0;

    // initialize cwd
    cwd = root;

    while (1)
    {
        char line[128];
        char cmdname[128];
        char pathname[128];
        char pwdbuf[128];

        printf("%s $ ", pwd(cwd, pwdbuf));
        fgets(line, 128, stdin);

        // initialize cmdname, pathname
        cmdname[0] = 0;
        pathname[0] = 0;
        sscanf(line, "%s %s", cmdname, pathname);

        if (strcmp(cmdname, "mkdir") == 0)
            add(pathname, 'd');
        else if (strcmp(cmdname, "rmdir") == 0)
            delete (pathname, 'd');
        else if (strcmp(cmdname, "creat") == 0)
            add(pathname, 'f');
        else if (strcmp(cmdname, "rm") == 0)
            delete (pathname, 'f');
        else if (strcmp(cmdname, "print") == 0)
            print(root, 0);
        else if (strcmp(cmdname, "pwd") == 0)
            printf("%s\n", pwd(cwd, pwdbuf));
        else if (strcmp(cmdname, "cd") == 0)
            cd(pathname);
        else if (strcmp(cmdname, "ls") == 0)
            ls(pathname);
        else if (strcmp(cmdname, "clear") == 0)
            clear(root);
        else if (strcmp(cmdname, "save") == 0)
        {
            FILE *fp = fopen(pathname, "w");
            save(root, fp);
            fprintf(fp, "%s\n", "x /"); // add the end-of-file mark
            fclose(fp);
        }
        else if (strcmp(cmdname, "reload") == 0)
        {
            clear(root);
            FILE *fp = fopen(pathname, "r");
            reload(fp);
            fclose(fp);
        }
        else if (strcmp(cmdname, "quit") == 0)
        {
            clear(root);
            break;
        }
        else
            printf("\n\t[main] error: unknown command.\n\n");
    }

    return 0;
}