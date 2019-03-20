#include <mpi.h>
#include <stdio.h>
#include <iostream>
#define MAX 1024


int main(int argc, char* argv[])
{
	
	MPI_Init(&argc, &argv);

	int rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char sentence[MAX];
	
	if(my_rank == 0){ //lector
		std::cout << "Escriba las oraciones que desee! \nDebe tomar en cuenta que cada ENTER es una oracion nueva\n"
                  << "Ademas que cada palabra se separa por caracteres no alfanumericos\nPara terminar envie un 1 como oracion\n";
        std::string sentenceInput;
		std::getline(std::cin, sentence);
		sentence = sentenceInput.c_str();
		MPI_Send(sentence, MAX, MPI_CHAR, 1, rank, MPI_COMM_WORLD);
		
	}else{ // contador
		int wrdCount = 0;
		MPI_Recv(sentence, MAX, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::string sentenceInput = sentence;
		char anterior = ',';
		for (int i = 0; i < sentenceInput.length(); ++i){
			if(isalnum(anterior) && !isalnum(sentence[i])){
				++wrdCount;
			}
			anterior = sentenceInput[i];
		}
		if(isalnum(sentenceInput[sentenceInput.length() - 1])){
			++wrdCount;
		}
		std::cout << sentenceInput << " " << wrdCount << std:endl;
	}

	MPI_Finalize();
	return 0;
}