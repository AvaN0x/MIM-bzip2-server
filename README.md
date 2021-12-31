# MIM-bzip2-server

## Sujet n°2 : Bzip2 au complet - BWT, M2F, RLE et Huffman
    Participants :
        RICATTE Clément  
        BRUNGARD LUC

**Warning**
Files must be located inside of the res folder.
For the server to find the res files, it needs to be executed in the parent folder of the whole project !

Réalisation d'un programme client / serveur avec des sockets (protocole TCP).

## Détails
#### BWT
- **encodeur**  
L'encodeur reprend ce qui a été vu en cours : la fonction `indexL(chaîne S)` devient `int encodeBWT(char *S, char *L)` car nous ne renvoyons pas uniquement l'indice de départ (idx) mais nous modifions également L grâce à son pointeur placé en paramètre. Aucune autre modification n'a été réalisée par rapport au cours.
- **decodeur**  
Le decodeur (`void decodeBWT(char *SL, int len, int idx, char *outS)`) reçoit en paramètres :
    - `L` une chaîne correspondant à la dernière colonne de la matrice des permutations du message à décoder,
    - `len` un entier correspondant à la longueur de la chaîne à décoder,
    - `idx` un entier correspondant à l'indice de départ de la recherche,
    - `outS` la chaîne décodée.  

L'algorithme consiste à tout d'abord créer la colonne `F`. Pour cela on copie `SL` dans `F` et on effectue un tri alphabétique ascendant sur `F`.  
Ensuite il faut créer la colonne `T`. On créer un tableau `ascii_T` d'entier de longueur 128, initialisé à **-1**. Puis on remplit `T` :
```c
pour i de 0 à len - 1
    si ascii_T[SL[i]] = -1 alors
        ascii_T[SL[i]] = premiereOccurence(F, SL[i]) // On cherche l'adresse de la première occurence de SL[i] dans SL
    fsi
    T[i] = ascii_T[SL[i]]
    ascii_T[SL[i]]++
fpour
```
On incrémente `ascii_T[SL[i]]` car `F` est trié par ordre croissant donc la prochaine occurence du caractère sera le prochain caractère.  
Il ne reste plus qu'à construire la chaîne de retour en commençant par l'`idx` reçu en arguments de la fonction.
```c
pour i de len-1 à 0
    outS[i] = L[idx]
    idx = T[idx]
fpour
```

---
#### M2F
- **encodeur**  
L'encodeur `void encodeM2F(char *S, int len, char *shifts)` reçoit en paramètres : 
    - `S` la chaîne à encoder,
    - `len` la longueur de la chaîne à encoder,
    - `shifts` un pointeur sur la chaîne qui sera encodée.  

Il faut créer un tableau de caractères nommé `alphabet` (table ASCII). Puis on parcourt chaque caractère de `S` : 
```
pour i de 0 à len - 1
    shifts[i] = 0
    // Recherche du caractère dans l'alphabet
    pour j de 0 à 127
        si (alphabet[j] == S[i])
            shifts[i] = j
            break
        fsi
    fpour

    // On décale tous les caractères de 0 à j-1 de 1 cran vers la droite
    decalerCaracteresDroite(j-1, alphabet)
    alphabet[0] = S[i]
fpour
```
- **decodeur**  
Le décodeur `void decodeM2F(char *shifts, int len, char *result)` reçoit en paramètres :
    - `shifts` la chaîne encodée,
    - `len` la longueur de la chaîne encodée,
    - `result` le résultat de la chaîne décodée.  
    
Il faut reconstruire un tableau de caractères nommé `alphabet` (table ASCII). Puis on exécute le même algorithme que l'encodeur. La *seule* différence sera qu'on enregistre dans la chaîne de retour le caractère trouvé à l'indice `shifts[i]` et non pas son indice.  

---
#### RLE

---
#### HUFFMAN
- **Types abstraits**  
```c
//type pour représenter un caratère de l'alphabet.
typedef unsigned int symbol_t 

//type pour représenter la fréquence d'un caractère dans une chaîne.
typedef unsigned int frequence_t

//type pour représenter un noeud dans un arbre binaire avec ses données
typedef struct elem_node_t
{
	symbol_t S;
	frequence_t F;
	char *code; //Le code "binaire" associé à l'arbre de Huffman (suite de '0' et '1')
	int lvl; //La hauteur du noeud
	struct elem_node_t *down, *up; //Respectivement enfant de droite et enfant de gauche
} node_t;

//Structure pour construction d'une liste chaînée
typedef struct elem_list_t
{
	node_t *n;
	struct elem_list_t *suc;
} list_t;
```

Considérons la chaîne à encoder `S`.  

- **Etape 1** : Lister la fréquence de chaque caractère de `S` dans un tableau `int charFrequences[128]`. L'indice 0 correspond au caractère de code ASCII 0.

- **Etape 2** : Construire le code de chaque caractère 
`void buildCodeHuffman(int *frequencies, char **HuffmanDico)` reçoit en paramètres :
    - `frequencies` le tableau des fréquences de chaque caractère,
    - `HuffmanDico` un tableau de longueur 128 qui contiendra le code de chaque caractère ASCII. 

    Primitives utilisées :  
        - `construireNoeud(symbol_t symbol, frequence_t frequence, chaine code, entier hauteur, node_t enfantDroite, node_t enfantGauche)`
        - `construireListeElement(node_t nNode)`
        - `insererNoeudTri(node_t nNode, list_t tete)`. Le tri est effectué de la manière suivante :
    ```
    si ((nElement->n->F > nElement->suc->n->F) || ((nElement->n->S > nElement->suc->n->S) && (nElement->n->F == nElement->suc->n->F)))
        echanger(nElement, nElement->suc)
    fsi
    ```

    ```c
    //Construction de la liste initiale
    list_t tete = NULL;
    Pour i de 0 à 127 faire :
        si frequencies[i] > 0 alors
            node_t nNode = construireNoeud(i, frequencies[i], "", 0, NULL, NULL)
            list_t nElement = construireListeElement(nNode)
            insererNoeudTri(nNode, tete)
        fsi
    fpour
    ```

    ```c
    //Construction de l'arbre de huffman
    tant qu il y a plus d une valeur dans la liste chaînée faire
        trouverMin(tete, min1, min2)
        min1.code += "0"
        min2.code += "1"
        //Création du nouveau noeud issu des min
        node_t nNode = construireNoeud('\0', min1.F + min2.F, "", max(min1.lvl, min2.lvl), min1, min2)
        list_t newElement = construireListeElement(nNode)
        tete = retirerMinListe(min1, min2, tete)
        parcourir toute la liste chainee 
            if (newElement->n->F > newElement->suc->n->F || (newElement->n->S == newElement->suc->n->S && newElement->n->F >= newElement->suc->n->F))
                echanger(newElement, newElement->suc)
            fsi
    ftq
    ```

    L'arbre ne possède plus qu'une valeur qui est la racine, on réalise alors un parcours de l'arbre pour concaténer le code de chaque noeud avec celui de ses enfants. Lorsque nous sommes arrivé à une feuille de l'arbre, on copie le code **inversé** du noeud au bon indice dans `HuffmanDico` (indice correspondant au code ASCII du symbole du noeud).


- **L'encodeur**  
`unsigned char *encodeHuffman(const char *str, int lgthStr, char **HuffmanDico, int *encodedLgth)` **renvoi la chaîne encodée** et reçoit comme paramètres :
    - `str` la chaîne à encoder,
    - `lgthStr` la longueur de la chaîne à encoder
    - `HuffmanDico` le dictionnaire contenant les codes de chaque caractère
    - `encodedLgth` un pointeur sur la longueur de la chaîne encodée  

    ```
    res[longueur(str)];
    count = 0
    encodedLgth = 0
    res[encodedLgth] = 0
    Pour chaque caractere c de str faire
        pour chaque caractere cc de HuffmanDico[c] faire
            bit = charToBit(cc) //renvoi cc - '0'
            res[encodedLgth] <<= 1
            res[encodedLgth] |= bit
            count++
            si (count == 8)
                res[++encodedLgth] = 0
                count = 0
            fsi
        fpour
    fpour
    si (count == 8)
        res[++encodedLgth] <<= 8 - count
    fsi
    ```

- **Le décodeur**  
`char *decodeHuffman(unsigned char *str, int lgthStr, char **HuffmanDico, int *decodedLgth)` renvoie la chaîne décodée et reçoit en paramètres :
    - `str` la chaîne encodée,
    - `lgthStr` la longueur de la chaîne encodée,
    - `HuffmanDico` le dictionnaire conteanant le code de chaque caractère,
    - `decodedLgth` la longueur de la chaîne décodée.  

    On initialise une tableau d'entier `candidatList` de longueur 128. Toutes les valeurs sont initialisées à 1. Si candidatList[0] = 1 cela signifie que le caractère de code ASCII 0 est candidat.

    On initialise un entier `cursor` qui indiquera notre index dans un caractère. Il est initialisé à -1.
    ```c
    res = ""
    decodedLgth = 0
    nbCandidat = 128
    candidatIdx = 8128; // Somme de 0 à 127
    Pour chaque octet o de str en commençant à l'indice 0
        Pour chaque bit b de o en commençant par le bit de poids fort : 
            cursor++
            pour chaque candidat dans candidatList faire :
                cbit = charToBit(HuffmanDico[candidat][cursor])
                si b != cbit alors
                    candidat = 0 // Il n'est plus candidat
                    nbCandidat -= 1
                    candidatIdx -= (entier)candidat
                fsi
                si nbCandidat == 1 alors
                    reinitialiserCandidatList(1) // Tous les caractères redeviennent candidats
                    nbCandidat = 128
                    candidatIdx = 8128; // Somme de 0 à 127
                    cursor = -1
                    res += (char)candidatIdx
                    decodedLgth++
                    break // passage au bit suivant
                fsi
            fpour
        fpour
    fpour
    ```
        