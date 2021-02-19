#define MAX_INPUT_SIZE 200      // input의 최대 크기
#define MAX_MEMORY_SIZE 1048576 // 메모리의 최대 크기(16X65536)
#define MAX_TABLE 20            // hash table의 최대 크기는 20
#define MAX_KEY 7               // key값의 최대 길이
#define MAX_FORMAT 7            // Format의 최대 길이

char* input;                    // input을 모두 저장
unsigned char*memory;           // (16X65536)의 메모리 저장 공간

char value[3][10];              // 명령어에 알맞는 인자 저장
int value_num;                  // 인자의 개수

char temp_value[4][15];			// input들을 쪼개어 저장한 값
int temp_value_num;				// input들을 쪼개어 저장한 값의 개수

int address;					// dump수행 이후 마지막 주소 저장

typedef struct node{			//history 구현을 위한 node
	char name[MAX_INPUT_SIZE];  //명령어와 인자들 저장
	struct node*next;           //다음 노드를 가리키는 포인터
}node;

typedef struct H_node {         //Hash table구현을 위한 node
	char key[MAX_KEY];          //Key값을 저장(mnemonic)
	char format[MAX_FORMAT];    //Formt을 저장
	int value;   				//Key값에 해당하는 opcode 저장
	struct H_node* next;		//다음 노드를 가리키는 포인터
}H_node;


node* his_head;					//History의 출력 구현시 필요한 머리(시작)
node* his_last;					//History의 추가 구현시 필요한 꼬리(끝)
 
H_node* hash_table[MAX_TABLE];	//Hash table

void Command();
/*----------------------------------------------------
명령어에 맞는 함수를 호출하는 부분                      
-->switch문을 사용하여 명령어에 맞게 함수 호출          
-->input값이 알맞은지 확인하기 위해 Check_Command()호출 
------------------------------------------------------*/
int Check_Command();
/*----------------------------------------------------
input이 사용할 수 있는 input인지 확인하는 부분
-->input의 값을 공백을 기준으로 temp_value에 저장한다.
-->temp_value의 첫번째 값을 명령어와 매칭
-->명령어에 맞는 인자의 개수 확인
-->인자로 들어올 수 있는지 확인
-->위의 조건을 만족했을 경우 value에 temp_value를 저장하고
-->알맞는 함수를 호출하기 위한 값 return
-------------------------------------------------------*/

int get_input();
/*-------------------------------------------------------
input을 받는 함수
-->input이 100을 넘어가면 오류로 판단한다.
-->개행 문자를 받으면 그 자리에'\0'를 저장하고 종료한다.
--------------------------------------------------------*/

void FreeMemory();
/*-------------------------------------------------------
프로그램을 종료하기 전에 할당된 메모리를 free해준다.
-->input과 메모리를 free한다
-->반복문을 통해서 history를 free한다
-->반복문을 통해서 hasy_table의 값들을 free해준다.
---------------------------------------------------------*/

void Help();
/*--------------------------------------------------------
Help명령어 실행
-->모든 명령어를 출력한다.
---------------------------------------------------------*/

void Quit();
/*--------------------------------------------------------
Quit명령어 실행
-->메모리를 free하고 프로그램을 종료한다.
----------------------------------------------------------*/

void Dir();
/*---------------------------------------------------------
Dir명령어를 실행한다.
-->현재 디렉터리에 있는 파일들을 모두 출력한다.
-----------------------------------------------------------*/

void History();
/*----------------------------------------------------------
History명령어를 실행한다.
-->여태까지 입력된 유효한 input들을  순서대로 출력한다.
-----------------------------------------------------------*/

void Add_History();
/*-----------------------------------------------------------
History linked_list에 추가한다
-->유효한 명령어가 들어왔다고 판단되면 Add_History가 호출된다.
-->현재 마지막 노드 뒤에 새로운 노드를 연결시킨다.
------------------------------------------------------------*/

void Dump();
/*------------------------------------------------------------
Dump명령어를 실행한다.
-->dump뒤에 붙은 인자의 개수가 0,1,2인 경우로 나누어 start와 end를 정한다.
-->end가 영역을 넘어서면 최대 영역으로 설정한다.
-->dump뒤에 인자가 없는 경우 address를 업데이트 해준다.
-->형식에 맞춰서 start와 end사이의 값을 출력한다.
------------------------------------------------------------*/

void Edit();
/*-----------------------------------------------------------
Edit명령어를 실행한다.
-->특정 메모리의 값을 특정 값으로 바꾼다.
------------------------------------------------------------*/

void Fill();
/*-----------------------------------------------------------
Fill명령어를 실행한다.
-->특정 메모리부터 특정 메모리까지의 값을 정해진 값으로 바꾼다.
-------------------------------------------------------------*/

void Reset();
/*-----------------------------------------------------------
Reset명령어를 실행한다.
-->dump의 모든 값을 0x00으로초기화 한다.
-------------------------------------------------------------*/

int Check_Address(char a[]);
/*-----------------------------------------------------------
유효한 범위의 주솟값인지 확인한다.
-->dump, edit, fill등의 함수에서 주솟값을 검증할 때 사용된다.
-->0x00000~0xFFFFF까지의 값만을 유효하게 받아들인다.
-->범위를 벗어나면 적당한 오류를 출력한다.
-------------------------------------------------------------*/

int Check_Value(char a[]);
/*-------------------------------------------------------------
데이터에 저장될 수 있는 유효한 값인지 확인한다.
-->edit, fill와 같은 함수들이 호출될 때 사용된다.
-->0x00~0xFF까지의 값만을 유효하게 받아들인다.
-->범위를 벗어나면 적당한 오류를 출력한다.
------------------------------------------------------------*/

int Get_Hash(char target[]);
/*---------------------------------------------------------
hash 식을 통해서 mnemonic값을 특정 수와 대응시킨다.
-->마지막에 20으로 나눠줌으로써 hashtable의 크기가 20을 넘지 않는다.
-----------------------------------------------------------*/

void Add_to_hash_table(int hash, int opcode, char temp_key[], char temp_format[]);
/*----------------------------------------------------------
hash table에 opcode.txt에서 읽은 값을 저장한다.
-->Get_Hash()함수를 사용하여 구한 hash값에 해당하는 table에 저장한다.
-->linked_list형태로 구현한다.
-->순서가 상관 없음으로 새로운 값을 앞에 추가한다.
------------------------------------------------------------*/

void Print_opcodelist();
/*-----------------------------------------------------------
hash table의 값들을 모두 출력한다.
-->0~20까지 hash tabe을 돈다.
-->각각의 linked list를 출력한다.
------------------------------------------------------------*/

void Find_opcode();
/*-----------------------------------------------------------
입력한 mnemonic에 해당하는 opcode를 출력한다,
-->입력이 유효하지 않거나 해당 mnemonic이 존재하지 않는다면, 오류 출력한다.
-------------------------------------------------------------*/












