#include <stdlib.h>

#include "../include/automate.h"
#include "../include/utils.h"


struct automate {
    unsigned int temps ; // le temps t actuelle, commence à 0
    cel** configuration_actuelle ; //chauqe char est un etat
    unsigned int dimension_max; // en gros la taille du tableau
    unsigned int nb_iterations_max; //
    unsigned int regle; // la regle a appliquer  (00011110) 2 = 30 || 0013100132 pour la somme 
    unsigned int nb_etats; //pas sûr wolfran =2(0,1) ; somme = 4(0,1,2,3)
    //comment définir le types de transition a effectuer ? somme || configuration des voisins ?
};

automate creer_automate(unsigned int dimension_max, unsigned int nb_iterations_max, unsigned int nb_etats){
    automate automate_cellulaire = (automate) malloc (sizeof(struct automate));
    automate_cellulaire->temps = 0;
    automate_cellulaire->configuration_actuelle = (cel**) malloc (sizeof(cel*) * dimension_max);
    for(unsigned int i = 0; i < dimension_max   ; i++){
        automate_cellulaire->configuration_actuelle[i] = (cel*) malloc (sizeof(cel) * dimension_max);
    }
    automate_cellulaire->nb_iterations_max = nb_iterations_max;
    automate_cellulaire->dimension_max = dimension_max;
    automate_cellulaire->nb_etats = nb_etats;

    return automate_cellulaire;
}

void supprimer_automate(automate automate_cellulaire){
    for(unsigned int i = 0; i < automate_cellulaire->dimension_max; i++){
        for(unsigned int j = 0; j < automate_cellulaire->dimension_max; j++){
            supprimer_cellule(&automate_cellulaire->configuration_actuelle[i][j]);
        }
        free(automate_cellulaire->configuration_actuelle[i]);
        automate_cellulaire->configuration_actuelle[i] = NULL;
    }
    free(automate_cellulaire->configuration_actuelle);
    automate_cellulaire->configuration_actuelle = NULL;
    free(automate_cellulaire);
    automate_cellulaire = NULL;
}
void afficher_automate(automate automate_cellulaire){
    printf("Règle : %u\n",automate_cellulaire->regle);
    printf("Règle binaire : %s\n",conversion_decimal_binaire((unsigned int)automate_cellulaire->regle));
    printf("Itérations : %u\n",automate_cellulaire->nb_iterations_max);
    printf("Dimensions Max. : %u\n\n", automate_cellulaire->dimension_max);

    for (unsigned int i = 0 ; i < automate_cellulaire->nb_iterations_max; i++){
        afficher_ligne(automate_cellulaire->configuration_actuelle[(int)i],automate_cellulaire->dimension_max);
        printf("\n");
    }
}

void afficher_ligne(cel* ligne,unsigned int dimensions_max){
    for(unsigned int i = 0; i < dimensions_max; i++){
        cel x = ligne[i];
        afficher_cellule(x);
    }
}

void set_regle(automate automate_cellulaire, int regle){
    automate_cellulaire->regle = regle;
}

void set_nb_iterations_max(automate automate_cellulaire, int n){
    automate_cellulaire->nb_iterations_max = n;
}

void set_dimension_max(automate automate_cellulaire, int n){
    automate_cellulaire->dimension_max = n;
}

void set_configuration_initiale(automate automate_cellulaire, char* configuration_initiale){
    for(unsigned int i = 0; i < automate_cellulaire->dimension_max; i++){
        automate_cellulaire->configuration_actuelle[0][i] = creer_cellule();
        set_etat(automate_cellulaire->configuration_actuelle[0][i], configuration_initiale[i] == '0' ? 0 : 1);
    }
}

void set_voisins(automate automate_cellulaire){
    set_voisin_gauche(automate_cellulaire->configuration_actuelle[0][0], NULL);
    set_voisin_droite(automate_cellulaire->configuration_actuelle[0][automate_cellulaire->dimension_max - 1], NULL);
    for(unsigned int i = 1; i < automate_cellulaire->dimension_max - 1; i++){
        set_voisin_gauche(automate_cellulaire->configuration_actuelle[0][i], automate_cellulaire->configuration_actuelle[0][i - 1]);
    }
}

cel** generer_automate(automate automate_cellulaire, unsigned int dimension_max, unsigned int regle, char* configuration_initiale){
    set_dimension_max(automate_cellulaire, dimension_max);
    set_regle(automate_cellulaire, regle);
    set_configuration_initiale(automate_cellulaire, configuration_initiale);
    //set_voisins(automate_cellulaire);
    return automate_cellulaire->configuration_actuelle;
}

//configuration_actuelle[15]
// for (int i=0;i<dimension_max;i++){
//     cel a = genererSuivant(configuration_actuelle[i])
//     cel * configuration_suivante[i]=a;
// }