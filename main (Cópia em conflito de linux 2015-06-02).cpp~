#include<iostream>
#include<vector>
#include<string>
#include<cstdio>
#include<pthread.h>
#include <fstream>
#include<cstdlib>


#define USAGE "Modo de uso:\n./exe arquivo\nTal que arquivo contem a definição dos processos segundo especificado\n"
#define ERRO1 "Arquivo de input mal formatado\n"

using namespace std;

typedef struct S_processo{
	int PID;
	int initTime; 
	int prioridade;
	int tempoProcessador; 
	int numBlocos;
	int impressora; 
	int scanner; 
	int modem;
	int disco;
}S_processo;


void verificarVector(vector<S_processo> vetor){
	int i;
	
	for(i=0;i<vetor.size();i++){
		cout << endl;
		cout << "PID: " << vetor[i].PID << endl;
		cout << "initTime: " << vetor[i].initTime << endl;
		cout << "Prioridade: " << vetor[i].prioridade << endl;
		cout << "Tempo processador: " << vetor[i].tempoProcessador << endl;
		cout << "numBlocos: " << vetor[i].numBlocos << endl;
		cout << "impressora: " << vetor[i].impressora << endl;
		cout << "scanner: " << vetor[i].scanner << endl;
		cout << "modem: " << vetor[i].modem << endl;
		cout << "disco: " << vetor[i].disco << endl;
		
	}
	
}

//recebe por parametro o arquivo de processos
int main(int argc, char** argv){
	ifstream inputFile;
	vector<S_processo> vetorProcessos;
	string linha;
	S_processo auxiliar;
	int i;
	int j;
	int k;
	
	if(argc != 2){
		printf(USAGE);
		return 1;
	}else{
		inputFile.open(argv[1]);
		if(!inputFile.is_open()){
			printf(USAGE);
			return 1;
		}
		
	}
	
	string aux;
	aux.clear();
	i=0,j=0,k=0;
	while(getline(inputFile,linha)){
		auxiliar.PID = i;
		
		for(k = 0;k<8;k++){
			while(linha[j] != ',' && linha[j] != '\0'){
				aux += linha[j];
			}
		
			switch(k){
				case 0:
					auxiliar.initTime = atoi(aux.c_str());
					break;
				case 1:
					auxiliar.prioridade = atoi(aux.c_str());
					break;
				case 2:
					auxiliar.tempoProcessador = atoi(aux.c_str());
					break;
				case 3:
					auxiliar.numBlocos = atoi(aux.c_str());
					break;
				case 4:
					auxiliar.impressora = atoi(aux.c_str());
					break;
				case 5:
					auxiliar.scanner = atoi(aux.c_str());
					break;
				case 6:
					auxiliar.modem = atoi(aux.c_str());
					break;
				case 7:
					auxiliar.disco = atoi(aux.c_str());
					break;
				default:
					break;
			}
			
			aux.clear();
		}
		vetorProcessos.push_back(auxiliar);
		i++;
	}

	
	//verificarVector(vetorProcessos);



	return 0;
}