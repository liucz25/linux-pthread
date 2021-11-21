#include <stdio.h>
#include<stdlib.h>
#include<string.h>

int help();
int quit();
#define CMD_MAX_LEN 128
#define DECS_LEN    1024
#define CMD_NUM     10

typedef struct DataNode
{
    char* cmd;
    char* desc;
    int   (*handler)();
    struct DataNode *next;
}tDataNode;

static tDataNode head[]=
{
    {"help","this is help cmd!",help,&head[1]},
    {"version","menu program v1.0",NULL,&head[2]},
    {"quit","quit the cmd ",quit,NULL}
};

int main()
{
    /* cmd line begins */
    while(1)
    {
        char cmd[CMD_MAX_LEN];
        printf("Input a cmd >");
        scanf("%s",cmd);
        tDataNode *p=head;
        while(p!=NULL)
        {
            if(strcmp(p->cmd,cmd)==0)
            {
                printf("%s  -  %s\n",p->cmd,p->desc);
                if (p->handler != NULL)
                {
                    p->handler();
                }
                    break;
            }
            p=p->next;
            if(p==NULL)
            {
                printf("worry cmd\n");
            }
        }
    }



    printf("Hello world\n");
    return 0;
}

int help()
{
    printf("this is help function\n");
    tDataNode *p=head;
    while(p!=NULL)
    {
        printf("%s   -   %s\n",p->cmd,p->desc);
        p=p->next;
    }
    return 0;
};
int quit()
{
    exit(0);
}
