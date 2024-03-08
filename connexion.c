#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_USERS 100
#define MAX_PASSWORD_LENGTH 25

typedef struct {
    char username[20];
    char password[20];
} User;

void saisirMotDePasse(char *password);

void loadUsers(const char *filename, User users[], int *numUsers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", filename);
        exit(1);
    }

    while ((*numUsers < MAX_USERS) && fscanf(file, "%s %s", users[*numUsers].username, users[*numUsers].password) == 2) {
        (*numUsers)++;
    }

    fclose(file);
}

int verifierUtilisateur(User *utilisateur, User users[], int numUsers) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, utilisateur->username) == 0 && strcmp(users[i].password, utilisateur->password) == 0) {
            return 1;
        }
    }
    return 0;
}

void logout() {
    printf("Déconnexion réussie !\n");
}

void disableEcho() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enableEcho() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}



void saisirMotDePasse(char *password) {
    printf("Entrez votre mot de passe : ");
    fflush(stdout);

    disableEcho();

    int i = 0;
    char ch;

    while ((ch = getchar()) != '\n' && ch != '\r' && i < MAX_PASSWORD_LENGTH - 1) {
        if (ch == 127 || ch == 8) {
            if (i > 0) {
                printf("\b \b");
                i--;
            }
        } else {
            password[i++] = ch;
            putchar('*');
        }
    }

    password[i] = '\0';
    enableEcho();
    printf("\n");
}
int SaisiUser() {

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
    do {
        do {
            valid = 0;

            printf("Veuillez saisir votre username : ");
            fgets(username, sizeof(username), stdin);
            if (username[0] == '\n' || username[0] == '\0') {
                puts("username invalide");
                valid = 1;
            }
        } while (valid == 1);

        saisirMotDePasse(pwd);

        username[strcspn(username, "\n")] = '\0';
        pwd[strcspn(pwd, "\n")] = '\0';
        
        // Copier les chaînes de caractères dans la structure User
        
        strcpy(utilisateur.username, username);
        strcpy(utilisateur.password, pwd);

         loadUsers("apprenant.txt", apprenants, &numApprenants);
         loadUsers("admin.txt", admins, &numAdmins);
        result = verifierUtilisateur(&utilisateur, apprenants, numApprenants);
        result = verifierUtilisateur(&utilisateur, admins, numAdmins);

        if (result == 0) {
            puts("username ou mot de passe invalide");
            puts("Voulez-vous réessayer de vous connecter ? (o/n)");
            scanf(" %c", &reponse);
            getchar();
        } else {
            break;
        }
    } while (reponse != 'n');

    return result;
}


int main() {
   
   

    // char username[15];
    // char pwd[MAX_PASSWORD_LENGTH];

    int utilisateur = SaisiUser();

    if (utilisateur == 1) {
        printf("Connecté en tant qu'apprenant.\n");
    } else {
        printf("Nom d'utilisateur ou mot de passe incorrect.\n");
    }

    logout();
    return 0;
}
