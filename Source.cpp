//#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;
int block_size, cash_size, number_of_blocks = 0;
int EXP, lvl = 0;
int cash[3][100000];
int set_cash[6][90000][16];

#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)

enum cacheResType { MISS = 0, HIT = 1 };

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr = 0;
	return (addr += (16 * 1024)) % (256 * 1024);
}

unsigned int memGen2()
{
	static unsigned int addr = 0;
	return (addr += 4) % (DRAM_SIZE);
}

unsigned int memGen3()
{
	return rand_() % (256 * 1024);
}

/////////////////////////////////////////
//////////// Cache Simulator/////////////
/////////////////////////////////////////
cacheResType cacheSim(unsigned int addr, int cash[3][100000], int type, int &block_counter, int index, int tag)
{
	int shift_offset = log2(block_size);
	bool detected = false;
	bool misses_flag = true;
	//////////////////////////////////////
	////////// Direct Mapped//////////////
	//////////////////////////////////////
	if (EXP == 1)
	{
		if (cash[0][index] == tag)
			return HIT;
		else
		{
			cash[0][index] = tag;
			return MISS;
		}
	}
	else if (EXP == 2) {
		if (cash[lvl][index] == tag)
			return HIT;
		else
		{
			cash[lvl][index] = tag;
			return MISS;
		}
	}

	//////////////////////////////////////
	/////////// fully associative/////////
	//////////////////////////////////////
	else if ((EXP == 3) || (EXP == 4))
	{

		if (type == 0)    // LRU /////////
		{
			if (block_counter < number_of_blocks)
			{
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == addr >> shift_offset)
					{
						detected = true;
						cash[1][i] = block_counter;
						block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					cash[0][block_counter] = addr >> shift_offset;
					cash[1][block_counter] = block_counter;
					return MISS;  //miss
				}
			}
			else  // block counter is more than block size
			{
				//check for existence
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == (addr >> shift_offset))
					{
						detected = true;
						cash[1][i] = block_counter;
						//block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					int compare = 0;
					for (int i = 1; i < number_of_blocks; i++)
					{
						if (cash[1][compare] > cash[1][i])
							compare = i;
					}
					cash[0][compare] = addr >> shift_offset;
					cash[1][compare] = block_counter;
					return MISS; //miss

				}
			}
		}   // end of LRU

		else if (type == 1)   // LFU ///////////////
		{
			if (block_counter < number_of_blocks)
			{
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == addr >> shift_offset)
					{
						detected = true;
						cash[1][i] = cash[1][i] + 1;
						block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					cash[0][block_counter] = addr >> shift_offset;
					cash[1][block_counter] = -1;
					return MISS;  //miss
				}
			}
			else  // block counter is more than block size
			{
				//check for existence
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == (addr >> shift_offset))
					{
						detected = true;
						cash[1][i]++;
						block_counter--;
						return HIT; //hit
					}
				}
				if (!detected)
				{
					int compare2 = 0;
					for (int i = 1; i < number_of_blocks; i++)
					{
						if (cash[1][compare2] >= cash[1][i])
							compare2 = i;
					}
					cash[0][compare2] = addr >> shift_offset;
					cash[1][compare2] = -1;
					return MISS; //miss
				}
			}

		}  // end if LFU

			/////////////////////////////////
			////////Random //////////////////
			/////////////////////////////////
		else if (type == 2)
		{
			if (block_counter < number_of_blocks)
			{
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == addr >> shift_offset)
					{
						detected = true;
						block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					cash[0][block_counter] = addr >> shift_offset;
					return MISS;  //miss
				}
			}
			else  // block counter is more than block size
			{
				//check for existence
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == (addr >> shift_offset))
					{
						detected = true;
						//block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					srand(time(NULL));
					cash[0][rand() % number_of_blocks] = addr >> shift_offset;
					return MISS; //miss
				}
			}
		}

		////////////////////////////////////////////
		//////////////// MRU //////////////////////
		///////////////////////////////////////////
		else if (type == 3)
		{
			if (block_counter < number_of_blocks)
			{
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == addr >> shift_offset)
					{
						detected = true;
						cash[1][i] = block_counter;
						block_counter--;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					cash[0][block_counter] = addr >> shift_offset;
					cash[1][block_counter] = block_counter;
					return MISS;  //miss
				}
			}
			else  // block counter is more than block size
			{
				//check for existence
				for (int i = 0; i < number_of_blocks; i++)
				{
					if (cash[0][i] == (addr >> shift_offset))
					{
						detected = true;
						cash[1][i] = block_counter;
						return HIT; //hit
					}
				}

				if (!detected)
				{
					int compare = 0;
					for (int i = 1; i < number_of_blocks; i++)
					{
						if (cash[1][compare] < cash[1][i])
							compare = i;
					}
					cash[0][compare] = addr >> shift_offset;
					cash[1][compare] = block_counter;
					return MISS; //miss

				}
			}
		}

	}  // end of Fully associative
		//////////////////////////////////////
	/////////// set associative///////////
	//////////////////////////////////////

	else if (EXP == 5)
	{

		index = index * addr;
		for (int i = 0; i < addr; i++)
		{
			if (cash[0][index + i] == tag)
			{
				block_counter--;
				return HIT;
			}
		}
		for (int i = 0; i < addr; i++)
		{
			if (cash[1][index + i] == -1)
			{
				cash[0][index + i] = tag;
				cash[1][index + i] = block_counter;
				return MISS;
			}
		}

		int compare = index;
		int m = 0;
		for (int i = 0; i < addr; i++)
		{
			if (cash[1][compare + m] > cash[1][index + i])
			{
				m = i;
			}
		}
		cash[0][compare + m] = tag;
		cash[1][compare + m] = block_counter;
		return MISS; //miss
	}

	else if (EXP == 6) {
		//index = index * addr;
		for (int i = 0; i < addr; i++)
		{
			if (set_cash[lvl][index][i] == tag)
			{
				block_counter--;
				return HIT;
			}
		}
		for (int i = 0; i < addr; i++)
		{
			if (set_cash[lvl + 1][index][i] == -1)
			{
				set_cash[lvl][index][i] = tag;
				set_cash[lvl + 1][index][i] = block_counter;
				return MISS;
			}
		}

		int compare = index;
		int m = 0;
		for (int i = 0; i < addr; i++)
		{
			if (set_cash[lvl + 1][compare][m] > set_cash[lvl + 1][index][i])
			{
				m = i;
			}
		}
		set_cash[lvl][compare][m] = tag;
		set_cash[lvl + 1][compare][m] = block_counter;
		return MISS; //miss

	}


	return HIT;
}

string msg[2] = { "Miss","Hit" };

//bool found(int index, int tag, int lvl) {
//	if (cash[lvl][index] == tag)
//		return HIT;
//	else
//	{
//		cash[lvl][index] = tag;
//		return MISS;
//	}
//}

int main(int argc, const char * argv[]) {

	int iter;
	cacheResType r;
	double hit_ratio = 0.0;
	unsigned int addr;
	int iterations = 1000000, shift;


	cout << "Please choose the expriment " << endl;
	cin >> EXP;
	

	int block_counter = 0;
	double hits = 0;
	int index = 0, tag = 0;
	int L2count = 0, L3count = 0;

	///////////////////////////////////////////////////////////////////
	if (EXP == 1)   //Direct_mapped
	{

		block_size = 4;

		//////////////////
		for (int k = 0; k < 5; k++) {
			memset(cash, -1, sizeof cash);
			for (int i = 0; i < 3; i++) {
				hits = 0;
				number_of_blocks = (64 * 1024) / block_size;

				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					shift = log2(block_size);
					index = (addr >> shift) % number_of_blocks;
					shift = log2(number_of_blocks + block_size);
					tag = addr >> shift;    // shifted the amount the offset and the index sizes
					r = cacheSim(addr, cash, 0, block_counter, index, tag);
					index = 0;
					tag = 0;
					//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
					if (msg[r] == "Hit")
					{
						hits++;
					}
				}

				hit_ratio = hits / iterations;
				cout << "Hit ratio " << hit_ratio << endl;
			}
			block_size *= 2;
			cout << '\n';
		}
	}

	if (EXP == 2)   //Direct_mapped
	{
		//memset(cash, -1, sizeof cash);
		block_size = 64;
		double hits1 = 0.0, hits2 = 0.0, hits3 = 0.0, miss1, miss2, miss3, amat1, amat2, amat3;
		int lvls = 3;

		//////////////////////////
		////////// 1 level////////
		/////////////////////////
		if (lvls == 1) {
			for (int i = 0; i < 3; i++) {
				memset(cash, -1, sizeof cash);
				hits1 = 0;
				hits2 = 0;
				hits3 = 0;
				L2count = 0;
				L3count = 0;

				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					number_of_blocks = (256 * 1024) / block_size;
					shift = log2(block_size);
					index = (addr >> shift) % number_of_blocks;
					shift = log2(number_of_blocks + 64);
					tag = addr >> shift;    // shifted the amount the offset and the index sizes
					lvl = 0;
					r = cacheSim(addr, cash, 0, block_counter, index, tag);
					
					if (msg[r] == "Hit")
					{
						hits1++;
					}

				}
				hit_ratio = hits1 / iterations;
				miss1 = 1 - hit_ratio;

				amat1 = 11 + miss1 * 100;
				cout << "memgen " << i << endl;
				cout << "access time Level1  " << amat1 << endl;

			}
		}
		////////////////////////////
		/////////// 2 levels////////
		////////////////////////////
		if (lvls == 2) {
			for (int i = 0; i < 3; i++) {
				memset(cash, -1, sizeof cash);
				hits1 = 0;
				hits2 = 0;
				hits3 = 0;
				L2count = 0;
				L3count = 0;

				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					number_of_blocks = (32 * 1024) / block_size;
					shift = log2(block_size);
					index = (addr >> shift) % number_of_blocks;
					shift = log2(number_of_blocks + 64);
					tag = addr >> shift;    // shifted the amount the offset and the index sizes
					lvl = 0;
					r = cacheSim(addr, cash, 0, block_counter, index, tag);
					
					if (msg[r] == "Hit")
					{
						hits1++;
					}
					else
					{
						number_of_blocks = (256 * 1024) / block_size;
						shift = log2(block_size);
						index = (addr >> shift) % number_of_blocks;
						shift = log2(number_of_blocks + 64);
						tag = addr >> shift;    // shifted the amount the offset and the index sizes
						lvl = 1;
						r = cacheSim(addr, cash, 0, block_counter, index, tag);
						L2count++;
						
						if (msg[r] == "Hit")
						{
							hits2++;
						}

					}
				}
				hit_ratio = hits1 / iterations;
				miss1 = 1 - hit_ratio;
				miss2 = (L2count - hits2) / L2count;

				amat2 = 11 + miss2 * 100;
				amat1 = 4 + miss1 * amat2;
				cout << "memgen " << i << endl;
				cout << "access time Level1  " << amat1 << endl;

			}
		}
		/////////////////////////
		////////// 3 levels//////
		//////////////////////////
		if (lvls == 3) {
			for (int i = 0; i < 3; i++) {
				memset(cash, -1, sizeof cash);
				hits1 = 0;
				hits2 = 0;
				hits3 = 0;
				L2count = 0;
				L3count = 0;

				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					number_of_blocks = (32 * 1024) / block_size;
					shift = log2(block_size);
					index = (addr >> shift) % number_of_blocks;
					shift = log2(number_of_blocks + 64);
					tag = addr >> shift;    // shifted the amount the offset and the index sizes
					lvl = 0;
					r = cacheSim(addr, cash, 0, block_counter, index, tag);
					
					if (msg[r] == "Hit")
					{
						hits1++;
					}
					else
					{
						number_of_blocks = (256 * 1024) / block_size;
						shift = log2(block_size);
						index = (addr >> shift) % number_of_blocks;
						shift = log2(number_of_blocks + 64);
						tag = addr >> shift;    // shifted the amount the offset and the index sizes
						lvl = 1;
						r = cacheSim(addr, cash, 0, block_counter, index, tag);
						L2count++;
						
						if (msg[r] == "Hit")
						{
							hits2++;
						}
						else {
							number_of_blocks = (8192 * 1024) / block_size;
							shift = log2(block_size);
							index = (addr >> shift) % number_of_blocks;
							shift = log2(number_of_blocks + 64);
							tag = addr >> shift;    // shifted the amount the offset and the index sizes
							lvl = 2;

							r = cacheSim(addr, cash, 0, block_counter, index, tag);
							L3count++;
							//index = 0;
							//tag = 0;
							if (msg[r] == "Hit")
							{
								hits3++;
							}

						}

					}
				}
				hit_ratio = hits1 / iterations;
				miss1 = 1 - hit_ratio;
				miss2 = (L2count - hits2) / L2count;
				miss3 = (L3count - hits3) / L3count;
				amat3 = 30 + miss3 * 100;
				amat2 = 11 + miss2 * amat3;
				amat1 = 4 + miss1 * amat2;
				cout << "memgen " << i << endl;
				cout << "access time Level1  " << amat1 << endl;

			}


		}

	}
	///////////////////////////////////////////////////////////////////
	else if (EXP == 3)  //  Fully associative
	{
		block_size = 16;
		cash_size = 4;

		for (int k = 0; k < 5; k++) {
			for (int i = 0; i < 3; i++) {
				block_counter = 0;
				hits = 0;
				memset(cash, -1, sizeof cash);
				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					number_of_blocks = (cash_size * 1024) / 16;
					r = cacheSim(addr, cash, 3, block_counter, index, tag);
					// cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[flag] <<")\n";

					if (msg[r] == "Hit")
					{
						hits++;
					}
					block_counter++;

				}

				hit_ratio = hits / iterations;
				cout << "Hit ratio " << hit_ratio << endl;

			} // end of fully associative main
			cout << '\n';
			cash_size *= 2;
		}

	}

	else if (EXP == 4)  //  Fully associative
	{
		block_size = 32;
		cash_size = 256;

		for (int k = 0; k <= 2; k++) {
			for (int i = 0; i < 3; i++) {
				block_counter = 0;
				hits = 0;
				memset(cash, -1, sizeof cash);
				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					number_of_blocks = (cash_size * 1024) / 16;
					r = cacheSim(addr, cash, k, block_counter, index, tag);
					// cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[flag] <<")\n";

					if (msg[r] == "Hit")
					{
						hits++;
					}
					block_counter++;

				}

				hit_ratio = hits / iterations;
				cout << "Hit ratio " << hit_ratio << endl;

			} // end of fully associative main

			for (int i = 0; i < 3; i++) {
				block_counter = 0;
				hits = 0;
				int hitsP = 0;
				int counts = 0;
				bool flag = true;
				memset(cash, -1, sizeof cash);
				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}

					if (counts == 5000 && (hitsP / counts * 1.0) < 0.3)
					{
						counts = 0;
						hitsP = 0;
						flag = !flag;
					}
					number_of_blocks = (cash_size * 1024) / 16;
					r = cacheSim(addr, cash, flag, block_counter, index, tag);
					// cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[flag] <<")\n";
					counts++;
					if (msg[r] == "Hit")
					{
						hitsP++;

						hits++;
					}
					block_counter++;

				}

				hit_ratio = hits / iterations;
				cout << "Hit ratio " << hit_ratio << endl;

			} // end of fully associative main

			cout << '\n';
		}
		for (int i = 0; i < 3; i++) {
			block_counter = 0;
			hits = 0;
			memset(cash, -1, sizeof cash);
			for (int iter = 0; iter < iterations; iter++)
			{
				if (i == 0) {
					addr = memGen1();
				}
				if (i == 1) {
					addr = memGen2();
				}
				if (i == 2) {
					addr = memGen3();
				}

				number_of_blocks = (cash_size * 1024) / 16;
				r = cacheSim(addr, cash, 3, block_counter, index, tag);

				if (msg[r] == "Hit")
				{
					hits++;
				}
				block_counter++;


			}
		}
	}

	else if (EXP == 5) // set associative
	{
		int no_of_ways;
		no_of_ways = 2;

		for (int k = 0; k < 4; k++) {
			number_of_blocks = (16 * 1024) / (8 * no_of_ways);
			block_size = 8;
			for (int i = 0; i < 3; i++) {
				hits = 0;
				block_counter = 0;
				memset(cash, -1, sizeof cash);

				for (int iter = 0; iter < iterations; iter++)
				{
					if (i == 0) {
						addr = memGen1();
					}
					if (i == 1) {
						addr = memGen2();
					}
					if (i == 2) {
						addr = memGen3();
					}
					shift = log2(block_size);
					index = (addr >> shift) % (number_of_blocks);
					shift = log2(number_of_blocks + block_size);
					tag = addr >> shift;
					r = cacheSim(no_of_ways, cash, 5, block_counter, index, tag);
					index = 0;
					tag = 0;
					if (msg[r] == "Hit")
					{
						hits++;
					}
					block_counter++;
				}
				hit_ratio = hits / iterations;
				cout << "Hit ratio " << hit_ratio << endl;
			}
			no_of_ways *= 2;
			cout << endl;
		}
	}

	else if (EXP == 6) // set associative
	{
		int no_of_ways;
		no_of_ways = 2;
		block_size = 64;
		double hits1 = 0.0, hits2 = 0.0, hits3 = 0.0, miss1, miss2, miss3, amat1, amat2, amat3;


		number_of_blocks = (16 * 1024) / (8 * no_of_ways);
		for (int lvls = 1; lvls <= 3; lvls++) {
			//////////////////////////
			////////// 1 level////////
			/////////////////////////
			if (lvls == 1) {
				for (int i = 0; i < 3; i++) {
					memset(set_cash, -1, sizeof set_cash);
					hits1 = 0;
					block_counter = 0;



					for (int iter = 0; iter < iterations; iter++)
					{
						if (i == 0) {
							addr = memGen1();
						}
						if (i == 1) {
							addr = memGen2();
						}
						if (i == 2) {
							addr = memGen3();
						}
						number_of_blocks = (256 * 1024) / (block_size * 8);
						shift = log2(block_size);
						index = (addr >> shift) % number_of_blocks;
						shift = log2(number_of_blocks + 64);
						tag = addr >> shift;    // shifted the amount the offset and the index sizes
						lvl = 0;
						r = cacheSim(8, cash, 6, block_counter, index, tag);
						block_counter++;

						if (msg[r] == "Hit")
						{
							hits1++;
						}

					}
					hit_ratio = hits1 / iterations;
					miss1 = 1 - hit_ratio;

					amat1 = 11 + miss1 * 100;
					cout << "memgen " << i << endl;
					cout << "access time Level1  " << amat1 << endl;

				}
			}
			////////////////////////////
			/////////// 2 levels////////
			////////////////////////////
			if (lvls == 2) {
				for (int i = 0; i < 3; i++) {
					memset(set_cash, -1, sizeof set_cash);
					hits1 = 0;
					hits2 = 0;
					L2count = 0;

					block_counter = 0;

					for (int iter = 0; iter < iterations; iter++)
					{
						if (i == 0) {
							addr = memGen1();
						}
						if (i == 1) {
							addr = memGen2();
						}
						if (i == 2) {
							addr = memGen3();
						}
						number_of_blocks = (32 * 1024) / (block_size * 8);
						shift = log2(block_size);
						index = (addr >> shift) % number_of_blocks;
						shift = log2(number_of_blocks + 64);
						tag = addr >> shift;    // shifted the amount the offset and the index sizes
						lvl = 0;
						r = cacheSim(8, cash, 6, block_counter, index, tag);
						block_counter++;

						if (msg[r] == "Hit")
						{
							hits1++;
						}
						else
						{
							number_of_blocks = (256 * 1024) / (block_size * 8);
							shift = log2(block_size);
							index = (addr >> shift) % number_of_blocks;
							shift = log2(number_of_blocks + 64);
							tag = addr >> shift;    // shifted the amount the offset and the index sizes
							lvl = 2;
							r = cacheSim(8, cash, 6, block_counter, index, tag);
							L2count++;
							block_counter++;

							if (msg[r] == "Hit")
							{
								hits2++;
							}

						}
					}
					hit_ratio = hits1 / iterations;
					miss1 = 1 - hit_ratio;
					miss2 = (L2count - hits2) / L2count;

					amat2 = 11 + miss2 * 100;
					amat1 = 4 + miss1 * amat2;
					cout << "memgen " << i << endl;
					cout << "access time Level1  " << amat1 << endl;

				}
			}
			/////////////////////////
			////////// 3 levels//////
			//////////////////////////
			if (lvls == 3) {
				for (int i = 0; i < 3; i++) {
					memset(set_cash, -1, sizeof set_cash);
					hits1 = 0;
					hits2 = 0;
					hits3 = 0;
					L2count = 0;
					L3count = 0;
					block_counter = 0;

					for (int iter = 0; iter < iterations; iter++)
					{
						if (i == 0) {
							addr = memGen1();
						}
						if (i == 1) {
							addr = memGen2();
						}
						if (i == 2) {
							addr = memGen3();
						}
						number_of_blocks = (32 * 1024) / (block_size * 8);
						shift = log2(block_size);
						index = (addr >> shift) % number_of_blocks;
						shift = log2(number_of_blocks + 64);
						tag = addr >> shift;    // shifted the amount the offset and the index sizes
						lvl = 0;
						r = cacheSim(8, cash, 6, block_counter, index, tag);
						block_counter++;

						if (msg[r] == "Hit")
						{
							hits1++;
						}
						else
						{
							number_of_blocks = (256 * 1024) / (block_size * 8);
							shift = log2(block_size);
							index = (addr >> shift) % number_of_blocks;
							shift = log2(number_of_blocks + 64);
							tag = addr >> shift;    // shifted the amount the offset and the index sizes
							lvl = 2;
							r = cacheSim(8, cash, 6, block_counter, index, tag);
							L2count++;
							block_counter++;


							if (msg[r] == "Hit")
							{
								hits2++;
							}
							else {
								number_of_blocks = (8192 * 1024) / (block_size * 16);
								shift = log2(block_size);
								index = (addr >> shift) % number_of_blocks;
								shift = log2(number_of_blocks + 64);
								tag = addr >> shift;    // shifted the amount the offset and the index sizes
								lvl = 4;

								r = cacheSim(16, cash, 6, block_counter, index, tag);
								L3count++;
								block_counter++;


								if (msg[r] == "Hit")
								{
									hits3++;
								}

							}

						}
					}
					hit_ratio = hits1 / iterations;
					miss1 = 1 - hit_ratio;
					miss2 = (L2count - hits2) / L2count;
					miss3 = (L3count - hits3) / L3count;
					amat3 = 30 + miss3 * 100;
					amat2 = 11 + miss2 * amat3;
					amat1 = 4 + miss1 * amat2;
					cout << "memgen " << i << endl;
					cout << "access time Level1  " << amat1 << endl;
					

				}
				

			}
			cout << endl;
		}
	}

	system("pause");
}