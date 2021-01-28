#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h> 

#define NUM_OF_REGISTERS 16
#define IO_REGISTERS 22
#define IMEMIN_LINE_SIZE 6
#define IMEMIN_MAX_SIZE 1024
#define DMEMIN_LINE_SIZE 9
#define DMEMIN_MAX_SIZE 4096
#define DISK_SECTORS_NUM 128
#define DISK_SECTORS_SIZE 128
#define MONITORX 352
#define MONITORY 288

typedef struct MemoryLine {   // A MemoryLine structure that represnt current Memory Line from files.
	unsigned int opcode;
	unsigned int rd;
	unsigned int rs;
	unsigned int rt;
	unsigned int imm_bit; // 1 - ML uses immi, 0 - else
	int immi;	// used only when imm_bit = 1;
}MemoryLine;

// Converts  decimal number to HEXA number, without end of string char - \0 
void Num2HEXA(int num, char *hex, int len) {
	int tmp;
	for (int i = len - 1; i >= 0; i--) {
		tmp = num % 16;
		if (tmp < 10)
			hex[i] = tmp + 48;
		else
			hex[i] = tmp + 55;
		num = num / 16;
	}
	hex[len] = '\0';  // Adds the end of string char 
}

// Convert immidiate to 2's complement
int Unsign2Sign(int imm) {
	int imm_sign = imm;
	if ((imm >> 19) == 1)
		imm_sign = imm_sign - pow(16, 5);
	return imm_sign;
}

// Convert signed number to unsigned number from 2's complement
int Sign2Unsigned(int imm) {
	int imm_unsigned = imm;
	if (imm_unsigned < 0) {
		imm_unsigned = imm_unsigned + pow(16, 5);
	}
	return imm_unsigned;
}


// Sign extention - using mask
int sign_exten(int num) {
	int ext = (0x000fffff & num);
	int mask = 0x00080000;  // Masking
	if (mask & num) {
		ext += 0xfff00000;
	}
	return ext;
}

// Add - Addition function
void add(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] + regs[ML.rt];
}

// Sub - Subtraction function
void sub(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] - regs[ML.rt];
}

// And
void and(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] & regs[ML.rt];
}
// Or
void or (int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] | regs[ML.rt];
}

// Xor
void xor(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] ^ regs[ML.rt];
}
// mul - Multiplication function
void mul(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] * regs[ML.rt];
}

// sll - Shit left function
void sll(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] << regs[ML.rt];
}

// srl - Shift right function - using mask
void srl(int *regs, MemoryLine ML) {
	int mask = 0x7fffffff;  // Masking
	regs[ML.rd] = regs[ML.rs] >> regs[ML.rt];
	if (regs[ML.rt] != 0) {
		mask = mask >> (regs[ML.rt] - 1);
		regs[ML.rd] = mask & (regs[ML.rs] >> regs[ML.rt]);
	}
}
// sra function
void sra(int *regs, MemoryLine ML) {
	regs[ML.rd] = regs[ML.rs] >> regs[ML.rt];
}

// beq - branch equal function -  register[rd] should be immidiate register when called - using mask
int beq(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] == regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// bne  - branch not equal function - register[rd] should be immidiate register when called - using mask
int bne(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] != regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// blt - branch lower than function - register[rd] should be immidiate register when called
int blt(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] < regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// bgt - branch greater than function -  register[rd] should be immidiate register when called - using mask
int bgt(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] > regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking 
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// ble function - register[rd] should be immidiate register when called - using mask
int ble(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] <= regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// bge function - register[rd] should be immidiate register when called - using mask
int bge(int *regs, MemoryLine ML, int PC) {
	if (regs[ML.rs] >= regs[ML.rt]) {
		int mask = 0b00000000001111111111;  // Masking
		return PC = mask & regs[ML.rd];  // Using R[rd][9:0]
	}
	else {
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		return PC;
	}
}

// jal function: when called reg[rd] should be the immi register
int jal(int *regs, MemoryLine ML, int PC) {
	if (ML.imm_bit == 1)
		regs[15] = PC + 2;
	else
		regs[15] = PC + 1;
	int mask = 0b00000000001111111111;
	return PC = mask & regs[ML.rd]; //taking only R[rd][9:0]
}

// lw - Load word function
void lw(int *regs, MemoryLine ML, int *mem) {
	regs[ML.rd] = mem[regs[ML.rs] + regs[ML.rt]];
}
// sw  - Store word function
void sw(int *regs, MemoryLine ML, int *mem) {
	mem[regs[ML.rs] + regs[ML.rt]] = regs[ML.rd];
}
// reti function
int reti(int *IORegs, MemoryLine ML, int PC, int *reti_bit) {
	*reti_bit = 1;
	return PC = IORegs[7];

}
// in function
void in(int *IORegs, MemoryLine ML, int *regs) {
	regs[ML.rd] = IORegs[regs[ML.rs] + regs[ML.rt]];
}
// out function
void out(int *IORegs, MemoryLine ML, int *regs) {
	IORegs[regs[ML.rs] + regs[ML.rt]] = regs[ML.rd];
}
// Execution of the memory line by different opcodes -  return PC place after the memory line occures
int EXE(int *regs, int *IORegs, MemoryLine ML, int PC, int *mem, int *hard_disk, int *reti_bit) {
	switch (ML.opcode)
	{
	case 0: {  // Add function called
		add(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 1: {  // Sub function called
		sub(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 2: {  // And function called
		and (regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 3: {  // Or function called
		or (regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 4: {  // Xor function called
		xor (regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 5: {  // mul function called
		mul(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 6: {  // sll function called
		sll(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 7: {  // sra function called
		sra(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 8: {  // srl function called
		srl(regs, ML);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 9: {  // beq function called
		PC = beq(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 10: {  // bne function called
		PC = bne(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 11: {  // blt function called
		PC = blt(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 12: {  // bgt function called
		PC = bgt(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 13: {  // ble function called
		PC = ble(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 14: {  // bge function called
		PC = bge(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 15: {  // jal function called
		PC = jal(regs, ML, PC);
		regs[0] = 0;
		break;
	}
	case 16: {  // lw function called
		lw(regs, ML, mem);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 17: {  // sw function called
		sw(regs, ML, mem);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 18: {  // reti function called
		PC = reti(IORegs, ML, PC, reti_bit);
		regs[0] = 0;
		break;
	}
	case 19: {  // in function called
		in(IORegs, ML, regs);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 20: {  // out function called
		out(IORegs, ML, regs);
		if (ML.imm_bit == 1)
			PC++;
		PC++;
		regs[0] = 0;
		break;
	}
	case 21: {  // halt function called
		PC = -1;
		regs[0] = 0;
		break;
	}
	default:
		PC++;   // In case of invalid opcode
	}
	return PC;
}

// String from imemin to Memory line
MemoryLine str2ML(char imemin[IMEMIN_MAX_SIZE][IMEMIN_LINE_SIZE], int PC) {
	MemoryLine head;
	char *ML = imemin[PC];
	char opcode_String[3], rd_String[2], rt_String[2], rs_String[3];
	opcode_String[0] = ML[0];
	opcode_String[1] = ML[1];
	rd_String[0] = ML[2];
	rs_String[0] = ML[3];
	rt_String[0] = ML[4];
	rd_String[1] = rs_String[1] = rt_String[1] = opcode_String[2] = '\0';
	head.opcode = (int)strtol(opcode_String, NULL, 16);
	head.rd = (int)strtol(rd_String, NULL, 16);
	head.rs = (int)strtol(rs_String, NULL, 16);
	head.rt = (int)strtol(rt_String, NULL, 16);
	if (head.rt > 15 || head.rd > 15 || head.rs > 15)  // In case of invalid register we decide that it will be 0 to all
		head.rt = head.rd = head.rs = 0;
	if (head.rt == 1 || head.rs == 1 || head.rd == 1) {
		head.imm_bit = 1;
		head.immi = Unsign2Sign((int)strtol(imemin[PC + 1], NULL, 16));  // Calculate immidiate by extracting imemin file , after that calculates the 2's complement
	}
	else {
		head.immi = 0;
		head.imm_bit = 0;
	}
	return head;
}

// Create line for HardWare register trace
void Line2HWRegTrace(char *line, int clk, MemoryLine ML, int *IORegs, int *regs) {
	char reg_Data[10];
	char clock_String[20];
	sprintf(clock_String, "%d", (clk+1));  // Convert clock to string
	strcpy(line, clock_String);
	if (ML.opcode == 19) 
		strcat(line, " Read ");
	else
		strcat(line, " Write ");
	switch (regs[ML.rs]+regs[ML.rt])
	{
	case 0: {
		strcat(line, "irq0enable ");
		break; }
	case 1: {
		strcat(line, "irq1enable ");
		break; }
	case 2: {
		strcat(line, "irq2enable ");
		break; }
	case 3: {
		strcat(line, "irq0status ");
		break; }
	case 4: {
		strcat(line, "irq1status ");
		break; }
	case 5: {
		strcat(line, "irq2status ");
		break; }
	case 6: {
		strcat(line, "irqhandler ");
		break; }
	case 7: {
		strcat(line, "irqreturn ");
		break; }
	case 8: {
		strcat(line, "clks ");
		break; }
	case 9: {
		strcat(line, "leds ");
		break; }
	case 10: {
		strcat(line, "reserved ");
		break; }
	case 11: {
		strcat(line, "timerenable ");
		break; }
	case 12: {
		strcat(line, "timercurrent ");
		break; }
	case 13: {
		strcat(line, "timermax ");
		break; }
	case 14: {
		strcat(line, "diskcmd ");
		break; }
	case 15: {
		strcat(line, "disksector ");
		break; }
	case 16: {
		strcat(line, "diskbuffer ");
		break; }
	case 17: {
		strcat(line, "diskstatus ");
		break; }
	case 18: {
		strcat(line, "monitorcmd ");
		break; }
	case 19: {
		strcat(line, "monitorx ");
		break; }
	case 20: {
		strcat(line, "monitory ");
		break; }
	case 21: {
		strcat(line, "monitordata ");
		break; }
	default: {
		strcat(line, "Invalid HW reg trace");
	}
	}
	if (ML.opcode == 19) 
		Num2HEXA(IORegs[regs[ML.rs] + regs[ML.rt]], reg_Data, 8);
		
	else 
		Num2HEXA(regs[ML.rd], reg_Data, 8);

	strcat(line, reg_Data);
	strcat(line, "\n");
}

// The next cycle of irq2
int NEXT_irq2(int cur_irq2, int clk, FILE *irq2, int *IORegs) {
	int NEXT_irq2;
	char line[DMEMIN_LINE_SIZE];
	if (IORegs[5] == 0 && cur_irq2 < clk ) {  // Updates when next cycle has reached and prev irq2 didnt finish
		if (!feof(irq2)) {	 // End of file have not reached
			fgets(line, DMEMIN_LINE_SIZE, irq2);  // Read next line
			while (strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0)  // Ignore blank spaces
				fgets(line, DMEMIN_LINE_SIZE, irq2);
			NEXT_irq2 = (int)strtol(line, NULL, 10);
			return NEXT_irq2;
		}
		else
			fclose(irq2);
	}
	else {
		return cur_irq2;
	}
	
}

// Updates the status of irq2
void UpdateStatus_irq2(int NEXT_irq2, int *IORegs, int clk) {
	if ((clk == NEXT_irq2 && IORegs[5] == 0) || (clk + 1 == NEXT_irq2 && IORegs[5] == 0))
		IORegs[5] = 1;
}

// Manage interrupt: change the Memory Line and PC to relevent interrupt
MemoryLine Manage_Inter(MemoryLine ML, int *reti_bit, int *PC, int *IORegs, char imemin[IMEMIN_MAX_SIZE][IMEMIN_LINE_SIZE]) {
	if (*reti_bit == 1) {  // Ready for new interrupt
		*reti_bit = 0;  // Change interrupt status to busy
		IORegs[7] = *PC;
		*PC = IORegs[6];
		return ML = str2ML(imemin, *PC);
	}
	else {  // Still busy
		return ML;
	}
}

// Manage timer - Updates  the status of the timer in every iteration
void ManageTimer(MemoryLine ML, int *IORegs) {
	if (IORegs[11] == 1) {  // Timer enabled
		if (ML.imm_bit == 0) {	// Memory line without immidiate
			if (IORegs[12] == IORegs[13]) {	// Reset the  timer and start irq0
				IORegs[12] = 0;
				IORegs[3] = 1;
			}
			else
				IORegs[12]++;
		}
		else {  // Memory line with immidiate
			if (IORegs[12] == IORegs[13]) {	 // Reset the timer and starts irq0
				IORegs[12] = 1;	 // A memory line with immidiate value have an increased duration of two clock cycles
				IORegs[3] = 1;
			}
			else if (IORegs[12] == (IORegs[13] - 1)) {
				IORegs[12] = 0;	 // A memory line with immidiate value have an increased duration of two clock cycles
				IORegs[3] = 1;
			}
			else
				IORegs[12] += 2;
		}
	}
}
//Hard Disk handle
void ManageDisk( int *disk_timer, MemoryLine ML, int *IORegs, int *mem, int HD[DISK_SECTORS_NUM][DISK_SECTORS_SIZE]) {
	if (IORegs[17] == 0) {  // Disk - free
		if (IORegs[14] != 0) {  //  Have Memory line to do
			*disk_timer = 1;  // Timer reset
			IORegs[17] = 1;  // Change disk status to busy
		}
	}
	else {  // When disk is busy
		if (*disk_timer < 1023) {
			if (ML.imm_bit == 0)
				*disk_timer += 1;
			else
				*disk_timer += 2;
		}
		else {
			if ((*disk_timer == 1023 && ML.imm_bit == 1) || *disk_timer == 1024) {	 // Memory line is done
				switch (IORegs[14]) {  // Execute Memory line
				case 1: {  // Reads disk to memory
					for (int i = 0; i < 128; i++) {
						if (IORegs[16] + i < DMEMIN_MAX_SIZE)
							mem[IORegs[16] + i] = HD[IORegs[15]][i];
					}
				}
				case 2: {	// Writes memory to disk
					for (int i = 0; i < 128; i++) {
						if (IORegs[16] + i < DMEMIN_MAX_SIZE)
							HD[IORegs[15]][i] = mem[IORegs[16] + i];
					}
				}
				}
				*disk_timer = 0;
				IORegs[17] = 0;	 // Free disk
				IORegs[14] = 0;  // Resets memory line for disk
				IORegs[4] = 1;  // Changes irq1status to 1	
			}
			else
				*disk_timer += 1;
		}
	}
}

// Manage the monitor
void ManageMonitor(MemoryLine ML, int *IORegs, int monitor[MONITORY][MONITORX]) {
	int x = IORegs[19];
	int y = IORegs[20];
	int DATA = IORegs[21];
	if (ML.opcode == 20 && IORegs[18] == 1) {	// Write instruction to monitor has been made
		monitor[y][x] = DATA;
		IORegs[18] = 0;
	}
}

// Creates new line for the leds file
void Line2Led(char *line, int clk, MemoryLine ML, int *regs) {
	char reg_Data[10];
	char clock_String[20];
	sprintf(clock_String, "%d", (clk + 1));  // Converting clock to string
	strcpy(line, clock_String);
	strcat(line, " ");
	Num2HEXA(regs[ML.rd], reg_Data, 8);
	strcat(line, reg_Data);
	strcat(line, "\n\0");
}

// Creates line for trace
void Line2Trace(int PC, char imemin[IMEMIN_MAX_SIZE][IMEMIN_LINE_SIZE], int *regs, char *line) {
	int immiSigned;
	char TempHEXAReg[9];
	char HEXA_PC[4];
	Num2HEXA(PC, HEXA_PC, 3);
	strcpy(line, HEXA_PC);
	strcat(line, " ");
	strcat(line, imemin[PC]);
	strcat(line, " 00000000");
	for (int i = 1; i < 16; i++) {
		strcat(line, " ");
		if (i == 1) {
			immiSigned = Sign2Unsigned(regs[i]);
			Num2HEXA(immiSigned, TempHEXAReg, 8);
			if (immiSigned >> 19 == 1) {  // Immidiate most significate bit = 1
				TempHEXAReg[2] = TempHEXAReg[1] = TempHEXAReg[0] = 'F';  // Sign extention
			}
		}
		else
			Num2HEXA(regs[i], TempHEXAReg, 8);
		strcat(line, TempHEXAReg);
	}
	strcat(line, "\n\0");
}

// Creates dmemout file
void Create_dmemout(int *memory, char *filename) {
	FILE *dmemout;
	char line[DMEMIN_LINE_SIZE + 1];
	int num;
	int tail = DMEMIN_MAX_SIZE - 1;
	if ((dmemout = fopen(filename, "w")) == NULL) 
		printf("Error while trying to open dmemout file\n");
	for (int j = tail; j >= 0; j--) 
		if (memory[j] != 0) {
			tail = j;
			j = -1;
		}
	if (memory[tail] == 0 && tail == DMEMIN_MAX_SIZE - 1)
		tail = -1;
	for (int i = 0; i <= tail; i++) {
		num = memory[i];
		num = Sign2Unsigned(num);
		Num2HEXA(num, line, 8);
		strcat(line, "\n");
		fprintf(dmemout, line);
	}
	fclose(dmemout);
}

// Creates the cycles file
void CreateCyclesTXT(int clk, int ML_counter, char *filename) {
	FILE *cycles;
	if ((cycles = fopen(filename, "w")) == NULL) 
		printf("Error while trying to open cycles file\n");
	fprintf(cycles, "%d\n", clk);
	fprintf(cycles, "%d\n", ML_counter);
	fclose(cycles);
}

//create diskout file
void Create_diskout(int HD[DISK_SECTORS_NUM][DISK_SECTORS_SIZE], char *filename) {
	char line[15];
	int num;
	int address = 127, sec = 127;
	FILE *diskout;
	
	if ((diskout = fopen(filename, "w")) == NULL)
		printf("Error while trying to open diskout file\n");
	for (int row = 127; row >= 0; row--) {  // Find last sector 
		for (int col = 127; col >= 0; col--) {
			if (HD[row][col] != 0) {
				sec = row;
				address = col;
				row = -1;
				col = -1;
			}
		}
	}
	if (sec == 127 && address == 127) 
		if(HD[sec][address]==0)
			sec = address = -1;
	for (int row = 0; row <= sec; row++) {
		for (int col = 0; col <= address; col++) {
			num = HD[row][col];
			num = Sign2Unsigned(num);
			Num2HEXA(num, line, 8);
			strcat(line, "\n");
			fprintf(diskout, line);
		}
	}
	fclose(diskout);
}

// Create the regout file
void Create_Regout(int *regs, char *filename) {  
	FILE *regout;
	int tmp;
	char line[DMEMIN_LINE_SIZE + 1];
	if ((regout = fopen(filename, "w")) == NULL)
		printf("Error while trying to open regout file\n");
	for (int i = 2; i < 16; i++) {
		tmp = regs[i];
		tmp = Sign2Unsigned(tmp);
		Num2HEXA(tmp, line, 8);
		strcat(line, "\n");
		fprintf(regout, line);
	}
	fclose(regout);
}

// Create monitor file TXT
void CreateMonitorTXTandYUV(int monitor[MONITORY][MONITORX], char *monitor_fp,char *monitor_yuv) {
	int tmp;
	char line[3];
	FILE *monitor_text_fp;
	FILE *monitor_yuv_fp;
	if ((monitor_text_fp = fopen(monitor_fp, "w")) == NULL) 
		printf("Error while trying to open monitor.txt file\n");
	if ((monitor_yuv_fp = fopen(monitor_yuv, "w")) == NULL)
		printf("Error while trying to open monitor.txt file\n");
	for (int y = 0; y < MONITORY; y++) {
		for (int x = 0; x < MONITORX; x++) {
			tmp = monitor[y][x];
			fprintf(monitor_yuv_fp, "%c", tmp);
			Num2HEXA(tmp, line, 2);
			fprintf(monitor_text_fp, line);
			fprintf(monitor_text_fp, "\n");
		}
	}	
	fclose(monitor_yuv_fp);
	fclose(monitor_text_fp);
}

// Generating imemin - opens imemin file - makes array of Memory lines strings
void Generate_imemin(char imemin[IMEMIN_MAX_SIZE][IMEMIN_LINE_SIZE], char *file_name) {
	FILE *imeminTextFile;
	char line[IMEMIN_LINE_SIZE];
	int i = 0;
	if ((imeminTextFile = fopen(file_name, "r")) == NULL)
		printf("Error with opening %s file\n",file_name);
	while(i < IMEMIN_MAX_SIZE) {  // Filling all the imemin array with memory line
		if (!feof(imeminTextFile)) {
			fgets(line, IMEMIN_LINE_SIZE, imeminTextFile);
			if (strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0)	// Ignore blank spaces
				continue;
			strcpy(imemin[i], line);
		}
		else { 
			strcpy(imemin[i], "0");  // Filling imemin array with 0 after end of the file have reached
		}
		i++;
	}
	fclose(imeminTextFile);
}

// Generating dmemin - open dmemin file - make memory array
void Generate_dmemin(int *dmemin, char *file_name) {
	FILE *dmeminTextFile;
	char line[DMEMIN_LINE_SIZE];
	int i = 0;
	if ((dmeminTextFile = fopen(file_name, "r")) == NULL) 
		printf("Error with opening %s file\n", file_name);
	while (!feof(dmeminTextFile)) {	
		fgets(line, DMEMIN_LINE_SIZE, dmeminTextFile); 
		if (strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0)  // Ignore blank spaces
			continue;
		dmemin[i] = Unsign2Sign((int)strtol(line, NULL, 16));
		i++;
	}
	fclose(dmeminTextFile);
}

// Generateing diskin - opens diskin file  -make 2D array of hard memory
void Generate_diskin(int hard_disk[DISK_SECTORS_NUM][DISK_SECTORS_SIZE], char *file_name) {
	FILE *HD_txt_file;  // Hard disk text file
	int num_of_sector = 0;
	int location_in_sector = 0;
	char line[DMEMIN_LINE_SIZE];
	if ((HD_txt_file = fopen(file_name, "r")) == NULL) 
		printf("Error with opening %s file\n", file_name);
	while (!feof(HD_txt_file)) { 
		fgets(line, DMEMIN_LINE_SIZE, HD_txt_file);
		if (strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0)  // Ignore blank spaces
			continue;
		hard_disk[num_of_sector][location_in_sector] = Unsign2Sign((int)strtol(line, NULL, 16));
		if (location_in_sector == 127) {
			num_of_sector = num_of_sector + 1;
			location_in_sector = 0;
		}
		else
			location_in_sector = location_in_sector + 1;
	}
	fclose(HD_txt_file);
}

// Increments clock in IO registerss array
int inc_clk(int clk, MemoryLine ML) {
	if (ML.imm_bit == 1) {
		if (clk > 0xffffffff-2)
			return 0;
		else
			return clk = clk + 2;  // clk += 2
	}
	else {
		if (clk  > 0xffffffff-1)
			return 0;
		else
			return clk = clk + 1;  // clk += 1
	}
}

// Main
int main(int argc, char* argv[]) {
	char imemin[IMEMIN_MAX_SIZE][IMEMIN_LINE_SIZE];		// imemin string array that contains instruction memory
	int memory[DMEMIN_MAX_SIZE] = { 0 };		//	Data Memory
	int regs[NUM_OF_REGISTERS] = { 0 };	//	Registers array
	int IO_Reg[IO_REGISTERS] = { 0 };		//	IO regs array
	int HD[DISK_SECTORS_NUM][DISK_SECTORS_SIZE] = { 0 }; // Hard disk
	int monitor[MONITORY][MONITORX] = { 0 };	// monitor 2D matrix
	int PC = 0;		//	PC current place
	int ML_counter = 0;	// Executed Memory lines counter
	int irq;	// Idicate if interrupt should be executed
	int reti_bit = 1;	// 1 - not busy
	int cur_irq2 = -1;	// Next cycle for irq2 interrupt.
	int clk = 0; //clock counte
	int disk_timer = 0; // Cycles timer - 1024 disk's cycles
	MemoryLine head;
	FILE *hwregtrace;
	FILE *trace;
	FILE *irq2;
	FILE *leds;

	char *imemin_fp = argv[1]; // "imemin.txt"
	char *dmemin_fp = argv[2];  //"dmemin.txt"
	char *diskin_fp = argv[3];  //"diskin.txt"
	char *irq2_fp = argv[4];  // "irq2in.txt"
	char *dmemout_fp = argv[5];  // "dmemout.txt"
	char *regout_fp = argv[6];  // "regout.txt"
	char *trace_fp = argv[7];  // "trace.txt"
	char *hwregtrace_fp = argv[8];  // "hwregtrace.txt"
	char *cycles_fp = argv[9];  //"cycles.txt"
	char *leds_fp = argv[10];  // "leds.txt"
	char *monitor_fp = argv[11];  // "monitor.txt"
	char *monitor_yuv = argv[12];  // "monitor.yuv"
	char *diskout_fp = argv[13];  // "diskout.txt"

	// Fetch instructions and data
	Generate_imemin(imemin, imemin_fp);
	Generate_dmemin(memory, dmemin_fp);
	Generate_diskin(HD, diskin_fp);

	if ((trace = fopen(trace_fp, "w")) == NULL)  // Open trace file
		printf("Error while trying to open trace.txt file\n");
	if ((leds = fopen(leds_fp, "w")) == NULL) // Open leds file
		printf("Error while trying to open leds.txt file\n");
	if ((hwregtrace = fopen(hwregtrace_fp, "w")) == NULL)  // Open hwregtrace file
		printf("Error while trying to open hwregtrace.txt file\n");
	if ((irq2 = fopen(irq2_fp, "r")) == NULL)  // Open irq2in
		printf("Error while trying to open irq2in.txt file\n");


	while (PC != -1) {
		head = str2ML(imemin, PC);  // Extract Memory lines from imemin file
		irq = ((IO_Reg[0] && IO_Reg[3]) || (IO_Reg[1] && IO_Reg[4]) || (IO_Reg[2] && IO_Reg[5]));
		if (irq == 1 && reti_bit == 1)	// Changing the memory line if ready
			head = Manage_Inter(head, &reti_bit, &PC, IO_Reg, imemin);

		if (head.imm_bit == 1)
			regs[1] = head.immi;	// Update the immidate register with new immidate
		cur_irq2 = NEXT_irq2(cur_irq2, IO_Reg[8], irq2, IO_Reg); // Calculate the next cycle of irq2
		UpdateStatus_irq2(cur_irq2, IO_Reg, IO_Reg[8]); // Updatethe status of irq2
		ManageTimer(head, IO_Reg);  // Manage timer for current clock cycle
		ManageDisk(&disk_timer, head, IO_Reg, memory, HD);	// Manage disk situation
		ManageMonitor(head, IO_Reg, monitor);

		char tr_line[155];
		Line2Trace(PC, imemin, regs, tr_line);
		fprintf(trace, tr_line);

		char hwregtrace_line[150];
		if (head.opcode == 20 || head.opcode == 19) {	// When call for the hw register happens
			Line2HWRegTrace(hwregtrace_line, IO_Reg[8], head, IO_Reg, regs);
			fprintf(hwregtrace, hwregtrace_line);
		}

		char led_line[150];
		if (head.opcode == 20 && (regs[head.rs] + regs[head.rt]) == 9) {	// When call for the led register occures
			if (regs[head.rd] != IO_Reg[9]) {	// When the new value of leds register is different from the previous one
				Line2Led(led_line, IO_Reg[8], head, regs);
				fprintf(leds, led_line);
			}
		}

		PC = EXE(regs, IO_Reg, head, PC, memory, HD, &reti_bit);
		ML_counter++;
		clk = inc_clk(IO_Reg[8], head);	// Calculate clock status after last memory line
		IO_Reg[8] = clk;	// Updates clock counter
	}

	Create_diskout(HD, diskout_fp);
	Create_dmemout(memory, dmemout_fp);
	CreateMonitorTXTandYUV(monitor, monitor_fp,monitor_yuv);
	CreateCyclesTXT(IO_Reg[8], ML_counter, cycles_fp);
	Create_Regout(regs, regout_fp);
	fclose(hwregtrace);
	fclose(leds);
	fclose(trace);
	fclose(irq2);
}
