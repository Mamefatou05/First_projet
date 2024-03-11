
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

typedef struct
{
    char username[MAX_SIZE];
    char matricule[MAX_SIZE];
    char prenom[MAX_SIZE];

    char classe[MAX_SIZE];
    char password[MAX_SIZE];
    char messages[MAX_SIZE];
} User;

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
void marquerPresenceApprenant(char *matricule, char *prenom, char *classe, char *password);
void marquerPresenceAdmin();
void afficherPresences();
void afficherPresenceDate(char *date);
void GenereFichier();
int verifierAdmin(char *password);
void EnvoiMessage();
void addToInbox(char *username, char *message) ;
int userExists(User users[], int numUsers, const char *username, const char *classe) ;
void EnvoiMessage();
void addToInbox(char *username, char *message) ;
int userExists(User users[], int numUsers, const char *username, const char *classe);
void MessageUser(User users[], int numUsers, char *username, char *message);
void MessageClass(User users[], int numUsers, char *classe, char *message);
void MessagePromotion(User users[], int numUsers, char *message) ;

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
        printf("%s %s", users[i].username, users[i].password);
    }
    return 0;
}

void logout()
{
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
                typeUtilisateur = 1; // Apprenant
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
        printf("%s %s", matriculeApprenant, matricule);
    }

    fclose(fichier);

    return trouve;
}

int Present(char *date, char *matricule)
{
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        return 0; // Impossible de vérifier la présence
    }

    char ligne[MAX_SIZE];
    char tempDate[20], tempMatricule[20];

    while (fgets(ligne, MAX_SIZE, fichier) != NULL)
    {
        if (strstr(ligne, date) != NULL)
        {
            sscanf(ligne, "| %s| %*s| %*s| %*s| %*s| %*s| %*s|", tempMatricule);
            if (strcmp(tempMatricule, matricule) == 0)
            {
                fclose(fichier);
                return 1; // Apprenant déjà présent
            }
        }
    }

    fclose(fichier);
    return 0; // Apprenant non présent
}

void marquerPresenceApprenant(char *matricule, char *prenom, char *classe, char *password)
{
    FILE *fichier = fopen("presences.txt", "a");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    time_t maintenant = time(NULL);
    struct tm *temps_info = localtime(&maintenant);
    char date[20];
    strftime(date, 20, "%Y-%m-%d", temps_info);

    if (Present(date, matricule))
    {
        printf("L'apprenant avec le matricule %s est déjà présent aujourd'hui.\n", matricule);
        fclose(fichier);
        return;
    }

    static char datePrecedente[20] = ""; // Garder la trace de la date précédente
    static int premierEnregistrement = 1;

    if (strcmp(date, datePrecedente) != 0 || premierEnregistrement)
    {
        if (!premierEnregistrement)
        {
            fprintf(fichier, "|*************************************|\n\n");
        }
        fprintf(fichier, "La liste des présences pour la date %s est :\n", date);
        fprintf(fichier, "\n");
        fprintf(fichier, "|Mat    |Nom  | Classe |Mdp |Heure    |\n");
        fprintf(fichier, "|*************************************|\n");
        strcpy(datePrecedente, date); // Mettre à jour la date précédente
        premierEnregistrement = 0;
    }

    char heure[20];
    strftime(heure, 20, "%H:%M:%S", temps_info);
    fprintf(fichier, "| %s| %s| %s| %s| %s|\n", matricule, prenom, classe, password, heure);
    fclose(fichier);
    printf("Présence de l'apprenant avec le matricule %s enregistrée à %s %s\n", matricule, date, heure);
    // rangerPresenceParDate() ; // You need to implement this function
}

void menuApprenant()
{
    time_t maintenant;
    struct tm *temps_info;
    char date[MAX_SIZE];
    int choix_apprenant;
    char matricule[MAX_SIZE];
    char username[MAX_SIZE];
    char prenom[MAX_SIZE];
    char classe[MAX_SIZE];
    char password[MAX_SIZE];
    do
    {
        printf("1-------MARQUER MA PRÉSENCE\n");
        printf("2-------NOMBRE DE MINUTES D’ABSENCE\n");
        printf("3-------MES MESSAGES (0)\n");
        printf("4-------DECONNEXION\n");
        printf("VOTRE CHOIX : ");
        scanf("%d", &choix_apprenant);

        while (getchar() != '\n')
            ;

        if (choix_apprenant == 1)
        {
            printf("Vous avez choisi de marquer votre présence\n");

            maintenant = time(NULL);
            temps_info = localtime(&maintenant);
            strftime(date, sizeof(date), "%Y-%m-%d", temps_info);

            printf("Entrer votre matricule : ");
            fgets(matricule, sizeof(matricule), stdin);
            matricule[strcspn(matricule, "\n")] = '\0';

            if (rechercherApprenant(username, matricule, prenom, classe, password))
            {
                marquerPresenceApprenant(matricule, prenom, classe, password);
                return;
            }
            else
            {
                printf("Matricule invalide.\n");
                sleep(2);
                continue;
            }
        }

        if (choix_apprenant == 4)
        {
            logout();
        }
        while (getchar() != '\n')
            ;
    } while (choix_apprenant >= 1 && choix_apprenant <= 4);
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
            maintenant = time(NULL);
            temps_info = localtime(&maintenant);
            strftime(date, sizeof(date), "%Y-%m-%d", temps_info);
            printf("%s", matricule);

            // if (Present(matricule, date))
            // {
            //     printf("L'apprenant avec le matricule %s est déjà présent aujourd'hui.\n", matricule);
            //     printf("cliquer sur entrer pour continuer.....\n");
            //     return;
            // }
            if (rechercherApprenant(username, matricule, prenom, classe, password))
            {
                marquerPresenceApprenant(matricule, prenom, classe, password);
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

void afficherPresences()
{
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    char ligne[100];

    while (fgets(ligne, sizeof(ligne), fichier) != NULL)
    {
        printf("%s", ligne);
    }

    fclose(fichier);
}


void afficherPresenceDate(char *date) {
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    char ligne[1000]; 
    int presenceTrouvee = 0;

    printf("La liste des présences pour la date %s est :\n", date);
    // printf("| Mat    |Nom  | Classe |Mdp |Heure   |\n");
    // printf("|*************************************|\n");
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (strstr(ligne, date) != NULL) {
            printf("%s", ligne);
            presenceTrouvee = 1;
        }
    }

    fclose(fichier);

    if (!presenceTrouvee) {
        printf("Aucune présence trouvée pour la date %s.\n", date);
    }
}
void GenereFichier()
{
    int choix_liste = 0;
    char date[20];
    do
    {
        printf("1-------Listes des présences\n");
        printf("2-------Listes des présences par dates\n");
        printf("Votre choix : \n");
        scanf("%d", &choix_liste);

        if (choix_liste == 1)
        {
            printf("Listes des presences :\n");
            afficherPresences();
        }
        else if (choix_liste == 2)
        {

            printf("Veuillez entrer la date au format 'YYYY-MM-DD': ");
            scanf("%s", date);

            if (!validerDate(date))
            {
                printf("Format de date incorrect. Veuillez saisir une date au format 'YYYY-MM-DD'.\n");
            }
            else
            {
                afficherPresenceDate(date);
            }
        }

        else
        {
            printf("Choix invalide. Veuillez entrer 1 ou 2.\n");
        }

    } while (choix_liste != 1 && choix_liste != 2);
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

        if (choix_admin == 3)
        {
            printf("Vous avez choisi de marquer la présence de chaque étudiant\n");
            marquerPresenceAdmin();
        }

        else if (choix_admin == 2)
        {
            GenereFichier();
        }
        else if (choix_admin == 5)
        {
            EnvoiMessage();
        }

        else if (choix_admin == 6)
        {
            logout();
            return ;
        }

        else
        {
            printf("Choix invalide\n");
        }

    } while (choix_admin >= 1 && choix_admin <= 6);
}

void EnvoiMessage()
{

    int choix_message;
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    loadApprenants("liste_apprenants.txt", apprenants, &numApprenants);
    char message[MAX_SIZE];
    char destinataire[MAX_SIZE];
    char date[11], heure[9];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(date, 11, "%Y-%m-%d", tm_info);
    strftime(heure, 9, "%H:%M:%S", tm_info);

    do
    {

        printf("Envoyer un message vers une classe\n");
        printf("Envoyer un message vers tous apprenants de la promo\n");
        printf("Envoyer un message vers des étudiants choisi\n");
        printf("Votre choix :  \n");
        scanf("%d", &choix_message);
        if (choix_message == 1)
        {
            MessageClass(apprenants, numApprenants, destinataire, message);
        }
        else if (choix_message == 2)
        {
            MessageUser(apprenants, numApprenants, destinataire, message);

        }
        else if (choix_message == 3)
        {
            MessagePromotion(apprenants, numApprenants, message);
        }
        else
        {
            printf("Choix Invalide\n");
        }

    } while (choix_message != 1, choix_message != 2, choix_message != 3);
}

int userExists(User users[], int numUsers, const char *username, const char *classe) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0){
            return 1; 
        }
        if (strcmp(users[i].classe, classe) == 0) {
            return 1; 
        }
    }
    return 0; 

}    
void addToInbox(char *username, char *message) {
    char filename[MAX_SIZE];
    sprintf(filename, "%s_inbox.txt", username); // Nom du fichier de la boîte de réception de l'apprenant

    FILE *file = fopen(filename, "w"); // Ouvrir le fichier en mode écriture
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return;
    }

    // Obtenir l'heure et la date actuelles
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[11], heure[9];
    strftime(date, 11, "%Y-%m-%d", tm_info);
    strftime(heure, 9, "%H:%M:%S", tm_info);

    // Écrire le détail du message dans le fichier
    fprintf(file, "|%s|%s|%s\n", date, heure, message);

    fclose(file);
}

void MessageUser(User users[], int numUsers, char *username, char *message) {

if (!userExists(users, numUsers, username, NULL)) {
        printf("Utilisateur non trouvé.\n");
        return;
    }    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Entrez le message pour %s : ", users[i].prenom);
            fflush(stdin);
            fgets(message, MAX_SIZE, stdin);
            // Ajouter le message à la boîte de réception de l'utilisateur
            addToInbox(username, message);
            break;
        }
    }
    
}

void MessageClass(User users[], int numUsers, char *classe, char *message) {
 if (!userExists(users, numUsers, NULL, classe)) {
        printf("Classe non trouvée.\n");
        return;
    }  for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].classe, classe) == 0) {
            printf("Entrez le message pour %s (%s) : ", users[i].prenom, users[i].classe);
            fflush(stdin);
            fgets(message, MAX_SIZE, stdin);
            addToInbox(users[i].username, message);
        }
    }
   
}

void MessagePromotion(User users[], int numUsers, char *message) {
    printf("Entrez le message pour toute la promotion : ");
    fflush(stdin);
    fgets(message, MAX_SIZE, stdin);

    for (int i = 0; i < numUsers; i++) {
        printf("Message envoyé à %s (%s) : %s\n", users[i].prenom, users[i].classe, message);
        // Ajouter le message à la boîte de réception de l'utilisateur
        addToInbox(users[i].username, message);
    }
}



int main()
{
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    User admins[MAX_USERS];
    int numAdmins = 0;

    int typeUtilisateur = SaisiUser();

    if (typeUtilisateur == 1)
    {
        printf("Connecté en tant qu'apprenant.\n");
        menuApprenant();
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
