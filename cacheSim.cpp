#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cmath>
#include <vector>
#include <list>

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;
using std::vector;
using std::list;
using std::prev;

class block {
public:
    unsigned tag;
    bool dirty;

	block() : tag(0), dirty(false) {}					// default constractor
    
    block(unsigned _tag, char op) : tag(_tag) {			// Constructor
		 (op == 'w') ? dirty = true : dirty = false;
	}
};


bool data_searching (vector<list<block>> &cache, unsigned cur_block, unsigned cache_SetSize, char operation, unsigned &cache_misses);
long l1_block_insert (vector<list<block>> &cache, unsigned cur_block, unsigned cache_SetSize, unsigned cache_Max_Assoc, char operation);
void l2_block_insert (vector<list<block>> &cache, unsigned cur_block, unsigned l2_SetSize, unsigned cache_Max_Assoc, vector<list<block>> &l1cache, unsigned l1_SetSize);



int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}
/////////////////////////////////////// end of given inialization /////////////////////////////////////

	 unsigned l1_Max_Assoc = pow(2, L1Assoc);               //amount of blocks available in the same set
	 unsigned l2_Max_Assoc = pow(2, L2Assoc);
	 unsigned l1_SetNumOfBits = L1Size - BSize - L1Assoc;   //number of bits representing the enteries of the cache
	 unsigned l2_SetNumOfBits = L2Size - BSize - L2Assoc;
	 unsigned l1_SetSize = pow(2, l1_SetNumOfBits);         //the size of the cache
	 unsigned l2_SetSize = pow(2, l2_SetNumOfBits);
	
	 vector<list<block>> l1_cache(l1_SetSize);               // vector of sets, lists represents associativity
	 vector<list<block>> l2_cache(l2_SetSize);               // in each element of the vector (each set), each element holds the value of a block's tag 

	 unsigned l1_miss = 0, l2_miss = 0, TotAccTime = 0, l1_AccessNum = 0, l2_AccessNum = 0, poop = 0; // for statistics

////////////////////////////////////// end of Barak and Shahar's inialization///////////////////////////////
	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}
		string cutAddress = address.substr(2); // Removing the "0x" part of the address	
		unsigned address_val = strtoul(cutAddress.c_str(), NULL, 16);
		unsigned cur_block = (address_val >> BSize);
		
					/*    THE HEART OF THE IMPLEMENTATION - THE GOOD FART    */
		
		l1_AccessNum++;
		TotAccTime += L1Cyc;

		if( !data_searching (l1_cache, cur_block, l1_SetSize, operation , l1_miss)){ //if block isn't in L1

			l2_AccessNum++;
			TotAccTime += L2Cyc;

			if( (WrAlloc == true) || (operation == 'r') ){ // "write back":   after remove of modified block in l1 (snooping),   put it at the head of the list in l2
				long dirtyBlock = l1_block_insert(l1_cache, cur_block, l1_SetSize, l1_Max_Assoc, operation);
				
				if (dirtyBlock != -1)
					data_searching (l2_cache, (unsigned)dirtyBlock, l2_SetSize, 'r', poop);
			} 

			if( !data_searching (l2_cache, cur_block, l2_SetSize,'r' , l2_miss) ){ // if block isn't in L2 
				
				TotAccTime += MemCyc;

				if( (WrAlloc == true) || (operation == 'r') )
					l2_block_insert(l2_cache, cur_block, l2_SetSize, l2_Max_Assoc, l1_cache,l1_SetSize);
			}
		}
	}

	double L1MissRate = ((double)l1_miss / (double)l1_AccessNum);
	double L2MissRate = ((double)l2_miss / (double)l2_AccessNum);
	double avgAccTime = ((double)TotAccTime / (double)l1_AccessNum);

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}


// Returnes if the dezired adress is in the cache.
bool data_searching (vector<list<block>> &cache, unsigned cur_block, unsigned cache_SetSize, 
					char operation, unsigned &cache_misses){

	unsigned cur_set = cur_block % cache_SetSize;   
	unsigned cur_tag = cur_block / cache_SetSize;
		
	for (auto it = cache[cur_set].begin(); it != cache[cur_set].end(); it++){        //looking for the dezired block inside the list (lists represents associativity)

		if (cur_tag == it->tag){
			//cout << " hit ";
			if(operation == 'w')
				it->dirty = true;                                                     
			cache[cur_set].splice(cache[cur_set].begin(), cache[cur_set], it);       //change location to the beggining of the list for keeping LRU method
			return true;
		}
	}

	cache_misses++; 
	return false;
}


long l1_block_insert (vector<list<block>> &cache, unsigned cur_block, unsigned cache_SetSize, unsigned cache_Max_Assoc, char operation){
	
	unsigned cur_set = cur_block % cache_SetSize;   
	unsigned cur_tag = cur_block / cache_SetSize;
	long dirtyBlock = -1;

	block newBlock(cur_tag, operation);
	cache[cur_set].push_front(newBlock);               //adds a new block to the list (represented by it's tag)

	if (cache[cur_set].size() == cache_Max_Assoc + 1){
		
		if (cache[cur_set].back().dirty == true) 										// for implementing "write back"
			dirtyBlock = cache[cur_set].back().tag * cache_SetSize;
		
		cache[cur_set].pop_back();
	}
	return dirtyBlock;
}


void l2_block_insert (vector<list<block>> &cache, unsigned cur_block, unsigned l2_SetSize, unsigned cache_Max_Assoc, 
					vector<list<block>> &l1cache, unsigned l1_SetSize){

	unsigned cur_set = cur_block % l2_SetSize;   
	unsigned cur_tag = cur_block / l2_SetSize;
	
		block newBlock(cur_tag, 'r');
		cache[cur_set].push_front(newBlock);               //adds a new block to the list (represented by it's tag)

	if (cache[cur_set].size() == cache_Max_Assoc + 1){

		unsigned l1cur_set = cur_block % l1_SetSize;/////////////////////////////////accessing l1 for snooping
		//unsigned l1cur_tag = cur_block / l1_SetSize;
	
		for (auto it = l1cache[l1cur_set].begin(); it != l1cache[l1cur_set].end(); it++){
			
			if (it->tag == cache[cur_set].back().tag){
				l1cache[l1cur_set].erase(it);
				break;
			}
		} /////////////////////////////////////////////////////////////////////////////
		
		cache[cur_set].pop_back();
	}
}
