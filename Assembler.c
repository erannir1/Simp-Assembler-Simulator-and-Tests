#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 500  // Maximum line size is 500 chars
#define REG_NAME_LENGTH 6  // Register/Opcode name size is under 6 chars

// Structures

// The Label struct and its related functions

/* The label is built from linked list that helps us store information about the labels.
It will be built at the first iteration and will be used at the second iteration.
After, it will be then erased*/

typedef struct Label
{
    char name[50];  // Name of label
    int loc;  // Stores line number
    struct Label* next;  // Pointer to next label
} 
label;  // Call name of the struct 

label* creat_lab(char name[50], int loc)  //  Create label function
{
    label* new_lab = (label*)malloc(sizeof(label)); // Label memory allocation
    if (new_lab != NULL) { // If successful insert data
        strcpy(new_lab->name,name);
        new_lab->loc = loc;
        new_lab->next = NULL;
    }
    return new_lab;
}

label* add(label* head, char name[50], int loc) {  // Adds a label to the front
    label* new_lab = creat_lab(name, loc);  // build label, if fails return null
	if (new_lab == NULL)
        return NULL;
    new_lab->next = head; // Next pointer will point to the front
    return new_lab;
}

int find(label* head, char name[50]) {  // Search label by "name" and returns location.
    char cur_lab[50];  // cur_lab - current label's name
    strcpy(cur_lab, head->name);
    while (strcmp(cur_lab, name) != 0) {
        head = head->next;
        if (head == NULL) {
            return -1;
        }
        strcpy(cur_lab, head->name);  // Update to current name
    }
    return head->loc;
}

void free_label(label* head) // Free memory from labels
{
    label* temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// MemoryLine struct and related functions
typedef struct MemoryLine {
    char opcode[REG_NAME_LENGTH];  // Initial opcode and length
    // Initial registers and length
    char rd[REG_NAME_LENGTH];
    char rs[REG_NAME_LENGTH];
    char rt[REG_NAME_LENGTH];
    char imm[50];  // Initial immediate value and size
    int position;  // Variable tha holds the position of the char in line
    struct MemoryLine* next;   // Link to the next memory line
}MemoryLine;

// Memory line Constructor
MemoryLine* create_line(char opcode[REG_NAME_LENGTH], char rd[REG_NAME_LENGTH], char rs[REG_NAME_LENGTH], char rt[REG_NAME_LENGTH], char imm[50], int position) {
    MemoryLine* new_line  = (MemoryLine*)malloc(sizeof(MemoryLine)); // Allocate memoryline's memory
    if (new_line != NULL) {  // Allocate check and applicate
        strcpy(new_line->opcode, opcode);
        strcpy(new_line->rd, rd);
        strcpy(new_line->rs, rs);
        strcpy(new_line->rt, rt);
        strcpy(new_line->imm, imm);
        new_line->position = position;
        new_line->next = NULL;
    }
    return new_line;
}

// Add line function
MemoryLine* add_line(MemoryLine* head, char opcode[REG_NAME_LENGTH], char rd[REG_NAME_LENGTH], char rs[REG_NAME_LENGTH], char rt[REG_NAME_LENGTH], char imm[50], int position)
{
    MemoryLine* tail; // Last line
    // create a line
    MemoryLine* new_line = create_line(opcode, rd, rs, rt, imm, position);
    if (new_line == NULL)  // Check for the new line
        return NULL;
    if (head == NULL)  // Check if there is memory lines
        return new_line; // If there isn't make new line
    tail = head;
    while (tail->next != NULL)  // Run to the end of the linked list
        tail = tail->next;
    tail->next = new_line;  // when reaching the end, add the new line
    return head;
}

//free the memory line's memory
void free_memLine(MemoryLine* head) {
    // temp - a pointer to a line we are going to destroy after updating head
    MemoryLine* temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Finds memory line by position
MemoryLine* GetByPos(MemoryLine* head, int position) {
    // go until you find
    while (head != NULL && head->position != position)
        head = head->next;
	return head;
}

// Memory struct and related functions.
typedef struct Memory
{
    MemoryLine* head;  // Head of memory line list
    int last;  // Position of last value 
}Memory;

//create memory structure
Memory* cre_mem(MemoryLine* head, int position1)
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->head = head;
	mem->last = position1;
	return(mem); //Return number of lines
}

// Free memory
void free_mem(Memory* mem) {
    free_memLine(mem->head);
    free(mem);
}

// part 2 - Iterations

// Searching for labels in file and adding them to the list
label* createLabelList(FILE *asem) {
    int rowIx = 0;      // Code row's index. where the PC will jump after reading label
    char line[MAX_LINE], note1;  // Current line, note1 is the first char
    char note;  //note - current char in label name
    char label_line[50];  // Contains the label's name after 1st iteration
    char dots[50];
    strcpy(dots, ":");
    int k, j;
    int option;  // Option will determine if the label is label with command line or not
    label* head = NULL;
    while (!feof(asem)) {  //Going through the file  
        fgets(line, MAX_LINE, asem);  // Read command from the assembler file
        option = 0;  // Reset option
        if (strcmp(line, "\n") == 0)
            continue;
        note1 = line[0];
        if (note1 == '#')
            continue;
        if (strstr(line, ".word") != NULL)
            continue;
        if (strstr(line, dots) != NULL) // If dots -> is label
        {
            if (strstr(line, "#") != NULL) // Check for ":" in comment
				if ((strstr(line, dots)) > (strstr(line, "#"))) {
					if (strstr(line, "$imm") != NULL)
						rowIx += 2;
					else
						rowIx++;
					continue;
				}
            k = 0; // Reading index
            j = 0; // Writing index
            do {
                note = line[k];  // Current char
                if (note != ':') {  // Second dots is the end of label
                    if (note != '\t')
                        if (note != ' ')
                        {
                            label_line[j] = note;  // Get the char and assign it to name string
                            j++;
                        }
                    k++;
                }
            } while (note != ':'); 
            label_line[j] = '\0';
            k++; // Checking if it is a label line
            while ((line[k] == ' ') || (line[k] == '\t'))
                k++;
            if ((line[k] == '\n') || (line[k] == '#'))
                option = 1;  // Option is 1 on if label is a single line
            head = add(head, label_line, rowIx); // Adding label to label list
            if (option == 1) { // Single label line - add label and decrement counter
                rowIx--;
            }
        }
        k = 0;
        if ((line[k] == '\t') || (line[k] == ' '))
            k++;
        if (line[k] == '\n')
            continue;
		if (strstr(line, "$imm") != NULL)
			rowIx += 2;
		else
			rowIx++;
      
    }
    return head;
}

MemoryLine *spword(MemoryLine* head, char line[MAX_LINE], int *position1, int *k)
{
	char wordP[15], wordN[15];  // wordP - address, wordN – data
	int j = 0;  // Index for string we copy to
	*k = 0;  // Reset k index
	while (isdigit(line[*k])==0 ) 
		*k=*k+1;  // Skip spaces
	j = 0;
	while (line[*k] != ' ') {
		wordP[j] = line[*k];
		j++;
		*k=*k+1;
	}
	wordP[j] = '\0'; *k = *k + 1;  // Complete string
	j = 0; 
	while (line[*k] != ' ')  // Copy Data
	{
		if (line[*k] == '\n')
			break;
		wordN[j] = line[*k];
		j++;
		*k=*k+1;
	}
	wordN[j] = '\0';
	int position = 0;  // Position - current line address, hex or decimal
	if (wordP[0] == '0') {  // Change Address int. when hex input
		if (wordP[1] == 'x' || wordP[1] == 'X')
			position = strtol(wordP, NULL, 16);
	}
	else position = atoi(wordP);  // Decimal input
		 // Save command in memory list. IND - indicator while writing to change command to .word
	char ind[5] = "IND";
	if (head==NULL)
		head=add_line(head, ind, ind, ind, ind, wordN, position);  // Saves line to line list
	else
		add_line(head, ind, ind, ind, ind, wordN, position);
	if (position > *position1)  // Updates location of the end of the memory
		*position1 = position;
	return head;
}

void readopcode(char line[MAX_LINE], char *opcode, int *k) {
	char note;  // Current read char 
	int j = 0;  // Index of copy
	do {  // Reading opcode untill dollar 
		note = line[*k]; 
		if (note != '$') 
		{
			if (note != '\t')  // Blank space check
				if (note != ' ')
				{
					opcode[j] = note; 
					j++;
				}
			*k=*k+1;
		}
	} while (note != '$');
	opcode[j] = '\0'; 
}

void rdollar(char line[MAX_LINE], int *k) {
	while (line[*k] != '$')
		*k=*k+1; 
}
void readreg(char line[MAX_LINE], char *reg, int *k) {
	int j = 0;
	while (line[*k] != ',')  // Read register
	{
		if (line[*k] != ' ' && line[*k] != '\t') {
			reg[j] = line[*k];
			j++;
		}
		*k=*k+1;
	}
	reg[j] = '\0';
	return reg;  // fun is void &&&&&&
}
void readimm(char line[MAX_LINE], char *imm, int *k) {
	while ((line[*k] == ' ') || (line[*k] == '\t') || (line[*k] == ',')) {
		*k=*k+1;
	}
	int j = 0;  // Index of copy char
	while (line[*k] != ' ')
	{
		if (line[*k] != '\t') {
			if ((line[*k] == '#') || (line[*k] == '\n'))
				break;
			imm[j] = line[*k];
			j++;
		}
		*k=*k+1;
	}
	imm[j] = '\0';
	return imm;  // fun is void &&&&&&
}

MemoryLine *readLine(char *line, int *position1, int *i, MemoryLine *head, int *k) {
	char option[REG_NAME_LENGTH], rd[REG_NAME_LENGTH], rs[REG_NAME_LENGTH], rt[REG_NAME_LENGTH], imm[50]; // the line's properties
	readopcode(line, option, k);  // Read the opcode
	readreg(line, rd, k);  // Read rd
	rdollar(line, k);  // Search $
	readreg(line, rs, k);  // Read rs
	rdollar(line, k);  // Search $
	readreg(line, rt, k);  // Read rt
	readimm(line, imm, k);  // Applicate immediate
	head = add_line(head, option, rd, rs, rt, imm, *i);
	*i=*i+1 ;
	if (*i > *position1)
		*position1 = *i;  // Update last line position
	return head;
}

Memory* SecondRun(FILE* file) {
	// k - Index of the current char being read, i - Current position
	// position1 - Last line of the memory file
	int k = 0, i = 0, position1 = 0;
	// char - Hold the current line.
	// note - save first character in line 
	// option, rd, rs, rt and are the command's values
	// dots - Detects label
	char line[MAX_LINE], note, *dots = ":";
	MemoryLine* head = NULL;
	while ((fgets(line, MAX_LINE, file)) != NULL || !feof(file)) {  // Read File until EOF or the file is null
		if (strcmp(line, "\n") == 0)
			continue; 
		note = line[0];  // Get first line
		if (note == '#') 
			continue; 
		char ifword[6] = ".word";  //  String for comparison
		int isword = 0;  // .word detection (Bool)
		if (strstr(line, ifword) != NULL) {  // In case special .word found
				head = spword(head, line, &position1, &k);
				isword = 1;
			}
		else if (strstr(line, dots) != NULL) { 
				if (strstr(line, "#") != NULL) {  // Check if dots is label or not
					if ((strstr(line, dots)) >= (strstr(line, "#"))) {  // Label line - check if the line include only label or order
						goto tag;  // Start reading
					}
				} // the following section check if #  appears after dots or if # isn't in the line
				k = 0;
				while (line[k] != ':')
					k++;
				k++;
				if (line[k] == '\n')
					continue;
				else
					while ((line[k] == ' ') || (line[k] == '\t'))
						k++;
				if (line[k] == '\n') 
					continue;
				if (line[k] == '#')
					continue;
				if (i > position1)
					position1 = i;  // Update last line position
			}
			else // Order line exclusive
			{
				k = 0;
				while ((line[k] == ' ') || (line[k] == '\t')) 
					k++;
				if (line[k] == '#') 
					continue;
				if (line[k] == '\n') 
					continue;
			}
				if (!isword) { // Not .word line
				tag:
					k = 0;
					head = readLine(line, &position1, &i, head, &k);
				}
	}
		return cre_mem(head, position1);
}

void printreg(char *reg, FILE *imemin, int *num)
{ // Convert register's name to a number
	if (strcmp(reg, "$zero") == 0)  // Constant zero
		fprintf(imemin, "0");
	else if (strcmp(reg, "$imm") == 0)  // Sign extended immediate 
		fprintf(imemin, "1");
	else if (strcmp(reg, "$v0") == 0)  // Result value
		fprintf(imemin, "2");
	else if (strcmp(reg, "$a0") == 0)  // Argument register
		fprintf(imemin, "3");
	else if (strcmp(reg, "$a1") == 0)  // Argument register
		fprintf(imemin, "4");
	else if (strcmp(reg, "$t0") == 0)  // Temp register
		fprintf(imemin, "5");
	else if (strcmp(reg, "$t1") == 0)  // Temp register
		fprintf(imemin, "6");
	else if (strcmp(reg, "$t2") == 0)  // Temp register
		fprintf(imemin, "7");
	else if (strcmp(reg, "$t3") == 0)  // Temp register
		fprintf(imemin, "8");
	else if (strcmp(reg, "$s0") == 0)  // Saved register
		fprintf(imemin, "9");
	else if (strcmp(reg, "$s1") == 0)  // Saved register
		fprintf(imemin, "A");
	else if (strcmp(reg, "$s2") == 0)  // Saved register
		fprintf(imemin, "B");
	else if (strcmp(reg, "$gp") == 0)  // Global pointer
		fprintf(imemin, "C");
	else if (strcmp(reg, "$sp") == 0)  // Stack pointer
		fprintf(imemin, "D");
	else if (strcmp(reg, "$fp") == 0)  // Frame pointer
		fprintf(imemin, "E");
	else if (strcmp(reg, "$ra") == 0)  // Return Adress
		fprintf(imemin, "F");
	else if (strcmp(reg, "IND") == 0)
		*num = 0;
	else
		fprintf(imemin, "0");
}

int printopcode(char *opcode, FILE *imemin)
{
	if (strcmp(opcode, "add") == 0) {  // R[rd] = R[rs] + R[rt]
		fprintf(imemin, "00"); return 1;
	}
	else if (strcmp(opcode, "sub") == 0) {  // R[rd] = R[rs] – R[rt]
		fprintf(imemin, "01"); return 1;
	}
	else if (strcmp(opcode, "and") == 0) {  // R[rd] = R[rs] & R[rt]
		fprintf(imemin, "02"); return 1;
	}
	else if (strcmp(opcode, "or") == 0) {  // R[rd] = R[rs] | R[rt]
		fprintf(imemin, "03"); return 1;
	}
	else if (strcmp(opcode, "xor") == 0) {  // R[rd] = R[rs] ^ R[rt]
		fprintf(imemin, "04"); return 1;
	}
	else if (strcmp(opcode, "mul") == 0) {  // R[rd] = R[rs] * R[rt]
		fprintf(imemin, "05"); return 1;
	}
	else if (strcmp(opcode, "sll") == 0) {  // R[rd] = R[rs] << R[rt]
		fprintf(imemin, "06"); return 1;
	}
	else if (strcmp(opcode, "sra") == 0) {  // R[rd] = R[rs] >> R[rt], arithmetic shift with sign extension
		fprintf(imemin, "07"); return 1;
	}
	else if (strcmp(opcode, "srl") == 0) {  // R[rd] = R[rs] >> R[rt], logical shift 
		fprintf(imemin, "08"); return 1;
	}
	else if (strcmp(opcode, "beq") == 0) {  // if (R[rs] == R[rt]) pc = R[rd][low bits 9:0]
		fprintf(imemin, "09"); return 1;
	}
	else if (strcmp(opcode, "bne") == 0) {  // if (R[rs] != R[rt]) pc = R[rd] [low bits 9:0]
		fprintf(imemin, "0A"); return 1;
	}
	else if (strcmp(opcode, "blt") == 0) {  // if (R[rs] < R[rt]) pc = R[rd] [low bits 9:0]
		fprintf(imemin, "0B"); return 1;
	}
	else if (strcmp(opcode, "bgt") == 0) {  // if (R[rs] > R[rt]) pc = R[rd] [low bits 9:0]
		fprintf(imemin, "0C"); return 1;
	}
	else if (strcmp(opcode, "ble") == 0) {  //  if (R[rs] <= R[rt]) pc = R[rd] [low bits 9:0]
		fprintf(imemin, "0D"); return 1;
	}
	else if (strcmp(opcode, "bge") == 0) {  // if (R[rs] >= R[rt]) pc = R[rd] [low bits 9:0]
		fprintf(imemin, "0E"); return 1;
	}
	else if (strcmp(opcode, "jal") == 0) {  // R[15] = next instruction address, pc = R[rd][9:0]
		fprintf(imemin, "0F"); return 1;
	}
	else if (strcmp(opcode, "lw") == 0) {  // R[rd] = DMEM[R[rs]+R[rt]]
		fprintf(imemin, "10"); return 1;
	}
	else if (strcmp(opcode, "sw") == 0) {  // DMEM[R[rs]+R[rt]] = R[rd]
		fprintf(imemin, "11"); return 1;
	}
	else if (strcmp(opcode, "reti") == 0) {  // PC = IORegister[7]
		fprintf(imemin, "12"); return 1;
	}
	else if (strcmp(opcode, "in") == 0) {  // R[rd] = IORegister[R[rs] + R[rt]]
		fprintf(imemin, "13"); return 1;
	}
	else if (strcmp(opcode, "out") == 0) {  // IORegister [R[rs]+R[rt]] = R[rd]
		fprintf(imemin, "14"); return 1;
	}
	else if (strcmp(opcode, "halt") == 0) {  // Halt execution, exit simulator
		fprintf(imemin, "15"); return 1;
	}
	else // on .word
		return 0;
}

// Print the memory into files
void WriteDataToFile(Memory* mem, FILE *imemin, FILE *dmemin)
{
	int i = 0, num = 0; int flag = 0;
	while (i <= mem->last) {
		MemoryLine *currLine = GetByPos(mem->head, i); // Get the current line's data once.
		// Printing opcode in imemin . if data for the i_th row doesn't , exist print zero in dmemin
		if (currLine != NULL)
			flag = printopcode(currLine->opcode, imemin); // Print opcode
		if (!flag && currLine != NULL) {  // Word for the .word command line check
			if ((strcmp(currLine->opcode, "IND") == 0)) {
				if ((currLine->imm[0] == '0') && ((currLine->imm[1] == 'x') || (currLine->imm[1] == 'X')))  // Immediate is hex
					num = strtol(currLine->imm, NULL, 16);
				else  // Immediate is decimal
					num = atoi(currLine->imm);
				fprintf(dmemin, "%08X", num);  // Print in hex
			}
		}
		if (currLine != NULL)
			printreg(currLine->rd, imemin, &num);  // Printing Rd
		if (currLine != NULL)
			printreg(currLine->rs, imemin, &num); // Printing Rs
		if (currLine != NULL) {
			printreg(currLine->rt, imemin, &num);  // Printing Rt
			fprintf(imemin, "\n");
		}
		// Check if we need to print the immediate and skip .word lines
		if (currLine == NULL)
			fprintf(dmemin, "%08X", 0 & 0xffffffff); // Null line , print zero to immediate
		else if (strcmp(currLine->opcode, "IND") != 0)
		{
			if (strcmp(currLine->rs,"$imm")==0 || strcmp(currLine->rd, "$imm") == 0 || strcmp(currLine->rt, "$imm") == 0) {
				
				if ((currLine->imm[0] == '0') && ((currLine->imm[1] == 'x') || (currLine->imm[1] == 'X')))
					num = strtol(currLine->imm, NULL, 16); //Check if immidiate in hex
				else num = atoi(currLine->imm);
			
			fprintf(imemin, "%05X", num & 0xfffff); // Print immediate in hex . The & 0xfff is supposed to shorte negative numbers to 5 hexadecimal digits
			
			fprintf(imemin, "\n");
			}
			fprintf(dmemin, "%08X", 0 & 0xffffffff);
		}
		if (i != mem->last) {  // Print \n except last line
			fprintf(dmemin, "\n");
		}
			i++;
		
	}
}


void Label_NameToNum(MemoryLine* head, label* lab)
	{
		char tmp[50];
		MemoryLine *curr = head;  // Current memory line
		while (curr != NULL) {
			int location = find(lab, curr->imm);  // find if there is a label on the immediate and if it exists
			if (location != -1) {
				_itoa(location, tmp, 10);  // Changes int (decimal) to string and applicate in tmp
				strcpy(curr->imm, tmp);  // Copy label location number to immidiate
			}
			if (strcmp(curr->imm, "$zero") == 0)  // If immediate is $zero
				strcpy(curr->imm, "0");  // Changes immediate to "0"
			curr = curr->next;
		}
	}

// part 3 - Main function
	int main(int argc, char* argv[]) {
		FILE *asem = fopen(argv[1], "r");
		if (asem == NULL) {  // Exit if null file
			exit(1);
		}
		label* labels = createLabelList(asem);  // Locate the labels and write locations of them
		fclose(asem);
		asem = fopen(argv[1], "r");
		if (asem == NULL) {
			exit(1);
		}
		Memory *memory = SecondRun(asem);  // Second iteration
		fclose(asem);
		Label_NameToNum(memory->head, labels);  // Change labels from words to numbers
		// Write Data to files
		FILE* dmemin = fopen(argv[2], "w");
		FILE* imemin = fopen(argv[3], "w");
		if (dmemin == NULL)
			exit(1);
		if (imemin == NULL)
			exit(1);
		WriteDataToFile(memory, imemin, dmemin);
		fclose(dmemin);
		fclose(imemin);
		// Free memory 
		free_label(labels);
		free_mem(memory);
	}
