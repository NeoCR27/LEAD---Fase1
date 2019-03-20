#include "Pair.h"
#include <semaphore.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <queue>
#include <stdio.h>

typedef struct
{
	int threadId;
	sem_t* semaphores;
	std::vector<Pair>* vector;
	std::queue<Pair>* queue;
	int* readingState;
}threadInfo;

void print_pairs(threadInfo* data)
{
	threadInfo* info = data;
	//int threadID = info->threadID;
	for(std::vector<Pair>::iterator pos = info->vector->begin(); pos != info->vector->end(); ++pos){
		std::cout << "Oracion: " << pos->sentence << ", Palabras: " << pos->wordCount << std::endl;
	}
	
}

void* run(void* data)
{
    threadInfo* info = (threadInfo*)data;
    int threadId = info->threadId;

	if(threadId == 0){ // lector
        std::cout << "Escriba las oraciones que desee! \nDebe tomar en cuenta que cada ENTER es una oracion nueva\n"
                    << "Ademas que cada palabra se separa por caracteres no alfanumericos\nPara terminar envie un 1 como oracion\n";
        std::string sentence;
        while((info->readingState[0]) == 1 && std::getline(std::cin, sentence)){
            if(sentence == "1"){
                info->readingState[0] = 0;
                Pair newPair(sentence);
            	info->queue->push(newPair);
				sem_post(&(info->semaphores[(threadId + 1)]));
				sem_wait(&(info->semaphores[threadId]));
				print_pairs(info);
            }else{
            	Pair newPair(sentence);
            	info->vector->push_back(newPair);
                info->queue->push(newPair);
				sem_post(&(info->semaphores[(threadId + 1)]));
		}
	}									
	}else{ // contador
			bool keepCounting = true;
			int counter = 0;
	    	while(keepCounting){
            	int wrdCount = 0;
				std::string sentence;
            	sem_wait(&(info->semaphores[threadId]));
            	while(!(info->queue->empty())){
                	int lenght = info->queue->front().sentence.length();
					sentence = info->queue->front().sentence;
					if(sentence == "1"){
						keepCounting = false;
						sem_post(&(info->semaphores[(threadId - 1)]));
					}else{
						char anterior = ',';
						for (int i = 0; i < lenght; i++){
				        	if(isalnum(anterior) && !isalnum(sentence[i])){
								wrdCount++;
							}
							anterior = sentence[i];
						}
						if(isalnum(sentence[sentence.length() - 1])){
							wrdCount++;
						}
						info->vector->at(counter).wordCount = wrdCount;
						++counter;
					}
				info->queue->pop();
				// sleep(5);	
			}
		}
	}
	return NULL;
}

void initialize_semaphores(sem_t* semaphores, int size)
{
	sem_init(&semaphores[0], 0, 1); // To init first sempahore with green light
	for(int index = 1; index < size; ++index)
	{
		sem_init(&semaphores[index], 0, 0);
	}
}

int main()
{
    threadInfo data[2]; // 2 procesos (en este caso 2 threads)
    sem_t semaphores[2];
    pthread_t threads[2];
    std::vector<Pair>* vector = new std::vector<Pair>();
    initialize_semaphores(semaphores, 2);
    std::queue<Pair>* queue = new std::queue<Pair>();
    int readingState[1] = {1};
    

    for(int index = 0; index < 2; ++index){
		data[index].threadId = index;
		data[index].semaphores = semaphores;
		data[index].vector = vector;
		data[index].queue = queue;
		data[index].readingState = readingState;
		pthread_create(&threads[index], NULL, run, (void*)&data[index]);
	}

	for(int index = 0; index < 2; ++index){
		pthread_join(threads[index], NULL);
	}

	delete vector;
	delete queue;
	
    return 0;
}
