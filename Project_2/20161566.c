#include "20161566.h"

int main() {
	int temp_opcode;	// opcode.txt의 opcode를 임시저장하는 변수
	char temp_key[MAX_OPCODE_SIZE];	// opcode.txt의 mnemonic을 임시저장하는 변수														
	char temp_format[MAX_FORMAT_SIZE];	// opcode.txt의 Format을 임시저장하는 변수												

	input = (char*)malloc(sizeof(char) * MAX_INPUT_SIZE);	// 모든 input을 통째로 저장하는 변수						
	memory = (unsigned char*)calloc(MAX_MEMORY_SIZE,sizeof(unsigned char));	// 메모리 공간 확보	

	his_head= (history_node*)malloc(sizeof(history_node));	//history의 머리를 생성한다
	his_head->next = NULL;	
	his_last=his_head;	//history의 꼬리가 첫 원소를 가리키게 한다.															

	symbol_head = (symbol_node*)malloc(sizeof(symbol_node));	//Symbol의 머리를 생성한다.
	symbol_head->next = NULL;

	address=-1;	//dump에서 사용될 주소를 -1로 저장한다.																	

	FILE* fp = fopen("opcode.txt", "r");	//파일을 읽기 형식으로 open, 실패시 오류 메세지											
	if (fp == NULL) {															
		printf("Failed to open 'opcode.txt'\n");
		return 0;
	}	

	while (0 < fscanf(fp, "%X %s %s", &temp_opcode, temp_key, temp_format)) {	//opcode.txt안의 값들을 한줄씩 읽는다.
		int hash = Get_Hash(temp_key);											
		Add_to_hash_table(hash, temp_opcode, temp_key, temp_format);			
	}
	fclose(fp);													

	while (1) {	//quit이 입력되기 전까지 반복 실행한다.																
		printf("sicsim> ");
		if(get_input()==1){	//get_input함수의 반환값이 1이라면 추가 실행													
			value_num=0;	//value_num의 값을 0으로 초기화한다.														
			memset(value,0,sizeof(value));	//value배열의 값들을 0으로 초기화한다.										
			Command();	//명령어에 따라 수행한다,
		}
		input = calloc(MAX_INPUT_SIZE, sizeof(char));	//input값을 초기화한다.						
	}
}

void Command() {	//Command함수																
	int a = Check_Command();	//Check_Command함수를 호출하여, 명령어를 확인한다.													
	if (a != 0) {
		switch (a) {	//a가 1~10까지 알맞은 함수를 호출
		case 1:
			Help();
			break;
		case 2:
			Dir();
			break;
		case 3:
			Quit();
			break;
		case 4:
			History();
			break;
		case 5:
			Dump();
			break;
		case 6:
			Edit();
			break;
		case 7:
			Fill();
			break;
		case 8:
			Reset();
			break;
		case 9:
			Find_opcode();
			break;
		case 10:
			Print_opcodelist();
			break;
		case 11:
			Type();
			break;
		case 12:
			Assemble();
			break;
		case 13:
			Symbol();
			break;
		}
	}
}

int Check_Command() {	//Check_Command함수, 명령어에 따라서 유효함을 판단한다.																				
	int size = strlen(input);																	
	memset(temp_value,0,sizeof(temp_value));													
	temp_value_num = 0;
	int i,j;
	//input을 공백을 기준으로 temp_value들로 나눈다.
	for (i = 0; i < size; i++) {																
		if (input[i] == ' ' || input[i] == '\t')												
			continue;
		for (j = i;; j++) {																		
			if (input[j] == ' ' || input[j] == '\t' || input[j] == '\0' || j == size)			
				break;
			if ((j - i) == MAX_INPUT_SIZE || temp_value_num > 3) {								
				printf("ERROR: Input is Too Long\n");
				return 0;
			}
			temp_value[temp_value_num][j - i] = input[j];										
			temp_value[temp_value_num][j - i + 1] = '\0';										
		}
		i = j;																					
		temp_value_num++;																		
	}

	for(i=1;i< temp_value_num -1;i++){	//dump, fill등 input 중 ','가 들어가는 input을 처리하기 위한 과정													
		size=strlen(temp_value[i]);						
		if(temp_value[i][size-1]==',')																
			temp_value[i][size-1]='\0';					
		else{														
			printf("ERROR: ','Needed in the right place\n");									
			return 0;
		}
	}

	//명령어로 인식되면, 인자의 수 등 명령어의 조건을 만족하는지 확인한다.

	if (strcmp(temp_value[0], "help") == 0 || strcmp(temp_value[0], "h") == 0) {	//명령어가 help라고 인식된 경우				
		if (temp_value_num == 1)																
			return 1;
		else {																					
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "dir") == 0 || strcmp(temp_value[0], "d") == 0) {	//명령어가 dir로 인식된 경우		
		if (temp_value_num == 1)																
			return 2;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "quit") == 0 || strcmp(temp_value[0], "q") == 0) {	//명령어가 quit이라고 인식된 경우			
		if (temp_value_num == 1)																
			return 3;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}					
	else if (strcmp(temp_value[0], "history") == 0 || strcmp(temp_value[0], "hi") == 0) {	//명령어가 history라고 인식된 경우		
		if (temp_value_num == 1)																
			return 4;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "dump") == 0 || strcmp(temp_value[0], "du") == 0) {	//명령어가 dump라고 인식된 경우			
		if (temp_value_num == 1)																
			return 5;
		else if (temp_value_num == 2) {															
			if (Check_Address(temp_value[1]) == 1) {											
				strcpy(value[0], temp_value[1]);												
				value_num = 1;																	
				return 5;																		
			}
			else
				return 0;																		
		}
		else if (temp_value_num == 3) {															
			if (Check_Address(temp_value[1]) == 1 && Check_Address(temp_value[2]) == 1) {		
				strcpy(value[0], temp_value[1]);												
				strcpy(value[1], temp_value[2]);
				value_num = 2;
				return 5;
			}
			else																				
				return 0;
		}
		else {																					
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "edit") == 0 || strcmp(temp_value[0], "e") == 0) {	//명령어가 edit이라고 인식된 경우			
		if (temp_value_num != 3) {																
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}	
		else {																					
			if (Check_Address(temp_value[1]) == 1 && Check_Value(temp_value[2]) == 1) {			
				strcpy(value[0], temp_value[1]);												
				strcpy(value[1], temp_value[2]);
				value_num = 2;
				return 6;
			}
			else																				
				return 0;	
		}
	}
	else if (strcmp(temp_value[0], "fill") == 0 || strcmp(temp_value[0], "f") == 0) {	//명령어가 fill이라고 인식된 경우			
		if (temp_value_num != 4) {																
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
		else {
			if ((Check_Address(temp_value[1]) == 1 && Check_Address(temp_value[2]) == 1) && Check_Value(temp_value[3]) == 1){	
				strcpy(value[0], temp_value[1]);																				
				strcpy(value[1], temp_value[2]);
				strcpy(value[2], temp_value[3]);
				value_num = 3;
				return 7;
			}		
			else																												
				return 0;
		}
	}
	else if (strcmp(temp_value[0], "reset") == 0) {	//명령어가 reset이라고 인식된 경우											
		if (temp_value_num == 1)																
			return 8;
		else{
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "opcode") == 0) {	//명령어가 opcode라고 인식된 경우											
		if(temp_value_num !=2){																	
			printf("ERROR: Right command Wrong use\n");	
			return 0;
		}
		else{																					
			size=strlen(temp_value[1]);	
			if(size>6){																									
				printf("ERROR: No such mnemonic\n");
				return 0;
			}	
			for(i=0;i<size;i++){																
				if('A'<= temp_value[1][i] && temp_value[1][i]<='Z')
					continue;
				else{
					printf("ERROR: No such mnemonic\n");
					return 0;
				}
			}
			strcpy(value[0], temp_value[1]);													
			value_num=1;
			return 9;
		}
	}
	else if (strcmp(temp_value[0], "opcodelist") == 0) {	//명령어가 opcodelist라고 인식된 경우										
		if(temp_value_num ==1)																	
			return 10;
		else{
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}																							
	else if(strcmp(temp_value[0], "type")==0){	//명령어가 type이라고 인식된 경우
		if(temp_value_num !=2){
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
		else{
			strcpy(value[0],temp_value[1]);
			return 11;
		}
	}
	else if(strcmp(temp_value[0], "assemble")==0){	//명령어가 assemble이라고 인식된 경우
		if(temp_value_num !=2){
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
		else{
			strcpy(value[0],temp_value[1]);
			return 12;
		}
	}
	else if (strcmp(temp_value[0], "symbol") == 0) {	//명령어가 symbol이라고 인식된 경우
		if (temp_value_num != 1) {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
		else
			return 13;
	}
	else {	//해당하는 명령어가 존재하지 않는다.
		printf("ERROR: Wrong Input\n");
		return 0;
	}
}

int get_input() {	//input을 처음 저장하는 함수																				
	int i = 0;
	while (1) {
		char temp = getchar();	//temp에 character하나씩 입력받는다.																	
		if (i >= MAX_INPUT_SIZE) {																
			printf("ERROR: Input Size Too Much\n");
			return 0;
		}

		if (temp == '\n') {	//개행 문자를 입력받으면 그 자리에'\0'를 저장하고 1을 반환한다.																	
			input[i] = '\0';
			return 1;
		}
		else {																					
			input[i++] = temp;
		}
	}
}

void FreeMemory() {	//quit함수에서 사용된다.																				
	free(input);	//동적으로 할당된 input, memory, hash_table, history_linkedlist등을 free한다.																				
	free(memory);
	history_node *temp=NULL;
	hash_node * temp2=NULL;
	symbol_node* temp3 = NULL;

	while(his_head !=NULL){																		
		temp=his_head;
		his_head=his_head->next;
		free(temp);
		temp=NULL;
	}

	while (symbol_head != NULL) {
		temp3 = symbol_head;
		symbol_head = symbol_head->next;
		free(temp3);
		temp3 = NULL;
	}

	for(int i=0;i<MAX_TABLE;i++){																
		if(hash_table[i]==NULL)
			continue;
		else{
			while(hash_table[i]!=NULL){
				temp2=hash_table[i];
				hash_table[i]=hash_table[i]->next;
				free(temp2);
				temp2=NULL;
			}
		}
	}
}

void Help() {	//Help명령어 실행시 출력해야되는 명령어의 모음																					
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp][start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	printf("assemble filename\ntype filename\nsymbol\n");
	Add_History();
}

void Quit() {	//Quit명령어 실행시 동적 할당 된 메모리를 free하고 프로그램을 종료한다.																					
	FreeMemory();
	exit(0);
}

void Dir() {																					//Dir명령어 실행
	DIR* dir = NULL;
	struct dirent* ent = NULL;
	struct stat temp;
	dir = opendir(".");																			//현재 디렉토리를 연다
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			lstat(ent->d_name, &temp);
			if (S_ISDIR(temp.st_mode))															//디렉토리 파일은 뒤에 '/'를 붙인다.
				printf("\t%s/", ent->d_name);
			else if (S_ISREG(temp.st_mode)) {
				if (0x0040 & temp.st_mode)														//실행 파일은 뒤에 '/'을 붙인다.
					printf("\t%s*", ent->d_name);
				else																			//그외의 파일은 그대로 출력한다.
					printf("\t%s", ent->d_name);
			}
		}
		printf("\n");
		closedir(dir);																			//현재 디렉토리를 닫는다.
	}
	else {																						//디렉토리 여는 것을 실패하면 오류를 출력한다.
		printf("ERROR: The Directory can not be opened\n");
	}
	Add_History();
}


void History(){	//History명령어 실행	
	Add_History();
	history_node* curr;	
	int i=1;
	curr =his_head->next;																		
	while(curr !=NULL){	//head->next부터 NULL까지 name을 출력한다.
		printf("%d\t%s\n",i,curr->name);
		i++;
		curr=curr->next;
	}
}

void Add_History(){	//History에 새로운 노드를 추가한다.																			
	history_node *new;
	new = (history_node*)malloc(sizeof(history_node));
	char temp_input[MAX_INPUT_SIZE];															
	memset(temp_input,0,sizeof(temp_input));
	for(int i=0;i< temp_value_num;i++){															
		strcat(temp_input, temp_value[i]);
		if(i>=1 && i!= temp_value_num -1)
			strcat(temp_input,", ");	//두번째 인자부터 뒤에는 ','를 붙인다.
		else
			strcat(temp_input," ");
	}	
	strcpy(new->name,temp_input);																
	new->next=NULL;
	his_last->next=new;	//history마지막에 새로운 노드를 붙인다.																		
	his_last=new;	
}

void Dump(){	//Dump명령어 실행																					
	long  start, end;	//start, end는 출력할 시작 주소와 끝 주소를 저장한다.																		
	long line ,i, j;

	if(value_num==0){	//dump 뒤에 인자가 하나도 없다면																			
		start=address+1;																		
		if(start>=MAX_MEMORY_SIZE)																
			start=0;
		end=start+159;																			
	}
	else if(value_num==1){	//dump 뒤에 인자가 하나 있는 경우																		
		start=strtol(value[0],NULL,16);															
		end=start+159;																			
	}
	else{	//dump 뒤에 인자가 두개 있는 경우																		
		start=strtol(value[0],NULL,16);															
		end=strtol(value[1],NULL,16);
	}

																								
	if(end>=MAX_MEMORY_SIZE)	//end값이 MAX_MEMORY_SIZE보다 커진다면 MAX_MEMORY_SIZE보다 하나 작은 주소를 저장한다.
		end=MAX_MEMORY_SIZE-1;

	if(end<start){	//end값이 start보다 작다면 오류를 출력하고 return한다.																				
		printf("ERROR: Address of end is smaller than Address of start\n");
		return;
	}
	
	line=(end/16)-(start/16)+1;	//총 줄의 개수																	
	for(i=0;i<line;i++){
		int temp=start/16+i;
		printf("%04X0 ",temp);																	
		for(j=0;j<16;j++){	//제일 앞의 주소를 16단위로 끊어서 출력하는 부분 예) 00345																			
			if(start<= (temp*16+j) && (temp*16+j)<=end)	//메모리에 저장된 값을 16진수로 출력 예)3F
				printf("%02X ",memory[temp*16+j]);
			else
				printf("   ");
		}	
		printf("; ");
		for(j=0;j<16;j++){																		
			if(start<=(temp*16+j) && (temp*16+j)<=end){	//앞의 값들에 해당되는 ASCII코드를 출력한다.
				if(0x20 <= memory[temp*16+j] && memory[temp*16+j]<=0x7E)						
					printf("%c",memory[temp*16+j]);
				else
					printf(".");
			}
			else
				printf(".");
		}
		printf("\n");
	}
	if(value_num==0)																			
		address=end;
	Add_History();
}

void Edit(){	//Edit 명령어 실행																						
	long address, change;																		

	address=strtol(value[0],NULL,16);															
	change=strtol(value[1],NULL,16);

	memory[address]=(char)change;																
	Add_History();
}

void Fill(){	//Fill 멸령어 실행																					
	long start, end, change;

	start=strtol(value[0],NULL,16);		//start, end, change에 value의 값들을 16진수로 저장한다.																
	end=strtol(value[1],NULL,16);
	change=strtol(value[2],NULL,16);

	if(end<start){	//end의 값이 start보다 작다면 오류를 출력한다,																				
		printf("ERROR: Address of end is smaller than Address of start\n");
		return;
	}
	
	for(long i=start;i<=end;i++)																
		memory[i]=(char)change;
	Add_History();
}

void Reset(){	//Reset명령어를 실행한다.																					
	free(memory);	//메모리를 free하고 새로 동적할당한다.
	memory=(unsigned char*)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));						
	Add_History();
}

int Check_Address(char a[]) {	//주소가  유효한지 확인한다.																
	int size = strlen(a);
	int i;
	int flag=0;		
	for (i = 0; i < size; i++) {	//주소의 값 하나하나 확인한다.																											
		if(flag==5){	//주소의 길이가 5를 넘으면 오류를 출력한다.																	
			printf("ERROR: Address is inaccessible\n");
			return 0;
		}//주소의 각 자리가 0~,a~f,A~F사이의 값이 아니라면 오류를 출력한다.
		if (('0' <= a[i] && a[i] <= '9') || ('A' <= a[i] && a[i] <= 'F') || ('a' <= a[i] && a[i] <= 'f')){	
			if(flag==0 && a[i]=='0')
				continue;
			else 
				flag++;
		}
		else {
			printf("ERROR: Address is inaccessible\n");
			return 0;
		}
	}																							
	return 1;
}

int Check_Value(char a[]) {	//값들이 유효한지 확인한다.																	
	int size = strlen(a);
	int i;
	int flag=0;
	for (i = 0; i < size; i++) {	//값을 각 자리별로 확인한다.															
		if(flag==2){	//값이 2자리를 넘으면 오류를 출력한다.																		
			printf("ERROR: Value is out of range\n");
			return 0;
		}	//값이 16진수인지 범위를 통해서 확인한다.
		if (('0' <= a[i] && a[i] <= '9') || ('A' <= a[i] && a[i] <= 'F') || ('a' <= a[i] && a[i] <= 'f')){	
			if(flag==0 && a[i]=='0')
				continue;
			else
				flag++;
		}
		else {
			printf("ERROR: Value is out of range\n");
			return 0;
		}
	}
	return 1;
}

int Get_Hash(char target[]) {	//mnemonic값이 들어오면(문자열) hash값을 구한다.										
	int i;
	int hash = 37;	//hash를 임의의 수로 지정														
	int size = strlen(target);
	for (i = 0; i < size; i++) {	//hash값을 정하는 식을 만든다.															
		hash = ((hash << 3) + target[i]) % MAX_TABLE;
	}

	return hash;																			
}	

void Add_to_hash_table(int hash, int opcode, char temp_key[], char temp_format[]) {	//hash_table에 값을 추가한다.			
	hash_node* new = (hash_node*)malloc(sizeof(hash_node));	//새로운 노드를 생성하고 값을 입력한다.						
	strcpy(new->key, temp_key);
	strcpy(new->format, temp_format);
	new->value = opcode;
	new->next = NULL;	//hash값에 해당하는 hash_table의 linkde_list에 연결한다.
	if (hash_table[hash] == NULL) {															
		hash_table[hash] = new;
	}
	else {
		new->next = hash_table[hash];
		hash_table[hash] = new;
	}
}

void Print_opcodelist() {	//opcodelist를 출력한다.									
	hash_node* temp = NULL;
	for (int i = 0; i < MAX_TABLE; i++) {	//0~MAX_TABLE까지 돌면서 linked_list를 출력한다.													
		if (hash_table[i] != NULL) {
			temp = hash_table[i];
			printf("%2d : ", i);
			while (temp->next != NULL) {
				printf("[%02X,%s] --> ", temp->value, temp->key);
				temp = temp->next;
			}
			printf("[%02X,%s]\n", temp->value, temp->key);
		}
		else {
			printf("%2d : \n", i);
		}
	}
	Add_History();
}

void Find_opcode(){	//입력한 mnemonic에 해당하는 opcode를 찾는다.														
	int hash=Get_Hash(value[0]);	//Get_hash를 통해서 해당 mneomic의 hash값을 찾는다.																
	hash_node* temp=hash_table[hash];
	while(temp!=NULL){														
		if(strcmp(temp->key,value[0])==0){	//hash값에 해당하는 linked_list를 지나면서 값을 찾는다.
			printf("opcode is %02X\n",temp->value);
			Add_History();
			return;
		}
		temp=temp->next;
	}
	printf("ERROR: No such mnemonic\n");								
}

void Type(){	//type명령어를 수행한다.
	char temp;
	char last_char;
	FILE *fp=fopen(value[0],"r");
	if(fp==NULL){	//파일이 존재하지 않으면 오류를 출력한다.
		printf("ERROR: No such File Exists\n");
		return;
	}
	while(0<fscanf(fp,"%c",&temp)){	//char단위로 파일을 읽는다.
		last_char=temp;
		printf("%c",temp);
	}
	if(last_char != '\n')	//마지막 char가 개행이 아니라면 개행을 해준다.
		printf("\n");
	fclose(fp);
	Add_History();
}

void Assemble() {
	int line = 5;	//코드의 line을 쓰기 위한 변수
	int i, j;
	int LOCCTR = 0;	//location 주소를 저장하기 위한 변수
	int base = -1;	//BASE의 주소를 저장하기 위한 변수
	int PC = 0;		//PC의 위치를 저장하기 위한 변수
	int start_address = 0; //시작  주소 저장
	int code_length = 0;	//코드 전체의 길이
	int biggest_size = 0;	// code길이 중 가장 긴 길이
	int location_save[1000];	// LOCCTR을 저장하여, PC의 위치, 즉 다음 LOCCTR을 구한다.
	int location_save_size = 1;	// LOCCTR을 저장한 배열의 크기를 정해준다.
	int last_location = 0;	//PC의 값을 구하기 위해 이전의 LOCCTR을 가리키는 값

	int obj_last_address;	// obj파일 생성시 마지막 주소의 값

	int modification[1000];	//obj파일의 modification에서 사용된다.
	int modification_size = 0;	//modification 배열의 크기

	//파일 관련 변수
	char* list_outputfile;	//lst파일 
	char* obj_temp_outputfile;	//obj 파일 생성 이전 임시 파일
	char* obj_outputfile;	//obj파일
	char* temp_outputfile;	//asm파일과 lst, obj 파일 중간의 임시 파일
	char* extender;
	FILE* fp = fopen(value[0], "r");
	FILE* list_fp;
	FILE* obj_fp;
	FILE* temp_fp;

	//파일 input 관련 변수
	int size;	//각 줄별 길이
	char file_input[MAX_INPUT_SIZE]; //각 줄을 저장하는 배열
	char token[4][MAX_TOKEN_SIZE];	//각 줄을 token형식으로 저장
	int token_num = 0;	//각 줄의 token 수
	char temp_token[6][MAX_TOKEN_SIZE];	//token으로 저장하기 이전 임시 토큰

	//file을 읽어서 연다
	if (fp == NULL) {
		printf("ERROR: No such File Exists\n");
		return;
	}
	extender = (char*)malloc(sizeof(value[0]));
	strcpy(extender, value[0]);
	extender = strtok(extender, ".");
	extender = strtok(NULL, ".");
	if (strcmp(extender, "asm") != 0) {
		printf("ERROR: Not '.asm' File.\n");
		return;
	}


	//출력 파일의 이름을 생성한다.
	list_outputfile = (char*)malloc(sizeof(value[0]));
	obj_temp_outputfile = (char*)malloc(sizeof(value[0]));
	temp_outputfile = (char*)malloc(sizeof(value[0]));
	obj_outputfile = (char*)malloc(sizeof(value[0]));
	strcpy(obj_temp_outputfile, value[0]);
	strcpy(list_outputfile, value[0]);
	strcpy(temp_outputfile, value[0]);
	strcpy(obj_outputfile, value[0]);
	obj_temp_outputfile = strtok(obj_temp_outputfile, ".");
	obj_outputfile = strtok(obj_outputfile, ".");
	list_outputfile = strtok(list_outputfile, ".");
	temp_outputfile = strtok(temp_outputfile, ".");
	strcat(obj_outputfile, ".obj");
	strcat(obj_temp_outputfile, "_temp.obj");
	strcat(list_outputfile, ".lst");
	strcat(temp_outputfile, ".txt");

	//임시 symbol table을 생성한다.
	temp_symbol_head = (symbol_node*)malloc(sizeof(symbol_node));
	temp_symbol_head->next = NULL;

	memset(modification, 0, sizeof(modification));
	memset(location_save, 0, sizeof(location_save));

	/*-------------------------------------------
	phase1 start
	파일을 읽어서 loc주소를 설정하여 새로 저장하고
	symbol들을 저장하여 임시 symbol table을 만든다
	-------------------------------------------*/

	temp_fp = fopen(temp_outputfile, "w");
	while (fgets(file_input, sizeof(file_input), fp) != NULL) {	//asm파일의 한줄씩 읽는다.
		size = strlen(file_input);	//각줄의 개행을 제외하고 저장한다.
		if(file_input[size-2]==13)	//만약 '\n'이전에 CR이 있다면
			size-=2;
		else
			size-=1;
		if (biggest_size < size)	// 가장 큰 줄의 크기를 저장한다.
			biggest_size = size;

		memset(token, 0, sizeof(token));	//토큰의 초기화
		token_num = 0;

		//한 줄을 공백을 기준으로 token으로 나눔
		for (i = 0; i < size; i++) {
			if (file_input[i] == ' ' || file_input[i] == '\t' || file_input[i] == '\n')
				continue;
			for (j = i;; j++) {
				if (file_input[j] == ' ' || file_input[j] == '\t' || file_input[j] == '\0' || file_input[j] == '\n' || j == size)
					break;
				if ((j - i) == MAX_TOKEN_SIZE || token_num == 4) { //에러 발생시 파일 삭제 후 종료
					Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
					return;
				}
				token[token_num][j - i] = file_input[j];
				token[token_num][j - i + 1] = '\0';
			}
			i = j;
			token_num++;
			if (strcmp(token[0], ".") == 0) {
				break;
			}
		}
		//읽은 줄이 주석인 경우 temp파일에 출력하고 다음 줄을 읽는다.
		if (strcmp(token[0], ".") == 0) {
			fprintf(temp_fp, "%-5d          ", line);
			fputs(file_input, temp_fp);
			line += 5;
			continue;
		}
		//줄 별로 처리를 시작한다.
		if (line == 5) { // start명령어 구분
			if (strcmp(token[1], "START") == 0) {
				LOCCTR = (int)strtol(token[2], NULL, 16);	//시작 주소를 입력받는다.
				start_address = LOCCTR;	//PC에 시작 주소를 저장해 놓는다.
				obj_last_address = start_address;	//obj파일 만드는데 사용하는 변수에 처음 주소를 저장한다.
				fprintf(temp_fp, "%-5d   %04X   ", line, LOCCTR);
				fputs(file_input, temp_fp);
			}
			else {	//첫 줄에 start명령어가 오지 않으면 오류를 출력한다.
				Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
				return;
			}
		}
		else {	//토큰의 개수로 경우의 수를 나눈다
			if (token_num == 0) { //토큰의 개수가 0인 경우 --> 줄번호만 출력
				fprintf(temp_fp, "%-5d\n", line);
			}
			else if (token_num == 1 || token_num == 2) { // 토큰의 개수가 1 또는 2인 경우 --> Symbol이 없고 instruction만 있는 경우
				int temp = Get_Location(token[0], NULL, LOCCTR, 1);	// instruction에 따른 증가한 LOCCTR을 받는다.
				if (temp >= 0) {	//정상적으로 LOCCTR이 증가한 경우
					fprintf(temp_fp, "%-5d   %04X   ", line, LOCCTR);
					fputs(file_input, temp_fp);
					LOCCTR = temp;
				}
				else if (temp == -1) {	//instruction를 읽던 도중 오류가 발생한 경우
					Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
					return;
				}
				else {//Base또는 END가 들어온 경우
					fprintf(temp_fp, "%-5d          ", line);
					fputs(file_input, temp_fp);
					line += 5;
					if (temp == -3)	//END가 들어왔다면 break로 종료한다.
						break;
					continue;
				}
			}
			else if (token_num == 3) {  // 토큰의 개수가 3개 --> symbol이 있는 instruction, 뒤에 인자가 2개인 instruction
				int temp_token_length = strlen(token[1]);
				if (token[1][temp_token_length - 1] == ',') { //Symbol이 없고 뒤에 인자가 2개인 경우 즉, 뒤에 BUFFER, X와 같은 형식이다.
					int temp = Get_Location(token[0], NULL, LOCCTR, 1);	//증가한 LOCCTR을 저장한다.
					if (temp > 0) {	//정상적으로 저장된 경우
						fprintf(temp_fp, "%-5d   %04X   ", line, LOCCTR);
						fputs(file_input, temp_fp);
						LOCCTR = temp;
					}
					else if (temp == -1) {	// 오류가 발생한 경우
						Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
						return;
					}
					else {	// Base 또는 END가 들어온 경우
						fprintf(temp_fp, "%-5d          ", line);
						fputs(file_input, temp_fp);
						line += 5;
						if (temp == -3)
							break;
						continue;
					}
				}
				else {	//symbol이 있는 instruction
					int temp = Get_Location(token[1], token[2], LOCCTR, 2); // Symbol을 저장하고 instruction도 처리해야 된다.
					if (temp > 0) {	//정상적으로 LOCCTR이 증가한 경우
						fprintf(temp_fp, "%-5d   %04X   ", line, LOCCTR);
						fputs(file_input, temp_fp);
						if (Add_Symbol(token[0], LOCCTR) == 0) {	//Symbol이 이미 있는 symbol인지 확인한다.
							Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
							return;
						}
						LOCCTR = temp;
					}
					else if (temp == -1) {	//오류 발생
						Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
						return;
					}
					else {	//BASE또는 END가 들어온 경우
						fprintf(temp_fp, "%-5d          ", line);
						fputs(file_input, temp_fp);
						line += 5;
						if (temp == -3)
							break;
						continue;
					}
				}
			}
			else { //토큰의 개수가 4개 이상 --> Symbol도 있고 뒤에 인자도 2개 이상인 경우
				int temp = Get_Location(token[0], NULL, LOCCTR, 1);	//증가한 LOCCTR 저장
				if (temp > 0) {	//정상적으로 주ㅡㅇ가한 경우
					fprintf(temp_fp, "%-5d   %04X   ", line, LOCCTR);
					fputs(file_input, temp_fp);
					if (Add_Symbol(token[0], LOCCTR) == 0) {	//symbol의 중복 확인
						Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
						return;
					}
					LOCCTR = temp;
				}
				else{	//오류가 발생한 경우
					Assemble_error_phase_1(temp_fp, fp, temp_outputfile, line);
					return;
				}
			}
		}
		location_save[location_save_size - 1] = LOCCTR;	//location_size에 locctr을 저장한다.
		location_save_size++;
		line += 5;
	}
	location_save_size -= 1;
	code_length = LOCCTR - start_address;	//전체 코드 길이를 저장한다.
	fclose(temp_fp);
	fclose(fp);
	/*---------------------------------------
	phase 1 over
	-----------------------------------------
	phase 2 start
	생성한 temp파일을 읽어서 lst파일과 
	임시 obj파일을 만든다. 
	----------------------------------------*/
	temp_fp = fopen(temp_outputfile, "r");
	list_fp = fopen(list_outputfile, "w");	//생성할 lst파일
	obj_fp = fopen(obj_temp_outputfile, "w");	//생성할 임시 obj파일
	line = 5;
	memset(file_input, 0, sizeof(file_input));		
	while (fgets(file_input, sizeof(file_input), temp_fp) != NULL) {	//임시파일을 한줄씩 읽는다.
		size = strlen(file_input);	//줄의 크기를 저장한다.
		if(file_input[size-2]==13)
			size-=2;
		else
			size-=1;
		file_input[size] = '\0';	//마지막 문자를 \0으로 저장한다.
		memset(temp_token, 0, sizeof(temp_token));
		memset(token, 0, sizeof(token));
		token_num = 0;

		//줄을 temp_token으로 나눈다.
		for (i = 0; i < size; i++) {
			if (file_input[i] == ' ' || file_input[i] == '\t' || file_input[i] == '\n')
				continue;
			for (j = i;; j++) {
				if (file_input[j] == ' ' || file_input[j] == '\t' || file_input[j] == '\0' || file_input[j] == '\n' || j == size)
					break;
				if ((j - i) == MAX_TOKEN_SIZE || token_num == 6) { //에러 발생시 파일 삭제 후 종료
					Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
					return;
				}
				temp_token[token_num][j - i] = file_input[j];
				temp_token[token_num][j - i + 1] = '\0';
			}
			i = j;
			token_num++;
			if (strcmp(temp_token[1], ".") == 0) {
				break;
			}
		}
		//주석인 경우 그대로 출력하고 넘긴다.
		if (strcmp(temp_token[1], ".") == 0) {
			fprintf(list_fp, "%s\n", file_input);
			line += 5;
			continue;
		}
		// temp token으로 저장된 것들을 정리하여 token에 저장한다.
		// token[0]에는 instruction의 자리가, token[1], token[2]에는 뒤의 인자가 들어온다.
		if (token_num >= 5) {	// temp_token이 5개 이상인 경우 (line, loc, symbol, instruction, 인자 한개 이상)
			if (token_num == 5 && strcmp(temp_token[3], "START") == 0) {	//start는 따로 구분한다.
				fprintf(obj_fp, "H%s  ", temp_token[2]);
				fprintf(obj_fp, "%06X%06X\n", PC, code_length);
				fprintf(obj_fp, "T%06X ", obj_last_address);
				fprintf(list_fp, "%s\n", file_input);
				line += 5;
				continue;
			}
			if (Check_Symbol_Location(temp_token[2]) != -1) {	//symbol이 있는 경우
				strcpy(token[0], temp_token[3]);
				strcpy(token[1], temp_token[4]);
				if (token_num == 6) {
					strcpy(token[2], temp_token[5]);
					token_num = 3;
				}
				else
					token_num = 2;
			}
			else {	//symbol이 없는 경우
				strcpy(token[0], temp_token[2]);
				strcpy(token[1], temp_token[3]);
				strcpy(token[2], temp_token[4]);
				token_num = 3;
			}
		}
		else if (token_num == 4) {// temp token의 개수가 4개인 경우 (instruction, 인자)
			strcpy(token[0], temp_token[2]);
			strcpy(token[1], temp_token[3]);
			token_num = 2;

		}
		else if (token_num == 3) {	//temp token의 개수가 3개인 경우
			if (strcmp(temp_token[1], "BASE") == 0 && Check_Symbol_Location(temp_token[2]) >=0) {	//base가 instruction자리에 들어오는 경우
				base = Check_Symbol_Location(temp_token[2]);
				fprintf(list_fp, "%s\n", file_input);
				line += 5;
				continue;
			}
			else if (strcmp(temp_token[1], "END") == 0) {	//END가 instruction자리에 들어온 경우
					fprintf(list_fp, "%s\n", file_input);
					line += 5;
					continue;
			}
			else {	//instruction만 한개 들어온 경우
				strcpy(token[0], temp_token[2]);
				token_num = 1;
			}
		}
		else if (token_num == 1) {	//줄 번호만 입력된 경우
			fprintf(list_fp, "%s\n", file_input);
			line += 5;
			continue;
		}	//그 외의 경우에는 오류를 출력한다.
		else {
			Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
			return;
		}
		//token 재정비가 완료되었다.

		int length = biggest_size + 5 - size;	//가장 긴 줄을 기준으로 object코드를 출력하기 전까지의 공백을 정한다.
		fprintf(list_fp, "%s", file_input);	//lst파일은 temp파일과 앞부분이 똑같다
		for (i = 0; i < length; i++)	
			fprintf(list_fp, " ");

		//object코드를 생성한다 (lst파일을 완성하고 임시 obj파일을 생성한다)
		if (location_save[last_location+1] - obj_last_address > MAX_OBJ_LINE) {	//obj파일에서 정한 줄의 길이보다 짧은지 비교한다.
			if ((strcmp(token[0], "RESB") != 0 && strcmp(token[0], "RESW") != 0)) {
				fprintf(obj_fp, "\nT%06X ", location_save[last_location]);	//크다면 개행을 하고 다음 주소를 출력한다.
				obj_last_address = location_save[last_location];
			}
		}
		PC = location_save[++last_location];	//PC의 값은 현재 보고 있는 줄의 다음 LOCCTR을 가리키고 있다.

		//instruction에 오는 문자열에 따라서 처리를 시작한다.
		if (strcmp(token[0], "BYTE") == 0) {	//Byte 상수가 온 경우
			if (token_num != 2) {	//토큰의 개수 조건을 만족해야 된다.
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}

			char save[MAX_TOKEN_SIZE];	//뒤의 인자의 경우를 나눈다.
			strcpy(save, token[1]);
			int byte_size = strlen(save);
			if (save[0] == 'X' && save[1] == '\'' && save[byte_size - 1] == '\'') {	//X즉 16진수로 들어온 경우
				strcpy(save, strtok(save, "\'"));
				strcpy(save, strtok(NULL, "\'"));
				byte_size -= 3;

				for (int i = 0; i < byte_size; i++) {	//범위 이내를 만족하는지 확인
					if (('A' <= save[i] && save[i] <= 'F') || ('a' <= save[i] && save[i] <= 'f') || ('0' <= save[i] && save[i] <= '9')) {}
					else {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
				}
				if (byte_size % 2 == 1) {
					fprintf(list_fp, "0");
					fprintf(obj_fp, "0");
				}
				fprintf(list_fp, "%s\n", save);	//그대로 출력한다.
				fprintf(obj_fp, "%s", save);
			}
			else if (save[0] == 'C' && save[1] == '\'' && save[byte_size - 1] == '\'') {	//C 즉 character로 들어온 경우
				strcpy(save, strtok(save, "\'"));
				strcpy(save, strtok(NULL, "\'"));
				byte_size -= 3;
				for (i = 0; i < byte_size; i++) {
					if (0x20 > save[i] || save[i] > 0x7E) {	//범위 내의 ascii 코드인지 확인
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
					else {
						int ascii = save[i];	//ascii코드 값으로 저장
						fprintf(list_fp, "%02X", ascii);
						fprintf(obj_fp, "%02X", ascii);
					}
				}
				fprintf(list_fp, "\n");	
			}
			else {	//그 외에 X나 C가 없다면 오류를 출력한다.
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}
		}
		else if (strcmp(token[0], "WORD") == 0) {	//WORD 상수가 들어온 경우
			if (token_num != 2) {	//토큰 개수 조건 만족하는지
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}
			int word_size = strlen(token[1]);
			for (i = 0; i < word_size; i++) {	//10진수로 들어오기 때문에 범위확인은 0~9
				if ('0' <= token[1][i] && token[1][i] <= '9') {}
				else {
					Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
					return;
				}
			}
			int change = atoi(token[1]);	//출력은 16진수로 출력한다.
			fprintf(list_fp, "%06X\n", change);
			fprintf(obj_fp, "%06X", change);

		}
		else if (strcmp(token[0], "RESW") == 0) {	//word변수가 들어온 경우
			if (token_num != 2) {
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}
			fprintf(list_fp, "\n");
		}
		else if (strcmp(token[0], "RESB") == 0) {	//byte 변수가 들어온 경우
			if (token_num != 2) {
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}
			fprintf(list_fp, "\n");
		}
		else { //그외의 instruction들을 처리한다.
			int format;
			int flag = 0;
			if (token[0][0] == '+') {	//format 4를 구분하기 위해 flag를 이용한다.
				strcpy(token[0], strtok(token[0], "+"));
				flag = 1;
			}
			format = Get_Format(token[0]) + flag;	//Get_Format에 flag를 더해 format을 정한다,
			if (format == 2) { // format 이 2인 경우
				if (token_num == 2) { //인자가 2개인 경우(레지스터 하나 사용)
					fprintf(list_fp, "%02X", Get_Opcode(token[0])); //opcode를 출력하고
					fprintf(obj_fp, "%02X", Get_Opcode(token[0]));
					int reg = Get_Register(token[1]);
					if (reg != -1) {
						fprintf(list_fp, "%d0\n", reg);	//레지스터 번호를 출력한다.
						fprintf(obj_fp, "%d0", reg);
					}
					else {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
				}
				else {	//인자가 3개인 경우(레지스터 두개 사용)
					fprintf(list_fp, "%02X", Get_Opcode(token[0]));	//opcode를 출력하고
					fprintf(obj_fp, "%02X", Get_Opcode(token[0]));

					int temp_size = strlen(token[1]);
					token[1][temp_size - 1] = '\0';	// ','를 없애주기 위해
					int reg1 = Get_Register(token[1]);
					int reg2 = Get_Register(token[2]);
					if (reg1 != -1 && reg2 != -1) {	//레지스터 번호를 출력한다.
						fprintf(list_fp, "%d%d\n", reg1, reg2);
						fprintf(obj_fp, "%d%d", reg1, reg2);
					}
					else {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
				}
			}
			else if (format == 3) {	//format 3인 경우
				/*----------------------------------------------------
				format 3, 4인 경우 object code는
				(opcode 6)(n)(i)(x)(b)(p)(e)(12)
				(opcode 6)(n)(i)(x)(b)(p)(e)(20)이다
				이것을 4bit단위로 나누어
				처음 8bit를 first_second변수에 4bit씩 16진수로 나눠서 저장하였다.
				즉 opcode가 39이고 n,i의 값이 10이라면
				3*16 + (1001+0010) --> 3+(1011) --> 3+11 --> 3B 로 저장하였다
				이는 실제 object코드의 머리에 해당한다.

				다음 xbpe 를 third로 묶어서 저장하였고

				뒤에 display는 4bit씩 묶어서 저장하였다.
				----------------------------------------------------*/
				int flag = 0;
				int first_second = 0;
				int third = 0;
				first_second = Get_Opcode(token[0]);// 1. mnemonic에 따른 opcode를 구한다

				if (token_num == 1) {	//token이 한개인 경우
					first_second += 3;
					if (strcmp(token[0], "RSUB") == 0) {
						fprintf(list_fp, "%02X%04X\n", first_second, start_address);
						fprintf(obj_fp, "%02X%04X", first_second, start_address);
					}
					else {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
				}
				else {
					//2. @, # 구분 --> n과 i를 결정
					if (token[1][0] == '@') {	//indeirect addressing인 경우
						first_second += 2;	// n, i 가 1,0 임으로
						strcpy(token[1], strtok(token[1], "@"));
					}
					else if (token[1][0] == '#') {	//immediate addressing인 경우
						first_second += 1;	// n, i 가 0, 1임으로
						strcpy(token[1], strtok(token[1], "#"));
						flag = 1;	
					}
					else {	//외에는 simple addressing 
						first_second += 3;	//n, i 가 1, 1임으로
					}
					fprintf(list_fp, "%02X", first_second);	//first와 second를 먼저 출력한다.
					fprintf(obj_fp, "%02X", first_second);

					//3. x를 결정 
					if (token_num == 3) {	
						if (strcmp(token[2], "X") == 0)
							third += 8;	//x가 있다면 xbpe중 8에 해당하는 위치임으로
						else {
							Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
							return;
						}
					}
					//4. b, p결정

					if (token_num == 3) {	//토큰이 3개인 경우 2번째 토큰 뒤의 ','처리
						int temp_size = strlen(token[1]);
						token[1][temp_size - 1] = '\0';
					}
					int target_location = Check_Symbol_Location(token[1]);	//비교할 target의 주소를 저장한다.

					if (flag == 1) { // immediate addressing
						if (target_location < 0) {	//symbol이 아닌 실제 값을 immeidate addressing하는 경우
							int checking_size = strlen(token[1]);
							int fourth = 0;
							for (int i = 0; i < checking_size; i++) {	//만족하는 값인지 확인한다. 
								if ('0' > token[1][i] || token[1][i] > '9') {
									Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
									return;
								}
							}
							fourth = atoi(token[1]);	//fourth는 16진수로 저장한 target이다
							fprintf(list_fp, "%X%03X\n", third, fourth);// b, p,e 는 모두 0이기 때문에 third와 fourth를 출력한다.
							fprintf(obj_fp, "%X%03X", third, fourth);
						}
						else {	//symbol을 target으로 둔 경우
							int difference = target_location - PC;
							if (-2048 > difference || difference > 2047) { //base 사용
								third += 4;	//xbpe중 bpe가 100임으로 4를 더한다.
								if (base == -1) {	//LDB가 등장한 적이 없다.
									Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
									return;
								}
								difference = target_location - base;	//차이를 저장, display에 출력한다.
								if (difference < 0) {	//차이가 0보다 작은 경우 2의 보수를 구한다.
									int shifting = (1 << 12) - 1;
									difference = (difference & shifting);
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
								else {
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
							}
							else { // pc 사용
								third += 2;	//xbpe중 bpe가 010임으로 2를 더한다.
								if (difference < 0) {	//차이가 0보다 작은 경우 2의 보수를 구한다.
									int shifting = (1 << 12) - 1;
									difference = (difference & shifting);
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
								else {
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
							}
						}
					}
					else { // simple, indirect addressing인 경우
						if (target_location < 0) {	// symbol이 존재하지 않을 경우 오류
							Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
							return;
						}
						else {
							int difference = target_location - PC; //차이를 저장
							if (-2048>difference || difference>2047) { //차이가 16진수 1000을 넘으면 base 사용
								third += 4;	//base를 사용하였다.
								if (base == -1) {
									Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
									return;
								}
								difference = target_location - base;
								if (difference < 0) {	//차이가 0보다 작은 경우 2의 보수를 구한다.
									int shifting = (1 << 12) - 1;
									difference = (difference & shifting);
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
								else {
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
							}
							else { // pc를 사용한다.
								third += 2;	//pc를 사용하였다
								if (difference < 0) {	//차이가 0보다 작은 경우 2의 보수를 구한다.
									int shifting = (1 << 12) - 1;
									difference = (difference & shifting);
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
								else {
									fprintf(list_fp, "%X%03X\n", third, difference);
									fprintf(obj_fp, "%X%03X", third, difference);
								}
							}
						}
					}
				}
			}
			else if (format == 4) {	//format 형식이 4인 경우
				int first_second = Get_Opcode(token[0]); //format 3과 동일한 과정을 거친다.
				int third = 0;
				int flag = 0;
				//ni변환
				if (token[1][0] == '@') {	
					first_second += 2;
					strcpy(token[1], strtok(token[1], "@"));
				}
				else if (token[1][0] == '#') {
					first_second += 1;
					strcpy(token[1], strtok(token[1], "#"));
					flag = 1;
				}
				else {
					first_second += 3;
				}
				fprintf(list_fp, "%02X", first_second);
				fprintf(obj_fp, "%02X", first_second);
				//x를 결정 
				if (token_num == 3) {
					if (strcmp(token[2], "X") == 0)
						third += 8;
					else {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
				}
				//bpd는 format4에서 001이다
				third += 1;
				fprintf(list_fp, "%X", third);
				fprintf(obj_fp, "%X", third);
				//주소값 결정
				if (token_num == 3) {
					int temp_size = strlen(token[1]);
					token[1][temp_size - 1] = '\0';
				}
				int target_location = Check_Symbol_Location(token[1]);
				if (flag == 1) { // immediate addressing	
					if (target_location < 0) {	//symbol을 immediate addressing하는 것이 아니라 값을 addressing 한다.
						int checking_size = strlen(token[1]);
						int fourth = 0;
						for (int i = 0; i < checking_size; i++) {
							if ('0' > token[1][i] || token[1][i] > '9') {
								Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
								return;
							}
						}
						fourth = atoi(token[1]);
						fprintf(list_fp, "%05X\n", fourth);
						fprintf(obj_fp, "%05X", fourth);
						//값을 addressing했음으로 modification은 할 필요가 없다.
					}
					else {
						fprintf(list_fp, "%05X\n", target_location);
						fprintf(obj_fp, "%05X", target_location);
						modification[modification_size++] = location_save[last_location - 1] + 1;	//modification할 자리를 저장한다.
					}
				}
				else {	//indirect addressing, simple addressing
					if (target_location < 0) {
						Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
						return;
					}
					else {
						fprintf(list_fp, "%05X\n", target_location);
						fprintf(obj_fp, "%05X", target_location);
						modification[modification_size++] = location_save[last_location - 1] + 1;	//modification할 자리를 저장한다.
					}
				}


			}
			else {
				Assemble_error_phase_2(list_fp, temp_fp, obj_fp, temp_outputfile, list_outputfile, obj_temp_outputfile, line);
				return;
			}



		}
		line += 5;
	}
	fprintf(obj_fp, "\n");	// modification할 곳을 출력한다.
	for (int i = 0; i < modification_size; i++)
		fprintf(obj_fp, "M%06X05\n", modification[i]);
	fprintf(obj_fp, "E%06X", location_save[0]);	//End를 출력한다.
	fclose(list_fp);
	fclose(temp_fp);
	fclose(obj_fp);
	/*-------------------------------------------
	phase 2 end

	-----------------------------------------
	
	last phase
	임시로 생성한 obj파일을 제대로 고친다.
	이때 추가할 내용은 한 줄에 있는 object코드의 총 길이이다
	이는 forprint라는 변수를 사용한다.
	-------------------------------------*/

	temp_fp = fopen(obj_temp_outputfile, "r");
	obj_fp = fopen(obj_outputfile, "w");

	size = 0;
	memset(file_input, 0, sizeof(file_input));
	while (fgets(file_input, sizeof(file_input), temp_fp) != NULL) {	//각 줄씩 읽어서 처리한다.
		if (size == 0) {	//첫째 줄인 경우 그대로 출력한다.
			fputs(file_input, obj_fp);
			size = 1;
			continue;
		}
		size = strlen(file_input);
		memset(token, 0, sizeof(token));
		token_num = 0;
		//각 줄별로 token을 부여함
		for (i = 0; i < size; i++) {	//토큰을 부여한다.
			if (file_input[i] == ' ' || file_input[i] == '\t')
				continue;
			for (j = i;; j++) {
				if (file_input[j] == ' ' || file_input[j] == '\t' || file_input[j] == '\0' || file_input[j] == '\n' || j == size)
					break;
				token[token_num][j - i] = file_input[j];
				token[token_num][j - i + 1] = '\0';
			}
			i = j;
			token_num++;
		}
		if (token_num == 2){	//T가 들어간 부분을 수정한다.
			int forprint=strlen(token[1])/2;
			fprintf(obj_fp, "%s%02X%s\n", token[0], forprint, token[1]);	//object코드의 길이를 2로 나눠서 중간에 출력한다.
		}
		else	//M과 E는 수정 없이 그대로 출력한다.
			fputs(file_input, obj_fp);
	}

	fclose(temp_fp);	//temp_obj파일을 닫는다.
	fclose(obj_fp);		//obj파일을 닫는다.
	remove(obj_temp_outputfile);	
	remove(temp_outputfile);	//임시 파일들을 삭제한다.
	symbol_head = temp_symbol_head;	//어셈블리가 성공적으로 이루어졌음으로 임시 symbol table을 정식으로 등록한다.
	Add_History();	//history에 저장
	printf("Successfully assemble %s.\n",value[0]);	//성공 출력
}



void Assemble_error_phase_1(FILE* fp, FILE* fp2, char a[], int line) {	//phase1에서 오류가 발생할 경우
	symbol_node* temp3 = NULL;	

	printf("ERROR: Line number %d\n", line);	//오류가 발생한 줄을 출력
	fclose(fp);	//열려있는 파일을 닫는다.
	fclose(fp2);
	remove(a);	//임시 파일을 삭제한다.

	while (temp_symbol_head != NULL) {	//temp_symbol_table을 free한다.
		temp3 = temp_symbol_head;
		temp_symbol_head = temp_symbol_head->next;
		free(temp3);
		temp3 = NULL;
	}
}

void Assemble_error_phase_2(FILE* fp, FILE* fp2, FILE* fp3, char a[], char b[], char c[], int line) {	//phase 2에서 오류가 발생한 경우
	Assemble_error_phase_1(fp, fp2, a, line);	//phase 1과 같은 과정을 거친다.
	fclose(fp3);	//추가적으로 obj파일을 닫는다.
	remove(b);	//임시 파일들을 삭제한다.
	remove(c);
}

int Get_Location(char target[], char save[], int location, int type) {	//instruction이 들어오는 자리에 들어온 문자열과 뒤의 인자를 확인하여 증가한 LoCCTR을 구한다.
	int compare_location = location;
	int flag = 0;
	int temp = 0;
	if (target[0] == '+') {	//format 4를 따로 지정하기 위해 flag 사용한다.
		target = strtok(target, "+");
		flag = 1;
	}
	int hash = Get_Hash(target);	//opcode의 hash값을 구한다.

	if (strcmp(target, "BYTE") == 0 && type == 2) { // 명령어가 BYTE인 경우
		int size = strlen(save);
		if (save[0] == 'X' && save[1] == '\'' && save[size - 1] == '\'') {	//16진수가 들어온 경우
			save = strtok(save, "\'");
			save = strtok(NULL, "\'");
			size -= 3;
			for (int i = 0; i < size; i++) {	//유효하다면
				if (('A' <= save[i] && save[i] <= 'F') || ('a' <= save[i] && save[i] <= 'f') || ('0' <= save[i] && save[i] <= '9'))
					continue;
				else
					return -1;
			}
			location += (size + 1) / 2;	//location은 총길이의 1/2만큼 증가한다.
		}
		else if (save[0] == 'C' && save[1] == '\'' && save[size - 1] == '\'') {	//character로 들어온 경우
			save = strtok(save, "\'");
			save = strtok(NULL, "\'");
			size -= 3;
			for (int i = 0; i < size; i++) {	//유효하다면
				if (0x20 > save[i] || save[i] > 0x7E)
					return -1;
			}
			location += size;	//총 길이만큼 증가
		}
		else
			return -1;
	}
	else if (strcmp(target, "WORD") == 0 && type == 2) { // 명령어가 WORD 인 경우
		temp = atoi(save);	//word는 10진수로 들어온다.
		if (temp <= 0 || temp > 0xFFFFFF)	//유효하다면
			return -1;
		else
			location += 3;	//3만큼 증가한다.
	}
	else if (strcmp(target, "RESW") == 0 && type == 2) { // 명령어가 RESW인 경우
		temp = atoi(save);	//개수를 저장한다.
		if (temp <= 0)
			return -1;
		else
			location += (3 * temp);	//개수만큼 3을 증가한다.
	}
	else if (strcmp(target, "RESB") == 0 && type == 2) { // 명령어가 RESB인 경우
		temp = atoi(save);	//개수를 저장한다.
		if (temp <= 0)
			return -1;
		else
			location += temp;	//개수만큼 증가한다.
	}
	else if (strcmp(target, "BASE") == 0) { // 명령어가 BASE인 경우
		return -2;
	}
	else if (strcmp(target, "END") == 0) { // 명령어가 END 인 경우
		return -3;
	}
	else {				//그 외의 경우
		hash_node* temp = hash_table[hash];	//앞에서 구한 hash값을 이용한다.
		while (temp != NULL) {
			if (strcmp(temp->key, target) == 0) {
				if (strcmp(temp->format, "1") == 0) {	//type 1인 경우 1증가
					location += 1;
				}
				else if (strcmp(temp->format, "2") == 0) {	//type 2인 경우 2증가
					location += 2;
				}
				else {
					if (flag == 1)	//type 4인 경우
						location += 4;
					else	//type 3인 경우
						location += 3;	
				}
			}
			temp = temp->next;
		}
	}
	if (location < MAX_MEMORY_SIZE && location != compare_location) // 새로운 locctr값이 바뀌었고, 유효하면 반환한다.
		return location;
	else// 그 외의 경우에는 오류 처리를 한다.
		return -1;
}

int Add_Symbol(char target[], int location) {	//symbol을 새로 추가한다.
	symbol_node* temp = temp_symbol_head;
	while (temp->next != NULL) {
		int compare = strcmp(temp->next->symbol, target);	//strcmp를 사용하여 정렬하여 linked list에 넣는다.
		if (compare < 0) {	//만약 0보다 작다면 다음
			temp = temp->next;
		}
		else if (compare == 0) {	//만약 이미 있다면 오류를 출력하기 위해 0을 반환한다.
			return 0;
		}
		else {
			symbol_node* new = (symbol_node*)malloc(sizeof(symbol_node));	//0보다 크다면 그 자리가 symbol의 자리이다.
			new->loc = location;
			strcpy(new->symbol, target);
			new->next = temp->next;
			temp->next = new;
			return 1;
		}
	}
	symbol_node* new = (symbol_node*)malloc(sizeof(symbol_node));	//그외에 끝까지 온 경우 끝에다가 new를 추가한다.
	new->loc = location;
	strcpy(new->symbol, target);
	new->next = temp->next;
	temp->next = new;
	return 1;
}

int Check_Symbol_Location(char target[]) {	//symbol의 locctr을 반환한다.
	if (temp_symbol_head->next == NULL) {	//저장된 symbol이 없다면 오류를 출력한다.
		printf("ERROR: no saved symbol\n");
		return -1;
	}
	else {	//linked list를 돌면서 해당하는 locctr을 반환한다.
		symbol_node* temp = temp_symbol_head->next;
		while (temp != NULL) {
			if (strcmp(target, temp->symbol) == 0)
				return temp->loc;
			temp = temp->next;
		}
		return -1;
	}
}

int Get_Format(char target[]) {	//opcode(mnemonic)에 해당하는 format을 반환한다.
	int hash = Get_Hash(target);
	hash_node* temp = hash_table[hash];
	while (temp != NULL) {
		if (strcmp(temp->key, target) == 0) {
			if (strcmp(temp->format, "1") == 0) {
				return 1;
			}
			else if (strcmp(temp->format, "2") == 0) {
				return 2;
			}
			else {	//format이 3,4 인 경우
				return 3;
			}
		}
		temp = temp->next;
	}
	return -1;
}

int Get_Register(char target[]) {	//sic/xe에서 사용하는 register 고유의 번호를 반환한다.
	if (strcmp(target, "A") == 0)
		return 0;
	else if (strcmp(target, "X") == 0)
		return 1;
	else if (strcmp(target, "L") == 0)
		return 2;
	else if (strcmp(target, "PC") == 0)
		return 8;
	else if (strcmp(target, "SW") == 0)
		return 9;
	else if (strcmp(target, "B") == 0)
		return 3;
	else if (strcmp(target, "S") == 0)
		return 4;
	else if (strcmp(target, "T") == 0)
		return 5;
	else if (strcmp(target, "F") == 0)
		return 6;
	else
		return -1;
}

int Get_Opcode(char target[]) {	//mnemonic에 해당하는 opcode를 반환한다.
	int hash = Get_Hash(target);
	hash_node* temp = hash_table[hash];
	while (temp != NULL) {
		if (strcmp(temp->key, target) == 0) {
			return temp->value;
		}
		temp = temp->next;
	}
	return -1;
}

void Symbol() {	//Symbol 명령어를 수행한다.
	if (symbol_head->next == NULL) {	//assemble 이 성공적으로 이루어진 적이 없다면 오류를 출력한다.
		printf("ERROR: no saved symbol\n");
		return;
	}
	else {	//저장된 symbol table을 처음부터 출력한다. (이미 정렬 되어 있다.)
		symbol_node* temp = symbol_head->next;
		while (temp != NULL) {
			printf("\t%s\t%04X\n", temp->symbol, temp->loc);
			temp = temp->next;
		}
	}
	Add_History();
}
