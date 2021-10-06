# Equatorial mount

## What is a Equatorial mount ?

Une monture équatorial permet de compenser la rotation de la Terre pour prendre des photos du ciel et éviter les filets d'étoiles.

## Deux versions: 

### Avec le bluetooth: 
Utilise le module HC-06, permettant d'utiliser l'application mobile pour avoir plus de fonctionnalités :
- Différentes vitesses ( sidéral, sidéralx2, lunaire, solaire, stop )

- Methode D.A.R.V qui permet une mise en station très précise en utilisant la dérive des étoiles.
D.A.R.V 30sec ( vitesse sidéral pendant 5 secondes pour avoir le point de départ, 15 secondes en arrêt, 15 secondes en vitesse sidéralx2 )
D.A.R.V 120sec ( vitesse sidéral pendant 5 secondes pour avoir le point de départ, 60 secondes en arrêt, 60 secondes en vitesse sidéralx2 )

- Prochaine version, ajouter prise jack 3.5mm pour controler l'appareil photo ( intervallomètre )

###Sans le bluetooth:
La vitesse doit être écrit dans le programme et ensuite le téléverser dans la carte arduino.

#Recommandation: 

Je vous conseille de calibrer l'horloge de votre Arduino pour gagner en précision en suivant ce tutoriel : [Making Arduino timing more precise](https://lygte-info.dk/project/PrecisionTime%20UK.html)


