#pragma once
//imports
#include <fstream>
#include <chrono>
#include <random>
#include <cstdint>
#include <cstring>
#include <iostream>

//global variables
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[FONTSET_SIZE]
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xE0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//class initialization
class Chip8
{
public:
	Chip8();
	void LoadROM(char const* filename);

private:
	//member variables
	uint8_t randByte;
	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t video[64 * 32]{};
	uint16_t opcode{};
	std::default_random_engine randGen;
	//TODO: declare RNG
	//std::uniform_int_distribution<unsigned int> randInt;
	std::uniform_int_distribution<uint8_t> randByte;

	//instruction/opcode declaration
	//00E0: CLS - clear the display
	void OP_00E0();
	//00EE: RET -
	void OP_00EE();
	//0nnn: SYS addr -
	void OP_0nnn();
	//1nnn: JP addr -
	void OP_1nnn();
	//2nnn: CALL addr -
	void OP_2nnn();
	//3xkk: SE Vx, byte -
	void OP_3xkk();
	//4xkk: SNE Vx, byte -
	void OP_4xkk();
	//5xy0: SE Vx, Vy -
	void OP_5xy0();
	//6xkk: LD Vx, byte -
	void OP_6xkk();
	//7xkk: ADD Vx, byte -
	void OP_7xkk();
	//8xy0: LD Vx, Vy -
	void OP_8xy0();
	//8xy1: OR Vx, Vy -
	void OP_8xy1();
	//8xy2: AND Vx, Vy -
	void OP_8xy2();
	//8xy3: XOR Vx, Vy -
	void OP_8xy3();
	//8xy4: ADD Vx, Vy -
	void OP_8xy4();
	//8xy5: SUB Vx, Vy -
	void OP_8xy5();
	//8xy6: SHR Vx {, Vy} -
	void OP_8xy6();
	//8xy7: SUBN Vx, Vy -
	void OP_8xy7();
	//8xyE: SHL Vx {, Vy} -
	void OP_8xyE();
	//9xy0: SNE Vx, Vy -
	void OP_9xy0();
	//Annn: LD I, addr -
	void OP_Annn();
	//Bnnn: JP V0, addr -
	void OP_Bnnn();
	//Cxkk: RND Vx, byte -
	void OP_Cxkk();
	//Dxyn: DRW Vx, Vy, nibble -
	void OP_Dxyn();
	//Ex9E: SKP Vx -
	void OP_Ex9E();
	//ExA1: SKNO Vx -
	void OP_ExA1();
	//Fx07: LD Vx, DT -
	void OP_Fx07();
	//Fx0A: LD Vx, K -
	void OP_Fx0A();
	//Fx15: LD DT, Vx -
	void OP_Fx15();
	//Fx18: LD ST, Vx -
	void OP_Fx18();
	//Fx1E: ADD I, Vx -
	void OP_Fx1E();
	//Fx29: LD F, Vx -
	void OP_Fx29();
	//Fx33: LD B, Vx -
	void OP_Fx33();
	//Fx55: LD [I], Vx -
	void OP_Fx55();
	//Fx65: LD Vx, [I] -
	void OP_Fx65();
};

//chip8 constructor
Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	//initialize pc
	pc = START_ADDRESS;

	//loads fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	//TODO: initialize RNG
	//randInt = std::uniform_int_distribution<unsigned int>(0, 255);
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
	//randByte = uint8_t (randInt);
}

void Chip8::LoadROM(char const* filename)
{ 
	//open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	
	if (file.is_open())
	{
		//get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		//go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		//load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		//free the buffer
		delete[] buffer;
	}
}

//instruction/opcode implementation
void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {}
void Chip8::OP_0nnn() {}
void Chip8::OP_1nnn() {}
void Chip8::OP_2nnn() {}
void Chip8::OP_3xkk() {}
void Chip8::OP_4xkk() {}
void Chip8::OP_5xy0() {}
void Chip8::OP_6xkk() {}
void Chip8::OP_7xkk() {}
void Chip8::OP_8xy0() {}
void Chip8::OP_8xy1() {}
void Chip8::OP_8xy2() {}
void Chip8::OP_8xy3() {}
void Chip8::OP_8xy4() {}
void Chip8::OP_8xy5() {}
void Chip8::OP_8xy6() {}
void Chip8::OP_8xy7() {}
void Chip8::OP_8xyE() {}
void Chip8::OP_9xy0() {}
void Chip8::OP_Annn() {}
void Chip8::OP_Bnnn() {}
void Chip8::OP_Cxkk() {}
void Chip8::OP_Dxyn() {}
void Chip8::OP_Ex9E() {}
void Chip8::OP_ExA1() {}
void Chip8::OP_Fx07() {}
void Chip8::OP_Fx0A() {}
void Chip8::OP_Fx15() {}
void Chip8::OP_Fx18() {}
void Chip8::OP_Fx1E() {}
void Chip8::OP_Fx29() {}
void Chip8::OP_Fx33() {}
void Chip8::OP_Fx55() {}
void Chip8::OP_Fx65() {}

int main(int arg, char* argv[])
{

}