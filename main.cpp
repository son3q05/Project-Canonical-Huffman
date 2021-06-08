#include <iostream>
#include <string.h>
#include "CanonicalHuffman.cpp"

int main(){
	cout<<"Canonical Huffman Coding - By Nguyen Thai Son\n\n";
    //Encode Phase
	cout<<"Encode Phase!"<<endl;
    string input,output;
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
	cout<<endl<<"Decode Phase!"<<endl;
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
