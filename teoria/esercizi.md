# Risposte HPC

<https://www.moreno.marzolla.name/teaching/HPC/esercizi.pdf>

## Domanda 1

Lo speedup è dato dalla formula

S(p) = Tserial/Tparallel(p) ~= Tparallel(1)/Tparallel(p)

dove:

- p: numero di unità di computazione parallele utilizzate.
- Tserial: tempo di esecuzione (wall-clock time) del codice seriale.
- Tparallel(p): tempo di esecuzione (wall-clock time) del codice parallelo equivalente in funzione di p.

Sia a la porzione di tempo di esecuzione di codice non parallellizzabile e 1-a la porzione perfettamente parallellizzabile,

Tparallel(p) = a _ Tserial + ((1-a) _ Tserial)/p (Dove in questo caso per Tserial si intende lo stesso codice parallelo ma con p=1)

Di conseguenza, lo speedup teorico massimo per p -> infinito è 1 / a.

Uno speedup superlineare si può ottenere quando la versione parallela presenta ottimizzazioni ulteriori alla semplice distribuzione del carico rispetto alla versione seriale:

- Quando si usano istruzioni SIMD, la CPU effettua meno cicli FETCH-DECODE-EXECUTE lavorando su più dati contemporaneamente.
- Quando all'aumentare delle unità di esecuzione, il sottoproblema per ogni unità è abbastanza piccolo da sfruttare meglio le risorse (e.g. cache)
- Quando è presente hardware eterogeneo, in particolare una distinzione nella CPU tra P-core ed E-core e/o la presenza di core virtuali (Intel HyperThreading), che può causare un aumento netto del wall-clock time quando si supera una certa quantità di thread utilizzati.

## Domanda 2

```
int my_Reduce(const int *v) {
    int n = numero di processi mpi totale;
    int rank = rank mpi del processo attuale;
    
    // immaginando una struttura ad albero binario con il rank 0 come root, 
    // prendo i figli sinistro e destro del processo attuale
    int rank_left = (rank * 2) + 1;
    int rank_right = (rank * 2) + 2;
    int parent = (rank - 1) / 2; // arrotondato per difetto grazie alla divisione intera del C
    
    int result = *v;
    
    int val_left;
    int val_right;
    if (rank_left < n) {
        // esiste il figlio sinistro, mi faccio mandare il valore
        MPI_Recv con buffer = &val_left, count = 1, tipo = MPI_INT, src = rank_left;
        result += val_left;
    }
    if (rank_right < n) {
        // esiste il figlio destro, mi faccio mandare il valore
        MPI_Recv con buffer = &val_right, count = 1, tipo = MPI_INT, src = rank_right;
        result += val_right;
    }
    
    if (rank > 0) {
        // esiste il padre, gli mando il valore
        MPI_Send con buffer = &result, count = 1, tipo = MPI_INT, dest = parent;
    }
    return *result;
}
```

## Domanda 3

```

int search_k(int *v, int N, int k) {
    int rank = rank mpi del processo corrente;
    int comm_size = numero totale di processi mpi;
    
    int n = N / comm_size;
    int data_buf[n]; // buffer dove ogni processo riceve la sua porzione di dati
    int indexes_buf[comm_size]; // buffer dove il processo 0 accumula i risultati di ogni processo

    int index = N; // N = not found
    
    // distribuisci il vettore v tra i vari processi
    MPI_Scatter con 
        src = 0,
        send buffer = v,
        send count = N,
        send type = MPI_INT,
        recv buffer = data_buf,
        recv count = n,
        recv type = MPI_INT;
    
    // cerca k nella propria porzione assegnata
    for (int i = 0; i < n; i++) {
        if (data_buf[i] == k) {
            index = i + (rank * n);  // sistema indice per renderlo assoluto in v
            break;
        }
    }
    
    // ERRORE: meglio usare MPI_Reduce con operazione min (richiede che il valore "not found" sia N e non -1)
    
    // accumula nel processo 0 gli indici trovati
    MPI_Gather con
        dest = 0,
        send buffer = &index,
        send count = 1,
        send type = MPI_INT,
        recv buffer = indexes_buf,
        recv count = comm_size,
        recv type = MPI_INT;
    
    if (rank == 0) {
        // ritorna il primo indice valido tra i risultati
        for (int i = 0; i < comm_size; i++) {
            int index = indexes_buf[i];
            if (index != N) {
                return index;
            }
        }
    }
    
    return N; // not found
}
```

## Domanda 4

Il ciclo interno è parallelizzabile tramite `omp parallel for` aggiungendo la clausola `reduction(+:c[i])`, mentre il ciclo esterno non presenta dipendenze quindi è parallellizzabile con un semplice `omp parallel for`.

È meglio parallellizzare solo il ciclo esterno per evitare di creare un nuovo pool di thread ogni ciclo.

Come scheduling, è meglio utilizzare scheduling dinamico in quanto il carico di lavoro è sbilanciato verso le righe iniziali.

```
#pragma omp parallel for default(none) shared(c)
for (int i=0; i<n; i++) {
    c[i] = 0;
    for (int j=i; j<n; j++) {
        c[i] += A[i][j] * b[j];
    }
}
```

## Domanda 5

```
for (int idx= 0; idx< (b-a)*(d-c); idx++) {
    int i = a + (idx / (d-c));
    int j = c + (idx % (d-c));
    f(i, j);
}
```
