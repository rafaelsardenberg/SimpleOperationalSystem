#include<iostream>
#include<vector>
#include<string>
#include<cstdio>
#include <fstream>
#include<cstdlib>
#include <unistd.h>


#define USAGE "Modo de uso:\n./exe arquivo\nTal que arquivo contem a definição dos processos segundo especificado\n"
#define ERRO1 "Arquivo de input mal formatado\n"

using namespace std;

typedef struct S_processo{
	int PID;
	int tempoInicializacao; 
	int prioridade;
	int tempoProcessador; 
	int numBlocos;
	int impressora; 
	int scanner; 
	int modem;
	int disco;
	int tempoFaltando;
	int jaAlocado;
}S_processo;

typedef struct S_memoria{
	int ocupado;
}S_memoria;

S_memoria vetorMemoria[1024];

int modem;
int scanner;
int impressora;
int disco;

void verificarVector(vector<S_processo> vetor){
	unsigned int i;
	
	for(i=0;i<vetor.size();i++){
		cout << endl;
		cout << "PID: " << vetor[i].PID << endl;
		cout << "tempoInicializacao: " << vetor[i].tempoInicializacao << endl;
		cout << "Prioridade: " << vetor[i].prioridade << endl;
		cout << "Tempo processador: " << vetor[i].tempoProcessador << endl;
		cout << "numBlocos: " << vetor[i].numBlocos << endl;
		cout << "impressora: " << vetor[i].impressora << endl;
		cout << "scanner: " << vetor[i].scanner << endl;
		cout << "modem: " << vetor[i].modem << endl;
		cout << "disco: " << vetor[i].disco << endl;
		
	}
	
}

void printDisparador(S_processo processo){
	cout << endl << "Dispatcher =>" << endl;
	cout << "PID: " << processo.PID << endl;
	cout << "tempo de inicializaçao: " << processo.tempoInicializacao << endl;
	cout << "prioridade: " << processo.prioridade << endl;
	cout << "tempo de processador: " << processo.tempoProcessador << endl;
	cout << "numero de blocos: " << processo.numBlocos << endl;
	cout << "impressora: " << processo.impressora << endl;
	cout << "scanner: " << processo.scanner << endl;
	cout << "modem: " << processo.modem << endl;
	cout << "disco: " << processo.disco << endl;
		
}

int verificarFinalizou(vector<S_processo> vetor){
	unsigned int i;
	
	for(i = 0; i<vetor.size();i++){
		if(vetor[i].tempoFaltando > 0){
			return 0;
		}
	}
	
	return 1;
	
}

int escolherProcesso(vector<S_processo> vetor, int tempo){
	unsigned int i;
	int processoEscolhido;
	
	processoEscolhido = -1;
	for(i = 0; i<vetor.size();i++){
		
		if(vetor[i].tempoFaltando > 0 && vetor[i].tempoInicializacao <= tempo){
			if(processoEscolhido != -1) {
				if(vetor[i].prioridade < vetor[processoEscolhido].prioridade) {
					processoEscolhido = i;
				}
			} else {
				processoEscolhido = i;	
			}
		}
	}
	
	for(i = 0; i< vetor.size();i++){
		if(processoEscolhido != i && vetor[i].tempoFaltando > 0 && vetor[i].tempoInicializacao <= tempo){
			if(vetor[i].prioridade > 0){
				vetor[i].prioridade--;
			}
		}
	}
	
	
	return processoEscolhido;
	
}

bool alocarMemoria(int blocos){
	int i;
	int j;
	
	i = 0;
	while(i < 1024 && vetorMemoria[i].ocupado == 1){
		i++;
	}
	
	
	if(i >= 1024){
		return false;
	}else{
		j = i;
		for(i = i;i<blocos + j;i++){
			vetorMemoria[i].ocupado = 1;
		}
		
		return true;
	}
	
}

int memoriaUtilizada(void){
	int i;
	int espacoUtilizado = 0;
	
	for(i=0;i<1024;i++){
		if(vetorMemoria[i].ocupado == 1){
			espacoUtilizado++;
		}
	}
	
	return espacoUtilizado;
}

void desalocar(int blocos){
	int i,j;
	int feito;
	j = 0;
	for(i = 0;i< blocos;i++){
		feito = 0;
		while(feito == 0){
			if(vetorMemoria[j].ocupado == 1){
				vetorMemoria[j].ocupado = 0;
				feito = 1;
			}else{
				j++;
			}
			
		}
	}
}

int temEspaco(int requisicao){
	if(1024 - memoriaUtilizada() >= requisicao){
		return 1;
	}else{
		return 0;
	}
}

void processador(vector<S_processo> vetor){
	int tempo = 0;
	int finalizou = 0;
	int indiceP;
	int pidAnterior;
	bool alocou;
	int memoriaEmUso;
	
	pidAnterior = -1;
	while(!finalizou){
		
		indiceP = escolherProcesso(vetor, tempo);
		
		if(indiceP != -1){
			if(vetor[indiceP].jaAlocado == 0){
				if(temEspaco(vetor[indiceP].numBlocos)) {			
					alocou = alocarMemoria(vetor[indiceP].numBlocos);
					printDisparador(vetor[indiceP]);
					cout << endl << "*********Processo " << vetor[indiceP].PID << "**************" << endl;
					
					cout << "instrucao " << (vetor[indiceP].tempoProcessador - vetor[indiceP].tempoFaltando + 1) << endl;
					vetor[indiceP].tempoFaltando--;
					pidAnterior = vetor[indiceP].PID;
					
					vetor[indiceP].jaAlocado = 1;
				}else{
					cout << "Nao ha mais espaco em memoria" << endl;
					cout << "Finalizando SO." << endl;
					finalizou = 1;
				}
			} else {
				if(pidAnterior != vetor[indiceP].PID){
					printDisparador(vetor[indiceP]);
					cout << endl << "*********Processo " << vetor[indiceP].PID << "**************" << endl;
				}
				
				cout << "instrucao " << (vetor[indiceP].tempoProcessador - vetor[indiceP].tempoFaltando + 1) << endl;
				vetor[indiceP].tempoFaltando--;
				pidAnterior = vetor[indiceP].PID;
			}
			
			//verificar se acabou
			if(vetor[indiceP].tempoFaltando == 0){
				//acabou!
				//desalocar
				memoriaEmUso = memoriaUtilizada();
				cout << memoriaEmUso << " blocos de memória sendo utilizados" << endl;
				desalocar(vetor[indiceP].numBlocos);
				cout << "Finalizando processo " << vetor[indiceP].PID << ", Liberando memória." << endl << endl;
				impressora = 0;
				scanner = 0;
				modem = 0;
				disco = 0;
			}
			
			if(vetor[indiceP].impressora != 0 && impressora == 0){
				cout << "Impressora " << vetor[indiceP].impressora << " em uso pelo processo." << endl;
				impressora = 1;
			}
			if(vetor[indiceP].scanner != 0 && scanner == 0){
				cout << "Scanner em uso pelo processo." << endl;
				scanner = 1;
			}
			if(vetor[indiceP].modem != 0 && modem == 0){
				cout << "Modem em uso pelo processo." << endl;
				modem = 1;
			}
			if(vetor[indiceP].disco != 0 && disco == 0){
				cout << "Disco " << vetor[indiceP].disco << " em uso pelo processo." << endl;
				disco = 1;
			}
		}
		
		if(!finalizou){
			finalizou = verificarFinalizou(vetor);
		}
		usleep(1000000);
		tempo++;
		
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
	int tempoAtual;
	int x;
	
	
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
	
	for(x=0;x<1024;x++){
		vetorMemoria[x].ocupado = 0;
	}
	
	impressora = 0;
	disco = 0;
	modem = 0;
	scanner = 0;
	
	string aux;
	aux.clear();
	i=0;
	j=0;
	k=0;
	while(getline(inputFile,linha)){
		auxiliar.PID = i;
		j = 0;
		
		for(k = 0;k<8;k++){
			while(linha[j] != ',' && linha[j] != '\0' && j < linha.size()){
				aux += linha[j];
				j++;
			}
			j++;
		
			switch(k){
				case 0:
					auxiliar.tempoInicializacao = atoi(aux.c_str());
					break;
				case 1:
					auxiliar.prioridade = atoi(aux.c_str());
					break;
				case 2:
					auxiliar.tempoProcessador = atoi(aux.c_str());
					auxiliar.tempoFaltando = atoi(aux.c_str());
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
			
			auxiliar.jaAlocado = 0;
			aux.clear();
		}
		vetorProcessos.push_back(auxiliar);
		i++;
		linha.clear();
	}

	
	//verificarVector(vetorProcessos);
	
	processador(vetorProcessos);
	


	return 0;
}