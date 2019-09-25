#include <stdio.h>
#include <string.h>

#include "../include/regle.h"
#include "../include/utils.h"

struct regle {
    char* _regle; // la regle a appliquer  (00011110) 2 = 30 || 0013100132 pour la somme par ex
    int (*_type_regle) (char*, unsigned int, unsigned int, unsigned int);
    void (*_affichage_regle) (int);
    unsigned int nb_etats;
};

regle creer_regle(){
    regle r = (regle) malloc (sizeof(struct regle));
    r->_regle = NULL;
    r->_type_regle = NULL;
    r->_affichage_regle = NULL;
    r->nb_etats=0;

    return r;
}

void supprimer_regle(regle* r){
    regle regle_a_supprimer = *r;
    free(regle_a_supprimer->_regle);
    regle_a_supprimer->_regle = NULL;
    regle_a_supprimer->_type_regle = NULL;
    regle_a_supprimer->_affichage_regle = NULL;
    regle_a_supprimer->nb_etats=NULL;
    free(regle_a_supprimer);

    regle_a_supprimer = NULL;
}

void set_regle(regle r, char* s){
    size_t taille_s = strlen(s);
    r->_regle = (char*) malloc (sizeof(char) * taille_s + 1);
    strcpy(r->_regle, s);
    r->_regle[taille_s] = '\0';
}

void set_type_regle(regle r, int (*type_regle) (char*, unsigned int, unsigned int, unsigned int)){
    r->_type_regle = type_regle;
}

void set_affichage_regle(regle r, void (*affichage_regle) (int)){
    r->_affichage_regle = affichage_regle;
}

void set_nb_etats(regle r,unsigned int nb_etats){
    r->nb_etats=nb_etats;
}

char* get_regle(regle r){
    return r->_regle;
}

int (*get_type_regle(regle r))(char*, unsigned int, unsigned int, unsigned int){
    return r->_type_regle;
}

void (*get_affichage_regle(regle r))(int){
    return r->_affichage_regle;
}

int get_nb_etats(regle r){
    return r->nb_etats;
}

int regle_binaire(char* regle, unsigned int e_cg, unsigned int e_cm, unsigned int e_cd){
    int x = conversion_binaire_decimal(e_cg, e_cm, e_cd);
    return regle[strlen(regle) - 1 - x] - 48;
}

int regle_somme(char* regle, unsigned int e_cg, unsigned int e_cm, unsigned int e_cd){
    int x = e_cg + e_cm + e_cd;
    return regle[x] - 48;
}

int regle_ionas(char* regle, unsigned int e_cg, unsigned int e_cm, unsigned int e_cd){
    int x = e_cg +e_cd+e_cm;
    if (x<0)
        x=0;
    return regle[x] - 48;
}