#include <iostream>
#include <string.h>
#include "CanonicalHuffman.cpp"

int main(){
	cout<<"Canonical Huffman Coding - By Nguyen Thai Son\n\n";
	
	string input,output;
    
	//Encode Phase
	char check;
	cout<<"Encode Phase!"<<endl;
	
	cout<<"Do you want to Encode?<y/n> ";
	cin>>check;
	if(!(check=='Y'||check=='y')) goto DECODE_PHASE;
    
    cout<<"Input your Plaintext file name: ";
    cin>>input;

    Huffman EncodeObj;

	const char* inputFile = input.c_str();
    EncodeObj.EncodeInput(inputFile);

    cout<<"Input your Destination File name: ";
	cin>>output;
	output.append(".huff");
    const char* outputFile=output.c_str();
    EncodeObj.Encode(outputFile);


	//Decode Phase
	DECODE_PHASE:
	cout<<endl<<"Decode Phase!"<<endl;
	
	cout<<"Do you want to Decode?<y/n> ";
	cin>>check;
	if(!(check=='Y'||check=='y')) return 0;

	cout<<"Input your encoded file(file with .huff extension): ";
	cin>>input;

	Huffman DecodeObj;

	const char* De_inputFile = input.c_str();

	cout<<"Input your Destination File name: ";
	cin>>output;

	const char* De_outputFile=output.c_str();
    DecodeObj.Decode(De_inputFile,De_outputFile);

    return 0;
}
