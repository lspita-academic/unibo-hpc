# Domande d'esame

<https://www.moreno.marzolla.name/teaching/HPC/esempio-domande-esame.pdf>

## Domanda 1

I problemi adatti ad essere risolti su una architettura a memoria distribuita sono quelli dove si presenta una struttura "embarassingly parallel", con task quasi o del tutto indipendenti.
Questo perchè tramite architettura a memoria distribuita ogni nodo di computazione ha la sua memoria privata, rendendo quindi facile la distribuzione del carico di lavoro senza avere problemi di race conditions ed evitando eventuali bottleneck dati dalla condivisione delle risorse.
L'elevata scalabilità di questa archietettura permette inoltre di aumentare facilmente il numero di nodi per adattarsi alla grandezza dell'input.
Infine, il rapporto tra computazione e comunicazione molto elevato in questo tipo di problemi (il tempo passato a svolgere operazioni è molto maggiore rispetto a quello passato a comunicare), rendendo quindi questa architettura efficiente in quanto si possono ridurre le comunicazioni al minimo per ridurre il tempo perso.

## Domanda 2

Consideriamo le tre istruzioni S1, S2, S3 del corpo del ciclo.
 - S1(i) non ha dipendenze
 - S2(i) dipende da S1(i-1)
 - S3(i) dipende da S1(i-1) e S2(i)

Trasponendo il ciclo per le istruzioni S2 ed S3, possiamo rimuove la dipendenza con il ciclo precedente.

```
a[0] = f(0);
b[0] = 0;
c[0] = -3;

if (n > 1) {
    // Calcoliamo S1, S2 ed S3 che rimangono fuori dal ciclo dopo la trasposizione
    a[n-1] = f(n-1);
    b[1] = a[0] * 3;
    c[1] = (a[0] / 3) - b[1];
}

for (i=1; i<n-1; i++) {
    a[i] = f(i);
    b[i+1] = a[i] * 3;
    c[i+1] = (a[i] / 3) - b[i+1];
}
```

## Domanda 3

Una GPU moderna è costituita da diversi livelli di memoria e unità di computazione.

Il nodo di computazione fondamentale è il thread, raccolti in blocchi a due o tre dimensioni raccolti a loro volta in griglie con altrettante dimensioni.
Questi thread vengono gestiti dallo scheduler in WARP, ovvero gruppi di 32 thread che eseguono la stessa istruzione in parallelo. I thread di uno stesso warp non possono eseguire istruzioni differenti, di conseguenza quando si incontrano delle situazioni di branching (e.g. if statement), i due branch vengono eseguiti serialmente, disattivando temporaneamente i thread che non soddisfano la condizione per entrare in quella ramificazione.

Per quanto riguarda la memoria, ogni thread presenta una memoria locale interna (e opzionalmente un livello di cache L1). Ogni blocco presenta invece una memoria condivisa tra i vari thread. Infine, è presente un ultimo livello di memoria globale condiviso tra tutti i blocchi.

Seguendo questo modello di memoria a strati, più una memoria è lontana dal thread più sarà grande ma anche lenta: nonostante i thread possano accedere direttamente alla memoria globale della GPU (ultimo strato), la memoria condivisa a livello di blocco serve principalmente come "cache" manuale per evitare che letture ripetute debbano accedere alla memoria più lenta.

Queste memorie lavorano leggendo e scrivendo i dati con cache line di 128 bit, di conseguenza lavorare su dati contigui in memoria favorisce notevolmente le performance.

## Domanda 4

Le primitive MPI di comunicazione collettiva permettono di eseguire operazioni che coinvolgono tutti i processi mpi attivi tramite una singola funzione, chiamata da tutti i processi coinvolti.
Un esempio di ciò è la funzione MPI_Reduce, che esegue una riduzione dei valori forniti da ogni processo tramite l'operazione associativa specificata e fornendo il risultato finale al processo destinatario.
Utilizzare queste funzioni permette non solo una semplificazione del codice, ma anche una maggiore ottimizzazione delle comunicazioni: nel caso della reduce, ad esempio, MPI utilizza una struttura di comunicazione ad albero per ridurre le comunicazioni ad un ordine di grandezza logaritmico rispetto la quantità di valori.

## Domanda 5

Vantaggi:
- Elevata scalabilità: dovuta all'indipendenza di ogni nodo di computazione e dall'assenza di bottleneck per la condivisione delle risorse.
- Assenza di race conditions: siccome ogni nodo presenta una sua memoria locale non condivisa, non si possono avere race conditions di nessun tipo.

Svantaggi:
- Difficoltà di programmazione: la struttura di un programma a memoria distribuita è in generale molto complessa e soprattutto diversa dalla controparte seriale, necessitando spesso di un ragionamento sul funzionamento da zero invece di una semplice ristrutturazione del codice seriale originale, oltre che l'aggiunta di un'elevata quantità di codice per la comunicazione.
- Comunicazione lenta: le operazioni di comunicazione sono una delle parti più lente di un programma, richiedendo tempi ordini di grandezza superiori persino rispetto all'accesso alla memoria principale. Bisogna quindi ridurre i messaggi al minimo, così da evitare bottleneck pesanti.
- ERRORE: deadlock

## Domanda 6

1. Un approccio con paradigma SIMD permette di eseguire le moltiplicazioni dei vari elementi in parallelo, in quanto operazioni indipendenti tra di loro.
4. Questo problema si tratta di uno stencil, particolarmente adatto alle operazioni di tipo SIMD in quanto la somma dei tre valori per ottenere la nuova cella può essere effettuata in una singola istruzione.

## Domanda 7

1. `__global__ void my_kernel(int *x)`: dichiarazione di un kernel, ovvero una funzione presente sul device (GPU) e chiamabile dall'host (CPU).
2. `*x = *x + 1;`: corpo del kernel, eseguito da ogni thread lanciato per l'esecuzione.
3. `int h_a = 42;`: dichiarazione di una variabile int h_a sull'host.
4. `int *d_a;`: dichiarazione di una variabile puntatore d_a sull'host che conterrà l'indirizzo di memoria del device su cui verrà copiato il valore di h_a.
5. `const size_t size = sizeof(int);`: dichiarazione di una variabile size_t h_a sull'host contenente la dimensione in byte occupata da un intero, così da poter specificare la quantità di memoria necessaria da allocare sul device per copiare il valore di h_a.
6. `cudaMalloc((void **)&d_a, size);`: allocazione di 'size' byte sulla memoria del device. L'indirizzo di memoria associato verrà salvato dentro d_a.
7. `cudaMemcpy(d_a, &h_a, size, cudaMemcpyHostToDevice);`: copia del valore di h_a dall'host al device, nell'indirizzo contenuto in d_a. h_a viene trattato come un buffer generico (per questo serve l'operatore & per prenderne l'indirizzo) e vengono specificati la quantità di byte da copiare (in questo caso esattamente la grandezza dell'intero).
8. `my_kernel<<<1,1>>>(d_a);`: invocazione del kernel: nelle parentesi angolari vengono specificati rispettivamente il numero di blocchi ed il numero di thread per blocco con cui invocare il kernel. Viene poi passato come parametro l'indirizzo di memoria del device su cui è stato copiato il valore di h_a.
9. `cudaMemcpy(&h_a, d_a, size, cudaMemcpyDeviceToHost);`: copia del valore contenuto all'indirizzo del device d_a nell'indirizzo di h_a nell'host. h_a viene trattato come un buffer generico (per questo serve l'operatore & per prenderne l'indirizzo) e vengono specificati la quantità di byte da copiare (in questo caso esattamente la grandezza dell'intero). Questa funzione aspetta che tutte le invocazioni precedenti di kernel, in quanto asincrone, finiscano, per poi procedere con il trasferimento dei dati.
10. `printf(“%d\n”, h_a);`: stampa del nuovo valore di h_a. In questo caso, verrà stampato "43" in quanto il kernel è stato eseguito da un solo thread.
11. `cudaFree(d_a);`: deallocazione della memoria sul device allocata in precedenza (equivalente della funzioen free per la memoria dell'host).

## Domanda 8

Lo speedup di un'applicazione parallela è un valore reale che indica l'aumento in performance relativo al programma equivalente parallelo. Si basa sul wall-clock time ed è una metrica di tipo "higher is better".

Speedup(p) = T_serial / T_parallel(p) ~= T_parallel(1) / T_parallel(p)

dove
- p: numero di unità di computazione parallele.
- T_serial: wall-clock time del programma seriale.
- T_parallel(p): wall-clock time del programma equivalente parallelo, in funzione di p.

Il range teorico dello speedup è 1 <= S <= p.
Sia a un valore reale tra 0 e 1 che indica la porzione di programma da eseguire obbigatoriamente in maniera seriale (e.g. a causa di dipendenze), il tempo di esecuzione del programma parallelo si può calcolare come

T_parallel(p) = a * T_serial + (1-a) * T_serial/p (dove come tempo seriale sarebbe più giusto utilizzare T_parallel(1))

Rendendo quindi lo speedup massimo per p che tende ad infinito 1 / a.

L'efficienza indica la frazione di potenza computazionale effettivamente utilizzata per risolvere il problema. Anche questa è una metrica di tipo "higher is better".

Nel campo dello scaling, ne esistono di due tipi:
- Strong scaling efficiency: si aumenta il numero di unità di computazione p mantenendo la quantità di lavoro totale costante.

S(p) = Speedup(p) / p

- Weak scaling efficiency: si aumenta la quantità di lavoro totale all'aumentare delle unità di computazione, mantenendo il carico di lavoro per unità costante.

W(p) = T1/Tp

con:
- T1: tempo di esecuzione di un'unità di lavoro da parte di un'unità di computazione.
- Tp: tempo di esecuzione di p unità di lavoro da parte di p unità di computazione.

## Domanda 9

L'accumolo delle varie forze sulla particella i corrisponde ad una semplice riduzione ottenibile direttamente con i costrutti di openmp.
L'accumulo delle forze sulla particella j, invece, dipende anche dai cicli esterni precedenti, rendendo quindi parallellizzabile solo il ciclo interno.

Per evitare di ricreare il pool di thread ogni ciclo, però, è possibile riutilizzare i thread della stessa regione parallela per ogni iterazione.

Utilizzerei uno scheduling statico in quanto ogni ciclo il carico verrebbe perfettamente distribuito tra i thread.
```
struct Particle {
    float force; /* la forza è rappresentata da un singolo valore reale */
    /* altri attributi della particella, non mostrati */
};

Particle particles[N];
int i, j;

...

#pragma omp parallel default(none) shared(particles) private(i, j, N)
for (i=0; i<N; i++) {
    #pragma omp for reduction(+:particles[i].force)
    for (j=i+1; j<N; j++) {
        const float f = gravity(i, j);
        particles[i].force += f;
        particles[j].force += f;
    }
}
```
