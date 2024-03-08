
#include "projet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>

#define MAX_USERS 100
#define MAX_PASSWORD_LENGTH 25

typedef struct
{
    char username[20];
    char password[20];
} User;

typedef struct
{
    char nom[50];
    char prenom[50];
    int presence;
} Apprenant;

void afficherListePresences();
void marquerPresence();
void afficherClasses();
void marquerPresenceApprenant(int code);
int afficherClassesApprenant();
void saisirMotDePasse(char *password);
void menuAdmin();
void loadUsers(const char *filename, User users[], int *numUsers);
int verifierUtilisateur(User *utilisateur, User users[], int numUsers);
void logout();
void disableEcho();
void enableEcho();
int SaisiUser();
int rechercherApprenantParCode(int code);
int verifierPresenceApprenant(int code);
bool estDejaPresent(int code);
void getMotDePasse(char *password);

void loadUsers(const char *filename, User users[], int *numUsers)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    while ((*numUsers < MAX_USERS) && fscanf(file, "%s %s", users[*numUsers].username, users[*numUsers].password) == 2)
    {
        (*numUsers)++;
    }

    fclose(file);
}

int verifierUtilisateur(User *utilisateur, User users[], int numUsers)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].username, utilisateur->username) == 0 && strcmp(users[i].password, utilisateur->password) == 0)
        {
            return 1;
        }
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

    while ((ch = getchar()) != '\n' && ch != '\r' && i < MAX_PASSWORD_LENGTH - 1)
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
    char pwd[MAX_PASSWORD_LENGTH];
    char reponse;
    int result;
    int valid;
    do
    {
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

        loadUsers("apprenant.txt", apprenants, &numApprenants);
        loadUsers("admin.txt", admins, &numAdmins);

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
            puts("Voulez-vous réessayer de vous connecter ? (o/n)");
            scanf(" %c", &reponse);
            getchar();
        }

    } while (reponse != 'n');

    return typeUtilisateur;
}

int rechercherApprenantParCode(int code)
{
    FILE *fichier = fopen("liste_apprenant.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier de la liste des apprenants.\n");
        exit(1);
    }

    int codeApprenant;
    char nom[50], prenom[50];
    while (fscanf(fichier, "%d %s %s", &codeApprenant, nom, prenom) != EOF)
    {
        if (codeApprenant == code)
        {
            fclose(fichier);
            return 1; // L'apprenant est trouvé
        }
    }

    fclose(fichier);
    return 0;
}

bool estDejaPresent(int code)
{
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier des présences.\n");
        exit(1);
    }

    int codePres;
    char datePres[20];
    time_t maintenant = time(NULL);
    struct tm *temps_info = localtime(&maintenant);
    char date[11];
    strftime(date, sizeof(date), "%Y-%m-%d", temps_info);

    while (fscanf(fichier, "%d %s", &codePres, datePres) != EOF)
    {
        if (codePres == code && strcmp(date, datePres) == 0)
        {
            fclose(fichier);
            return true;
        }
    }

    fclose(fichier);
    return false;
}

void marquerPresenceApprenant(int code)
{
    if (estDejaPresent(code))
    {
        printf("L'apprenant avec le code %d est déjà présent aujourd'hui.\n", code);
        return;
    }

    FILE *fichier = fopen("presences.txt", "a");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        exit(1);
    }

    time_t maintenant = time(NULL);
    struct tm *temps_info = localtime(&maintenant);
    char date[20];
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", temps_info);

    fprintf(fichier, "%d %s\n", code, date);
    fclose(fichier);

    printf("Présence de l'apprenant avec le code %d enregistrée à %s\n", code, date);
}

int verifierMotDePasseAdmin(char *password)
{

    User admins[MAX_USERS];
    int numAdmins = 0;
    loadUsers("admin.txt", admins, &numAdmins);

    for (int i = 0; i < numAdmins; i++)
    {
        if (strcmp(admins[i].password, password) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void marquerPresence()
{
    char choix[5];
    char pwd[MAX_PASSWORD_LENGTH];
    time_t maintenant;
    struct tm *temps_info;
    char date[20];

    do
    {
        printf("Entrer le code de l'apprenant (Q to quit): ");
        fgets(choix, sizeof(choix), stdin);
        choix[strcspn(choix, "\n")] = '\0';
        if (strcmp(choix, "Q") == 0 || strcmp(choix, "q") == 0)
        {
            printf("Entrer votre mot de passe: ");

            getMotDePasse(pwd);
            printf("\n");
            if (verifierMotDePasseAdmin(pwd))
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
            int code = atoi(choix);
            maintenant = time(NULL);
            temps_info = localtime(&maintenant);
            strftime(date, sizeof(date), "%Y-%m-%d", temps_info);
            if (estDejaPresent(code))
            {
                printf("L'apprenant avec le code %d est déjà présent aujourd'hui.\n", code);
                continue;
            }
            if (rechercherApprenantParCode(code))
            {
                marquerPresenceApprenant(code);
            }
            else
            {
                printf("L'apprenant avec le code %d n'existe pas.\n", code);
                sleep(2);
                continue;
            }
        }
        while (getchar() != '\n');
            
    } while (1);
}

void afficherListePresences()
{
    FILE *fichier = fopen("presences.txt", "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier des présences.\n");
        return;
    }

    printf("Liste des présences :\n");
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL)
    {
        printf("%s", ligne);
    }

    fclose(fichier);
}

void menuAdmin()
{

    int choix_admin;
    do
    {
        printf("1-------GESTION DES ÉTUDIANTS\n");
        printf("2-------GÉNÉRATIONS DES FICHIERS\n");
        printf("3-------MARQUER LES PRESENCES\n");
        printf("4-------LISTES DES PRÉSENCES\n");
        printf("5-------ENVOYER UN MESSAGES\n");
        printf("6-------DECONNEXION\n");
        printf("VOTRE CHOIX : ");
        scanf("%d", &choix_admin);
        if (choix_admin == 3)
        {
            printf("Vous avez choisi de marquer la présence de chaque étudiant\n");
            marquerPresence();
        }

        if (choix_admin == 4)
        {
            printf("Vous avez choisi d'afficher la liste des étudiants\n");

            afficherListePresences();
        }
        if (choix_admin == 6)
        {
            logout();
        }

    } while (choix_admin >= 1 && choix_admin <= 6);
}

int main()
{
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    User admins[MAX_USERS];
    int numAdmins = 0;

    loadUsers("apprenant.txt", apprenants, &numApprenants);
    loadUsers("admin.txt", admins, &numAdmins);
    int typeUtilisateur = SaisiUser();

    if (typeUtilisateur == 1)
    {
        printf("Connecté en tant qu'apprenant.\n");
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
