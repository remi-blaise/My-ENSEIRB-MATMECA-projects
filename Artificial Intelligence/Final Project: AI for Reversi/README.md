# Intelligence Artficielle - Reversi - 2018/2019

Par Rémi Blaise & Christian Bonnaud


**IMPORTANT: Tous nos scripts utilisent python3**


## Notre travail

Vous trouverez dans ce dépôt notre implémentation d'une intelligence artificielle jouant au jeu du Reversi.
Nous avons tout d'abord implémenté une fonction alphabeta permettant un parcours de l'arbre des possibilités, tout en effectuant des "coupes", afin de réduire le champs d'exploration sans perte.

Une fois ceci fait, nous avons réfléchi aux différentes fonctions heuristiques possibles.
Nous en avons finalement retenu plusieurs :
- l'heuristique de `basic` qui se contente de retourner la différence entre les pions amis et ennemis; ainsi que des variantes tentant de prendre en compte l'avancement de la partie pour pondérer le résultat de l'heuristique.
- l'heuristique `victory` qui attribue une valeur '+infinie' en cas de victoire, '-infinie' en cas de défaite, et 0 sinon.
- l'heuristique `forcing` qui essaie de minimiser le nombre de coups jouables par l'aversaire, tout en maximisant nos propres possibilités.
- l'heuristique de position `pos` qui donne des valeurs positives ou négatives en cas de possession ou non de certaines cases du Board.
- l'heuristique `impregnable` qui compte le nombre de pions posés qui ne peuvent plus être repris par l'adversaire.

Nous avons considéré qu'il était efficace de découper la partie en 3 phases de 96/3 = 32 coups chacunes. En effet, certaines heuristiques donnent de très bons résultats sur le premier tier alors que d'autres sont bien meilleures sur le troisième.

Nous avons effectué de nombreux tests (4760 parties en tout, merci les 16 VM AWS <3) dans le but d'évaluer l'apport / l'efficacité de chaque heuristique et de tester des combinations. Nous avons ainsi essayé de nombreuses combinaisons linéaires des heuristiques dans le but de trouver les meilleures pondérations et les meilleurs compromis entre temps d'évaluation de plusieurs heuristiques et apports de chacune au taux de victoire.
Vous pouvez retrouver tous nos résultats à l'adresse suivante :
https://docs.google.com/spreadsheets/d/1t3yHuX9MaxwL15x_97doOIcFFXhO6ja_9A08coZgDsw/edit?usp=sharing

À cela s'ajoute une bibliothèque d'ouverture que nous avons généré, en simulant avec l'algorithme minimax tous les coups possibles, et ajoutant uniquement ceux qui obtiennent le meilleur score pour un coup joué.

Nous avons sélectionné le meilleur de tous ces tests pour concocter notre heuristique finale. En voici la recette :
1. Consultation de la bibiothèque d'ouverture pour jouer les premiers coups.
2. Utilisation de l'heuristique `basic` jusqu'au 32ème coup. De manière assez singulière, c'est cette heuristique considérée comme 'basique' qui nous donne les meilleurs résultats pour cette phase.
3. Utilisation de l'heuristique de `forcing` du 32ème coup au 64ème.
4. Utilisation de l'heuristique nommée `bfi_315_30`, combinaison linéaire des heuristiques `basic`, `forcing` et `impregnable`.
5. Exploration de l'entièreté de l'arbre restant sur les 10 derniers coups de la partie grâce à l'heuristique `victory`. L'utilisation unique de cette heuristique extrèmement rapide à calculer nous permet de nous assurer de bien atteindre la fin de la partie lors de l'approfondissement itératif.

De manière générale, les résultats obtenus ne sont cependant pas aussi bons que ce que nous aurions souhaité, la combinaison de chaque nouvelle heuristique ayant un prix fort en terme de performance et donc de profondeur d'exploration. De très nombreuses améliorations auraient encore pu être apportée, mais le temps est compté !
Nous sommes tout de même fier de notre travail, en espérant que cela porte ses fruits !


## Organisation du dépôt

Vous trouverez l'ensemble de nos sources dans le dossier `src`:
    - `CR_ai` contient notre code source.
    - `CR_generate_opening_library` est le script permettant de générer notre bibliothèque d'ouverture.
    - `CR_Player` est une implémentation de `playerInterface` (le fichier `myPlayer` est adapté en conséquence).
    - `fight.py` et `fight` permettant de lancer (respectivement 1 et 40 parties) de deux de nos IA.
Le fichier restant sont ceux fournis (inchangés).

De manière anecdotique, la commande `make` compile notre librairie avec Cythonize (`setup.py`), de manière à pouvoir la distribuer à d'autres groupes (pas bête hé).
