#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define MAX_APPRENANTS 100
#define MAX_LONGUEUR_INFO 50

typedef struct
{
    int id;
    char matricule[MAX_LONGUEUR_INFO];
    char username[MAX_LONGUEUR_INFO];
    char nom[MAX_LONGUEUR_INFO];
    char prenom[MAX_LONGUEUR_INFO];
    char mot_de_passe[MAX_LONGUEUR_INFO];
    char classe[MAX_LONGUEUR_INFO];
} Apprenant;

void genererMatriculeId(Apprenant *a, int i) {
    a->id = ++i;
    a->matricule[0] = 'M';
    a->matricule[1] = 'A';
    a->matricule[2] = 'T';
    a->matricule[3] = a->prenom[0];
    a->matricule[4] = a->nom[0];
    a->matricule[5] = '0' + (a->id % 10); // Conversion de l'ID en caractère
    a->matricule[6] = '\0';



    // sprintf(a->matricule, "MAT-%c%c-%2d", Initialprenom, Initialnom, a->id);
}

int main()
{
    Apprenant apprenants[MAX_APPRENANTS];
    int nombre_apprenants = 0;
    Apprenant a;

    // Demander à l'utilisateur de saisir les informations des apprenants
    printf("Entrez les informations des apprenants (username, prenom, nom, classe, mot de passe).\n");
    printf("Entrez 'fin' pour terminer la saisie.\n");

    while (nombre_apprenants < MAX_APPRENANTS)
    {
        printf("Apprenant %d : ", nombre_apprenants + 1);
        scanf("%s", apprenants[nombre_apprenants].username);

        if (strcmp(apprenants[nombre_apprenants].username, "fin") == 0)
        {
            break;
        }

        scanf("%s %s %s %s",
              apprenants[nombre_apprenants].prenom,
              apprenants[nombre_apprenants].nom,
              apprenants[nombre_apprenants].classe,
              apprenants[nombre_apprenants].mot_de_passe);

        genererMatriculeId(&apprenants[nombre_apprenants], nombre_apprenants);

        nombre_apprenants++;
    }
    // char Initialprenom = a.prenom[0];
    // char Initialnom = a.nom[0];
    // printf("%c %c", Initialprenom, Initialnom);

    // Ouvrir le fichier en mode d'écriture
    FILE *fichier = fopen("apprenant.txt", "w");
    if (fichier == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier.");
        return 1;
    }

    fprintf(fichier, "| ID | Matricule | username | Prénom | Nom  | Classe  | Mot de passe |\n");
    fprintf(fichier, "|********************************************************************|\n");

    // Écriture des données des apprenants dans le fichier
    for (int i = 0; i < nombre_apprenants; i++) {
        fprintf(fichier, "| %-2d | %-9s | %-9s | %-6s | %-4s | %-7s | %-12s |\n",
        
                apprenants[i].matricule,
                apprenants[i].username,
                apprenants[i].prenom,
                apprenants[i].classe,
                apprenants[i].mot_de_passe);
    }

    // Fermeture du fichier
    fclose(fichier);

    printf("%d apprenants enregistrés avec succès dans le fichier 'presences.txt'.\n", nombre_apprenants);

    return 0;
}
