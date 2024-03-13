#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 50
#define MAX_USERS 100

typedef struct
{
    char username[MAX_SIZE];
    char matricule[MAX_SIZE];
    char prenom[MAX_SIZE];
    char classe[MAX_SIZE];
    char password[MAX_SIZE];
    char messages[MAX_SIZE];
    int total_retard;
    int renvoi;
} User;

// Fonction pour charger les apprenants à partir d'un fichier
void loadApprenants(char *filename, User apprenants[], int *numApprenants)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    // Lecture des apprenants à partir du fichier
    while ((*numApprenants < MAX_USERS) &&
           (fscanf(file, "%s %s %s %s %s",
                   apprenants[*numApprenants].username,
                   apprenants[*numApprenants].matricule,
                   apprenants[*numApprenants].prenom,
                   apprenants[*numApprenants].classe,
                   apprenants[*numApprenants].password) == 5))
    {
        (*numApprenants)++;
    }

    fclose(file);
}

// Fonction pour rechercher un apprenant spécifique
int rechercheApprenant(User apprenants[], int numApprenants, char *username)
{
    for (int i = 0; i < numApprenants; i++)
    {
        if (strcmp(apprenants[i].username, username) == 0)
        {
            return i; // Retourne l'indice de l'apprenant trouvé
        }
    }
    return -1; // Retourne -1 si l'apprenant n'est pas trouvé
}

// Fonction pour écrire les apprenants dans un fichier
void writeApprenants(char *filename, User apprenants[], int numApprenants)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    // Écriture des apprenants dans le fichier
    for (int i = 0; i < numApprenants; i++)
    {
        fprintf(file, "%s %s %s %s %s\n", apprenants[i].username, apprenants[i].matricule, apprenants[i].prenom, apprenants[i].classe, apprenants[i].password);
    }

    fclose(file);
}


 void ajoutApprenant(User apprenants[], int *numApprenants)
{
    if (*numApprenants >= MAX_USERS)
    {
        printf("Nombre maximum d'apprenants atteint.\n");
        return;
    }

    User newApprenant;
    printf("Entrez le nom d'utilisateur: ");
    scanf("%s", newApprenant.username);

    // Vérifier si l'apprenant existe déjà
    if (rechercheApprenant(apprenants, *numApprenants, newApprenant.username) != -1)
    {
        printf("L'apprenant existe déjà.\n");
        return;
    }

    printf("Entrez le matricule: ");
    scanf("%s", newApprenant.matricule);
    printf("Entrez le prénom: ");
    scanf("%s", newApprenant.prenom);
    printf("Entrez la classe: ");
    scanf("%s", newApprenant.classe);
    printf("Entrez le mot de passe: ");
    scanf("%s", newApprenant.password);

    apprenants[*numApprenants] = newApprenant;
    (*numApprenants)++;
    printf("Apprenant ajouté avec succès.\n");

    // Écrire la liste mise à jour dans le fichier
    writeApprenants("liste_apprenants.txt", apprenants, *numApprenants);
}

// Fonction pour modifier les informations d'un apprenant
void modifierApprenant(User apprenants[], int numApprenants)
{
    char matriculeRecherche[MAX_SIZE];
    printf("Entrez le matricule de l'apprenant à modifier : ");
    scanf("%s", matriculeRecherche);

    int index = rechercheApprenant(apprenants, numApprenants, matriculeRecherche);
    if (index != -1)
    {
        printf("Entrez le nouveau matricule: ");
        scanf("%s", apprenants[index].matricule);
        printf("Entrez le nouveau prénom: ");
        scanf("%s", apprenants[index].prenom);
        printf("Entrez la nouvelle classe: ");
        scanf("%s", apprenants[index].classe);
        printf("Entrez le nouveau mot de passe: ");
        scanf("%s", apprenants[index].password);
        printf("Apprenant modifié avec succès.\n");

        // Écrire la liste mise à jour dans le fichier
        writeApprenants("liste_apprenants.txt", apprenants, numApprenants);
    }
    else
    {
        printf("Apprenant non trouvé.\n");
    }
}

// Fonction pour supprimer un apprenant
void supprimerApprenant(User apprenants[], int *numApprenants)
{
    char matriculeRecherche[MAX_SIZE];
    printf("Entrez le matricule de l'apprenant à supprimer : ");
    scanf("%s", matriculeRecherche);

    int index = rechercheApprenant(apprenants, *numApprenants, matriculeRecherche);
    if (index != -1)
    {
        // Déplacer les apprenants suivants vers l'indice précédent
        for (int i = index; i < *numApprenants - 1; i++)
        {
            apprenants[i] = apprenants[i + 1];
        }

        (*numApprenants)--;
        printf("Apprenant supprimé avec succès.\n");

        // Écrire la liste mise à jour dans le fichier
        writeApprenants("liste_apprenants.txt", apprenants, *numApprenants);
    }
    else
    {
        printf("Apprenant non trouvé.\n");
    }
}


int main()
{
    User apprenants[MAX_USERS]; 
    int count = 0;               

    loadApprenants("liste_apprenants.txt", apprenants, &count);

     int choix;
    do {
        printf("\n1. Ajouter un apprenant\n2. Modifier un apprenant\n3. Supprimer un apprenant\n4. Quitter\n");
        printf("Entrez votre choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajoutApprenant(apprenants, &count);
                break;
            case 2:
                modifierApprenant(apprenants, count);
                break;
            case 3:
                supprimerApprenant(apprenants, &count);
                break;
            case 4:
                printf("Fin du programme.\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix != 4);

    return 0;
}
