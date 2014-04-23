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
	byte version,yy,mm,dd;
	longint recordsCount;
	integer headerSize,recordSize;
	byte trash[20];
};

struct field
{
	char fieldName[11],fieldType;
	longint address;
	byte fieldSize,fieldDecCnt,trash[14];
};

FILE *dbFile,*readme;
char *dbFileName = new char[255];
bool dbFileAble = false;
header dbHead;
field dbFields[128];
char dbBuffer[4096];
int dbFieldCnt;
char** dbFieldContent;
char tmp;


void changeFont();
void gotoxy(int xpos, int ypos);
void push_back(char *st,char ch);
bool openFile(char *dbFileName);
void getHeader();
void getFields();
void getRecords();
bool editField(int rec, int field, char *value);
void makeNew();
bool deleteRecord(int i);
void printRecord(int i);
int findRecord(char *key);
bool isDeleted(int i);


int main()
{
	system("color 07");
	setlocale(LC_ALL,"");
	changeFont();
	bool leave = false, pleave = false;
	int code = 0, pcode = 0, punkts = 3, p = 0;
	char mmenu[3][30] = {"�i������ DBF ����","��� ��������","���i�"};
	char fmenu[4][40] = {"I�������i� ��� ����", "�������� ������", "������ �����", "�����"};
	char rmenu[4][30] = {"���������� ���� �����", "���������� ���� ����", "�������� �����", "�����"};
	while (!leave)
	{
		do
		{
			cls;
			puts("------------------------------- ����i�� ���i�: --------------------------------");
			for (int i = 0; i < punkts; i++)
			{
				int k = (80 - strlen(mmenu[i])) / 2;
				if (i==p)
				{
					gotoxy(29,8+i*2);
					putchar('>');
				}
				gotoxy(k,8+i*2); puts(mmenu[i]);
			}
			code = _getch();
			if (code == 80)
				p > 1 ? p = 0 : ++p;
			if (code == 72)
				p < 1 ? p = 2 : --p;
		} while (code != 13);
		switch (p) 
			{
			case 0:
				cls;		
				puts("DBF ����� � ������i� ��������i�:");
				system("dir /B /D *.dbf");
				do
				{
					fflush(stdin);
					putchar('\n');
					printf("����i�� ����� ����� >: ");
					gets(dbFileName);
					if (!strstr(dbFileName,".dbf"))
					{
						strcat(dbFileName,".dbf");
						dbFileName[strlen(dbFileName)] = '\0';
					}
					dbFileAble = openFile(dbFileName);
					if (!dbFileAble)
						printf("�� ���� �i������ %s ��������� �����...\n",dbFileName);
				} while (!dbFileAble);
				getRecords();
				
				do
				{
					int fp = 0; pcode = 0;
					do
					{
						cls;
						puts("------------------------------- ����i�� ���i�: --------------------------------");
						for (int i = 0; i < 4; i++)
						{
							int k = (80 - strlen(fmenu[i])) / 2;
							if (i==fp)
							{
								gotoxy(23,8+i*2);
								putchar('>');
							}
							gotoxy(k,8+i*2); puts(fmenu[i]);
						}
						pcode = _getch();
						if (pcode == 80)
							fp > 2 ? fp = 0 : ++fp;
						if (pcode == 72)
							fp < 1 ? fp = 3 : --fp;
							
					} while (pcode!=13);
					switch (fp) 
					{
					case 0:
						cls;
						printf("I�������i� ��� %s\n\n",dbFileName);
						printf("��� ����: %d\n",dbHead.version);
						printf("���������: %d\n",(int)dbHead.trash[17]);
						printf("������ ���������: %d.%d.%d\n",dbHead.dd,dbHead.mm,dbHead.yy);
						printf("������ �����i�: %d\n",dbHead.recordsCount);
						printf("������ ���i�: %d\n",dbFieldCnt);
						printf("����i� ����: %d\n",dbHead.headerSize);
						printf("����i� ������: %d\n\n",dbHead.recordSize);
						wait;
						break;
					case 1:
						{
						cls;
						int cnt = dbHead.recordsCount;
						int i = 0;
						int listcode = 0;
						do 
						{	
							bool fl = false;
							for (int each = 0; each < dbHead.recordsCount; each++)
								if (!isDeleted(each))
									fl = true;

							if (!fl)
							{
								cls;
								puts("� ���i �i�����i ������...\n");
								wait;
								break;
							}
							if (isDeleted(i))
							{
								while (isDeleted(i))
									i >= dbHead.recordsCount - 1 ? i = 0 : i++;
								continue;
							}
							cls;
							gotoxy(30,0); printf("�������� �����: #%d of %d\n",i+1,dbHead.recordsCount);
							for (int j = 0; j < 80; j++)
								putchar('-');
							gotoxy(10,2); printf("%c �������� ��������� �����",char(26));
							gotoxy(45,2); printf("%c �������� ��������i� �����",char(27));
							gotoxy(10,3); printf("ESC ����������� �� ����");
							gotoxy(45,3); printf("� ���������� �����\n");								
							for (int j = 0; j < 80; j++)
								putchar('-');
							putchar('\n');
							printRecord(i);
							listcode = _getch();
							if (listcode == 77 || listcode == 80)
								i>=dbHead.recordsCount-1 ? i = 0 : ++i;
							while (isDeleted(i))
								i >= dbHead.recordsCount - 1 ? i = 0 : i++;
							if (listcode == 75 || listcode == 72)
								i<=0 ? i = dbHead.recordsCount-1 : --i;
							while (isDeleted(i))
								i == 0 ? i = dbHead.recordsCount-1 : --i;
							if (strchr("Ee",char(listcode)))
							{
								int rp = 0; int rcode = 0;
								
								do
								{
									cls;
									puts("------------------------------- ����i�� ���i�: --------------------------------");
									for (int i = 0; i < 4; i++)
									{
										int k = (80 - strlen(rmenu[i])) / 2;
										if (i==rp)
										{
											gotoxy(23,8+i*2);
											putchar('>');
										}
										gotoxy(k,8+i*2); puts(rmenu[i]);
									}
									rcode = _getch();
									if (rcode == 80)
										rp > 2 ? rp = 0 : ++rp;
									if (rcode == 72)
										rp < 1 ? rp = 3 : --rp;
							
									} while (rcode!=13);
								
								if (rp == 1)
								{
									cls;
									puts("�����i�� ���� ��� �����������");
									for (int j = 0; j < dbFieldCnt; j++)
										printf("\n%d: %s",j,dbFields[j].fieldName);
									printf("\n\n����i�� ����� ���� >: ");
									int num;
									scanf("%d",&num);
									cin.ignore();
									bool success = false;
									do 
									{
										printf("\n���� �������� ��� %s >: ",dbFields[num].fieldName);
										char *temp = new char[255];
										gets(temp);
										success = editField(i,num,temp);
										if (!success)
											puts("���i����i��i��� ���i�, ��������� �����");
										delete(temp);
									} while (!success);
									puts("\n���i���...\n");
									wait;
								}
								if (rp == 0)
								{
									cls;
								//	cin.ignore();
									for (int j = 0; j < dbFieldCnt; j++)
									{
										bool success = false;
										do
										{
											printf("\n���� �������� ��� %s >: ",dbFields[j].fieldName);
											char *temp = new char[255];
											gets(temp);
											success = editField(i,j,temp);
											delete(temp);
										} while (!success);
									}
									puts("\n���i���...\n");
									wait;
								}
								if (rp==2)
								{
									cls;
									deleteRecord(i) ? puts("����� ���������� �� ���������") : puts("����� ��� ���������");
									cnt--;
									putchar('\n');
									wait;
								}
								if (rp==3)
									continue;
							
							}
								
						} while (listcode!=27);
						char savecode = 0;
						do
						{
							cls;
							if (dbHead.trash[17]==201)
								break;
							printf("����������� ��i��? (y/n) >: ");
							scanf("%c",&savecode);
							if (strchr("Yy",savecode))
							{
								makeNew();
								puts("\n���i���... ������ ����������!\n");
								wait;
							}
							if (strchr("Nn",savecode))
							{
								puts("\n������ ����������!\n");
								wait;
							}
						} while (!strchr("YyNn",savecode));
						break;
						}
					case 3:
						pleave = true;
						break;
					case 2:
						cls;
						char *key = new char[255];
						fflush(stdin);
						printf("����i�� ���� :> ");
						gets(key);
						int num = findRecord(key);
						if (num == -1)
							printf("%s �i�����i�...\n\n",key);
						else
						{
							for (int q = 0; q < dbFieldCnt; q++)
							{
								if (dbFields[q].fieldType == 'M')
									continue;
								gotoxy(2,q+2); puts(dbFields[q].fieldName);
								if (q==0) gotoxy(23,q+2);
								else gotoxy(23,q+2); 
								puts(dbFieldContent[num*dbFieldCnt+q]);
							}
							putchar('\n');
						}	
						wait;
						break;
					}
				} while (!pleave);
				break;
		
			case 1:
				cls;
				char ch;
				readme = fopen("readme.txt","r");
				if (!readme)
				{
					cls;
					puts("�� ���� �i������ readme.txt\n");
					wait;
					continue;
				}
				while (!feof(readme))
				{
					ch = fgetc(readme);
					putchar(ch);
				}
				putchar('\n');
				fclose(readme);
				wait;
				break;
			case 2:
				leave = true;
				break;
			}
	} 
}

void changeFont()
{
	typedef BOOL (WINAPI *SETCONSOLEFONT)(HANDLE, DWORD);     
	SETCONSOLEFONT SetConsoleFont;
	HMODULE hmod = GetModuleHandleA("KERNEL32.DLL");    
	SetConsoleFont =(SETCONSOLEFONT) GetProcAddress(hmod, "SetConsoleFont");  
	if (!SetConsoleFont) exit(1);
	SetConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE),6); 
	SetConsoleOutputCP(1251) ; 
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
	dbFieldContent = new char* [dbHead.recordsCount*dbFieldCnt];
	for (int i = 0; i < dbHead.recordsCount*dbFieldCnt; i++)
		dbFieldContent[i] = new char[dbHead.recordSize];
}

void getRecords()
{

	getFields();
	for (int i = 0; i < dbHead.recordsCount*dbFieldCnt; i++)
		dbFieldContent[i][0] = '\0';
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

bool editField(int rec, int field, char *value)
{
	if (dbFields[field].fieldType == 'N')
	{
		for (int j = 0; j < strlen(value); j++)
			if (!isdigit(value[j])&&!strchr("-.,",value[j]))
				return false;
	}

	if (dbFields[field].fieldType == 'L')
		if (!strchr("YyNnTtFf?",value[0]))
			return false;
		
	field == 0 ? dbFieldContent[rec*dbFieldCnt][1]='\0' : dbFieldContent[rec*dbFieldCnt+field][0]='\0';

	strcat(dbFieldContent[rec*dbFieldCnt+field],value);
	if (dbFields[field].fieldSize > strlen(value))
		for (int i = strlen(value); i < dbFields[field].fieldSize; i++)
			push_back(dbFieldContent[rec*dbFieldCnt+field],' ');
	return true;
}

void makeNew()
{
	fclose(dbFile);
	FILE *tmp = fopen(dbFileName,"wb");
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
	}
	fclose(tmp);
}

bool deleteRecord(int i)
{
	char del = dbFieldContent[i*dbFieldCnt][0];
	if (del=='*')
		return false;
	dbFieldContent[i*dbFieldCnt][0] = '*';
	return true;
}

void printRecord(int i)
{
	for (int j = 0; j < dbFieldCnt; j++)
	{
		if (dbFields[j].fieldType == 'M')
			continue;
		
		gotoxy(23,j+6); puts(dbFields[j].fieldName);
		if (j==0) gotoxy(44,j+6);
		else gotoxy(45,j+6); 
		puts(dbFieldContent[i*dbFieldCnt+j]);
	} 
}
int findRecord(char *key)
{
	for (int i = 0; i < dbHead.recordsCount; i++)
		for (int j = 0; j < dbFieldCnt; j++)
			if (strstr(dbFieldContent[i*dbFieldCnt+j],key))
				return i;
	return -1;
}

bool isDeleted(int i)
{
	return dbFieldContent[i*dbFieldCnt][0] == '*';
}
