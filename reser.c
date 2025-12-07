#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    char nom[50];
    int capacite;
    float tarif_horaire;
} Salle;

typedef struct {
    int id;
    char nom_client[50];
    char nom_salle[50];
    char date[11];       // format YYYY-MM-DD
    char heure_debut[6]; // format HH:MM
    char heure_fin[6];   // format HH:MM
    int nombre_personnes;
    float tarif;
} Reservation;

// ----------- Arbre binaire pour les réservations -----------
typedef struct Node {
    Reservation data;
    struct Node *left;
    struct Node *right;
} Node;

Salle salles[MAX];
int nb_salles = 0;
Node *racine = NULL; // racine de l'arbre
int compteur_id = 0;

// ---------------- MENU ----------------
void menu() {
    printf("\n--- MENU PRINCIPAL ---\n");
    printf("1. Ajouter une salle\n");
    printf("2. Ajouter une réservation\n");
    printf("3. Afficher les réservations\n");
    printf("4. Générer une facture\n");
    printf("5. Sauvegarder les données\n");
    printf("6. Statistiques\n");
    printf("0. Quitter\n");
    printf("Choix : ");
}

// ---------------- FONCTION POUR COMPARER HEURES ----------------
int convertirHeureEnMinutes(char *heure) {
    int h, m;
    sscanf(heure, "%d:%d", &h, &m);
    return h * 60 + m;
}

// ---------------- INSÉRER DANS ARBRE ----------------
Node* inserer(Node* root, Reservation r) {
    if (root == NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->data = r;
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (r.id < root->data.id)
        root->left = inserer(root->left, r);
    else if (r.id > root->data.id)
        root->right = inserer(root->right, r);
    return root;
}

// ---------------- RECHERCHER ----------------
Reservation* rechercher(Node* root, int id) {
    if (root == NULL) return NULL;
    if (id == root->data.id) return &root->data;
    if (id < root->data.id) return rechercher(root->left, id);
    else return rechercher(root->right, id);
}

// ---------------- VERIFIER DISPONIBILITE ----------------
int verifierDisponibilite(Node* root, Reservation r) {
    if (root == NULL) return 1;
    if (strcmp(r.nom_salle, root->data.nom_salle) == 0 &&
        strcmp(r.date, root->data.date) == 0) {
        int debut_r = convertirHeureEnMinutes(r.heure_debut);
        int fin_r   = convertirHeureEnMinutes(r.heure_fin);
        int debut_exist = convertirHeureEnMinutes(root->data.heure_debut);
        int fin_exist   = convertirHeureEnMinutes(root->data.heure_fin);

        if (!(fin_r <= debut_exist || debut_r >= fin_exist)) {
            return 0; // conflit
        }
    }
    return verifierDisponibilite(root->left, r) && verifierDisponibilite(root->right, r);
}

// ---------------- AJOUTER SALLE ----------------
void ajouterSalle() {
    printf("Nom de la salle : ");
    scanf("%s", salles[nb_salles].nom);
    printf("Capacité : ");
    scanf("%d", &salles[nb_salles].capacite);
    printf("Tarif horaire : ");
    scanf("%f", &salles[nb_salles].tarif_horaire);
    nb_salles++;
    printf("Salle ajoutée avec succès.\n");
}

// ---------------- AJOUTER RESERVATION ----------------
void ajouterReservation() {
    Reservation r;
    r.id = ++compteur_id;
    printf("Nom du client : ");
    scanf("%s", r.nom_client);
    printf("Nom de la salle : ");
    scanf("%s", r.nom_salle);
    printf("Date (YYYY-MM-DD) : ");
    scanf("%s", r.date);
    printf("Heure début (HH:MM) : ");
    scanf("%s", r.heure_debut);
    printf("Heure fin (HH:MM) : ");
    scanf("%s", r.heure_fin);
    printf("Nombre de personnes : ");
    scanf("%d", &r.nombre_personnes);

    int trouve = 0;
    for (int i = 0; i < nb_salles; i++) {
        if (strcmp(r.nom_salle, salles[i].nom) == 0) {
            int duree = convertirHeureEnMinutes(r.heure_fin) - convertirHeureEnMinutes(r.heure_debut);
            r.tarif = salles[i].tarif_horaire * (duree / 60.0);
            trouve = 1;
            break;
        }
    }

    if (!trouve) {
        printf("Salle introuvable. Réservation annulée.\n");
        return;
    }

    if (!verifierDisponibilite(racine, r)) {
        printf(" Conflit détecté : la salle est déjà réservée à ce créneau.\n");
        return;
    }

    racine = inserer(racine, r);
    printf("Réservation ajoutée avec succès.\n");
}

// ---------------- AFFICHER RESERVATIONS ----------------
void afficherInOrder(Node* root) {
    if (root == NULL) return;
    afficherInOrder(root->left);
    printf("ID: %d | Client: %s | Salle: %s | Date: %s | %s-%s | Tarif: %.2f DT\n",
           root->data.id,
           root->data.nom_client,
           root->data.nom_salle,
           root->data.date,
           root->data.heure_debut,
           root->data.heure_fin,
           root->data.tarif);
    afficherInOrder(root->right);
}

void afficherReservations() {
    if (racine == NULL) {
        printf("Aucune réservation.\n");
        return;
    }
    afficherInOrder(racine);
}

// ---------------- GENERER FACTURE ----------------
void genererFacture(int id) {
    Reservation* r = rechercher(racine, id);
    if (r != NULL) {
        printf("\n--- FACTURE ---\n");
        printf("Client : %s\n", r->nom_client);
        printf("Salle  : %s\n", r->nom_salle);
        printf("Date   : %s\n", r->date);
        printf("Durée  : %s - %s\n", r->heure_debut, r->heure_fin);
        printf("Montant: %.2f DT\n", r->tarif);
    } else {
        printf("Réservation introuvable.\n");
    }
}

// ---------------- SAUVEGARDER ----------------
void sauvegarderRec(Node* root, FILE* f) {
    if (root == NULL) return;
    sauvegarderRec(root->left, f);
    fprintf(f, "%d;%s;%s;%s;%s;%s;%d;%.2f\n",
            root->data.id,
            root->data.nom_client,
            root->data.nom_salle,
            root->data.date,
            root->data.heure_debut,
            root->data.heure_fin,
            root->data.nombre_personnes,
            root->data.tarif);
    sauvegarderRec(root->right, f);
}

void sauvegarder() {
    FILE *f = fopen("reservations.txt", "w");
    if (f == NULL) {
        printf("Erreur ouverture fichier.\n");
        return;
    }
    sauvegarderRec(racine, f);
    fclose(f);
    printf("Données sauvegardées dans reservations.txt\n");
}

// ---------------- CHARGER ----------------
void charger() {
    FILE *f = fopen("reservations.txt", "r");
    if (f == NULL) {
        printf("Aucune donnée existante.\n");
        return;
    }
    Reservation r;
    while (fscanf(f, "%d;%49[^;];%49[^;];%10[^;];%5[^;];%5[^;];%d;%f\n",
                  &r.id,
                  r.nom_client,
                  r.nom_salle,
                  r.date,
                  r.heure_debut,
                  r.heure_fin,
                  &r.nombre_personnes,
                  &r.tarif) ==  {
        racine = inserer(racine, r);
        if (r.id > compteur_id) compteur_id = r.id;
    }
    fclose(f);
    printf("Réservations chargées depuis reservations.txt\n");
}// ---------------- STATISTIQUES ----------------
void statistiquesRec(Node* root, float *CA, int *mois, int *nb_res) {
    if (root == NULL) return;

    // Chiffre d'affaires par salle
    for (int i = 0; i < nb_salles; i++) {
        if (strcmp(salles[i].nom, root->data.nom_salle) == 0) {
            CA[i] += root->data.tarif;
            nb_res[i]++;
        }
    }

    // Nombre de réservations par mois
    int m;
    sscanf(root->data.date + 5, "%2d", &m); // extrait MM de YYYY-MM-DD
    mois[m]++;

    statistiquesRec(root->left, CA, mois, nb_res);
    statistiquesRec(root->right, CA, mois, nb_res);
}

void statistiques() {
    if (racine == NULL) {
        printf("Aucune donnée pour les statistiques.\n");
        return;
    }

    float CA[MAX] = {0};
    int nb_res_salle[MAX] = {0};
    int mois[13] = {0};

    statistiquesRec(racine, CA, mois, nb_res_salle);

    // Chiffre d'affaires par salle
    printf("\n--- Chiffre d'affaires par salle ---\n");
    for (int i = 0; i < nb_salles; i++) {
        printf("Salle %s : %.2f DT (%d réservations)\n",
               salles[i].nom, CA[i], nb_res_salle[i]);
    }

    // Nombre de réservations par mois
    printf("\n--- Nombre de réservations par mois ---\n");
    for (int i = 1; i <= 12; i++) {
        printf("Mois %02d : %d réservations\n", i, mois[i]);
    }

    // Salle la plus populaire
    int max = 0;
    char salle_populaire[50] = "";
    for (int i = 0; i < nb_salles; i++) {
        if (nb_res_salle[i] > max) {
            max = nb_res_salle[i];
            strcpy(salle_populaire, salles[i].nom);
        }
    }

    printf("\n--- Salle la plus populaire ---\n");
    if (max > 0)
        printf("Salle %s avec %d réservations\n", salle_populaire, max);
    else
        printf("Aucune réservation enregistrée.\n");
}

// ---------------- MAIN ----------------
int main() {
    int choix, id;
    charger(); // Charger les données au démarrage
    do {
        menu();
        scanf("%d", &choix);
        switch (choix) {
            case 1: ajouterSalle(); break;
            case 2: ajouterReservation(); break;
            case 3: afficherReservations(); break;
            case 4: printf("ID réservation : "); scanf("%d", &id); genererFacture(id); break;
            case 5: sauvegarder(); break;
            case 6: statistiques(); break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);
    return 0;
}
