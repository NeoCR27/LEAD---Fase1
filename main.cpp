#include "Pair.h"
#include <semaphore.h>
#include <iostream>
#include <vector>

typedef struct
{
	size_t totalThreads;
	size_t threadId;
	sem_t* semaphores;
	std::vector<Pair>* vector;
	std::queue<Pair>* queue;
	bool* readingState;
}threadInfo;


void* run(void* data)
{
    threadInfo* info = (threadInfo*)data;
    size_t totalThreads = info->totalThreads;
    size_t threadId = info->threadId;


	if(threadId == 0){ // lector
        std::cout << "Escriba las oraciones que desee! \nDebe tomar en cuenta que cada ENTER es una oracion nueva\n"
                    << "Ademas que cada palabra se separa por caracteres no alfanumericos\nPara terminar envie un 1 como oracion\n";
        std::string sentence;
        while(readingState == true && std::getline(std::cin, sentence)){
            if(sentence == "1"){
               info->readingState = false;
               Pair newPair(sentence);
               info->queue->push(newPair);
            }else{
                Pair newPair(sentence);
                info->vector->push_back(newPair);
                info->queue->push(newPair);
                sem_post(&(info->semaphores[(threadId + 1)));
            }
        }
	}else{ // contador
		bool keepCounting = true;
	    while(keepCounting){
            int counter = 0;
            int wrdCount = 0;
            sem_wait(&(info->semaphores[threadId]));
            while(!(info->queue->empty())){
                int lenght = info->vector->at(counter).sentence.length();
                for (int i = 0; i < lenght; i++){
                  /*  if(isalnum(anterior) && !isalnum(oracionActual[i])){
                        wrdCount++;
                        anterior = oracionActual[i];
                    }*/
                    /*if(isalnum(oracionActual[oracionActual.length()])){
                    wrdCount++;
                }*/
                }
                info->vector->at(counter).wordCount = wrdCount;
                ++counter;
                if(info->queue->pop().sentence == "1"){
					keepCounting = false;
				}
              //  sem_wait(&(info->semaphores[threadId]));
              }
        //  return NULL;
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
    std::vector<Pair>* vector;
    initialize_semaphores(semaphores, 2);
    std::queue<Pair>* queue;
  //  bool* readingState = true;
    

    for(int index = 0; index < 2; ++index){
		data[index].totalThreads = 2;
		data[index].threadId = index;
		data[index].semaphores = semaphores;
		*data[index].readingState = true;
		data[index].vector = vector;
		data[index].queue = queue;
		pthread_create(&threads[index], NULL, run, (void*)&data[index]);
	}

	for(int index = 0; index < 2; ++index){
		pthread_join(threads[index], NULL);
	}

    return 0;
}
