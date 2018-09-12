#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <cstdlib>            //rand()
#include <vector>             //vetor
#include <ctime>


//fumante 0 => tem fumo     |
//fumante 1 => tem papel    | id do fumante = ao do recurso que tem infinito
//fumante 2 => tem isqueiro |


int getRand(int n){
    srand(time(NULL));
    return rand()%n;
} // -> por algum motivo não funciona da forma esperada


std::condition_variable  vez_do[3];
std::mutex mtx_fum[3];

std::condition_variable  vez_do_agente;
std::mutex mtx_agente;

int recurso_disponivel_1;
int recurso_disponivel_2;
bool pode_pegar = false;
int recursos[] = {0,1,2};

void fumante(int recurso);
void agente();


char const *getRecursoString(int recurso) //função para  facilitar a identificação dos recursos
{
    switch(recurso) {
        case 0 : 
            return "Fumo";
            break;
        case 1 : 
            return "Papel";
            break;
        case 2 : 
            return "Isqueiro";
            break;
    }
    
    return "char";
}
    


void fumante (int recurso){
    
    int meu_recurso_infinito = recurso; //na inicializacao sei qual o meu recurso
    
    
    while(1){
        std::unique_lock<std::mutex> lck(mtx_fum[meu_recurso_infinito]);
        
        while(!pode_pegar) vez_do[meu_recurso_infinito].wait(lck); //espero a minha vez de pegar os recursos
        
        pode_pegar = false;
        
        std::cout << "fumante que tem " << getRecursoString(meu_recurso_infinito) << " está fumando " << '\n';
        
        recurso_disponivel_1 = -1;
        recurso_disponivel_2 = -1;
        
        std::cout << "fumante que tem " << getRecursoString(meu_recurso_infinito) << " parou de fumar " << "\n\n\n";
        
        
        vez_do_agente.notify_one(); //avisa o agente que pode fornecer de novo.
    
    }
}

void agente(){
    int i = 0;
    
    while(i < 20){
        std::unique_lock<std::mutex> lck(mtx_agente);
        
        while(pode_pegar) vez_do_agente.wait(lck); //espero terem fumado para colocar os recursos novamente
        
        std::cout << "agente está disponbilizando os recursos " << "\n";
        
        std::vector<int> vec_de_recursos (recursos, recursos + sizeof(recursos) / sizeof(int) );
        
        int rec1 = rand()%3; //pega um dos 3 recursos
        
        //std::cout << getRecursoString(vec_de_recursos.at(0)) << getRecursoString(vec_de_recursos.at(1)) << getRecursoString(vec_de_recursos.at(2)) << '\n';
        
        recurso_disponivel_1 = vec_de_recursos.at(rec1);
        //std::cout << recurso_disponivel_1 << '\n';
        
        vec_de_recursos.erase (vec_de_recursos.begin()+rec1);
        
        //std::cout << getRecursoString(vec_de_recursos.at(0)) << getRecursoString(vec_de_recursos.at(1)) << '\n';
        
        recurso_disponivel_2 =  vec_de_recursos.at(rand()%2); //pega um dos 2 recursos restantes
        //std::cout << recurso_disponivel_2 << '\n';
        
        vec_de_recursos.clear();
    
        std::cout << "agente disponibilizou " << getRecursoString(recurso_disponivel_1) << " e " << getRecursoString(recurso_disponivel_2) << "\n";
        
        pode_pegar = true; //permite que peguem os recursos
        
        int recs = recurso_disponivel_1 + recurso_disponivel_2;
        
        if(recs == 3){
            std::cout << "avisando o fumante que tem " << getRecursoString(0) << "\n\n\n";
            vez_do[0].notify_one(); //avisa o fumante que tem  fumo para pegar os recursos.
        }
        if(recs == 2){
            std::cout << "avisando o fumante que tem " << getRecursoString(1) << "\n\n\n";
            vez_do[1].notify_one(); //avisa o fumante que tem  papel para pegar os recursos.
        }
        if(recs == 1){
            std::cout << "avisando o fumante que tem " << getRecursoString(2) << "\n\n\n";
            vez_do[2].notify_one(); //avisa o fumante que tem  isqueiro para pegar os recursos.
        }    
        i++;
    
    }
    
}

int main()
{

    std::thread A(fumante,0);  //
    std::thread B(fumante,1);  //Inicializando as threads de fumantes
    std::thread C(fumante,2);  //
                                
    std::thread Agente(agente);//Inicializando a thread do agente
     

    vez_do_agente.notify_one();// Dizendo para o agente começar a disponibilizar os recursos

    Agente.join();//Esperando o agente terminar de disponibilizar os recursos
    
    
    
    std::cout << "agente terminou suas tarefas "<<'\n';
    

    
    return 0;
    
}