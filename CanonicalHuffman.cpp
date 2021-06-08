//Project Static Huffman Coding, Using Canonical Huffman Code instead of Normal Huffman Codebook
//Nguyen Thai Son - 931924
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>

using namespace std;

//Define Const
#define MAX_NODE 511 //2*256-1 (2n-1)

//NODE: struct for node in Huffman Tree
struct NODE {
    unsigned char	c;		// character
	int		freq;	// Frequency
	bool	used;	// Flag to check if node is used(True) or not(False)
	//Using (int) - ASCII code of next NODE - as index
	int		nLeft;	// Left node
	int		nRight;	// Right node
};

//Bit Length to save the info for rebuild tree in header
struct BitCode {
	string code;
	int length;
};

struct Header {
	char c; 	//character
	int len;	//len
};

//The Huffman Tree
class Huffman {
		NODE huffTree [MAX_NODE];
		BitCode Bit[256];
		Header* Head;
		int UsedChar;
		string data;
		long int data_len;
	public:
		Huffman();
		~Huffman() {
			delete [] Head;
		};
		void PrintFreqTable();
		//Input from *filename for Encoding
		void EncodeInput(const char* filename);
		//Input from *filename for Decoding
		string DecodeInput(const char* filename);
		bool Get2MinElements(int &i,int &j,int NodeN);
		int CreateHuffmanTree();
		void PrintTree(int node, int tab);
		void HuffmanTraverse(int node, int nbit, int & max_length);
		int InitiateBitcodeTable(int root);
		void CreateBitcodeTable(int max_length);
		void PrintBitcodeTable();
		string FromCharToCode();
		string ConvertStringToBit(const string codestring, int& remainbit);
		string ConvertBitToString(const string bitstring, int remainbit);
		string FromCodeToChar(string code,char max_len);
		//Encode to file *filename
		void Encode(const char* filename);
		//Decode to file *filename
		void Decode(const char* ifname, const char* ofname);
};

Huffman::Huffman() {
	for (int i = 0; i < MAX_NODE; i++) {
		huffTree[i].c = i; 			//(int) to (unsigned char)
		huffTree[i].freq = 0;
		huffTree[i].used = false;
		huffTree[i].nLeft = -1;
		huffTree[i].nRight = -1;
	}
}

void Huffman::EncodeInput(const char* filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {cout<<"Can't open this file"<<endl; return;}
	char c;
	long int nChar=0;
	//Read Frequency
	while (ifs.get(c)) {
		data.push_back(c);
		huffTree[c].freq ++;
		nChar++;
	}

	data_len=nChar;
	ifs.close();
	cout<<"Data:"<<data<<endl;

}

void Huffman::PrintFreqTable() {
	int count=0;
	cout<<"Frequency Table: \n";
	for (int i = 0; i < 256; i++) {
		if (huffTree[i].freq > 0) {
			count++;
			cout<<(char)i<<": "<<huffTree[i].freq<<endl;
		}
	}
	cout<<endl;
	UsedChar=count;
	Head=new Header[UsedChar];
}

bool Huffman::Get2MinElements(int &i,int &j,int NodeN) {
	i = -1;
	j = -1;

// Find 2 Elements with Smallest Frequency
	for (int k = 0; k < NodeN; k++)
		if (huffTree[k].used == false && huffTree[k].freq > 0) { //check k is used or not, and is it in the file or not
			if (i == -1) {
				i = k;
			}
			else if (j == -1) {
				j = k;
			}
			else {
				if (huffTree[i].freq > huffTree[j].freq) {		//if (i>j) && (i>k) -> i=k
					if (huffTree[k].freq < huffTree[i].freq) {
						i = k;
					}
				}
				else {											//if (i<j) && (k<j) -> j=k
					if (huffTree[k].freq < huffTree[j].freq) {
						j = k;
					}
				}
			}
		}
/* Rearrange i and j, as i<j
	There are 2 cases that we have to change:
		huffTree[i].freq > huffTree[j].freq
		huffTree[i].freq == huffTree[j].freq && (huffTree[i].c > huffTree[j].c) - sort by ASCII(or alphabet) order
*/
	if (i != -1 && j!= -1) {
		if ( (huffTree[i].freq > huffTree[j].freq) || ((huffTree[i].freq > huffTree[j].freq) && (huffTree[i].c > huffTree[j].c))) {
			int temp = i;
			i = j;
			j = temp;
		}
		return true;
	}
	else {
		return false;
	}
}

int Huffman::CreateHuffmanTree() {
	int NodeN = 256;
	int i, j;
	bool found = false; //flag to check that we can found 2 min or not
	while (true) {
		found = Get2MinElements(i, j, NodeN);
		if (!found) {
			break;
		}

		// If found:
		huffTree[NodeN].c = (huffTree[i].c < huffTree[j].c) ? huffTree[i].c : huffTree[j].c; //new c will be the smaller
		huffTree[NodeN].freq = huffTree[i].freq + huffTree[j].freq; //sum of 2 child's freq
		huffTree[NodeN].nLeft = i;
		huffTree[NodeN].nRight = j;

		// Mark node i, node j as used
		huffTree[i].used = true;
		huffTree[j].used = true;

		//Mark node NodeN isn't used
		huffTree[NodeN].used = false;
		NodeN++;

	}
	return NodeN - 1; //This will be the Root Node
}

void Huffman::PrintTree(int node, int tab) {
	if (node == -1) {
		return;
	}
	for (int i = 0; i < tab; i++) {
		cout<<"\t";
	}
	if (huffTree[node].nLeft == -1 && huffTree[node].nRight == -1)	{
		if (huffTree[node].c!='\n') cout<<huffTree[node].c<<endl;
		else cout<<'\\'<<'n'<<endl;
	}
	else {
		if (huffTree[node].c!='\n') cout<<huffTree[node].c<<"..\n";
		else cout<<'\\'<<'n'<<"..\n";
		PrintTree(huffTree[node].nLeft, tab + 1);
		PrintTree(huffTree[node].nRight, tab + 1);
	}
}

void Huffman::HuffmanTraverse(int node, int nbit, int &max_length) { //Save the bit Length to build bitcode with Canonical Huffman
	if (nbit>max_length) max_length=nbit;

	if (node == -1) {
		return;
	}


	if (huffTree[node].nLeft == -1 && huffTree[node].nRight == -1) { //if leaf
		Bit[node].length = nbit;
		return;
	}
	else {

		HuffmanTraverse(huffTree[node].nLeft, nbit + 1,max_length);

		HuffmanTraverse(huffTree[node].nRight, nbit + 1,max_length);
	}
}

int Huffman::InitiateBitcodeTable(int root) {
	//Initiation
	for (int i = 0; i < 256; i++) {
		Bit[i].length = 0;
	}

	//Get length table
	int nbit=0;
	int max_length=0;
	HuffmanTraverse(root,nbit,max_length);
	return max_length;
}

void Huffman::CreateBitcodeTable(int max_length) {
	//-----------Create Canonical Bitcode-------------
	//Get the min length of bitcode
	int min_length=1;
	while (min_length<=max_length) {
		for (int i=0;i<256;i++) {
			if (Bit[i].length==min_length) {goto MINFOUND;} //escape the while loop
		}
		min_length++;
	}

	MINFOUND:
	//Create Header Table
	int index=0;
	for (int len=min_length; len<=max_length; len++) {
		for (int j=0;j<256;j++){
			if (Bit[j].length==len) {
				Head[index].len=len;
				Head[index].c=j;
				index++;
			}
		}
	}

	//Generate codebook base on Header
	int mask=0;
	for(int i=0;i<UsedChar;i++) {
		if (i==0) {		//The First Element's code will be the [len] of bit 0;
			for (int j=0;j<Head[i].len;j++) Bit[Head[i].c].code.append("0");
		}
		else {		//Current Element's code if not the First
			mask++;	//Increase the bit code by 1
			//if len increase, left shift mask with len_shift = amount of len increment
			mask=mask<<(Head[i].len-Head[i-1].len);
			//convert mask from int to string
			for (int j=Head[i].len-1;j>=0;j--) {

				Bit[Head[i].c].code.push_back((char)('0'+(mask>>j & 1)));
			}
		}
	}
}

void Huffman::PrintBitcodeTable() {
	for(int i=0;i<UsedChar;i++) {
		if (Head[i].c!='\n') cout<<Head[i].c<<": ";
		else cout<<'\\'<<'n'<<": ";
		cout<<Bit[Head[i].c].code;
		cout<<endl;
	}
}

string Huffman::FromCharToCode() {
	string temp;
	for (long int i=0; i<data_len; i++){
		temp+=Bit[data[i]].code;
	}
	return temp;
}

string Huffman::ConvertStringToBit(const string codestring, int& remainbit){
	char temp;
	string ret;
	int m;
	int nbit=codestring.length();
	if (nbit%8==0)  m=nbit/8; else m=nbit/8+1;
	int index=0;
	for(int part=0;part<m-1;part++){
		temp=0;
        for(int bit=0;bit<8;bit++){
            index=(part*8+bit);
            temp |= ((codestring[index] - '0') << bit);
        }
		ret.push_back(temp);
	}
	temp=0;
	remainbit=(m*8-nbit);
	for(int bit=0;bit<8-remainbit;bit++){
            index=((m-1)*8+bit);
            temp |= ((codestring[index] - '0') << bit);
    }
	temp=temp<<remainbit;
	ret.push_back(temp);
	return ret;
}

string Huffman::ConvertBitToString(const string bitstring, int remainbit) {
	char temp;
	string ret;
	int len = bitstring.length();

    for (int part = 0; part < len - 1; part++){
        for (int bit=0;bit<8;bit++) {
			temp=((bitstring[part]>>bit&1)+'0');
			ret.push_back(temp);
		}
    }
	char last=bitstring[len-1]>>remainbit;
	for (int bit=0;bit<8-remainbit;bit++) {
			temp=((last>>bit&1)+'0');
			ret.push_back(temp);
		}
    return ret;
}

void Huffman::Encode(const char* filename) {
	//Already Initiated and Input data from file
	cout<<"Preparing for Encoding!"<<endl;

	//Step 1: Frequency Table
	cout<<"Frequency Table has been created!"<<endl;
	PrintFreqTable();

	//Step 2: The Huffman Tree!
	int Root= CreateHuffmanTree();
	cout<<endl<<"Huffman Tree has been created!"<<endl;
	PrintTree(Root,0);

	//Step 3: The codebook
	int ml=InitiateBitcodeTable(Root);
	CreateBitcodeTable(ml);
	cout<<endl<<"Codebook has been created!"<<endl;
	PrintBitcodeTable();

	//Step 4:Choose Header type and Write Header to file
		//note for Header: if UsedChar<=30: used type 1, else type 2
			//type 1: 1(max_len)(UsedChar)(bit_len)(number of len 1, number of len 2,...)(char1,char2,...)
			//type 2:2(max_len)(len of (char)0, len of (char)1,...., len of char(255))
	int remain;
	ofstream out;
	out.open(filename,ios::trunc);
	if (!out.is_open()) {cout<<"Can't open this file"<<endl; return;}
	if (UsedChar<=30) {
		cout<<endl<<"Using mode 1"<<endl;
		out<<'1';			//type 1
		out<<(char)(Head[UsedChar-1].len+'0'); //max_len
		out<<(char) (UsedChar+'0');		//UsedChar
		//Get Header Info
			//get number of elements of each len
		int H_Info[Head[UsedChar-1].len];int max=0;
		for (int i=0;i<Head[UsedChar-1].len;i++) {H_Info[i]=0;}
		for (int i=0;i<UsedChar;i++) {H_Info[Head[i].len-1]++;}
		for (int i=0;i<Head[UsedChar-1].len;i++) {if(H_Info[i]>max) max=H_Info[i];}

		int bit_len=0;
		if (max<8) bit_len=3;
		else if (max<16) bit_len=4;
		else bit_len=5;
		out<<(char)bit_len;
			//Make it into string of bit
		string HeaderStr;
		for (int i=0; i<Head[UsedChar-1].len;i++){
			for (int j=bit_len-1;j>=0;j--){
				HeaderStr.push_back((char) ('0'+(H_Info[i]>>j&1)));
			}
		}
			//convert to the bit type
		string HeaderFinal;
		HeaderFinal=ConvertStringToBit(HeaderStr,remain);

		for (int i=0;i<UsedChar;i++) {HeaderFinal.push_back(Head[i].c);}
		//Write to file
		out<<HeaderFinal;
		cout<<endl<<"Header had been writen to Output file!"<<endl;
	}
	else{	//UsedChar>30;
		cout<<endl<<"Using mode 2"<<endl;
		out<<'2';	//type 2

		int bit_len;
			//get bit_len;
		if (Head[UsedChar-1].len<8) bit_len=3;
		else if (Head[UsedChar-1].len<16) bit_len=4;
		else if (Head[UsedChar-1].len<32) bit_len=5;
		else if (Head[UsedChar-1].len<64) bit_len=6;
		else if (Head[UsedChar-1].len<128) bit_len=7;
		else bit_len=8;
		out<<(char) (Head[UsedChar-1].len+'0'); //max_len
			//Make string of bit
		string HeaderStr;
		for (int i=0; i<256;i++){
			for (int j=bit_len-1;j>=0;j--){
				HeaderStr.push_back('0'+(Bit[i].length>>j&1));
			}
		}

			//convert to the bit type
		string HeaderFinal;
		HeaderFinal=ConvertStringToBit(HeaderStr,remain);
		//Write to file
		out<<HeaderFinal;
		cout<<endl<<"Header had been writen to Output file!"<<endl;
	}

	//Step 5: Translate the character to Code and write to output
	string CodeStr;
	CodeStr=FromCharToCode();
	//
	cout<<"CodeStr:"<<CodeStr<<endl;
	//
	string CodeFinal;
	CodeFinal=ConvertStringToBit(CodeStr,remain);
	CodeFinal+=(char)(remain+'0');
	//
	cout<<"CodeFinal:"<<CodeFinal<<endl;
	//

	out<<CodeFinal;
	out.close();
	cout<<endl<<"Data had been translated to bit code and save to the Output File!"<<endl;
	cout<<endl<<"Encode has Finished!"<<endl;
}

string Huffman::DecodeInput(const char* filename) {
	char mode,mlen,uChar,H_bit_len;
	int bit_len,max_len;
	cout<<"Reading mode, max length and(if mode 1) UsedChar"<<endl;
	ifstream ifs;
	ifs.open(filename);
	ifs>>mode;
	ifs>>mlen;
	max_len=(int)(mlen-'0');

	if (mode=='1') {
		cout<<"File encoded with mode 1"<<endl;
		//Read UsedChar
		ifs>>uChar;
		UsedChar=(int)(uChar-'0');
		ifs>>H_bit_len;
		bit_len=(int)H_bit_len;

		string HeaderBitcode;
		char temp;
		int len;
		if ((max_len*bit_len)%8==0) {len=max_len*bit_len/8;} else {len=max_len*bit_len/8+1;}
		for(int i=0;i<len;i++) {
			ifs.get(temp);
			HeaderBitcode.push_back(temp);
		}
		cout<<"Bitcode has been readed!"<<endl;

		string HeaderStringCode;
		HeaderStringCode=ConvertBitToString(HeaderBitcode,len*8-max_len*bit_len);
		cout<<"Convert Bitcode to String succesfully!"<<endl;

		int H_Info[max_len];
		int index=0;
		for (int i=0;i<max_len;i++) {
			H_Info[i]=0;
			for (int j=0;j<bit_len;j++) {
				H_Info[i]|=(HeaderStringCode[index]-'0')<<(bit_len-1-j);
				index++;
			}
		}


		//ReadChar
		Head = new Header [UsedChar];
		for (int i=0;i<UsedChar;i++) {ifs.get(Head[i].c);}

		//Re-gen code len of char
		index=0;
		for (int i=0;i<max_len;i++) {
			for (int j=0;j<H_Info[i];j++) {
				Head[index].len=i+1;
				index++;
			}
		}
		cout<<"Header has been rebuilt"<<endl;

		//Fill array Bit
		int mask=0;
		for(int i=0;i<UsedChar;i++) {
			if (i==0) {		//The First Element's code will be the [len] of bit 0;
				for (int j=0;j<Head[i].len;j++) Bit[Head[i].c].code+='0';
			}
			else {		//Current Element's code if not the First
				mask++;	//Increase the bit code by 1
				//if len increase, left shift mask with len_shift = amount of len increment
				mask=mask<<(Head[i].len-Head[i-1].len);
				//convert mask from int to string
				for (int j=Head[i].len-1;j>=0;j--) {
					Bit[Head[i].c].code.push_back('0'+(mask>>j & 1));
				}
			}
		}
		cout<<"Rebuild code book successfully!"<<endl;
		PrintBitcodeTable();
	}
	else {		//mode = '2'
		cout<<"File encoded with mode 2"<<endl;
		if (max_len<8) bit_len=3;
		else if (max_len<16) bit_len=4;
		else if (max_len<32) bit_len=5;
		else if (max_len<64) bit_len=6;
		else if (max_len<128) bit_len=7;
		else bit_len=8;
		string HeaderBitcode;
		char temp;
		int len;
		if ((256*bit_len)%8==0) {len=256*bit_len/8;} else {len=256*bit_len/8+1;}
		for(int i=0;i<len;i++) {
			ifs.get(temp);
			HeaderBitcode.push_back(temp);
		}
		cout<<"Bitcode has been readed!"<<endl;

		string HeaderStringCode;
		HeaderStringCode=ConvertBitToString(HeaderBitcode,len*8-256*bit_len);
		cout<<"Convert Bitcode to String succesfully!"<<endl;

		UsedChar=0; int index=0;
		for (int i=0; i<256;i++){
			Bit[i].length=0;
			for (int j=bit_len-1;j>=0;j--){

				Bit[i].length|=(HeaderStringCode[index]-'0')<<j;
				index++;
			}
			if (Bit[i].length>0) UsedChar++;

		}
		Head = new Header [UsedChar];
		CreateBitcodeTable(max_len);
		cout<<"Header and Bitcode rebuild successfully!"<<endl;
		cout<<"Rebuild Codebook Successfully!"<<endl;
		PrintBitcodeTable();
	}

	string ret;
	char t;
	while (ifs.get(t)) {
		ret.push_back(t);
	}
	ret.push_back((char)(max_len+'0'));
	cout<<endl<<"Main code has been readed!"<<endl;
	cout<<endl<<"Input successfully!"<<endl;
	ifs.close();
	return ret;
}

string Huffman::FromCodeToChar(string code,char max_len){
	string ret;
	string token;
	char check=0;
	int pos=0;
	while (pos<code.length()) {
		if (check==0) {token.clear(); check=1;}
		token+=code[pos];
    	for (int i=0;i<256;i++){
			if (Bit[i].code==token) {
				ret.push_back(i);
				check=0;
				break;
			}
		}
		if (token.length()>max_len) {cout<<endl<<"Something when wrong!"<<endl; break;}
		pos++;
	}
	return ret;
}

void Huffman::Decode(const char* ifname, const char* ofname){
	string maincode_bit=DecodeInput(ifname);

	char max_len=maincode_bit[maincode_bit.length()-1]-'0';
	maincode_bit=maincode_bit.substr(0,maincode_bit.length()-1);

	int remain=(int) (maincode_bit[maincode_bit.length()-1]-'0');
	maincode_bit=maincode_bit.substr(0,maincode_bit.length()-1);

	string maincode_str=ConvertBitToString(maincode_bit,remain);
	cout<<endl<<"Main code has been converted to string of code!"<<endl;

	string maindata=FromCodeToChar(maincode_str,max_len);
	ofstream ofs;
	ofs.open(ofname);
	ofs<<maindata;
	cout<<endl<<"Code has been converted to Data and saved to "<<ofname<<endl; 
	ofs.close();
}


