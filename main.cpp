#include <cstdio>
#include <fstream>
#include <iostream>
#include <conio.h>
#include <Windows.h>


using namespace std;

#define cls system("cls")
#define wait system("pause")

typedef unsigned __int8 byte;
typedef __int32 longint;
typedef __int16 integer;

struct header
{
	byte version;
	byte yy,mm,dd;
	longint recordsCount;
	integer headerSize;
	integer recordSize;
	byte trash[20];
};

struct field
{
	char fieldName[11];
	char fieldType;
	longint address;
	byte fieldSize;
	byte fieldDecCnt;
	byte trash[14];
};

FILE *dbFile,*readme;
char *dbFileName;
bool dbFileAble = false;
header dbHead;
field dbFields[128];
char dbBuffer[4096];
int dbFieldCnt;
char dbFieldContent[2001][4096];
char tmp;


void gotoxy(int xpos, int ypos);
void push_back(char *st,char ch);
bool openFile(char *dbFileName);
void getHeader();
void getFields();
void getRecords();
void editField(int rec, int field, char *value);
void makeNew();
bool deleteRecord(int i);



int main()
{
	int code = 0;
	int pcode = 0;
	do
	{
		cls;
		puts("Good afternoon sir, please make a choise: \n");
		puts("1: Open DBF file");
		puts("2: About program\n");
		puts("ESC: Exit");
		code = _getch();
		switch (code) 
		{
		case 49:
			do
			{
				cls;
				dbFileName = "travel.dbf";
				dbFileAble = openFile(dbFileName);

			} while (!dbFileAble);
			getHeader();
			getFields();
			getRecords();
			do
			{
				cls;
				puts("Make a choise: \n");
				puts("1: Show information about this database");
				puts("2: Show database\n");
				puts("ESC: Exit");
				pcode = _getch();
				switch (pcode) 
				{
				case 49:
					cls;
					printf("Information about %s\n\n",dbFileName);
					printf("Last update: %d.%d.%d\n",dbHead.dd,dbHead.mm,dbHead.yy);
					printf("Total records: %d\n",dbHead.recordsCount);
					printf("Total fields: %d\n",dbFieldCnt);
					printf("Size of header: %d\n",dbHead.headerSize);
					printf("Size of record: %d\n\n",dbHead.recordSize);
					wait;
					break;
				case 50:
					cls;
					for (int i = 0; i < dbHead.recordsCount; i++)
					{
						cls;
						for (int j = 0; j < dbFieldCnt; j++)
							printf("%10s     %20s\n",dbFields[j].fieldName,dbFieldContent[i*dbFieldCnt+j]);
						wait;
					}
					wait;
					break;
				}
			} while (pcode != 27);
			break;
		
		case 50:
			cls;
			char ch;
			readme = fopen("readme.txt","r");
			while (!feof(readme))
			{
				ch = fgetc(readme);
				putchar(ch);
			}
			putchar('\n');
			fclose(readme);
			wait;
			break;
		}
	} while (code != 27);
	
}


void gotoxy(int xpos, int ypos)
{
  COORD scrn;    
  HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
  scrn.X = xpos; scrn.Y = ypos;
  SetConsoleCursorPosition(hOuput,scrn);
}

void push_back(char *st,char ch)
{
	int len = strlen(st);
	st[len] = ch;
	st[len+1] = '\0';
}

bool openFile(char *dbFileName)
{
	dbFile = fopen(dbFileName,"rb");
	return dbFile;
}

void getHeader()
{
	fseek(dbFile,0,SEEK_SET);
	fread(&dbHead,32,1,dbFile);
}

void getFields()
{
	getHeader();
	dbFieldCnt = (dbHead.headerSize-33) / 32;
	fseek(dbFile,32,SEEK_SET);
	for (int i = 0; i < dbFieldCnt; i++)
		fread(&dbFields[i],32,1,dbFile);
	
	fread(&tmp,1,1,dbFile);
}

void getRecords()
{
	getFields();
	fseek(dbFile,dbHead.headerSize,SEEK_SET);
	for (int i = 0; i < dbHead.recordsCount; i++)
	{
		fread(dbBuffer,dbHead.recordSize,1,dbFile);
		int contentStart = 1;
		push_back(dbFieldContent[i*dbFieldCnt],dbBuffer[0]);
		for (int j = 0; j < dbFieldCnt; j++)
		{
			for (int k = contentStart; k < (int)dbFields[j].fieldSize+contentStart; k++)
				push_back(dbFieldContent[i*dbFieldCnt+j],dbBuffer[k]);
			contentStart += dbFields[j].fieldSize;
		}
	}
}

void editField(int rec, int field, char *value)
{
	if (field == 0)
		dbFieldContent[rec*dbFieldCnt][1]='\0';
	else
		dbFieldContent[rec*dbFieldCnt+field][0]='\0';
	strcat(dbFieldContent[rec*dbFieldCnt+field],value);
	if (dbFields[field].fieldSize > strlen(value))
		for (int i = strlen(value); i < dbFields[field].fieldSize; i++)
			push_back(dbFieldContent[rec*dbFieldCnt+field],' ');
	//cout << (int)dbFields[0].fieldSize << " " << strlen(dbFieldContent[rec*dbFieldCnt+field]) << endl;
}

void makeNew()
{
	FILE *tmp = fopen("tmp.dbf","wb");
	fwrite(&dbHead,32,1,tmp);
	for (int i = 0; i < dbFieldCnt; i++)
		fwrite(&dbFields[i],32,1,tmp);
	char end = 13;
	fwrite(&end,sizeof(char),1,tmp);
	char *temp = new char[dbHead.recordSize+1];
	for (int i = 0; i < dbHead.recordsCount; i++)
	{
		temp[0] = '\0';
		for (int j = 0; j < dbFieldCnt; j++)
		{
			strcat(temp,dbFieldContent[i*dbFieldCnt+j]);
			temp[strlen(temp)+1] = '\0';
		}
		fwrite(temp,dbHead.recordSize,1,tmp);
		//cout << temp << endl;
		
	}
}

bool deleteRecord(int i)
{
	char del = dbFieldContent[i*dbFieldCnt][0];
	if (del=='*')
		return false;
	dbFieldContent[i*dbFieldCnt][0] = '*';
	return true;
}

