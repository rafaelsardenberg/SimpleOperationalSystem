#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <cstdlib>
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
	int enderecoInicial;
	int bloqueado;
}S_processo;

typedef struct S_memoria{
	int ocupado;
}S_memoria;

S_memoria vetorMemoria[1024];
vector<S_processo> vetorProcessos;

int modem;
int scanner;
int impressora;
int disco;

void verificarVector(){
	unsigned int i;
	
	for(i=0;i<vetorProcessos.size();i++){
		cout << endl;
		cout << "PID: " << vetorProcessos[i].PID << endl;
		cout << "tempoInicializacao: " << vetorProcessos[i].tempoInicializacao << endl;
		cout << "Prioridade: " << vetorProcessos[i].prioridade << endl;
		cout << "Tempo processador: " << vetorProcessos[i].tempoProcessador << endl;
		cout << "numBlocos: " << vetorProcessos[i].numBlocos << endl;
		cout << "impressora: " << vetorProcessos[i].impressora << endl;
		cout << "scanner: " << vetorProcessos[i].scanner << endl;
		cout << "modem: " << vetorProcessos[i].modem << endl;
		cout << "disco: " << vetorProcessos[i].disco << endl;
		
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

int verificarFinalizou(){
	unsigned int i;
	
	for(i = 0; i<vetorProcessos.size();i++){
		if(vetorProcessos[i].tempoFaltando > 0){
			return 0;
		}
	}
	
	return 1;
	
}

int temEspaco(int requisicao){
	int i;
	int cont = 0;

	for(i=0;i<1024;i++){
		if(cont == requisicao) {
			return i - requisicao;
		} else if(vetorMemoria[i].ocupado != 1){
			cont++;
		} else {
			cont = 0;
		}
	}
	return -1;
}

void alocarMemoria(int enderecoInicial, int numBlocos){
	int i;
	
	for(i=enderecoInicial; i < enderecoInicial + numBlocos; i++){
		vetorMemoria[i].ocupado = 1;
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

//TO-DO: Desalocar de acordo com o endereço inicial do bloco
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

int escolherProcesso(int tempo){
	unsigned int i;
	int processoEscolhido;
	int enderecoInicial;
	int achouProcesso = 0;

	while(!achouProcesso) {
		processoEscolhido = -1;
		for(i = 0; i<vetorProcessos.size();i++){
			if(vetorProcessos[i].tempoFaltando > 0 && vetorProcessos[i].tempoInicializacao <= tempo && vetorProcessos[i].bloqueado == 0){
				if(processoEscolhido != -1) {
					if(vetorProcessos[i].prioridade < vetorProcessos[processoEscolhido].prioridade) {
						processoEscolhido = i;
					}
				} else {
					processoEscolhido = i;	
				}
			}
		}
		
		//Verifica se existe memória disponível
		if(processoEscolhido != -1) {
			//Verifica se o processo já foi alocado
			if(vetorProcessos[processoEscolhido].jaAlocado == 0){
				enderecoInicial = temEspaco(vetorProcessos[processoEscolhido].numBlocos);
				//Verifica se existe memória
				if(enderecoInicial != -1) {
					vetorProcessos[processoEscolhido].enderecoInicial = enderecoInicial;	
					vetorProcessos[processoEscolhido].jaAlocado = 1;		
					alocarMemoria(enderecoInicial, vetorProcessos[processoEscolhido].numBlocos);					
					achouProcesso = 1;
				} else {
					vetorProcessos[processoEscolhido].bloqueado = 1;
				}
			} else {
				//Se já tiver sido alocado
				achouProcesso = 1;
			}
		} else {
			//Retorna sem achar nenhum processo possível de ser executado
			achouProcesso = 1;
		}
	}

	//Desbloqueia todos os processos
	for(i = 0; i<vetorProcessos.size();i++){
		vetorProcessos[i].bloqueado = 0;
	}

	//Aging
	for(i = 0; i< vetorProcessos.size();i++){
		if(processoEscolhido != i && vetorProcessos[i].tempoFaltando > 0 && vetorProcessos[i].tempoInicializacao <= tempo){
			if(vetorProcessos[i].prioridade > 0){
				vetorProcessos[i].prioridade--;
			}
		}
	}
	
	return processoEscolhido;
}

void processador(){
	int tempo = 0;
	int finalizou = 0;
	int indiceP;
	int pidAnterior;
	int memoriaEmUso;
	
	pidAnterior = -1;
	while(!finalizou){
		indiceP = escolherProcesso(tempo);

		if(indiceP != -1){
			if(pidAnterior != vetorProcessos[indiceP].PID){
				printDisparador(vetorProcessos[indiceP]);	
			
				//Impressão
				cout << endl << "*********Processo " << vetorProcessos[indiceP].PID << "**************" << endl;
			}
			
			//Impressão
			cout << "instrucao " << (vetorProcessos[indiceP].tempoProcessador - vetorProcessos[indiceP].tempoFaltando + 1) << endl;

			vetorProcessos[indiceP].tempoFaltando--;
			pidAnterior = vetorProcessos[indiceP].PID;

			//Verifica se acabou
			if(vetorProcessos[indiceP].tempoFaltando == 0){
				memoriaEmUso = memoriaUtilizada();
				cout << memoriaEmUso << " blocos de memória sendo utilizados" << endl;
				desalocar(vetorProcessos[indiceP].numBlocos);
				cout << "Finalizando processo " << vetorProcessos[indiceP].PID << ", Liberando memória." << endl << endl;
				impressora = 0;
				scanner = 0;
				modem = 0;
				disco = 0;
			}
		
			//TO-DO	
			if(vetorProcessos[indiceP].impressora != 0 && impressora == 0){
				cout << "Impressora " << vetorProcessos[indiceP].impressora << " em uso pelo processo." << endl;
				impressora = 1;
			}
			if(vetorProcessos[indiceP].scanner != 0 && scanner == 0){
				cout << "Scanner em uso pelo processo." << endl;
				scanner = 1;
			}
			if(vetorProcessos[indiceP].modem != 0 && modem == 0){
				cout << "Modem em uso pelo processo." << endl;
				modem = 1;
			}
			if(vetorProcessos[indiceP].disco != 0 && disco == 0){
				cout << "Disco " << vetorProcessos[indiceP].disco << " em uso pelo processo." << endl;
				disco = 1;
			}
		}

		if(!finalizou){
			finalizou = verificarFinalizou();
		}
		usleep(1000000);
		tempo++;
	}
}

//recebe por parametro o arquivo de processos
int main(int argc, char** argv){
	ifstream inputFile;
	
	string linha;
	S_processo auxiliar;
	int i;
	int j;
	int k;
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
			
			auxiliar.bloqueado = 0;
			auxiliar.jaAlocado = 0;
			aux.clear();
		}
		vetorProcessos.push_back(auxiliar);
		i++;
		linha.clear();
	}

	processador();
	
	return 0;
}