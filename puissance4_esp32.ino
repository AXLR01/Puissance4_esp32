/*-------------------------------------------------------------------------------
# Name:        Puissance 4
# Purpose:     Realiser un puissance 4 avec une bande de led et un esp32
#
# Author:      AXLR
#
# Created:     25/02/2019
# Copyright:   (c) AXLR 2019
# Licence:     <your licence>
#-------------------------------------------------------------------------------*/
/*************************************************************************************************/
/********************************* Debut des librairies et defines********************************

/* librairies */
#include <Adafruit_DotStar.h>
#include <SPI.h>       
/*Defines */
#define NUMPIXELS 42   // Nbr de LEDS
#define DATAPIN    13  // Pin Spi
#define CLOCKPIN   14  // Pin de clock
#define RESTART     33 // Pin du bouton pour restart
#define DROITE      25 // Pin du bouton pour aller a droite
#define GAUCHE      26 // Pin du bouton pour aller a gauche
#define JOUE        27 // Pin du bouton pour aller joue
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR); // objet bande LED
/********************************* Fin des librairies et defines *********************************
//***********************************************************************************************/

/********************************* Debut des variables ******************************************
//***********************************************************************************************/
/* Damier */
enum caseType{empty, player1, player2}; // creation de type perso qui prend seulenment 3 valeurs
caseType memoire[43]; // creation d'une memoire des coups
int damierPos[] = { // damier des positions des leds
  41, 40, 39, 38, 37, 36,
  30, 31, 32, 33, 34, 35,
  29, 28, 27, 26, 25, 24,
  18, 19, 20, 21, 22, 23,
  17, 16, 15, 14, 13, 12,
   6,  7,  8,  9, 10, 11,
   5,  4,  3,  2,  1,  0
  };
int videColonnes[] = {5, 5, 5, 5, 5, 5, 5}; // gestion des places dans chaque colonne
int choixColonne = 3; // position initiale de l'indicateur

/* Couleurs */
uint32_t yellow  = 0xFFF000;  // couleur jaune en hexa
uint32_t orange  = 0xCF5300;  // couleur orange en hexa
uint32_t cyan  = 0x00FFFF;    // couleur cyan en hexa
uint32_t purple  = 0xFF33FC;  // couleur violet en hexa
uint32_t red  = 0xFF0000;     // Couleur rouge en hexa
uint32_t blue = 0x0000FF;     // Couleur bleu en hexa
uint32_t green = 0x00FF00;    // Couleur vert en hexa
uint32_t black = 0x000000;    // Couleur noire/eteient en hexa
uint32_t color; // stockage de la couleur du joueur en cour

/* Joueur */
bool joueur1=true; // gestion etat joueur 1
bool joueur2=false; // gestion etat joueur 2
caseType joueur; // joueur en cour

/* Animation victoire */
int fin[4]; // retient les jetons aligne

/********************************* Fin des variables *********************************************
**************************************************************************************************/

/************************************************************************************************/
/********************************* Debut des FCT ************************************************/

/************************************************************************************************/
/********************************* Debut FCT Pause **********************************************/
void pause (int tmp) // permet de faire une pause dans le programme
{ 
  long Temps; // comparateur de temps ecoulé 
  Temps = millis() ;
  while((millis() - Temps )  < tmp ) {} // ne fais rien tant que temps est unferieur a millis()
}
/********************************* Fin FCT Pause **************************************************
**************************************************************************************************/

/************************************************************************************************/
/********************************* Debut FCT Reset **********************************************/
void reset () // reinitialise le jeu pour une nouvel partie
{ 
  int i = 0; // compteur
  for ( i=0; i<=41; i++ )  // reset la memoire
  {
    memoire[i]=empty; 
  }
  for ( i=0; i<=6; i++ )  // allume la colonne 1 en rouge
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, red); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=6; i<=12; i++ )  // allume la colonne 2 orange
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, orange); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=12; i<=18; i++ )  // allume la colonne 3 en jaune
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, yellow); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=18; i<=24; i++ )  // allume la colonne 4 en vert
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, green); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=24; i<=30; i++ )  // allume la colonne 5 en cyan
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, cyan); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=30; i<=36; i++ )  // allume la colonne 6 en blue
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, blue); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  for ( i=36; i<=42; i++ )  // allume la colonne 7 en violet
  {
    memoire[i]=empty; 
    strip.setPixelColor(i, purple); // allumer les leds jusqu'à la led i
    strip.setBrightness(40);
    strip.show();
  }
  // eteint tout
  pause(300);
  for ( i=0; i<=42; i++ ) // eteint les leds apres 300ms
  {
    strip.setPixelColor(i, black); // etient le damier
    strip.show();
  }
  // RAZ de l'espace dans les colones et du joueur initiale et tour joueur
  for(int i=0; i< 7; i++)
     videColonnes[i] = 5;
  choixColonne = 3;
  joueur1=true;
  joueur2=false;
  for ( int j = 0; j<4; j++)
  {fin[j]=0;}
}
/********************************* Fin FCT Reset **************************************************
**************************************************************************************************/

void next() // decale vers la droite quand colone remplis
{
  if ( (videColonnes[choixColonne]==-1))
    {
       if (videColonnes[choixColonne]== -1)
      {
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne + 1;
        }
      }
      if(choixColonne > 6)
      {
        choixColonne = 0;
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne + 1;
        }
      }
    }
}

/************************************************************************************************/
/********************************* Debut FCT Bouton *********************************************/
bool button(int pin) // lis l'etat d'un bouton en fct de son pin en pull down
{
  bool tmp; // renvoie si le bouton a ete presse
  int etatButton; // état du bouton a l instant 
  int lastButton = HIGH; // memoire de l etat du bouton
  etatButton = digitalRead(pin); // lis l etat du bouton et l affecte a etatbouton
  if(((etatButton != lastButton)) && (etatButton == LOW)) // condition du pull down 
  {
    tmp = true; // le bouton a ete presse
    lastButton = etatButton; // mise a jour du dernier etat de bouton
    next();
  }
  else{tmp=false;} // le bouton n a pas ete presse
  return tmp;
}
/********************************* Fin FCT Bouton *************************************************
**************************************************************************************************/

/************************************************************************************************/
/************************ Debut FCT Animation et momoire ****************************************/
void AnimationEtMemoire() 
  {
    int i = 0; // compteur
    int j = 0; // compteut
    // change couleur joueur
    if (joueur1==true) 
    {color=red;}
    else if (joueur2==true)
    {color=yellow;}
    // fin change coueleur joueur
    
    // aniamtion chute de jeton
    for (j = 0; j <= videColonnes[choixColonne]; j++) 
      {
        strip.setPixelColor(physicalIndexLed(choixColonne * 6 + (5-j)), color) ;
        strip.setBrightness(40) ;
        strip.show() ;
        pause(100);
        strip.setPixelColor(physicalIndexLed(choixColonne * 6 + (5-j)), black) ; 
        strip.setBrightness(40) ;
        strip.show() ;
      } 
      // fin animation chute de jeton
      
      // mise a jour de la memoire
    if (joueur1==true)  
      {memoire[choixColonne * 6 + (5-videColonnes[choixColonne])] = player1;} 
    else if (joueur2==true)
      {memoire[choixColonne * 6 + (5-videColonnes[choixColonne])] = player2; } 
    videColonnes[choixColonne] = videColonnes[choixColonne] - 1; 
      // fin mise a jour de la memoire
    
    // affiche les led des joueur 
    for ( i=0; i<42; i++ )  
    {
      if(memoire[i] == player1)  // affiche les led de joueur 1 
      {
        strip.setPixelColor(physicalIndexLed(i), red); 
        strip.setBrightness(40);
        strip.show();
      }
      if(memoire[i] == player2)  // affiche les led de joueur 2
      {
        strip.setPixelColor(physicalIndexLed(i), yellow); 
        strip.setBrightness(40);
        strip.show();
      }
    }
     // fin affiche les led des joueur 
    }
/*********************** Fin FCT Animation et memoire *********************************************
**************************************************************************************************/

/************************************************************************************************/
/************************************ Debut FCT jeton ********************************************/
bool jeton() // change le tour des joueurs
{
   if ((joueur1==true)&&(joueur2==false))
   {
    joueur1 = false;
    joueur2 = true;
    joueur = player2;
   }
   else
   {
    joueur1 = true;
    joueur2 = false;
    joueur = player1;
   }
}
/************************************ Fin FCT jeton ***********************************************
**************************************************************************************************/

int physicalIndexLed(int ledNumber) { // retourne la led en cour
  return damierPos[ledNumber];
}

/************************************************************************************************/
/***************************** Debut FCT deplacement ********************************************/
void dpl_blink() // deplace l'indicateur de jeu de colonne en colonne 
  {
    while(button(JOUE)!=true)
  {
    next();
    // blink led en fct tour joueur
    strip.setPixelColor(physicalIndexLed(choixColonne * 6 + 5), blue) ; 
    strip.setBrightness(40);
    strip.show() ;
    pause(100) ;
    if(joueur1) 
      {
        strip.setPixelColor(physicalIndexLed(choixColonne * 6 + 5), red) ;
        strip.setBrightness(40); 
      }
    else
    {
      strip.setPixelColor(physicalIndexLed(choixColonne * 6 + 5), yellow) ;
      strip.setBrightness(40);
    }
    strip.show() ;
    pause(100) ;
    strip.setPixelColor(physicalIndexLed(choixColonne * 6 + 5), 0); 
    strip.show();
    // fin blink en fct joueur
    
    if(button(GAUCHE)==true) // mouvement a gauche                     
    {                                                                                                              
      choixColonne--;
      if (videColonnes[choixColonne]== -1)
      {
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne - 1;
        }
      }
      if(choixColonne < 0)
      {
        choixColonne = 6;
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne - 1;
        }
      }
                                                                                                   
    }     
    else if (button(DROITE)==true)  // mouvement droite                  
    {                                                                                                                       
      choixColonne++;
      if (videColonnes[choixColonne]== -1)
      {
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne + 1;
        }
      }
      if(choixColonne > 6)
      {
        choixColonne = 0;
        while ( videColonnes[choixColonne] == -1)
        {
          choixColonne = choixColonne + 1;
        }
      }
                                                                                                    
    }                                                 
    else if (button (RESTART)== true) // gestion du la fct restart
      {reset();}  
  }
  }
/****************************** Fin FCT deplacement ***********************************************
**************************************************************************************************/

/****************************** Debut condition WIN ***********************************************
**************************************************************************************************/

/* Condition de victoire verticale */
bool verticale() // check les condition de victoire verticalement
{
  int win = 0;
  int k =0;
  for (int i = 0; i<41; i++) // parcour tout le damier
  {
    if ( (memoire[i] == memoire[i+1])&&(memoire[i] == joueur )&&(i%6 != 5) ) // regarde si memoire de i et i+1 sont identique et appartienne au mm joueur
    {
      win = win + 1; // nombre de pion alligné
      fin[k]=i;
      k=k+1;
      if (win == 3)
      {
        fin[k] = i +1;
        return true; // 4 pions aligné verticalement
      }
    }
    else // si les pions ne sont pas cote a cote remise a zero
    {
      win = 0;
      k=0;
    }
  } 
  return false; // si rien dans le for retourne faux pas securité 
}
/* Condition de victoire horizontale */
bool horizontale() // check les condition de victoire horizontalment
{
  int win = 0;
  int k =0;
  for (int i = 0; i<7; i++) // parcour le damier en colonne, seulment la première
  {
    for ( int j = i; j<42; j=j+6) // parcour le damier sur la ligne, toute la ligne
    {
      if ( (memoire[j] == memoire[j+6])&&(memoire[j] == joueur )) // regarde si memoire de j et j+6 sont identique et appartienne au mm joueur
      {
        win = win + 1; // nombre de pion alligné
        fin[k]=j;
        k=k+1;
        if (win == 3)
        {
          fin[k] = j + 6;
          return true; // 4 pions aligné verticalement
        }
      }
      else // si les pions ne sont pas cote a cote remise a zero
      {
        win = 0;
        k=0;
      }
    }   
  }
  return false;
}
/* Condition de victoire diagonale droite */
bool diagonaleD() // check les condition de victoire sur la diagonale droie
{
  int win = 0;
  int k =0;
  for ( int i = 0; i<21; i++) // parcoure tte la bande
  {
    for ( int j=i; j<42; j=j+7)
    {      
      if ( (memoire[j] == memoire[j+7])&&(memoire[j] == joueur )&&(j%6 != 5)) // regarde si memoire de j et j+7 sont identique et appartienne au mm joueur
      {
        win = win + 1; // nombre de pion alligné
        fin[k]=j;
        k=k+1;
        if (win == 3)
        {
          fin[k] = j + 7;
          return true; // 4 pions aligné verticalement
        }
      }
      else // si les pions ne sont pas cote a cote remise a zero
      {
        win = 0;
        k=0;
      }
    }
  }
  return false;
}
/* Condition de victoire diagonale gauche */
bool diagonaleG() // check les condition de victoire sur la diagonale gauche
{
  int win = 0;
  int k =0;
  for ( int i = 3; i<24; i++) // parcoure tte la bande
  {
    for ( int j=i; j<39; j=j+5)
    {      
      if ( (memoire[j] == memoire[j+5])&&(memoire[j] == joueur )&&(j%6 != 0)) // regarde si memoire de j et j+6 sont identique et appartienne au mm joueur
      {
        win = win + 1; // nombre de pion alligné
        fin[k]=j;
        k=k+1;
        if (win == 3)
        {
          fin[k] = j + 5;
          return true; // 4 pions aligné verticalement
        }
      }
      else // si les pions ne sont pas cote a cote remise a zero
      {
        win = 0;
        k = 0 ;
      }
    }
  } 
  return false;
}
/* Check tt en mm temps */
bool victory () // appel tte les fcts pour check si victoire
{
  if ((verticale()==true) || ( (horizontale()==true) ) || (diagonaleD()==true) || (diagonaleG()==true) )
  {
    return true;
  }  
  else
  {
    return false;
  }
}
/****************************** Fin condition WIN ************************************************
**************************************************************************************************/

/****************************** Fin FCT  **********************************************************
**************************************************************************************************/

/************************************************************************************************/
/************************************** Debut Setup ********************************************/
void setup() 
{
  Serial.begin(9600);
  pinMode(RESTART,INPUT_PULLUP); // initialisation bouton
  pinMode(DROITE,INPUT_PULLUP); // initialisation bouton
  pinMode(GAUCHE,INPUT_PULLUP); // initialisation bouton
  pinMode(JOUE,INPUT_PULLUP); // initialisation bouton
  #if defined(_AVR_ATtiny85_) && (F_CPU == 16000000L) // initialisation bande led
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif
  strip.begin();
  strip.show();  
}
/****************************** Fin Setup **********************************************************
**************************************************************************************************/

/************************************************************************************************/
/************************************** Debut LOOP *********************************************/
void loop() 
{
  dpl_blink();  
  if (button(JOUE) == HIGH)
  {
    AnimationEtMemoire();  
    if ( victory() == 1)
    {
      while(button(RESTART)!=true) // affiche les leds de victoire tan que reset n'est pas vrai
      {
        strip.setPixelColor(physicalIndexLed(fin[0]), color) ;
        strip.setPixelColor(physicalIndexLed(fin[1]), color) ;
        strip.setPixelColor(physicalIndexLed(fin[2]), color) ;
        strip.setPixelColor(physicalIndexLed(fin[3]), color) ;
        strip.show() ;
        pause(100);
        strip.setPixelColor(physicalIndexLed(fin[0]), black) ;
        strip.setPixelColor(physicalIndexLed(fin[1]), black) ;
        strip.setPixelColor(physicalIndexLed(fin[2]), black) ;
        strip.setPixelColor(physicalIndexLed(fin[3]), black) ; 
        strip.show() ;
        pause(100);
      }
      reset();
    }
    jeton();
  }
}
/****************************** Fin LOOP **********************************************************
**************************************************************************************************/
