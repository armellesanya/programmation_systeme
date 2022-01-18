# include <semaphore.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h> 
# include <errno.h> 


sem_t mutex;
sem_t empty;
sem_t full;


int* table = NULL; 
int dim, c, p, cnum, pnum, compt = 0;
//int nbProdAttent = 0, nbConsAttent = 0;

int produire(int val){
	return (val + 1);
}

int consommer(int val){
	return val;
}

void *producteur(void* arg){
	int val = 0, item = 0; 
	for(int a = 0; a < dim; a++){
		if (compt == dim) {
			sem_post(&full);
			sem_wait(&empty);
			printf("En attente de libération d'espace\n");
		}
		else if(sem_wait(&empty) == 0){
			
			sem_post(&mutex); //verrouillage du buffer
			
			printf("\ntable[%d] = %d ;", item, table[item]);
			val = produire(table[item]) ;//production
    			
    			table[item] = val; //remplissage
			
    			sem_post(&full); //une place vient d'être occupée
    			
    			sem_post(&mutex); //déverrouillage du buffer
    			
    			compt++;
    			
    			printf("Le producteur %d a produit %d dans table[%d]\n", pnum, table[item], item);
    			
    			item = (item + 1) % dim; 
		} 
		
  	}
  	usleep(2000000);
  	pthread_exit(NULL); 
}


void *consommateur(void* arg)
{
  int item = 0, val; 
  
	  for(int b = 0; b < dim; b++){
		  if (compt == 0) {
			sem_post(&empty); 
			sem_wait(&full);
			printf("En attente d'occupation d'espace\n");
		   }
		   else if(sem_wait(&full) == 0){
	    		
		    	 sem_post(&mutex);
		    	 
		    	printf("\ntable[%d] = %d; \n", item, table[item]);
		    	 val = consommer(table[item]);//consommation
		    	
		    	sem_post(&empty);//une place vient d'être libérée
		    	 
		    	 sem_post(&mutex);
		    	 
		    	 compt--;
		   
		    	 printf("Le consommateur %d a consommé %d de table[%d]\n", cnum, val, item);
		    	 
		    	 item = (item + 1) % dim; 
		    	 
		     }
		    
	}
	usleep(6000000);
	pthread_exit(NULL); 
}


int main(int argc, char *argv[]){

	printf("Nombre de producteurs:");
	scanf("%d",&p);
	
	printf("Nombre de consommateurs:");
	scanf("%d",&c);
	
	printf("Quantite d'elements à produire:");
	scanf("%d",&dim);
	
	pthread_t consom[c], product[p] ; 
	
	table = calloc(dim , sizeof(int));
	
	if(table == NULL){
		printf("Tableau vide!!");
		exit(0);	
	}
	
	sem_init(&mutex, 0 , 1);
	sem_init(&empty, 0 , dim);  // buffer vide
	sem_init(&full, 0 , 0);
	int err, num;
	
	for (int i = 0; i < p; i++){
		num = i;
		err = pthread_create(&product[i], NULL, producteur, (void*)&num);
		if (err != 0) {
			printf("Echec de la création du thread [%d]: status = %d", i, err );
			return EXIT_FAILURE;;
		}
		pnum = i;
	}
	
	for(int j = 0; j < c; j++){
		num = j;
		err = pthread_create(&consom[j], NULL, consommateur, (void*)&num);
		if (err != 0) {
			printf("Echec de la création du thread [%d]: status = %d", j, err);
			return EXIT_FAILURE;;
		}
		cnum = j;
	}
	
	for(int j = 0; j < c; j++){
		cnum = j;
		pthread_join(consom[j], NULL); 
		printf("Fin du consommateur [%d]\n", j);
	}
	
	for (int i = 0; i < p; i++){
		pnum = i;	
		pthread_join(product[i], NULL);
		printf("Fin du producteur [%d]\n", i);
		
	}
	
	sem_destroy(&full);
	sem_destroy(&empty);
	 
	free(table);
	return 0;
}

/*
 else if(nbConsAttent == c){
		     	pthread_exit(NULL);
		     }
else if (nbProdAttent == p){
			pthread_exit(NULL);
		}		     
//printf("création du producteur [%d]\n", i);  
printf("création du consommateur [%d]\n", j);   
		     */
