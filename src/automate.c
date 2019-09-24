#include "../include/automate.h"
#include "../include/affichage.h"

struct automate {
    unsigned int temps ; // le temps t actuelle, commence à 0
    cel** configuration_actuelle ; //chauqe char est un etat
    unsigned int dimension_max; // en gros la taille du tableau
    unsigned int nb_iterations_max; //
    regle _regle;
    void (*affichage) (automate);
    char *configuration_initiale;
};

automate creer_automate(unsigned int dimension_max, unsigned int nb_iterations_max){
    automate automate_cellulaire = (automate) malloc (sizeof(struct automate));
    automate_cellulaire->temps = 0;
    automate_cellulaire->configuration_actuelle = (cel**) malloc (sizeof(cel*) * nb_iterations_max);
    for(unsigned int i = 0; i < nb_iterations_max; i++){
        automate_cellulaire->configuration_actuelle[i] = (cel*) malloc (sizeof(cel) * dimension_max);
    }
    automate_cellulaire->_regle = NULL;
    automate_cellulaire->nb_iterations_max = nb_iterations_max;
    automate_cellulaire->dimension_max = dimension_max;

    return automate_cellulaire;
}

void supprimer_automate(automate* automate_cellulaire_ptr){
    automate automate_cellulaire = *automate_cellulaire_ptr;
    for(unsigned int i = 0; i < automate_cellulaire->nb_iterations_max; i++){
        for(unsigned int j = 0; j < automate_cellulaire->dimension_max; j++){
            supprimer_cellule(&automate_cellulaire->configuration_actuelle[i][j]);
        }
        free(automate_cellulaire->configuration_actuelle[i]);
        automate_cellulaire->configuration_actuelle[i] = NULL;
    }
    free(automate_cellulaire->configuration_actuelle);
    automate_cellulaire->configuration_actuelle = NULL;
    free(automate_cellulaire->configuration_initiale);
    automate_cellulaire->configuration_initiale=NULL;
    supprimer_regle(&automate_cellulaire->_regle);
    automate_cellulaire->_regle=NULL;
    free(automate_cellulaire);
    automate_cellulaire = NULL;
}
void afficher_automate(automate automate_cellulaire){
    automate_cellulaire->affichage(automate_cellulaire);
}

void set_affichage(automate a ,void (*afficher_automate)(automate)){
    a->affichage = afficher_automate;
}

void set_configuration_initiale(automate automate_cellulaire, char* configuration_initiale){
    for(unsigned int i = 0; i < automate_cellulaire->dimension_max; i++){
        automate_cellulaire->configuration_actuelle[0][i] = creer_cellule();
        set_etat(automate_cellulaire->configuration_actuelle[0][i], configuration_initiale[i] == '0' ? 0 : 1);
    }
    automate_cellulaire->configuration_initiale = (char*) malloc (sizeof(char) * strlen(configuration_initiale) + 1);
    strcpy(automate_cellulaire->configuration_initiale, configuration_initiale);
}

void set_voisins(automate automate_cellulaire, unsigned int k){
    for(unsigned int i = 1; i < automate_cellulaire->dimension_max; i++){
        set_voisin_gauche(automate_cellulaire->configuration_actuelle[k][i], automate_cellulaire->configuration_actuelle[k][i - 1]);
    }
    set_voisin_gauche(automate_cellulaire->configuration_actuelle[k][0], automate_cellulaire->configuration_actuelle[k][automate_cellulaire->dimension_max - 1]);
}

cel** get_configuration_actuelle(automate automate_cellulaire){
    return automate_cellulaire->configuration_actuelle;
}

unsigned int get_nb_iterations_max(automate automate_cellulaire){
    return automate_cellulaire->nb_iterations_max;
}

unsigned int get_dimension_max(automate automate_cellulaire){
    return automate_cellulaire->dimension_max;
}

char* get_regle_automate(automate automate_cellulaire){
    return get_regle(automate_cellulaire->_regle);
}

void (*get_affichage_regle_automate(automate automate_cellulaire))(int){
    return get_affichage_regle(automate_cellulaire->_regle);
}

void set_regle_automate(automate automate_cellulaire, regle r){
    automate_cellulaire->_regle = r;
}

cel** generer_automate(automate automate_cellulaire){
    set_voisins(automate_cellulaire, 0);
    for(unsigned int i = 1; i < automate_cellulaire->nb_iterations_max; i++){
        for(unsigned int j = 0; j < automate_cellulaire->dimension_max; j++){
            cel cellule = creer_cellule();
            regle r = automate_cellulaire->_regle;
            set_etat(cellule, etat_suivant(automate_cellulaire->configuration_actuelle[i-1][j], get_regle(r), get_type_regle(r)));
            automate_cellulaire->configuration_actuelle[i][j] = cellule;
        }
        set_voisins(automate_cellulaire, i);
    }
    return automate_cellulaire->configuration_actuelle;
}

automate lire_fichier_automate(){
    unsigned int nb_iterations = 0;
    unsigned int dimension = 0;
    char* regle_string = NULL;
    
    char* config_init = NULL;
    int (*type_regle)(char*, unsigned int, unsigned int, unsigned int) = NULL;
    void (*type_affichage)(automate) = NULL;
    void (*affichage_cellule)(int) = NULL;

    FILE* fp;
    char chaine[20000];

    regex_t preg;
    int err; 
    const char *str_regex = "\"(type_affichage|type_regle|nb_iteration|dimension|config_init|regle)\"=([0123456789]*);$" ;

    err = regcomp(&preg, str_regex, REG_EXTENDED | REG_NEWLINE); 

    if (err == 1){
        printf("Erreur à la compilation du regex !");
        exit(1);
    }

    fp = fopen("cfg/test2.config","r");
    
    if (fp == NULL){
        fprintf(stderr,"Erreur lors de l'ouverture du fichier en lecture !");
        exit(1);
    }

    int match;
    size_t nmatch = 0;
    regmatch_t *pmatch = NULL;

    nmatch=preg.re_nsub;

    char *type ;
    int start_type ;
    int end_type ;
    size_t size_type ;

    char *valeur;
    int start_valeur;
    int end_valeur;
    size_t size_valeur;

    while(fgets(chaine,20000,fp) != NULL){

        pmatch = realloc(pmatch, sizeof(*pmatch) * nmatch);

        if (pmatch){
            match = regexec(&preg,chaine,nmatch,pmatch,0);
        }            

        if (match == 0){
            type = NULL;
            valeur = NULL;
            start_type = pmatch[1].rm_so;
            end_type = pmatch[1].rm_eo;
            size_type = end_type - start_type;

            start_valeur = pmatch[1].rm_eo+2;
            end_valeur = pmatch[0].rm_eo-1;
            size_valeur = end_valeur - start_valeur;
               
            type = realloc (type, sizeof(*type) * (size_type + 1));
            valeur = realloc(valeur, sizeof(*valeur) * (size_valeur + 1));
            
            if(type){
               strncpy(type, &chaine[start_type], size_type);
               type[size_type] = '\0';
            }

            if(valeur){
                strncpy (valeur, &chaine[start_valeur], size_valeur);
                valeur[size_valeur] = '\0';
            }

            if(!strcmp(type, "nb_iteration")){
                if(nb_iterations!=0){
                    printf("Duplication du type : \"nb_iteration\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    nb_iterations=(unsigned int)conversion_char_int(valeur);
                }
            }else if(!strcmp(type, "dimension")){
                if(dimension != 0){
                    printf("Duplication du type : \"dimension\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    dimension=(unsigned int) conversion_char_int(valeur);
                }
            }else if(!strcmp(type, "regle")){
                if (regle_string != NULL){
                    printf("Duplication du type : \"regle\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    regle_string = (char*) malloc (sizeof(char) * strlen(valeur) + 1);
                    regle_string = strcpy(regle_string, valeur);
                }
            }else if(!strcmp(type, "config_init")){
               if (config_init != NULL){
                    printf("Duplication du type : \"config_init\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    config_init = (char*) malloc (sizeof(char) * strlen(valeur) + 1);
                    config_init = strcpy(config_init, valeur);
                }
            }else if(!strcmp(type, "type_regle")){
                if(type_regle != NULL){
                    printf("duplication du type : \"type_regle\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    switch (conversion_char_int(valeur)){
                        case 0: {
                            type_regle = &regle_binaire;
                            affichage_cellule = &afficher_cellule_binaire;
                            break;
                        }
                        case 1: {
                            type_regle = &regle_somme;
                            affichage_cellule = &afficher_cellule_somme;
                            break;
                        }
                        default: {
                            printf("Erreur de la regle. Arrêt du programme !");
                            exit(1);
                            break;
                        }
                    }
                }
            }else if(!strcmp(type, "type_affichage")){
                if(type_affichage != NULL){
                    printf("Duplication du type : \"type_affichage\". Arrêt du programme !\n");
                    exit(1);
                }else{
                    switch(conversion_char_int(valeur)){
                        case 0: {
                            type_affichage=&afficher_automate_console;
                            break;
                        }
                        case 1: {
                            type_affichage=&afficher_automate_pgm;
                            break;
                        }
                        default: {
                            printf("Erreur de l'affichage, arrêt du programme.");
                            exit(1);
                            break;
                        }
                    }
                }
            }else{
                printf("\n\nErreur type inconnu !\n\n");
                exit(1);
            }

            free (type);
            free (valeur);

        }else{
            printf("Fichier corrompu !\n");
            exit(1);
        }
    }
    if(dimension == 0 || regle_string == NULL || config_init == NULL || nb_iterations == 0 ||type_affichage== NULL || type_regle == NULL){
        printf("Fichier incomplet pour l'éxecution du programme. Arrêt du programme !\n");
        exit(1);
    }

    automate a = creer_automate(dimension,nb_iterations);

    regle r = creer_regle();
    set_regle(r, regle_string);
    set_type_regle(r, type_regle);
    set_affichage_regle(r, affichage_cellule);

    set_regle_automate(a, r);

    set_configuration_initiale(a, config_init);
    set_affichage(a,type_affichage);

    generer_automate(a);

    free(pmatch);
    free(regle_string);
    free(config_init);
    regfree(&preg);

    fclose(fp);
    return a;
}

automate lecture_runtime_automate(){
    unsigned int nb_iteration_max;
    unsigned int dimension_max;

    printf("Bienvenue dans le générateur d'automate cellulaire !\n");
    printf("Afin de générer un automate, veuillez rentrer les paramètres suivants :\n");

    printf("Nombre d'itérations : ");
    scanf("%ud\n", &nb_iteration_max);

    printf("Dimension maximale : ");
    scanf("%ud\n", &dimension_max);

    char configuration_initiale[dimension_max];
    char* regle = (char*) malloc (sizeof(char) * dimension_max);
    unsigned int type_regle;
    unsigned int type_affichage;

    printf("Configuration initiale : ");
    scanf("%s", configuration_initiale);
    
    while(strlen(configuration_initiale) != dimension_max){
        printf("\nVous devez rentrer une configuration initiale dont le nombre de cellule est égale à la dimension de l'automate !\n");
        scanf("%s", configuration_initiale);
    }
    
    printf("Règle : ");
    scanf("%s", regle);

    printf("Type règle : \n");
    printf("1 - Règle binaire\n");
    printf("2 - Règle somme\n");
    scanf("%ud", &type_regle);

    printf("Type affichage : \n");
    printf("1 - Affichage console\n");
    printf("2 - Génération d'une image ppm\n");
    scanf("%ud", &type_affichage);

    automate a = creer_automate(dimension_max, nb_iteration_max);
    
}
