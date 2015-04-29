#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
//�����Լ������ӣ���c��������ͳ���������stl 
#include <vector>

#define SYMBOLHASHTABLESIZE 12 
#define SYMBOLSTRINGTABLESIZE 8192
#define TOKENSIZE 32 
#define WORDINWORDSETSIZE TOKENSIZE
#define getchar() mygetchar()//override ʹ��ȡ���ļ��н��� 
//���ʷ��ű��� 
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
	SymbolInfo* data;//�����붨��Ϊ�ṹ�ģ������Ӻ�������ʱ�����٣�ֻ�ܶ���Ϊָ�� 
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
int getSymbolHashValue();//����ʶ��hash���ֵ������� 
int constant();//constant store in constant table
void error(char * errString, int lineNumber);
void writeSymbolTable();
int getStringLenEndBySpace(char *s);//��Ϊ�����ڷ�����������' '���֣�����ֱ�Ӵ�ӡ��ʹ��ʾ����������㵱ǰ���ŵĳ��ȣ��� writeSymbolTable()�л��õ� 
FILE *enterFilep;
FILE *outFilep;
FILE *errorFilep;
FILE *symbolTableFilep; 
int lineNumber=1;
//stl�Ķ���ĬĬ�ŵ����constant���õ�����Ҫ�������Ǵ泣����
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
	while(!feof(enterFilep)){//while(!feof(enterFilep)) feof()��һ�����ԣ����ļ���ǰָ��λ���ļ�ĩβʱ(�൱��fseek(fp,0,SEEK_END))����ʱ���ִ��һ�ζ��������ļ�������־�ͻᱻ���ã����Ե��ļ�βʱ����һ�� 
		//��ȡʱ���ܻ������ļ�ĩβ����Ϊ���з��Ĵ��ڣ���������Ҫ�ǿմ��� 
		WordSet *p = LexAnalyze();
		if(p != NULL){
//			fprintf(outFilep,"%d, %d\n",p->type,p->location); //����Ϊ�α��е���������ڸ�Ϊʵ���е����
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
	if(character == -1) {//�����ļ�β�Ĵ��� 
		return NULL;
	} 
	WordSet * p = (WordSet *)malloc(sizeof(WordSet));
	memset(token,0,TOKENSIZE);
	if(isalpha(character)){
		while(isalpha(character)||isdigit(character)){
//			strcat(token,&character); ���� character��string����Խ�磬ֱ�Ӳ�������
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
		//�����retract();�ͻᶪʧһ���ַ����˴�retract();��ǰ���getchar()Ӧ�óɶԳ��� 
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

//��;�ļ�����ĩβʱ����Ҫ�д����� ��ĩβ�����-1����getnbc�д��� 
void mygetchar(){
	character = fgetc(enterFilep);
	if(character == '\n') lineNumber++;
}

void getnbc(){
	//�����ļ�β���߷ǿհ��ַ������� 
	if(character==-1||character>0x20) return;
	else {
		getchar();
		getnbc();
	}
}

void retract(){
	//����ʱ���ܻ��������У����뽫�кŸ��˻ز��� 
	//�ļ�����ʱҲ��һ��bug�������е�������������û�л��з��Ļ�������Ҫ���Ƿ����ļ�β�ļ�⣬��Ҳ��cfree���ļ�ĩβû���ؿ�һ�е�warning��ԭ�� 
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

//�����ʶ���ĺ�������token�е��ַ�������ű����鵽���������ڷ��ű��еı�� ��symbolHashTableArray��hash��ֵ�ĵڼ�����������Լ����Ż�����ʾһ����Ԫ�飬����symbolHashTableArray�б�ţ�����ĵڼ�������
//���鲻��������ñ�ʶ�����������ڷ��ű��еı�� 
//���еķ��ű��Կո�' 'Ϊ�ָ�� 
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
	if(ppre == NULL){//pΪNULL ppreҲΪNULL ˵��������һ��Ԫ�ض�û�� 
		p = (SymbolHashNode *)malloc(sizeof(SymbolHashNode));
		symbolHashTableArray [getSymbolHashValue()] = p;
	}else{//ppre��Ϊ�գ�˵��������Ԫ�أ����������һ����û���ҵ� 
		ppre->pnext=(SymbolHashNode *)malloc(sizeof(SymbolHashNode));
		p = ppre->pnext;
	}
	p->pNameHeadInCharacterString=&symbolStringTable[strlen(symbolStringTable)] ;
	strcpy(&symbolStringTable[strlen(symbolStringTable)],token);
	symbolStringTable[strlen(symbolStringTable)]=' ';
	p->data = (SymbolInfo*)malloc(sizeof(SymbolInfo));
	//��Ϣ���ֵ���趨������
	p->pnext = NULL;
	return location; 
}

//����ʶ��hash���ֵ�������,Hash�㷨���£�
//��0-9ӳ��Ϊ0-9��A-Zӳ���10-36��a-zӳ��Ϊ37-63�����ʶ�������ַ���֮�ͣ����ȡģSYMBOLHASHTABLESIZE hash��Ĵ�С 
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
	SymbolHashNode ** pIterator=symbolHashTableArray;//�˴�Ϊ2��ָ�룬ָ�������++����Ҫ��2��ָ����� 
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
