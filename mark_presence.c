#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
typedef struct
{
    char nom[50];
    char prenom[50];
    int presence; // 0 pour absent, 1 pour présent
} Apprenant;


void afficherListePresences();
void marquerPresence(Apprenant *etudiants, int nbEtudiants);
void afficherClasses();
void marquerPresenceApprenant(Apprenant *apprenant, int nbapprenant) ;
int afficherClassesApprenant() ;

int main()
{   
    int nbApprenants = 0; 
    Apprenant apprenants[ nbApprenants]; 
    
    int choix;
    // do
    // {
    //     printf("\nMenu Admin:\n");
    //     printf("1. Gestion des étudiants\n");
    //     printf("2. Génération de fichiers\n");
    //     printf("3. Marquer les présences\n");
    //     printf("4. Listes des présences\n");
    //     printf("5. Envoyer un message\n");
    //     printf("6. Quitter\n");
    //     printf("Votre choix : ");
    //     scanf("%d", &choix);
    //     if (choix == 3)
    //     {
    //         printf("Vous avez choisi de marquer la présence de chaque étudiant\n");
    //         afficherClasses(); // Appel de la fonction pour afficher les classes et marquer les présences
    //     }

    //     if (choix == 4)
    //     {
    //        afficherListePresences() ;
    //     }
        
    // } while (choix != 5);




    return 0;
}
// pour le menu des admin 
void marquerPresence(Apprenant *apprenant, int nbapprenant) {
    char presence[10];
    printf("Veuillez entrer la présence des étudiants (\"present\" pour présent, \"absent\" pour absent):\n");
    FILE *fichier;
    fichier = fopen("presences.txt", "a"); 

    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    // Lire les présences déjà enregistrées dans le fichier
    int id;
    char nom[50], prenom[50], date[20];
    int presenceExistante;
    while (fscanf(fichier, "%d: %s %s: %d - %[^\n]", &id, nom, prenom, &presenceExistante, date) != EOF) {
        // Vérifier si une présence a déjà été enregistrée pour cet étudiant à la date actuelle
        time_t maintenant = time(NULL);
        struct tm *temps_info = localtime(&maintenant);
        char dateActuelle[20];
        strftime(dateActuelle, 20, "%Y-%m-%d", temps_info);
        if (strcmp(dateActuelle, date) == 0) {
            printf("La présence de %s %s a déjà été enregistrée pour aujourd'hui.\n", nom, prenom);
            continue;
        }
    }

    // Revenir au début du fichier pour ajouter de nouvelles présences
    fseek(fichier, 0, SEEK_END);


    for (int i = 0; i < nbapprenant; i++) {
        printf("%d - %s %s: ", i + 1, apprenant[i].nom, apprenant[i].prenom);
        scanf("%s", presence);

        if (strcmp(presence, "present") == 0) {
            apprenant[i].presence = 1;
        } else if (strcmp(presence, "absent") == 0) {
            apprenant[i].presence = 0;
        } else {
            printf("Veuillez entrer \"present\" ou \"absent\".\n");
            i--; // Pour répéter la saisie pour cet étudiant
            continue; // Passer à l'itération suivante de la boucle
        }

        time_t maintenant = time(NULL);
        struct tm *temps_info = localtime(&maintenant);
        char date[20];
        strftime(date, 20, "%Y-%m-%d %H:%M:%S", temps_info);

        fprintf(fichier, "%d: %s %s: %d - %s\n", i + 1, apprenant[i].nom, apprenant[i].prenom, apprenant[i].presence, date);
    }

    fclose(fichier);
}

void afficherClasses()
{
    FILE *f_classes = fopen("class.txt", "rb");
    if (f_classes == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier des classes.\n");
        exit(1);
    }

    printf("Liste des classes :\n");
    char classe[50];
    int i = 0;
    while (fgets(classe, sizeof(classe), f_classes) != NULL)
    {
        i = i + 1;
        printf("%d - %s", i, classe);
    }
    fclose(f_classes);

    int choix_classe;
    printf("\n");
    printf("Choisissez une classe : ");
    scanf("%d", &choix_classe);

    char nom_fichier[50];
    switch (choix_classe)
    {
    case 1:
        strcpy(nom_fichier, "dev_web.txt");
        break;
    case 2:
        strcpy(nom_fichier, "ref_dig.txt");
        break;
    case 3:
        strcpy(nom_fichier, "dev_data.txt");
        break;
    default:
        printf("Classe invalide.\n");
        exit(1);

        
    }

    FILE *f_apprenants = fopen(nom_fichier, "r");
    if (f_apprenants == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier des apprenants de la classe.\n");
        exit(1);
    }
    Apprenant talibe[100];
    int nb_apprenants = 0;
    char ligne[100];

    while (fgets(ligne, sizeof(ligne), f_apprenants) != NULL)
    {
        sscanf(ligne, "%s %s", talibe[nb_apprenants].nom, talibe[nb_apprenants].prenom);
        nb_apprenants++;
    }
    fclose(f_apprenants);

    // Affichage de la liste des apprenants
    printf("Liste des apprenants de la classe %d :\n", choix_classe);
    for (int i = 0; i < nb_apprenants; i++)
    {
        printf("%s %s\n", talibe[i].nom, talibe[i].prenom);
    }

    // Appel de la fonction marquerPresence() avec les apprenants de la classe choisie
    marquerPresence(talibe, nb_apprenants);
}

void afficherListePresences() {
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier des présences.\n");
        return;
    }

    printf("Liste des présences :\n");
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        printf("%s", ligne);
    }

    fclose(fichier);
}








    // do
    // {
    //     printf("\nMenu Apprenant:\n");
    //     printf("1. Marquer ma présence\n");
    //     printf("2. Nombre de minutes d'absence\n");
    //     printf("3. Mes messages (0)\n");
    //     printf("4. Quitter\n");
    //     printf("Votre choix : ");
    //     scanf("%d", &choix);

    //     if (choix == 1)
    //     {
    //        marquerPresenceApprenant(apprenants, nbApprenants);
            
    //     }
        

    // } while (choix >= 1 || choix <= 4);
// pour le menu apprenant

int afficherClassesApprenant(){
    FILE *f_classes = fopen("class.txt", "r");
    if (f_classes == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier des classes.\n");
        exit(1);
    }
     
    printf("Liste des classes :\n");
    char classe[50];
    int i = 0;
    while (fgets(classe, sizeof(classe), f_classes) != NULL)
    {
        i = i + 1;
        printf("%d - %s", i, classe);
    }
    fclose(f_classes);

    int choix_class_app;
    printf("\n");
    printf("Choisissez une classe : ");
    scanf("%d", &choix_class_app);

    char nom_fichier[50];
    switch (choix_class_app)
    {
    case 1:
        strcpy(nom_fichier, "dev_web.txt");
        break;
    case 2:
        strcpy(nom_fichier, "ref_dig.txt");
        break;
    case 3:
        strcpy(nom_fichier, "dev_data.txt");
        break;
    default:
        printf("Classe invalide.\n");
        exit(1);

    return choix_class_app ;
        
    }
    
}

void marquerPresenceApprenant(Apprenant *apprenant, int nbapprenant) {
    char presence[10];
    char nom[50], prenom[50];
    int choix_class_app = afficherClassesApprenant();
    printf("Veuillez entrer votre nom: ");
    scanf("%s", nom);
    printf("Veuillez entrer votre prenom: ");
    scanf("%s", prenom);

    // Trouver l'apprenant dans la liste
    int index = -1;
    for (int i = 0; i < nbapprenant; i++) {
        if (strcmp(apprenant[i].nom, nom) == 0 && strcmp(apprenant[i].prenom, prenom) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Vous n'êtes pas dans la liste des étudiants.\n");
        return;
    }

    // Vérifier si l'apprenant est déjà enregistré comme présent par l'administrateur
    FILE *fichier;
    fichier = fopen("presences.txt", "r");

    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    int dejaPresent = 0;
    int id;
    char nomtalibe[50], prenomtalibe[50], date[20];
    int presenceExistante;
    while (fscanf(fichier, "%d: %s %s: %d - %[^\n]", &id, nomtalibe, prenomtalibe, &presenceExistante, date) != EOF) {
        // Vérifier si une présence a déjà été enregistrée pour cet étudiant à la date actuelle
        time_t maintenant = time(NULL);
        struct tm *temps_info = localtime(&maintenant);
        char dateActuelle[20];
        strftime(dateActuelle, 20, "%Y-%m-%d", temps_info);
        if (strcmp(nom, nomtalibe) == 0 && strcmp(prenom, prenomtalibe) == 0 && strcmp(dateActuelle, date) == 0) {
            dejaPresent = 1;
            printf("Vous êtes déjà enregistré comme présent par l'administrateur pour aujourd'hui.\n");
            break;
        }
    }

    fclose(fichier);

    if (dejaPresent) {
        return;
    }

    printf("Êtes-vous présent (Oui/Non)? ");
    scanf("%s", presence);

    if (strcmp(presence, "Oui") == 0 || strcmp(presence, "oui") == 0) {
        apprenant[index].presence = 1;
    } else if (strcmp(presence, "Non") == 0 || strcmp(presence, "non") == 0) {
        apprenant[index].presence = 0;
    } else {
        printf("Veuillez répondre avec 'Oui' ou 'Non'.\n");
        return;
    }

    // Enregistrer la présence dans le fichier
    fichier = fopen("presences.txt", "a"); // Ouvrir le fichier en mode ajout

    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    time_t maintenant = time(NULL);
    struct tm *temps_info = localtime(&maintenant);
    char datePresence[20];
    strftime(datePresence, 20, "%Y-%m-%d %H:%M:%S", temps_info);

    fprintf(fichier, "%s %s: %d - %s\n", nom, prenom, apprenant[index].presence, datePresence);

    fclose(fichier);

    printf("Votre présence a été enregistrée avec succès.\n");
}
