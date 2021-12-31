# MIM-bzip2-server

## Sujet n°2 : Bzip2 au complet - BWT, M2F, RLE Huffman

    Participants :
        RICATTE Clément  
        BRUNGARD LUC

**Warning**
Files must be located inside of the res folder.
For the server to find the res files, it needs to be executed in the parent folder of the whole project !

Réalisation d'un programme client / serveur avec des sockets (protocole TCP).

### Détails
#### BWT
- **encodeur**  
L'encodeur reprend ce qui a été vu en cours : la fonction `indexL(chaîne S)` devient `int encodeBWT(char *S, char *L)` car non ne renvoyons uniquement l'indice de départ (idx) et modifions L grâce à son pointeur placé en paramètre. Aucune autre modification n'a été réalisée par rapport au cours.
- **decodeur**  
Le decodeur (`void decodeBWT(char *SL, int len, int idx, char *outS)`) reçoit en paramètres :
    - `L` une chaîne correspondant à la dernière colonne de la matrice des permutations du message à décoder,
    - `len` un entier correspondant à la longueur de la chaîne à décoder,
    - `idx` un entier correspondant à l'indice de départ de la recherche,
    - `outS` la chaîne décodée.  
L'algorithme consiste à tout d'abord créer la colonne `F`. Pour cela on copie `SL` dans `F` et on effectue un tri ascendant sur `F`.  
Ensuite il faut créer la colonne `T`. On créer un tableau `ascii_T` d'entier de longueur 128, initialisé à **-1**. Puis on remplit `T` :
```
pour i de 0 à len - 1
    si ascii_T[SL[i]] = -1 alors
        ascii_T[SL[i]] = premiereOccurence(SL[i])
    T[i] = ascii_T[SL[i]]
    ascii_T[SL[i]]++
fpour
```
On incrémente `ascii_T[SL[i]]` car `F` est trié par ordre croissant donc la prochaine occurence du caractère sera le prochain caractère.  
Il ne reste plus qu'à construire la chaîne de retour.
```
pour i de len-1 à 0
    outS[i] = L[idx]
    idx = T[idx]
fpour
```
#### M2F

#### RLE

#### HUFFMAN
