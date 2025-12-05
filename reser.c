
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

Salle salles[MAX];
Reservation reservations[MAX];
int nb_salles = 0, nb_reservations = 0;

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

// ---------------- VERIFIER DISPONIBILITE ----------------
int verifierDisponibilite(Reservation r) {
    int debut_r = convertirHeureEnMinutes(r.heure_debut);
    int fin_r   = convertirHeureEnMinutes(r.heure_fin);

    for (int i = 0; i < nb_reservations; i++) {
        if (strcmp(r.nom_salle, reservations[i].nom_salle) == 0 &&
            strcmp(r.date, reservations[i].date) == 0) {

            int debut_exist = convertirHeureEnMinutes(reservations[i].heure_debut);
            int fin_exist   = convertirHeureEnMinutes(reservations[i].heure_fin);

            if (!(fin_r <= debut_exist || debut_r >= fin_exist)) {
                return 0; // conflit
            }
        }
    }
    return 1; // disponible
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
    r.id = nb_reservations + 1;
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

    if (!verifierDisponibilite(r)) {
        printf("⚠️ Conflit détecté : la salle est déjà réservée à ce créneau.\n");
        return;
    }

    reservations[nb_reservations++] = r;
    printf("Réservation ajoutée avec succès.\n");
}

// ---------------- AFFICHER RESERVATIONS ----------------
void afficherReservations() {
    if (nb_reservations == 0) {
        printf("Aucune réservation.\n");
        return;
    }
    for (int i = 0; i < nb_reservations; i++) {
        printf("ID: %d | Client: %s | Salle: %s | Date: %s | %s-%s | Tarif: %.2f DT\n",
               reservations[i].id,
               reservations[i].nom_client,
               reservations[i].nom_salle,
               reservations[i].date,
               reservations[i].heure_debut,
               reservations[i].heure_fin,
               reservations[i].tarif);
    }
}

// ---------------- GENERER FACTURE ----------------
void genererFacture(int id) {
    for (int i = 0; i < nb_reservations; i++) {
        if (reservations[i].id == id) {
            printf("\n--- FACTURE ---\n");
            printf("Client : %s\n", reservations[i].nom_client);
            printf("Salle  : %s\n", reservations[i].nom_salle);
            printf("Date   : %s\n", reservations[i].date);
            printf("Durée  : %s - %s\n", reservations[i].heure_debut, reservations[i].heure_fin);
            printf("Montant: %.2f DT\n", reservations[i].tarif);
            return;
        }
    }
    printf("Réservation introuvable.\n");
}

// ---------------- SAUVEGARDER ----------------
void sauvegarder() {
    FILE *f = fopen("reservations.txt", "w");
    if (f == NULL) {
        printf("Erreur ouverture fichier.\n");
        return;
    }
    for (int i = 0; i < nb_reservations; i++) {
        fprintf(f, "%d;%s;%s;%s;%s;%s;%d;%.2f\n",
                reservations[i].id,
                reservations[i].nom_client,
                reservations[i].nom_salle,
                reservations[i].date,
                reservations[i].heure_debut,
                reservations[i].heure_fin,
                reservations[i].nombre_personnes,
                reservations[i].tarif);
    }
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
    while (fscanf(f, "%d;%49[^;];%49[^;];%10[^;];%5[^;];%5[^;];%d;%f\n",
                  &reservations[nb_reservations].id,
                  reservations[nb_reservations].nom_client,
                  reservations[nb_reservations].nom_salle,
                  reservations[nb_reservations].date,
                  reservations[nb_reservations].heure_debut,
                  reservations[nb_reservations].heure_fin,
                  &reservations[nb_reservations].nombre_personnes,
                  &reservations[nb_reservations].tarif) == 8) {
        nb_reservations++;
    }
    fclose(f);
    printf("%d réservations chargées depuis reservations.txt\n", nb_reservations);
}


// ---------------- STATISTIQUES ----------------
void statistiques() {
    if (nb_reservations == 0) {
        printf("Aucune donnée pour les statistiques.\n");
        return;
    }

    // Chiffre d'affaires par salle
    printf("\n--- Chiffre d'affaires par salle ---\n");
    for (int i = 0; i < nb_salles; i++) {
        float totalCA = 0;
        int count = 0;
        for (int j = 0; j < nb_reservations; j++) {
            if (strcmp(salles[i].nom, reservations[j].nom_salle) == 0) {
                totalCA += reservations[j].tarif;
                count++;
            }
        }
        printf("Salle %s : %.2f DT (%d réservations)\n", salles[i].nom, totalCA, count);
    }

    // Nombre de réservations par mois
    printf("\n--- Nombre de réservations par mois ---\n");
    int mois[13] = {0};
    for (int i = 0; i < nb_reservations; i++) {
        int m;
        sscanf(reservations[i].date + 5, "%2d", &m); // extrait MM de YYYY-MM-DD
        mois[m]++;
    }
    for (int i = 1; i <= 12; i++) {
        printf("Mois %02d : %d réservations\n", i, mois[i]);
    }

    // Salle la plus populaire
    int max = 0;
    char salle_populaire[50] = "";
    for (int i = 0; i < nb_salles; i++) {
        int count = 0;
        for (int j = 0; j < nb_reservations; j++) {
            if (strcmp(salles[i].nom, reservations[j].nom_salle) == 0) {
                count++;
            }
        }
        if (count > max) {
            max = count;
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