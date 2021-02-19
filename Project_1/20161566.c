#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h> 
#include<string.h>
#include "20161566.h"

int main() {
	int temp_opcode;															// opcode.txt의 opcode를 임시저장하는 변수
	char temp_key[MAX_KEY];														// opcode.txt의 mnemonic을 임시저장하는 변수
	char temp_format[MAX_FORMAT];												// opcode.txt의 Format을 임시저장하는 변수

	input = (char*)malloc(sizeof(char) * MAX_INPUT_SIZE);						// 모든 input을 통째로 저장하는 변수
	memory = (unsigned char*)calloc(MAX_MEMORY_SIZE,sizeof(unsigned char));		// 메모리 공간 확보

	his_head= (node*)malloc(sizeof(node));										//history의 머리를 생성한다
	his_last=his_head;															//history의 꼬리가 첫 원소를 가리키게 한다.

	address=-1;																	//dump에서 사용될 주소를 -1로 저장한다.

	FILE* fp = fopen("opcode.txt", "r");										//파일을 읽기 형식으로 open, 실패시 오류 메세지
	if (fp == NULL) {															
		printf("Failed to open 'opcode.txt'\n");
		return 0;
	}	

	while (0 < fscanf(fp, "%X %s %s", &temp_opcode, temp_key, temp_format)) {	//opcode.txt안의 값들을 한줄씩 읽는다.
		int hash = Get_Hash(temp_key);											//mnemonic의 값에 해당하는 hash값을 Get_Hash함수를 사용하여 구한다.
		Add_to_hash_table(hash, temp_opcode, temp_key, temp_format);			//위에 임시 저장한 값들로 hash_table을 작성한다.
	}
	fclose(fp);													

	while (1) {																	//quit이 입력되기 전까지 반복 실행한다.
		printf("sicsim> ");
		if(get_input()==1){														//get_input함수의 반환값이 1이라면 추가 실행
			value_num=0;														//value_num의 값을 0으로 초기화한다.
			memset(value,0,sizeof(value));										//value배열의 값들을 0으로 초기화한다.
			Command();															//명령어에 따라 수행한다,
		}
		input = calloc(MAX_INPUT_SIZE, sizeof(char));							//input값을 초기화한다.
	}
}

void Command() {																//Command함수
	int a = Check_Command();													//Check_Command함수를 호출하여, 명령어를 확인한다.
	if(a!=0){																	//0을 return 받으면 아무것도 하지 않는다.
		switch (a) {															//a가 1~10까지 알맞은 함수를 호출
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
		}
	}
}

int Check_Command() {																			//Check_Command함수, 명령어에 따라서 유효함을 판단한다.
	int size = strlen(input);																	//input의 크기를 size에 저장한다.
	memset(temp_value,0,sizeof(temp_value));													//temp_value의 값과 temp_value_num의 값을 초기화 한다.
	temp_value_num = 0;
	int i,j;

	for (i = 0; i < size; i++) {																//input의 값을 해체하여 temp_value에 넣는다.
		if (input[i] == ' ' || input[i] == '\t')												//공백은 무시한다.
			continue;
		for (j = i;; j++) {																		//공백이나 input이 끝날때까지 temp_value에 저장한다.
			if (input[j] == ' ' || input[j] == '\t' || input[j] == '\0' || j == size)			
				break;
			if ((j - i) == 15 || temp_value_num > 3) {											//temp_value_num이 3보다 크거나, temp_value하나에 저장되는 값이 14가 넘으면 0을 return한다.
				printf("ERROR: Input is Too long\n");
				return 0;
			}
			temp_value[temp_value_num][j - i] = input[j];										
			temp_value[temp_value_num][j - i + 1] = '\0';										//마지막에 '/0'을 넣어 문자열로 사용할 수 있게 한다.
		}
		i = j;																					
		temp_value_num++;																		//temp_value_num을 1증가시켜 input을 나누어 자정한다.
	}

	for(i=1;i< temp_value_num -1;i++){															//dump, fill등 input 중 ','가 들어가는 input을 처리하기 위한 과정
		size=strlen(temp_value[i]);						
		if(temp_value[i][size-1]==',')															//올바른 자리에 있다면 ','대신 '\0'넣는다	
			temp_value[i][size-1]='\0';					
		else{														
			printf("ERROR: ','Needed in the right place\n");									//올바른 위치에 없다면 이는 오류임으로 0을 return 한다.
			return 0;
		}
	}


	if (strcmp(temp_value[0], "help") == 0 || strcmp(temp_value[0], "h") == 0) {				//명령어가 help라고 인식된 경우
		if (temp_value_num == 1)																//temp_value_num, 즉 인자의 개수가 1개라면  유효함으로 1을 반환한다.
			return 1;
		else {																					//그외에는 명려어가 맞지만, 인자가 틀렸기 때문에 오류를 출력한다.
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "dir") == 0 || strcmp(temp_value[0], "d") == 0) {			//명령어가 dir로 인식된 경우
		if (temp_value_num == 1)																//인자 개수가 1개라면 2반환, 이외의 경우에는 오류를 출력
			return 2;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "quit") == 0 || strcmp(temp_value[0], "q") == 0) {			//명령어가 quit이라고 인식된 경우
		if (temp_value_num == 1)																//인자 개수가 1개라면 3반환, 이외의 경우에는 오류를 출력
			return 3;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}					
	else if (strcmp(temp_value[0], "history") == 0 || strcmp(temp_value[0], "hi") == 0) {		//명령어가 history라고 인식된 경우
		if (temp_value_num == 1)																//인자 개수가 1개라면 4반환, 이외의 경우에는 오류를 출력
			return 4;
		else {
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "dump") == 0 || strcmp(temp_value[0], "du") == 0) {			//명령어가 dump라고 인식된 경우
		if (temp_value_num == 1)																//인자 개수가 1개라면 5를 그대로 반환.
			return 5;
		else if (temp_value_num == 2) {															//인자 개수가 2개라면, start의 주소도 있을 수 있음으로 확인.
			if (Check_Address(temp_value[1]) == 1) {											//2번째 temp_value, 즉 start주소로 추정되는 값이 주소값으로 쓰일 수 있는지 확인
				strcpy(value[0], temp_value[1]);												//된다면 value[0]에 temp_value[1]을 저장
				value_num = 1;																	//value 개수 1추가
				return 5;																		//5반환
			}
			else
				return 0;																		//주소값이 유효하지 않다면 0반환
		}
		else if (temp_value_num == 3) {															//인자의 개수가 3개라면, start와 end의 주소가 유효한지 확인
			if (Check_Address(temp_value[1]) == 1 && Check_Address(temp_value[2]) == 1) {		//Check_Address를 통해서 두 주소값 모두 확인
				strcpy(value[0], temp_value[1]);												//유효하다면 value에 저장 후 5반환
				strcpy(value[1], temp_value[2]);
				value_num = 2;
				return 5;
			}
			else																				//하나라도 유효하지 않다면 0 반환
				return 0;
		}
		else {																					//인자의 개수가1,2,3이외의 값이라면 오류를 출력
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "edit") == 0 || strcmp(temp_value[0], "e") == 0) {			//명령어가 edit이라고 인식된 경우
		if (temp_value_num != 3) {																//인자의 개수가 3이 아니라면 오류를 출력한다.
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}	
		else {																					//인자가 3이라면 주소와 값이 유효한지 확인한다.
			if (Check_Address(temp_value[1]) == 1 && Check_Value(temp_value[2]) == 1) {			//Check_Address, Check_Value를 통해서 값과 주소가 유효한지 확인
				strcpy(value[0], temp_value[1]);												//유효하다면 value에 넣고 value_num을 조정한 뒤 6을 반환한다.
				strcpy(value[1], temp_value[2]);
				value_num = 2;
				return 6;
			}
			else																				//하나라도 유효하지 않다면 0을 반환한다.
				return 0;	
		}
	}
	else if (strcmp(temp_value[0], "fill") == 0 || strcmp(temp_value[0], "f") == 0) {			//명령어가 fill이라고 인식된 경우
		if (temp_value_num != 4) {																//인자의 개수가 4개가 아닌 경우 오류를 출력하고 0을 반환한다.
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
		else {
			if ((Check_Address(temp_value[1]) == 1 && Check_Address(temp_value[2]) == 1) && Check_Value(temp_value[3]) == 1){	//주소와 값이 유효한지 확인한다.
				strcpy(value[0], temp_value[1]);																				//유효하다면 value에 넣고 value_num을 조정한 뒤 7을 반환한다.
				strcpy(value[1], temp_value[2]);
				strcpy(value[2], temp_value[3]);
				value_num = 3;
				return 7;
			}		
			else																												//주소나 값 중 하나라도 유효하지 않다면 0을 반환한다.
				return 0;
		}
	}
	else if (strcmp(temp_value[0], "reset") == 0) {												//명령어가 reset이라고 인식된 경우
		if (temp_value_num == 1)																//인자가 1개라면 8을, 그 외에는 0을 반환한다.
			return 8;
		else{
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}
	else if (strcmp(temp_value[0], "opcode") == 0) {											//명령어가 opcode라고 인식된 경우
		if(temp_value_num !=2){																	//인자가 2가 아니라면 오류를 출력하고 0을 반환한다.
			printf("ERROR: Right command Wrong use\n");	
			return 0;
		}
		else{																					//0이 아니라면 입력받은 두번째 인자의 값이 mnemonic인지 확인한다.
			size=strlen(temp_value[1]);	
			if(size>6){																			//최대 길이는 6임으로 6보다 크면 오류를 출력하고 0을 반환한다.							
				printf("ERROR: No such mnemonic\n");
				return 0;
			}	
			for(i=0;i<size;i++){																//mnemonic은 모두 대문자임으로 이 범위를 벗어난다면 오류를 출력하고 0을 반환
				if('A'<= temp_value[1][i] && temp_value[1][i]<='Z')
					continue;
				else{
					printf("ERROR: No such mnemonic\n");
					return 0;
				}
			}
			strcpy(value[0], temp_value[1]);													//위 과정을 다 통과하였다면 mnemonic후보가 될 수 있음으로 value에 저장하고 9반환
			value_num=1;
			return 9;
		}
	}
	else if (strcmp(temp_value[0], "opcodelist") == 0) {										//명령어가 opcodelist라고 인식된 경우
		if(temp_value_num ==1)																	//인자가 1이라면 10을 그 외에는 오류를 출력하고 0을 반환한다.
			return 10;
		else{
			printf("ERROR: Right command Wrong use\n");
			return 0;
		}
	}																							//imput에 명령어가 있어야 할 자리에 올바른 명령어가 입력되지 않은 경우0을 반환
	else {
		printf("ERROR: Wrong Input\n");
		return 0;
	}
}


int get_input() {																				//input을 처음 저장하는 함수
	int i = 0;
	while (1) {
		char temp = getchar();																	//temp에 character하나씩 입력받는다.
		if (i >= MAX_INPUT_SIZE) {																//input의 크기가 MAX_INPUT_SIZE를 넘으면 오류를 출력한다.
			printf("ERROR: Input Size Too Much\n");
			return 0;
		}

		if (temp == '\n') {																		//개행 문자를 입력받으면 그 자리에'\0'를 저장하고 1을 반환한다.
			input[i] = '\0';
			return 1;
		}
		else {																					//평범한 문자나 공백이 오면 그대로 저장한다.
			input[i++] = temp;
		}
	}
}

void FreeMemory() {																				//quit함수에서 사용된다.
	free(input);																				//동적으로 할당된 input, memory, hash_table, history_linkedlist등을 free한다.
	free(memory);
	node *temp=NULL;
	H_node * temp2=NULL;

	while(his_head !=NULL){																		//his_head부터 NULL까지 쭉 free한다.
		temp=his_head;
		his_head=his_head->next;
		free(temp);
		temp=NULL;
	}
	for(int i=0;i<MAX_TABLE;i++){																//table 0부터 20까지 돌면서 linked_list안의 값들을 free한다.
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

void Help() {																					//Help명령어 실행시 출력해야되는 명령어의 모음
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp][start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	Add_History();
}

void Quit() {																					//Quit명령어 실행시 동적 할당 된 메모리를 free하고 프로그램을 종료한다.
	FreeMemory();
	exit(0);
}

void Dir() {																					//Dir명령어 실행
	DIR* dir=NULL;																				
	struct dirent* ent=NULL;
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
	node* curr;	
	int i=1;
	curr =his_head->next;																		//head->next부터 NULL까지 name을 출력한다.
	while(curr !=NULL){
		printf("%d\t%s\n",i,curr->name);
		i++;
		curr=curr->next;
	}
}

void Add_History(){																				//History에 새로운 노드를 추가한다.
	node *new;
	new = (node*)malloc(sizeof(node));
	char temp_input[MAX_INPUT_SIZE];															//temp_value에 저장된 값들을 temp_input에 연결하는 과정
	memset(temp_input,0,sizeof(temp_input));
	for(int i=0;i< temp_value_num;i++){															//두번째 인자부터 뒤에는 ','를 붙인다.
		strcat(temp_input, temp_value[i]);
		if(i>=1 && i!= temp_value_num -1)
			strcat(temp_input,", ");
		else
			strcat(temp_input," ");
	}	
	strcpy(new->name,temp_input);																//생성된 temp_input을 새로운 노드의 name에 저장한다.
	new->next=NULL;
	his_last->next=new;																			//history마지막에 새로운 노드를 붙인다.
	his_last=new;	
}

void Dump(){																					//Dump명령어 실행
	long  start, end;																			//start, end는 출력할 시작 주소와 끝 주소를 저장한다.
	long line ,i, j;

	if(value_num==0){																			//dump 뒤에 인자가 하나도 없다면
		start=address+1;																		//address에 저장된 값에 1을 더한 지점을 시작점으로 잡는다.
		if(start>=MAX_MEMORY_SIZE)																//start주소가 MAX_MEMORY_SIZE를 넘었다면 0으로 초기화 한다.
			start=0;
		end=start+159;																			//end주소는 start에 159를 더한 값으로 한다.
	}
	else if(value_num==1){																		//dump 뒤에 인자가 하나 있는 경우
		start=strtol(value[0],NULL,16);															//value[0]에 저장된 값을 16진수로 변환하여 start에 저장한다.
		end=start+159;																			//end주소는 start에 159를 더한 값으로 한다.
	}
	else{																						//dump 뒤에 인자가 두개 있는 경우
		start=strtol(value[0],NULL,16);															//value[0], value[1]의 값들을 16진수로 변환하여 start, end에 저장한다.
		end=strtol(value[1],NULL,16);
	}

																								//end값이 MAX_MEMORY_SIZE보다 커진다면 MAX_MEMORY_SIZE보다 하나 작은 주소를 저장한다.
	if(end>=MAX_MEMORY_SIZE)
		end=MAX_MEMORY_SIZE-1;

	if(end<start){																				//end값이 start보다 작다면 오류를 출력하고 return한다.
		printf("ERROR: Address of end is smaller than Address of start\n");
		return;
	}
	
	line=(end/16)-(start/16)+1;																	//총 줄의 개수
	for(i=0;i<line;i++){
		int temp=start/16+i;
		printf("%04X0 ",temp);																	//제일 앞의 주소를 16단위로 끊어서 출력하는 부분 예) 00345
		for(j=0;j<16;j++){																		//메모리에 저장된 값을 16진수로 출력 예)3F
			if(start<= (temp*16+j) && (temp*16+j)<=end)
				printf("%02X ",memory[temp*16+j]);
			else
				printf("   ");
		}	
		printf("; ");
		for(j=0;j<16;j++){																		//앞의 값들에 해당되는 ASCII코드를 출력한다.
			if(start<=(temp*16+j) && (temp*16+j)<=end){
				if(0x20 <= memory[temp*16+j] && memory[temp*16+j]<=0x7E)						//단 범위가 주어진 범위에 해당하는 ASCII코드들만 출력한다.
					printf("%c",memory[temp*16+j]);
				else
					printf(".");
			}
			else
				printf(".");
		}
		printf("\n");
	}
	if(value_num==0)																			//dump뒤에 인자가 없는 경우 adress의 값을 update해줘야 된다.
		address=end;
	Add_History();
}

void Edit(){																					//Edit 명령어 실행
	long address, change;																		

	address=strtol(value[0],NULL,16);															//address에 value[0]의 값을 16진수로, change에는 value[1]의 값을 16진수로 저장한다.
	change=strtol(value[1],NULL,16);

	memory[address]=(char)change;																//주소에 해당하는 값을 change값으로 바꿔준다.
	Add_History();
}

void Fill(){																					//Fill 멸령어 실행
	long start, end, change;

	start=strtol(value[0],NULL,16);																//start, end, change에 value의 값들을 16진수로 저장한다.
	end=strtol(value[1],NULL,16);
	change=strtol(value[2],NULL,16);

	if(end<start){																				//end의 값이 start보다 작다면 오류를 출력한다,
		printf("ERROR: Address of end is smaller than Address of start\n");
		return;
	}
	
	for(long i=start;i<=end;i++)																//start부터 end까지의 값들을 change로 초기화 한다.
		memory[i]=(char)change;
	Add_History();
}

void Reset(){																					//Reset명령어를 실행한다.
	free(memory);
	memory=(unsigned char*)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));						//메모리를 free하고 새로 동적할당한다.
	Add_History();
}

int Check_Address(char a[]) {																	//주소가  유효한지 확인한다.
	int size = strlen(a);
	int i;
	int flag=0;		
	for (i = 0; i < size; i++) {																//주소의 값 하나하나 확인한다.											
		if(flag==5){																			//주소의 길이가 5를 넘으면 오류를 출력한다.
			printf("ERROR: Address is inaccessible\n");
			return 0;
		}
		if (('0' <= a[i] && a[i] <= '9') || ('A' <= a[i] && a[i] <= 'F') || ('a' <= a[i] && a[i] <= 'f')){	//주소의 각 자리가 0~,a~f,A~F사이의 값이 아니라면 오류를 출력한다.
			if(flag==0 && a[i]=='0')
				continue;
			else 
				flag++;
		}
		else {
			printf("ERROR: Address is inaccessible\n");
			return 0;
		}
	}																							//위의 조건을 충족하면 유효한 주소라고 판단하여 1을 반환한다.
	return 1;
}

int Check_Value(char a[]) {																		//값들이 유효한지 확인한다.
	int size = strlen(a);
	int i;
	int flag=0;
	for (i = 0; i < size; i++) {																//값을 각 자리별로 확인한다.
		if(flag==2){																			//값이 2자리를 넘으면 오류를 출력한다.
			printf("ERROR: Value is out of range\n");
			return 0;
		}
		if (('0' <= a[i] && a[i] <= '9') || ('A' <= a[i] && a[i] <= 'F') || ('a' <= a[i] && a[i] <= 'f')){	//값이 16진수인지 범위를 통해서 확인한다.
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

int Get_Hash(char target[]) {																	//mnemonic값이 들어오면(문자열) hash값을 구한다.
	int i;
	int hash = 37;																				//hash를 임의의 수로 지정
	int size = strlen(target);
	for (i = 0; i < size; i++) {																//hash값을 정하는 식을 만든다.
		hash = ((hash << 3) + target[i]) % MAX_TABLE;
	}

	return hash;																			
}	
void Add_to_hash_table(int hash, int opcode, char temp_key[], char temp_format[]) {				//hash_table에 값을 추가한다.
	H_node* new = (H_node*)malloc(sizeof(H_node));												//새로운 노드를 생성하고 값을 입력한다.
	strcpy(new->key, temp_key);
	strcpy(new->format, temp_format);
	new->value = opcode;
	new->next = NULL;
	if (hash_table[hash] == NULL) {																//hash값에 해당하는 hash_table의 linkde_list에 연결한다.
		hash_table[hash] = new;
	}
	else {
		new->next = hash_table[hash];
		hash_table[hash] = new;
	}
}

void Print_opcodelist() {																		//opcodelist를 출력한다.
	H_node* temp = NULL;
	for (int i = 0; i < MAX_TABLE; i++) {														//0~MAX_TABLE까지 돌면서 linked_list를 출력한다.
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

void Find_opcode(){																				//입력한 mnemonic에 해당하는 opcode를 찾는다.
	int hash=Get_Hash(value[0]);																//Get_hash를 통해서 해당 mneomic의 hash값을 찾는다.
	H_node* temp=hash_table[hash];
	while(temp!=NULL){																			//hash값에 해당하는 linked_list를 지나면서 값을 찾는다.
		if(strcmp(temp->key,value[0])==0){
			printf("opcode is %02X\n",temp->value);
			Add_History();
			return;
		}
		temp=temp->next;
	}
	printf("ERROR: No such mnemonic\n");														//만약 같은 값이 없다면 오류를 출력한다.
}
