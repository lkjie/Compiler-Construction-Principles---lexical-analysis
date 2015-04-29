#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
//懒得自己造轮子，纯c代码里面就常数表用了stl 
#include <vector>

#define SYMBOLHASHTABLESIZE 12 
#define SYMBOLSTRINGTABLESIZE 8192
#define TOKENSIZE 32 
#define WORDINWORDSETSIZE TOKENSIZE
#define getchar() mygetchar()//override 使读取在文件中进行 
//单词符号编码 
#define BEGIN 1
#define END 2
#define INTEGER 3
#define IF 4
#define THEN 5
#define ELSE 6
#define FUNCTION 7
#define READ 8
#define WRITE 9
#define SYMBOL 10
#define CONSTANT 11
#define E 12
#define NE 13
#define LE 14
#define L 15
#define GE 16
#define G 17
#define SUB 18
#define MUL 19
#define ASSIGN 20
#define LPAR 21
#define RPAR 22
#define SEM 23

char character;
char token[TOKENSIZE]={
	0
};

typedef struct wordanalyzset{
	int type;
	int location;
	char word[WORDINWORDSETSIZE];
}WordSet;

//Symbol table define
typedef struct symbolInfo{
	int a;
}SymbolInfo;

//hash table is exist, i do not implement the free MEM handle
typedef struct symbolhashnode{
	char * pNameHeadInCharacterString;
	SymbolInfo* data;//本来想定义为结构的，但是子函数分配时会销毁，只能定义为指针 
	struct symbolhashnode *pnext;
}SymbolHashNode;
char symbolStringTable[SYMBOLSTRINGTABLESIZE]={
	0
};//Symbol Hash table string
SymbolHashNode *symbolHashTableArray[SYMBOLHASHTABLESIZE]={
	0
};
//symboe hash table handle:search 

WordSet * LexAnalyze();
void mygetchar();
void getnbc();
void retract();
int reserve();
int symbol();//search token in symbol table
int getSymbolHashValue();//将标识符hash后的值计算出来 
int constant();//constant store in constant table
void error(char * errString, int lineNumber);
void writeSymbolTable();
int getStringLenEndBySpace(char *s);//因为符号在符号数组中以' '区分，所以直接打印会使显示有误，这里计算当前符号的长度，在 writeSymbolTable()中会用到 
FILE *enterFilep;
FILE *outFilep;
FILE *errorFilep;
FILE *symbolTableFilep; 
int lineNumber=1;
//stl的东西默默放到最后，constant里用到，主要的作用是存常数表
std::vector<int> constantTable;
 
int main(int argc, char *argv[])
{
	char filestring[512];
	//open file
	printf("enter the source file path:\n");
//	scanf("%s",filestring);
	if((enterFilep = fopen("D:\\in.txt","rt")) == NULL){
		printf("open source file error!\n");
		return 0;
	}
	printf("enter the output file path:\n");
//	scanf("%s",filestring);
	if((outFilep = fopen("D:\\out.dyd","wt")) == NULL){
		printf("open source file error!\n");
		return 0;
	}
	printf("enter the error file path:\n");
//	scanf("%s",filestring);
	if((errorFilep = fopen("D:\\error.err","wt")) == NULL){
		printf("open source file error!\n");
		return 0;
	}
	printf("enter the symbolTable file path:\n");
//	scanf("%s",filestring);
	if((symbolTableFilep = fopen("D:\\symbolTable.txt","wt")) == NULL){
		printf("open source file error!\n");
		return 0;
	}
	while(!feof(enterFilep)){//while(!feof(enterFilep)) feof()有一个特性：当文件当前指针位于文件末尾时(相当于fseek(fp,0,SEEK_END))，此时如果执行一次读操作，文件结束标志就会被设置，所以到文件尾时会多读一次 
		//读取时可能会遇到文件末尾（因为换行符的存在），所以需要非空处理； 
		WordSet *p = LexAnalyze();
		if(p != NULL){
//			fprintf(outFilep,"%d, %d\n",p->type,p->location); //此行为课本中的输出，现在改为实验中的输出
			fprintf(outFilep,"%16s %2d                EOLN %d\n",p->word,p->type,lineNumber);
						if(lineNumber == 71){
							int i =1;
						}
			free(p); 
		}
	}
	writeSymbolTable();
	printf("Hello C-Free!\n");
	return 0;
}

WordSet * LexAnalyze(){
	getchar();
	getnbc();
	if(character == -1) {//读到文件尾的处理 
		return NULL;
	} 
	WordSet * p = (WordSet *)malloc(sizeof(WordSet));
	memset(token,0,TOKENSIZE);
	if(isalpha(character)){
		while(isalpha(character)||isdigit(character)){
//			strcat(token,&character); 警告 character当string可能越界，直接操作即可
			token[strlen(token)] = character;
			getchar();
		}
		retract();
		strcpy(p->word,token);
		if(reserve() != 0){
			p->type = reserve();
			p->location = -1;
			return p;
		}else{
			p->type = SYMBOL;
			p->location = symbol();
			return p;
		}
	}else if(isdigit(character)){
		while(isdigit(character)){
			token[strlen(token)] = character;
			getchar();
		}
		retract();
		strcpy(p->word,token);
		p->type = CONSTANT;
		p->location = constant();
		return p;
	}else if(character=='<'){
		getchar();
		if(character=='>'){
			strcpy(p->word,"<>");
			p->type = NE;
			p->location = -1;
			return p;
		}else if(character=='='){
			strcpy(p->word,"<=");
			p->type = LE;
			p->location = -1;
			return p;
		}else{
			retract();
			strcpy(p->word,"<");
			p->type = L;
			p->location = -1;
			return p;
		}
	}else if(character=='>'){
		getchar();
		if(character=='='){
			strcpy(p->word,">=");
			p->type = GE;
			p->location = -1;
			return p;
		}else{
			retract();
			strcpy(p->word,">");
			p->type = G;
			p->location = -1;
			return p;
		}
	}else if(character=='='){
		getchar();
		if(character=='='){
			strcpy(p->word,"==");
			p->type = E;
			p->location = -1;
			return p;
		}else{
		//如果无retract();就会丢失一个字符，此处retract();与前面的getchar()应该成对出现 
			retract();
			error("illegal operator '=' here.",lineNumber);
		}
	}else if(character=='-'){
		strcpy(p->word,"-");
		p->type = SUB;
		p->location = -1;
		return p;
	}else if(character=='*'){
		strcpy(p->word,"*");
		p->type = MUL;
		p->location = -1;
		return p;
	}else if(character==':'){
		getchar();
		if(character=='='){
			strcpy(p->word,":=");
			p->type = ASSIGN;
			p->location = -1;
			return p;
		}else{
			retract();
			error("illegal operator ':' here.",lineNumber);
		}
	}else if(character=='('){
		strcpy(p->word,"(");
		p->type = LPAR;
		p->location = -1;
		return p;
	}else if(character==')'){
		strcpy(p->word,")");
		p->type = RPAR;
		p->location = -1;
		return p;
	}else if(character==';'){
		strcpy(p->word,";");
		p->type = SEM;
		p->location = -1;
		return p;
	}else{
		error("illegal character here.",lineNumber);
	}
	free(p);
	return NULL;
}

//中途文件读到末尾时必须要有错误处理 ，末尾会读入-1，在getnbc中处理 
void mygetchar(){
	character = fgetc(enterFilep);
	if(character == '\n') lineNumber++;
}

void getnbc(){
	//读到文件尾或者非空白字符都返回 
	if(character==-1||character>0x20) return;
	else {
		getchar();
		getnbc();
	}
}

void retract(){
	//回退时可能会遇到换行，必须将行号给退回才行 
	//文件结束时也是一个bug，会运行到里面来如果最后没有换行符的话，必须要做是否在文件尾的检测，这也是cfree报文件末尾没有重开一行的warning的原因 
	if(character == '\n') lineNumber--;
	if(feof(enterFilep))return;
	fseek(enterFilep,-1L,SEEK_CUR);
}

int reserve(){
	if(strcmp(token,"begin") == 0){
		return BEGIN;
	}else if(strcmp(token,"end") == 0){
		return END;
	}else if(strcmp(token,"integer") == 0){
		return INTEGER;
	}else if(strcmp(token,"if") == 0){
		return IF;
	}else if(strcmp(token,"then") == 0){
		return THEN;
	}else if(strcmp(token,"else") == 0){
		return ELSE;
	}else if(strcmp(token,"function") == 0){
		return FUNCTION;
	}else if(strcmp(token,"read") == 0){
		return READ;
	}else if(strcmp(token,"write") == 0){
		return WRITE;
	}else return 0;
}

//处理标识符的函数，对token中的字符串查符号表，若查到，返回它在符号表中的编号 （symbolHashTableArray中hash后值的第几个，这里可以继续优化，显示一个二元组，（在symbolHashTableArray中编号，后面的第几个））
//若查不到，存入该标识符，并返回在符号表中的编号 
//单列的符号表以空格' '为分割符 
int symbol(){
	int location = 0; 
	SymbolHashNode * p=symbolHashTableArray [getSymbolHashValue()];
	SymbolHashNode * ppre = p;
	while(p!=NULL){
		if(strncmp(p->pNameHeadInCharacterString,token,strlen(token))==0){
			return location;
		}
		ppre = p;
		p=p->pnext;
		location++;
	}
	if(ppre == NULL){//p为NULL ppre也为NULL 说明数组上一个元素都没有 
		p = (SymbolHashNode *)malloc(sizeof(SymbolHashNode));
		symbolHashTableArray [getSymbolHashValue()] = p;
	}else{//ppre不为空，说明数组有元素，搜索到最后一个仍没有找到 
		ppre->pnext=(SymbolHashNode *)malloc(sizeof(SymbolHashNode));
		p = ppre->pnext;
	}
	p->pNameHeadInCharacterString=&symbolStringTable[strlen(symbolStringTable)] ;
	strcpy(&symbolStringTable[strlen(symbolStringTable)],token);
	symbolStringTable[strlen(symbolStringTable)]=' ';
	p->data = (SymbolInfo*)malloc(sizeof(SymbolInfo));
	//信息域的值的设定，暂无
	p->pnext = NULL;
	return location; 
}

//将标识符hash后的值计算出来,Hash算法如下：
//将0-9映射为0-9，A-Z映射成10-36，a-z映射为37-63，求标识符各个字符串之和，相加取模SYMBOLHASHTABLESIZE hash表的大小 
int getSymbolHashValue(){
	int i=0;
	int sum=0;
	for	(;i<strlen(token);i++){
		if(token[i]>='0'&&token[i]<='9'){
			sum+=token[i]-'0';
		}else if(token[i]>='A'&&token[i]<='Z'){
			sum+=token[i]-'A'+10;
		}else if(token[i]>='a'&&token[i]<='z'){
			sum+=token[i]-'a'+37;
		}
	}
	return sum%SYMBOLHASHTABLESIZE;
} 

//
int constant(){
	int i = atoi(token);
	constantTable.push_back(i);
	return constantTable.size()-1;
}

void error(char * errString, int lineNumber){
	fprintf(errorFilep,"***LINE:%d  %s\n",lineNumber,errString);
}

void writeSymbolTable(){
	if((symbolTableFilep=fopen("D:\\symbolTable.txt","wt"))==NULL){
		printf("write to symbolTable file error!\n");
		return;
	}
	SymbolHashNode ** pIterator=symbolHashTableArray;//此处为2重指针，指针数组的++操作要用2重指针完成 
	SymbolHashNode *pIn;
	for(int i = 0;i<SYMBOLHASHTABLESIZE;i++,pIterator++){
		pIn = *pIterator;
		while(pIn !=NULL){
			char tmpStr[128]={
				0
			};
			fprintf(symbolTableFilep,"%s,%d;",strncpy(tmpStr,pIn->pNameHeadInCharacterString,
			getStringLenEndBySpace(pIn->pNameHeadInCharacterString)),pIn->data->a);
			pIn = pIn->pnext;
		}
		fprintf(symbolTableFilep,"\n");
	}
}

int getStringLenEndBySpace(char *s){
	int len=0;
	while(*s != ' ') s++,len++;
	return len; 
}
