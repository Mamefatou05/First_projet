#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nom[50];
    char prenom[50];
    int presence; // 0 pour absent, 1 pour présent
} Apprenant;

void marquerPresence(Apprenant *etudiants, int nbEtudiants);
void afficherClasses();

int main() {
    int choix;
    do {
        printf("\nMenu Admin:\n");
        printf("1. Gestion des étudiants\n");
        printf("2. Génération de fichiers\n");
        printf("3. Marquer les présences\n");
        printf("4. Envoyer un message\n");
        printf("5. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        if (choix == 3) {
            printf("Vous avez choisi de marquer la présence de chaque étudiant\n");
            afficherClasses(); // Appel de la fonction pour afficher les classes et marquer les présences
        }
    } while (choix != 5);

    return 0;
}

void marquerPresence(Apprenant *etudiants, int nbEtudiants) {
    FILE *fichier;
    fichier = fopen("presences.txt", "a"); // Ouvrir le fichier en mode ajout

    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    printf("Veuillez entrer la présence des étudiants (1 pour présent, 0 pour absent):\n");
    for (int i = 0; i < nbEtudiants; i++) {
        time_t maintenant = time(NULL);
        struct tm *temps_info = localtime(&maintenant);
        char date[20];
        strftime(date, 20, "%Y-%m-%d %H:%M:%S", temps_info);

        printf("%d - %s %s: ", i + 1, etudiants[i].nom, etudiants[i].prenom);
        scanf("%d", &etudiants[i].presence);
        fprintf(fichier, "%d: %s %s: %d - %s\n", i + 1, etudiants[i].nom, etudiants[i].prenom, etudiants[i].presence, date);
    }

    fclose(fichier);
}

void afficherClasses() {
    FILE *f_classes = fopen("class.txt", "r");
    if (f_classes == NULL) {
        printf("Erreur lors de l'ouverture du fichier des classes.\n");
        exit(1);
    }
    
    printf("Liste des classes :\n");
    char classe[50];
    while (fgets(classe, sizeof(classe), f_classes) != NULL) {
        printf("%s", classe);
    }
    fclose(f_classes);

    int choix_classe;
    printf("Choisissez une classe : ");
    scanf("%d", &choix_classe);
    
    char nom_fichier[50];
    sprintf(nom_fichier, "classe%d.txt", choix_classe);
    FILE *f_apprenants = fopen(nom_fichier, "r");
    if (f_apprenants == NULL) {
        printf("Erreur lors de l'ouverture du fichier des apprenants de la classe.\n");
        exit(1);
    }
    
    printf("Liste des apprenants de la classe %d :\n", choix_classe);
    Apprenant etudiants[100]; // Suppose que le nombre maximal d'apprenants est 100, ajustez-le selon vos besoins
    int nbEtudiants = 0;
    while (fscanf(f_apprenants, "%s %s", etudiants[nbEtudiants].nom, etudiants[nbEtudiants].prenom) != EOF) {
        printf("%s %s\n", etudiants[nbEtudiants].nom, etudiants[nbEtudiants].prenom);
        nbEtudiants++;
    }
    fclose(f_apprenants);
    
    marquerPresence(etudiants, nbEtudiants); // Appel de la fonction pour marquer les présences
}
