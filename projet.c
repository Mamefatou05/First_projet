
#include "projet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_USERS 100
#define MAX_SIZE 50

char currentUsername[MAX_SIZE];

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

typedef struct
{
    char matricule[MAX_SIZE];
    char prenom[MAX_SIZE];
    char classe[MAX_SIZE];
    char password[MAX_SIZE];
    char date[20];
    char heure[20];
    int minutes_retard;
} Presence;

#define MAX_MESSAGES 100

typedef struct
{
    char expediteur[MAX_SIZE];
    char destinataire[MAX_SIZE];
    char date[MAX_SIZE];
    char heure[MAX_SIZE];
    char message[MAX_SIZE * 5];
} Message;

void loadApprenants(char *filename, User apprenants[], int *numApprenants);
void loadAdmins(char *filename, User admins[], int *numAdmins);
int verifierUtilisateur(User *utilisateur, User users[], int numUsers);
void logout();
void disableEcho();
void enableEcho();
void getMotDePasse(char *password);
int SaisiUser();
int rechercherApprenant(char *username, char *matricule, char *prenom, char *classe, char *password);
int Present(char *matricule, char *date);
int calculerRetard(char *heure_arrivee);
void marquerPresenceApprenant(char *matricule, char *prenom, char *classe, char *password, char *heure_arrivee);
void marquerPresenceAdmin();
void GenerePresence();
void afficherPresences();
void GenereFichier();
int verifierAdmin(char *password);
int userExists(User users[], int numUsers, char *username, char *classe);
void addToInbox(char *expediteur, char *username, char *date, char *heure, char *message);
void MessageClass(char *expediteur, User users[], int numUsers, char *classe, char *message);
void MessageUser(char *expediteur, User users[], int numUsers, char *destinataires[], int numDestinataires, char *message);
void MessagePromotion(char *expediteur, User users[], int numUsers, char *message);
void EnvoiMessage();
void decrementerMessagesLus(char *username);
int AfficherMessages(char *username);
void afficherRetardsParMatricule();
int BunieuwulMaBayi(char *username);
int RechercheApprenant(User apprenants[], int numApprenants, char *username);
void writeApprenants(char *filename, User apprenants[], int numApprenants);
void ajoutApprenant(User apprenants[], int *numApprenants);
void modifierApprenant(User apprenants[], int numApprenants);
void supprimerApprenant(User apprenants[], int *numApprenants);
void Gestion_etudiant() ;

void menuAdmin();
void menuApprenant();

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

void loadAdmins(char *filename, User admins[], int *numAdmins)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    while ((*numAdmins < MAX_USERS) && (fscanf(file, "%s %s",
                                               admins[*numAdmins].username,
                                               admins[*numAdmins].password) == 2))
    {
        (*numAdmins)++;
    }
    printf("%s %s", admins[*numAdmins].username, admins[*numAdmins].password);
    fclose(file);
}

int verifierUtilisateur(User *utilisateur, User users[], int numUsers)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].username, utilisateur->username) == 0 &&
            strcmp(users[i].password, utilisateur->password) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void logout()
{

    currentUsername[0] = '\0';

    printf("Déconnexion réussie !\n");
}

void disableEcho()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enableEcho()
{
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void getMotDePasse(char *password)
{
    int i = 0;
    char ch;

    disableEcho();

    while ((ch = getchar()) != '\n' && ch != '\r' && i < MAX_SIZE - 1)
    {
        if (ch == 127 || ch == 8)
        {
            if (i > 0)
            {
                printf("\b \b");
                i--;
            }
        }
        else
        {
            password[i++] = ch;
            putchar('*');
        }
    }

    password[i] = '\0';

    enableEcho();
}

int SaisiUser()
{
    int typeUtilisateur = 0;
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    User admins[MAX_USERS];
    int numAdmins = 0;
    User utilisateur;
    char username[15];
    char pwd[MAX_SIZE];
    char reponse;
    int result;
    int valid;
    do
    {

        loadApprenants("liste_apprenants.txt", apprenants, &numApprenants);
        loadAdmins("admin.txt", admins, &numAdmins);
        do
        {
            valid = 0;

            printf("Veuillez saisir votre nom d'utilisateur: ");
            fgets(username, sizeof(username), stdin);
            if (username[0] == '\n' || username[0] == '\0')
            {
                puts("nom d'utilisateur invalide");
                valid = 1;
            }
        } while (valid == 1);

        printf("Entrer votre mot de passe: ");

        getMotDePasse(pwd);
        printf("\n");

        username[strcspn(username, "\n")] = '\0';
        pwd[strcspn(pwd, "\n")] = '\0';

        strcpy(utilisateur.username, username);
        strcpy(utilisateur.password, pwd);

        result = verifierUtilisateur(&utilisateur, admins, numAdmins);

        if (result == 1)
        {
            typeUtilisateur = 2; // Administrateur
            break;
        }

        if (typeUtilisateur != 2)
        {
            result = verifierUtilisateur(&utilisateur, apprenants, numApprenants);
            if (result == 1)
            {
                typeUtilisateur = 1;               // Apprenant
                strcpy(currentUsername, username); // Stockage du nom d'utilisateur actuel

                break;
            }
        }
        else
        {
            puts("username ou mot de passe invalide");
            puts("Voulez-vous réessayer ? (o/n)");
            scanf(" %c", &reponse);
            getchar();
        }

    } while (reponse != 'n');

    return typeUtilisateur;
}

int rechercherApprenant(char *username, char *matricule, char *prenom, char *classe, char *password)
{
    FILE *fichier = fopen("liste_apprenants.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier de la liste des apprenants.\n");
        exit(1);
    }

    int trouve = 0;
    int id;
    char matriculeApprenant[MAX_SIZE];

    while (fscanf(fichier, "%s %s %s %s %s", username, matriculeApprenant, prenom, classe, password) != EOF)
    {
        if (strcmp(matriculeApprenant, matricule) == 0)
        {
            trouve = 1;
            break;
        }
        // printf("%s %s", matriculeApprenant, matricule);
    }

    fclose(fichier);

    return trouve;
}

int Present(char *matricule, char *date)
{
    FILE *fichier = fopen("Present.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier de présence.\n");
        return 0;
    }

    char ligne[MAX_SIZE];
    int minuteR;
    char matriculeP[20], dateP[20], prenomP[20], heureP[20], classeP[20], passwordP[20];

    while (fgets(ligne, MAX_SIZE, fichier) != NULL)
    {
        sscanf(ligne, "%s %s %s %s %s %s %d", matriculeP, prenomP, classeP, passwordP, dateP, heureP, &minuteR);
        if (strcmp(matriculeP, matricule) == 0 && strcmp(dateP, date) == 0)
        {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0;
}

int calculerRetard(char *heure_arrivee)
{
    int heures, minutes;
    sscanf(heure_arrivee, "%d:%d", &heures, &minutes);
    int retard = (heures - 8) * 60 + minutes - 15;
    return retard > 0 ? retard : 0;
}

void marquerPresenceApprenant(char *matricule, char *prenom, char *classe, char *password, char *heure_arrivee)
{
    FILE *fichier = fopen("Present.txt", "a+");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    time_t maintenant = time(NULL);
    struct tm *temps_info = localtime(&maintenant);
    char date[20];
    strftime(date, 20, "%Y-%m-%d", temps_info);
    char heure[20];
    strftime(heure, 20, "%H:%M:%S", temps_info);

    int minutes_retard = calculerRetard(heure_arrivee);

    if (temps_info->tm_hour >= 16)
    {
        printf("L'enregistrement de présence n'est plus autorisé après 16h.\n");
        fclose(fichier);
        return;
    }

    if (Present(matricule, date))
    {
        printf("L'apprenant avec le matricule %s est déjà marqué présent pour la date %s.\n", matricule, date);
        fclose(fichier);
        return;
    }

    fprintf(fichier, "%s %s %s %s %s %s %d\n", matricule, prenom, classe, password, date, heure, minutes_retard);
    fclose(fichier);
    printf("Présence de l'apprenant avec le matricule %s enregistrée à %s %s avec %d minutes de retard\n", matricule, date, heure, minutes_retard);
}

void loadPresences(char *filename, Presence presences[], int *numPresences)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    while ((*numPresences < MAX_USERS) && (fscanf(file, "%s %s %s %s %s %s %d",

                                                  presences[*numPresences].matricule,
                                                  presences[*numPresences].prenom,
                                                  presences[*numPresences].classe,
                                                  presences[*numPresences].password,
                                                  presences[*numPresences].date,
                                                  presences[*numPresences].heure,
                                                  &presences[*numPresences].minutes_retard) == 7))
    {
        (*numPresences)++;
    }

    fclose(file);
}

void obtenirAbsents(User *apprenants, Presence *presences)
{
    FILE *fichier = fopen("Absents.txt", "a");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }
    int numApprenants = 0;
    int numPresences = 0;
    loadPresences("Present.txt", presences, &numPresences);
    loadApprenants("liste_apprenants.txt", apprenants, &numApprenants);

    char *heure_limite = "16:00:00";
    for (int i = 0; i < numPresences; ++i)
    {
        if (i == 0 || strcmp(presences[i].date, presences[i - 1].date) != 0)
        {
            char *date = presences[i].date;

            fprintf(fichier, "Absents pour le %s :\n", date);

            for (int j = 0; j < numApprenants; ++j)
            {
                int absent = 1;

                for (int k = 0; k < numPresences; ++k)
                {
                    if (strcmp(presences[k].matricule, apprenants[j].matricule) == 0 &&
                        strcmp(presences[k].date, date) == 0)
                    {
                        if (strcmp(presences[k].heure, heure_limite) <= 0)
                        {
                            absent = 0;
                        }
                        break;
                    }
                }

                if (absent)
                {
                    fprintf(fichier, "%s %s %s %s \n", apprenants[j].matricule, apprenants[j].prenom, apprenants[j].classe, apprenants[j].password);
                }
            }

            fprintf(fichier, "\n");
        }
    }

    fclose(fichier);
}

void marquerPresenceAdmin()
{
    char choix[5];
    char pwd[MAX_SIZE];
    time_t maintenant;
    struct tm *temps_info;
    char date[MAX_SIZE];
    char matricule[MAX_SIZE];
    char username[MAX_SIZE];
    char prenom[MAX_SIZE];
    char classe[MAX_SIZE];
    char password[MAX_SIZE];

    do
    {
        // getchar();
        printf("Entrer le matricule de l'apprenant (Q pour quitter) : ");
        fgets(matricule, sizeof(matricule), stdin);
        matricule[strcspn(matricule, "\n")] = '\0';
        if (strcmp(matricule, "Q") == 0 || strcmp(matricule, "q") == 0)
        {
            printf("Entrer votre mot de passe: ");

            getMotDePasse(pwd);
            printf("\n");
            if (verifierAdmin(pwd))
            {
                break;
            }
            else
            {
                printf("Mot de passe incorrect. La déconnexion a été annulée.\n");
            }
        }
        else
        {
            char heure_arrivee[10];
            maintenant = time(NULL);
            temps_info = localtime(&maintenant);
            strftime(date, sizeof(date), "%Y-%m-%d", temps_info);
            strftime(heure_arrivee, sizeof(heure_arrivee), "%H:%M", temps_info);

            printf("%s", matricule);

            if (rechercherApprenant(username, matricule, prenom, classe, password))
            {
                marquerPresenceApprenant(matricule, prenom, classe, password, heure_arrivee);
            }
            else
            {
                printf("L'apprenant avec le code %s n'existe pas.\n", matricule);
                sleep(2);
                continue;
            }
        }

    } while (1);
}

int verifierAdmin(char *password)
{

    User admins[MAX_USERS];
    int numAdmins = 0;
    loadAdmins("admin.txt", admins, &numAdmins);

    for (int i = 0; i < numAdmins; i++)
    {
        if (strcmp(admins[i].password, password) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int validerDate(char *date)
{
    if (strlen(date) != 10)
        return 0;
    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
        {
            if (date[i] != '-')
                return 0;
        }
        else
        {
            if (!isdigit(date[i]))
                return 0;
        }
    }
    return 1;
}

void GenerePresence()
{

    FILE *fichier = fopen("Present.txt", "r");
    FILE *ficheG = fopen("genere.txt", "a");

    if (fichier == NULL || ficheG == NULL)
    {
        printf("Impossible d'ouvrir les fichiers.\n");
        exit(1);
    }

    char ligne[MAX_SIZE];
    char matricule[20], nom[20], classe[20], mdp[20], date[20], heure[20];
    int minuteR;

    char datePrecedente[20] = "";
    int presenceTrouvee = 0;

    while (fgets(ligne, MAX_SIZE, fichier) != NULL)
    {
        if (sscanf(ligne, "%s %s %s %s %s %s %d", matricule, nom, classe, mdp, date, heure, &minuteR) != 7)
        {
            printf("Erreur de lecture de la ligne : %s\n", ligne);
            continue;
        }
        if (strcmp(date, datePrecedente) != 0)
        {
            if (presenceTrouvee)
            {
                fprintf(ficheG, "|-------------------------------------------------------|\n");
            }
            fprintf(ficheG, "Présences pour la date : %s\n", date);
            fprintf(ficheG, "| Matricule | Nom  | Classe  | Mot de passe | Heure     |\n");
            fprintf(ficheG, "|*******************************************************|\n");
            strcpy(datePrecedente, date);
            presenceTrouvee = 1;
        }
        fprintf(ficheG, "| %-10s| %-4s | %-7s | %-12s | %-8s |\n", matricule, nom, classe, mdp, heure);
        fprintf(ficheG, "|-------------------------------------------------------|\n");
    }

    fclose(ficheG);
    fclose(fichier);
}

void GenerePresenceDate(char *date)
{

    if (!validerDate(date))
    {
        printf("Format de date invalide : %s\n", date);
        return;
    }
    FILE *fiche = fopen("genere.txt", "r");
    FILE *fichier = fopen("genereDate.txt", "w");

    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    char ligne[MAX_SIZE];
    int presenceTrouvee = 0;
    while (fgets(ligne, MAX_SIZE, fiche) != NULL)
    {
        char matricule[20], nom[20], classe[20], mdp[20], dateLue[20], heure[20];
        if (sscanf(ligne, "%s %s %s %s %s %s", matricule, nom, classe, mdp, dateLue, heure) != 6)
        {
            fprintf(fiche, "Erreur de lecture de la ligne : %s\n", ligne);
            continue;
        }

        fprintf(fichier, "Présences pour la date : %s\n", date);
        fprintf(fichier, "| Matricule | Nom  | Classe  | Mot de passe | Heure     |\n");
        fprintf(fichier, "|*******************************************************|\n");

        if (strcmp(dateLue, date) == 0)
        {
            fprintf(fichier, "| %-10s| %-4s | %-7s | %-12s | %-8s |\n", matricule, nom, classe, mdp, heure);
            fprintf(fichier, "|-------------------------------------------------------|\n");
            presenceTrouvee = 1;
        }
    }

    if (!presenceTrouvee)
    {
        fprintf(fichier, "Aucune présence trouvée pour la date %s.\n", date);
    }

    fclose(fichier);
    fclose(fiche);
}

void afficherPresences()
{
    FILE *ficheG = fopen("genere.txt", "r");
    if (ficheG == NULL)
    {
        printf("Impossible d'ouvrir le fichier genere.txt.\n");
        exit(1);
    }
    char ligne[MAX_SIZE];
    while (fgets(ligne, MAX_SIZE, ficheG) != NULL)
    {
        printf("%s", ligne);
    }
    fclose(ficheG);
}

void genererRapportRetards()
{
    FILE *fichier_present = fopen("Present.txt", "r");
    if (fichier_present == NULL)
    {
        printf("Impossible d'ouvrir le fichier de présence.\n");
        return;
    }

    User apprenants[MAX_SIZE]; // tableau pour stocker les apprenants
    int nb_apprenants = 0;
    int QUOTA_RETARD = 90;

    // Initialisation des apprenants
    for (int i = 0; i < MAX_SIZE; i++)
    {
        strcpy(apprenants[i].matricule, "");
        apprenants[i].total_retard = 0;
        apprenants[i].renvoi = 0;
    }

    char ligne[MAX_SIZE];
    while (fgets(ligne, MAX_SIZE, fichier_present) != NULL)
    {
        char matriculeP[20], dateP[20], prenomP[20], heureP[20], classeP[20], mot_de_passeP[20];
        int minutes_retard;
        sscanf(ligne, "%s %s %s %s %s %s %d", matriculeP, prenomP, classeP, mot_de_passeP, dateP, heureP, &minutes_retard);

        // Recherche de l'apprenant dans le tableau
        int found = 0;
        for (int i = 0; i < nb_apprenants; i++)
        {
            if (strcmp(apprenants[i].matricule, matriculeP) == 0)
            {
                found = 1;
                apprenants[i].total_retard += minutes_retard;
                if (apprenants[i].total_retard > QUOTA_RETARD)
                {
                    apprenants[i].renvoi = 1;
                }
                break;
            }
        }

        // Si l'apprenant n'est pas trouvé, l'ajouter au tableau
        if (!found)
        {
            strcpy(apprenants[nb_apprenants].matricule, matriculeP);
            apprenants[nb_apprenants].total_retard = minutes_retard;
            if (apprenants[nb_apprenants].total_retard > QUOTA_RETARD)
            {
                apprenants[nb_apprenants].renvoi = 1;
                nb_apprenants++;
            }
        }

        fclose(fichier_present);

        FILE *rapport_retards = fopen("RapportRetards.txt", "w");
        if (rapport_retards == NULL)
        {
            printf("Impossible d'ouvrir le fichier pour générer le rapport de retards.\n");
            return;
        }

        fprintf(rapport_retards, "Matricule\tTotal Retard (minutes)\tRenvoi\n");
        for (int i = 0; i < nb_apprenants; i++)
        {
            fprintf(rapport_retards, "%s\t\t%d\t\t\t%d\n", apprenants[i].matricule, apprenants[i].total_retard, apprenants[i].renvoi);
        }

        fclose(rapport_retards);

        printf("Le rapport de retards a été généré avec succès.\n");
    }
}

void afficherRetardsParMatricule()
{
    FILE *fichier_retards = fopen("RapportRetards.txt", "r");
    if (fichier_retards == NULL)
    {
        printf("Impossible d'ouvrir le fichier de rapport de retards.\n");
        return;
    }

    printf("Matricule\tTotal Retard (minutes)\n");

    char matricule[20];
    int retard, total_retard, renvoi;

    while (fprintf(fichier_retards, "%s\t\t%d\t\t\t%d\n", matricule, total_retard, renvoi))
    {
        printf("%s\t\t%d\t\t\t%d\n", matricule, total_retard, renvoi);
    }

    fclose(fichier_retards);
}

void GenereFichier()
{

    User apprenants[MAX_USERS];
    Presence presences[MAX_USERS];
    int numApprenants = 0;
    int numPresences = 0;
    int choix_liste;
    char date[20];
    do
    {
        printf("1-------Listes des présences\n");
        printf("2-------Listes des absences\n");
        printf("3-------Listes des cumul de retard par mois\n");
        printf("4-------Listes des absences par dates\n");
        printf("Votre choix : \n");
        scanf("%d", &choix_liste);
        if (choix_liste == 1)
        {
            printf("Listes des presences :\n");
            GenerePresence();
            printf("Fichier bien généré!!\n");
        }
        else if (choix_liste == 2)
        {
            printf("Listes des absences :\n");
            obtenirAbsents(apprenants, presences);
            printf("Fichier bien généré!!\n");
        }
        else if (choix_liste == 3)
        {
            printf("Veuillez entrer une date au format 'YYYY-MM-DD' : ");
            scanf("%s", date);
            GenerePresenceDate(date);
        }
        else if (choix_liste == 4)
        {
            printf("Listes des retards par mois :\n");
            genererRapportRetards();
            printf("Fichier bien généré!!\n");
        }

        else
        {
            printf("Choix invalide. Veuillez choisir entre 1 à 4.\n");
        }

    } while (choix_liste >= 1 && choix_liste <= 4);
}


int userExists(User users[], int numUsers, char *username, char *classe)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (username != NULL && strcmp(users[i].username, username) == 0)
        {
            return 1;
        }
        if (classe != NULL && strcmp(users[i].classe, classe) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void addToInbox(char *expediteur, char *username, char *date, char *heure, char *message)
{
    char filename[MAX_SIZE * 2];
    sprintf(filename, "%s_inbox.txt", username);
    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return;
    }

    fprintf(file, "%s %s %s %s %s\n", expediteur, username, date, heure, message);
    fclose(file);
}

void MessageClass(char *expediteur, User users[], int numUsers, char *classe, char *message)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].classe, classe) == 0)
        {
            char date[11], heure[9];
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(date, 11, "%Y-%m-%d", tm_info);
            strftime(heure, 9, "%H:%M:%S", tm_info);
            addToInbox(expediteur, users[i].username, date, heure, message);
        }
    }
}

void MessageUser(char *expediteur, User users[], int numUsers, char *destinataires[], int numDestinataires, char *message)
{
    for (int i = 0; i < numDestinataires; i++)
    {
        char date[11], heure[9];
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        strftime(date, 11, "%Y-%m-%d", tm_info);
        strftime(heure, 9, "%H:%M:%S", tm_info);
        addToInbox(expediteur, destinataires[i], date, heure, message);
    }
}

void MessagePromotion(char *expediteur, User users[], int numUsers, char *message)
{
    for (int i = 0; i < numUsers; i++)
    {
        char date[11], heure[9];
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        strftime(date, 11, "%Y-%m-%d", tm_info);
        strftime(heure, 9, "%H:%M:%S", tm_info);
        addToInbox(expediteur, users[i].username, date, heure, message);
    }
}

void EnvoiMessage()
{
    int choix_message;
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    loadApprenants("liste_apprenants.txt", apprenants, &numApprenants);
    char message[MAX_SIZE];
    int numDestinataires = 0;
    int numClasses = 0;
    char reponse;
    char expediteur[MAX_SIZE];
    printf("Entrez le nom de l'administrateur : ");
    fgets(expediteur, MAX_SIZE, stdin);
    expediteur[strcspn(expediteur, "\n")] = '\0';

    do
    {
        printf("Envoyer un message vers tous les apprenants de la promo\n");
        printf("Envoyer un message vers une classe\n");
        printf("Envoyer un message vers des étudiants choisis\n");
        printf("Votre choix :  ");
        scanf("%d", &choix_message);
        while ((getchar()) != '\n')
            ;

        if (choix_message == 1)
        {

            printf("Entrez le message pour toute la promotion : ");
            fgets(message, MAX_SIZE, stdin);
            MessagePromotion(expediteur, apprenants, numApprenants, message);
        }
        if (choix_message == 2)
        {
            printf("Entrez le message : ");
            fgets(message, MAX_SIZE, stdin);

            char **classes = malloc(MAX_USERS * sizeof(char *));
            if (classes == NULL)
            {
                printf("Erreur d'allocation de mémoire.\n");
                exit(EXIT_FAILURE);
            }

            int numClasses = 0;
            char reponse;
            do
            {
                classes[numClasses] = malloc(MAX_SIZE * sizeof(char));
                if (classes[numClasses] == NULL)
                {
                    printf("Erreur d'allocation de mémoire.\n");
                    exit(EXIT_FAILURE);
                }

                printf("Entrez la classe : ");
                fgets(classes[numClasses], MAX_SIZE, stdin);
                classes[numClasses][strcspn(classes[numClasses], "\n")] = '\0'; // Remove newline character

                if (!userExists(apprenants, numApprenants, NULL, classes[numClasses]))
                {
                    printf("La classe spécifiée n'existe pas.\n");
                    free(classes[numClasses]);
                    continue;
                }

                numClasses++;

                if (numClasses < MAX_USERS)
                {
                    printf("Voulez-vous ajouter une autre classe ? (O/N) : ");
                    scanf(" %c", &reponse);
                    while ((getchar()) != '\n')
                        ;
                }
            } while (reponse == 'O' || reponse == 'o');

            for (int i = 0; i < numClasses; i++)
            {
                MessageClass(expediteur, apprenants, numApprenants, classes[i], message);
                free(classes[i]);
            }
            free(classes);
        }
        else if (choix_message == 3)
        {
            printf("Entrez le message : ");
            fgets(message, MAX_SIZE, stdin);

            char **destinataires = malloc(MAX_USERS * sizeof(char *));
            if (destinataires == NULL)
            {
                printf("Erreur d'allocation de mémoire.\n");
                exit(EXIT_FAILURE);
            }

            int numDestinataires = 0;
            char reponse;
            do
            {
                destinataires[numDestinataires] = malloc(MAX_SIZE * sizeof(char));
                if (destinataires[numDestinataires] == NULL)
                {
                    printf("Erreur d'allocation de mémoire.\n");
                    exit(EXIT_FAILURE);
                }

                printf("Entrez le destinataire : ");
                fgets(destinataires[numDestinataires], MAX_SIZE, stdin);
                destinataires[numDestinataires][strcspn(destinataires[numDestinataires], "\n")] = '\0'; // Remove newline character

                if (!userExists(apprenants, numApprenants, destinataires[numDestinataires], NULL))
                {
                    printf("Le destinataire spécifié n'existe pas.\n");
                    free(destinataires[numDestinataires]);
                    continue;
                }

                numDestinataires++;

                if (numDestinataires < MAX_USERS)
                {
                    printf("Voulez-vous ajouter un autre destinataire ? (O/N) : ");
                    scanf(" %c", &reponse);
                    while ((getchar()) != '\n')
                        ;
                }
            } while (reponse == 'O' || reponse == 'o');

            MessageUser(expediteur, apprenants, numApprenants, destinataires, numDestinataires, message);

            for (int i = 0; i < numDestinataires; i++)
            {
                free(destinataires[i]);
            }
            free(destinataires);
        }

        else
        {
            printf("Choix Invalide\n");
        }
    } while (choix_message < 1 || choix_message > 3);
}

int BunieuwulMaBayi(char *username)
{
    char filename[MAX_SIZE * 2];
    sprintf(filename, "%s_inbox.txt", username);
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s\n", filename);
        return -1; // Retourne -1 en cas d'erreur
    }

    int nbLignes = 0;

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        nbLignes++;
    }

    fclose(file);

    return nbLignes;
}

// void supprimerTousMessages(char *username)
// {
//     char filename[MAX_SIZE * 2];
//     sprintf(filename, "%s_inbox.txt", username);
//     FILE *file = fopen(filename, "w");
//     if (file == NULL)
//     {
//         printf("Impossible d'ouvrir le fichier %s\n", filename);
//         return;
//     }

//     fclose(file);
// }

int AfficherMessages(char *username)
{
    char filename[MAX_SIZE * 2];
    sprintf(filename, "%s_inbox.txt", username);
    FILE *file = fopen(filename, "w+");
    if (file == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s\n", filename);
        return 0; // Retourner 0 s'il y a une erreur
    }

    Message messages[MAX_MESSAGES];
    int nbMessages = 0;

    while (nbMessages < MAX_MESSAGES && fscanf(file, "%s %s %s %s %[^\n]",
                                               messages[nbMessages].expediteur,
                                               messages[nbMessages].destinataire,
                                               messages[nbMessages].date,
                                               messages[nbMessages].heure,
                                               messages[nbMessages].message) != EOF)
    {
        nbMessages++;
    }

    fclose(file);

    if (nbMessages == 0)
    {
        printf("Aucun message trouvé pour l'utilisateur %s.\n", username);
        return 0;
    }

    // Afficher les messages
    for (int i = 0; i < nbMessages; i++)
    {
        printf("De : %s\n", messages[i].expediteur);
        printf("Date : %s %s\n", messages[i].date, messages[i].heure);
        printf("Message : ");
        puts(messages[i].message);
        printf("\n");
    }

    // Supprimer tous les messages lus et décrémenter le nombre de messages lus
    // for (int i = 0; i < nbMessages; i++)
    // {
    //     supprimerTousMessages(username);
    //     decrementerMessagesLus(username);
    // }

    printf("Tous les messages lus avec succès !!!\n");
    printf("Appuyez sur Entrée pour sortir....\n");

    return nbMessages;
}

// void decrementerMessagesLus(char *username)
// {
//     char filename[MAX_SIZE * 2];
//     sprintf(filename, "%s_inbox.txt", username);
//     FILE *file = fopen(filename, "r+");
//     if (file == NULL)
//     {
//         printf("Impossible d'ouvrir le fichier %s\n", filename);
//         return;
//     }

//     int count;
//     fscanf(file, "%d", &count);
//     if (count > 0)
//     {
//         count--;
//     }

//     // Revenir au début du fichier et écrire le nouveau nombre de messages
//     fseek(file, 0, SEEK_SET);
//     fprintf(file, "%d\n", count);

//     fclose(file);
// }


// Fonction pour rechercher un apprenant spécifique
int RechercheApprenant(User apprenants[], int numApprenants, char *username)
{
    for (int i = 0; i < numApprenants; i++)
    {
        if (strcmp(apprenants[i].username, username) == 0)
        {
            return i; 
        }
    }
    return -1; 
}

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

// Fonction pour ajouter un apprenant
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
    if (RechercheApprenant(apprenants, *numApprenants, newApprenant.username) != -1)
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
    char usernameM[MAX_SIZE];
    printf("Entrez le nom d'utilisateur de l'apprenant à modifier : ");
    scanf("%s", usernameM);

    int index = RechercheApprenant(apprenants, numApprenants, usernameM);
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
    char usernameS[MAX_SIZE];
    printf("Entrez le nom d'utilisateur de l'apprenant à supprimer : ");
    scanf("%s", usernameS);

    int index = RechercheApprenant(apprenants, *numApprenants, usernameS);
    if (index != -1)
    {
        // Déplacer les apprenants suivants vers l'indice précédent
        for (int i = index; i < *numApprenants - 1; i++)
        {
            apprenants[i] = apprenants[i + 1];
        }

        (*numApprenants)--;
        printf("Apprenant supprimé avec succès.\n");

        writeApprenants("liste_apprenants.txt", apprenants, *numApprenants);
    }
    else
    {
        printf("Apprenant non trouvé.\n");
    }
}

void Gestion_etudiant() {
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

                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix != 4);

}

void menuAdmin()
{

    int choix_admin;
    do
    {
        printf("1-------GESTION DES ÉTUDIANTS\n");
        printf("2-------GÉNÉRATIONS DES FICHIERS\n");
        printf("3-------MARQUER LES PRESENCES\n");
        printf("5-------ENVOYER UN MESSAGES\n");
        printf("6-------DECONNEXION\n");
        printf("VOTRE CHOIX : \n");
        scanf("%d", &choix_admin);
        while (getchar() != '\n')
            ;
        if (choix_admin == 1)
        {
             Gestion_etudiant() ;
        }
        else if (choix_admin == 2)
        {
            GenereFichier();
        }
        else if (choix_admin == 3)
        {
            printf("Vous avez choisi de marquer la présence de chaque étudiant\n");
            marquerPresenceAdmin();
        }

        else if (choix_admin == 5)
        {
            EnvoiMessage();
        }
        else if (choix_admin == 6)
        {
            logout();
            return;
        }

        else
        {
            printf("Choix invalide\n");
        }

    } while (choix_admin >= 1 && choix_admin <= 6);
}


void menuApprenant()
{
    time_t maintenant;
    struct tm *temps_info;
    char date[MAX_SIZE];
    int choix_apprenant;
    char matricule[MAX_SIZE];
    User apprenant;
    char *username = NULL;

    char prenom[MAX_SIZE];
    char classe[MAX_SIZE];
    char password[MAX_SIZE];

    //       strcpy(username, apprenant.username);

    // printf("le username est  %s", username);

    int nbM = 0;

    nbM = BunieuwulMaBayi(currentUsername);

    do
    {

        printf("1-------MARQUER MA PRÉSENCE\n");
        printf("2-------NOMBRE DE MINUTES D’ABSENCE\n");
        printf("3-------MES MESSAGES (%d)\n", nbM);
        printf("4-------DECONNEXION\n");
        printf("VOTRE CHOIX : ");
        scanf("%d", &choix_apprenant);

        while (getchar() != '\n')
            ;

        if (choix_apprenant == 1)
        {
            printf("Vous avez choisi de marquer votre présence\n");
            char heure_arrivee[10];
            maintenant = time(NULL);
            temps_info = localtime(&maintenant);
            strftime(date, sizeof(date), "%Y-%m-%d", temps_info);
            strftime(heure_arrivee, sizeof(heure_arrivee), "%H:%M", temps_info);

            printf("Entrer votre matricule : ");
            fgets(matricule, sizeof(matricule), stdin);
            matricule[strcspn(matricule, "\n")] = '\0';

            if (rechercherApprenant(username, matricule, prenom, classe, password))
            {
                marquerPresenceApprenant(matricule, prenom, classe, password, heure_arrivee);

                return;
            }
            else
            {
                printf("Matricule invalide.\n");
                sleep(2);
                continue;
            }
        }
        else if (choix_apprenant == 2)
        {
            afficherRetardsParMatricule();
        }

        else if (choix_apprenant == 3)
        {
            AfficherMessages(currentUsername);
            nbM = BunieuwulMaBayi(currentUsername);
        }

        if (choix_apprenant == 4)
        {
            logout();
        }
        while (getchar() != '\n')
            ;
    } while (choix_apprenant >= 1 && choix_apprenant <= 4);
}

int main()
{
    User apprenants;
    int numApprenants = 0;
    User admins[MAX_USERS];
    int numAdmins = 0;

    int typeUtilisateur = SaisiUser();

    if (typeUtilisateur == 1)
    {
        printf("Connecté en tant qu'apprenant.\n");
        menuApprenant(apprenants);
    }
    else if (typeUtilisateur == 2)
    {
        printf("Connecté en tant qu'administrateur.\n");
        menuAdmin();
    }
    else
    {
        printf("Nom d'utilisateur ou mot de passe incorrect.\n");
    }

    return 0;
}