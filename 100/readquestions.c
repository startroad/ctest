#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 2048
#define DEFAULT_FILE_NAME "questions.txt"
#define QUESTION_MIN 1
#define QUESTION_MAX 99
#define QUESTION_ALL (QUESTION_MAX+1)
#define QUESTION_TOK "=============================================================="


char g_filename[256] = DEFAULT_FILE_NAME;
static int g_current_question = 1;

int readfilelen()
{
	FILE* fp = NULL;
	int len=0;

	fp = fopen(DEFAULT_FILE_NAME, "r");
	if (fp != NULL)
	{
		fseek(fp,0,SEEK_END);
		len = ftell(fp);
		fclose(fp);
		printf("len=%d\n",len);
	}else
		printf("open failed\n");

	return len;
}

int readfile(char *buf, int len)
{
	FILE* fp = NULL;

	fp = fopen(DEFAULT_FILE_NAME, "r");
	if (fp != NULL)
        {
		fread(buf, len, 1, fp);
                fclose(fp);
        }else
                printf("open failed\n");

	return len;
}

void shwo_menu()
{
	printf("=============MENU============\n");
	printf("q:quit\n");
	printf("n:next question,(current:%d)\n",g_current_question);
	printf("p:prev question,(current:%d)\n",g_current_question);
	printf("g[num]:go to question num,(Example g55)\n");
	printf("a:show all question\n");
	printf("=============================\n");
}

void show_question(int num,char **question_buf)
{
	printf("show:%d\n",num);
	if(num != QUESTION_ALL)
		printf("%s\n",question_buf[num]);
	else
	{	
		int i=QUESTION_MIN;
		for(;i<QUESTION_MAX;i++)
		{
			if(question_buf[i]!=NULL)
			printf("%s\n",question_buf[i]);
		}
	}
}

void input_proc(char *input,char **question_buf)
{
	char key = input[0];
	int current_question_bak = g_current_question;

	switch(key)
	{
		case 'n':
			g_current_question++;
			break;
		case 'p':
			g_current_question--;
			break;
		case 'g':
			g_current_question=atoi(input+1);
			break;
		case 'a':
			show_question(QUESTION_ALL,question_buf);
			return;
	}

	if(g_current_question < QUESTION_MIN || g_current_question > QUESTION_MAX)
		g_current_question = current_question_bak;

	show_question(g_current_question,question_buf);
}

void buf_proc(char *buf, int len)
{
	char input[32];
	char *question_buf[QUESTION_MAX];
	char **tmp_buf=question_buf;

	*tmp_buf = strtok(buf,QUESTION_TOK);
	while(*tmp_buf != NULL)
	{
		tmp_buf++;
		*tmp_buf = strtok(NULL,QUESTION_TOK);
	}


	while(1)
	{
		shwo_menu();
		while(scanf("%s",input)<0);

		if(input[0] == 'q')
			break;
		else
			input_proc(input,question_buf);
	}

}

int main(int argc, char **argv)
{
	char *buf;
	int len;
	int ret;

	len = readfilelen();
	buf = (char *)malloc(len);
	if(buf == NULL)
		goto fail;

	ret = readfile(buf,len);
	buf_proc(buf,ret);

	free(buf);

fail:
	
	return 0;
}
