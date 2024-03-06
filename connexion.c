#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>



#define MAX_USERS 100

typedef struct {
    char username[20];
    char password[20];
} User;

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

void masquerMotDePasse(char *password) {
    int i = 0;
    printf("Mot de passe masqué: ");
    while (password[i] != '\0') {
        printf("*");
        i++;
    }
    printf("\n");
}

User saisirUser() {
    User u;

    printf("Donnez le nom d'utilisateur: ");
    fgets(u.username, sizeof(u.username), stdin);
    u.username[strcspn(u.username, "\n")] = '\0';

    printf("Donnez votre mot de passe: ");
    fflush(stdout); 

    char c;
    int i = 0;
    while ((c = getchar()) != '\n') {
        if (c == 127) { 
            if (i > 0) { 
                printf("\b \b"); 
                i--;
            }
        } else {
            u.password[i++] = c;
            printf("*"); 
            printf("\n");
       
        }

    }
    return u ;
}     


int verifierUtilisateur(User *utilisateur, User users[], int numUsers) {
    for (int i = 0; i < numUsers; i++) {
        // printf(" %s - %s\n", utilisateur->username, users[i].username);
        // printf(" %s - %s\n", utilisateur->password, users[i].password);
        if (strcmp(users[i].username, utilisateur->username) == 0 && strcmp(users[i].password, utilisateur->password) == 0) {
            return 1;
        }
    }
    return 0;
}


int main() {
    User apprenants[MAX_USERS];
    int numApprenants = 0;
    User admins[MAX_USERS];
    int numAdmins = 0;

    loadUsers("apprenant.txt", apprenants, &numApprenants);
    loadUsers("admin.txt", admins, &numAdmins);

    User utilisateur = saisirUser();

    if (verifierUtilisateur(&utilisateur, apprenants, numApprenants)) {
        printf("Connecté en tant qu'apprenant.\n");

    } else if (verifierUtilisateur(&utilisateur, admins, numAdmins)) {
        printf("Connecté en tant qu'administrateur.\n");

    } else {
        printf("Nom d'utilisateur ou mot de passe incorrect.\n");
    }

    return 0;
}


