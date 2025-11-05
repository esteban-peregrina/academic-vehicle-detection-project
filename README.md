# Fonctionnement
## Detection des contours
La detection des contours est réalisée par une succession de traitements assez simple, réalisés image par image. On commence par lisser l'image avec un flou Gaussien, puis on la convertit en HSV pour pouvoir extraire la saturation, qui va nous servir à réaliser un seuillage pour détecter la route (qui est nettement plus sombre que le reste de l'image). Quelques opérations de morphologie (fermeture puis ouverture) nous permettent ensuite d'améliorer l'image en supprimant les petits éléments parasites (on choisit un petit élément structurant rectangulaire). Enfin, pour ne garder que les contours des deux voies, on compare la surface de l'ensemble des contour detectés, et on garde les deux dont la surface est la plus grande.

## Detection des véhicules
Afin de detecter et suivre les vehicules, on utilise la différence absolue entre l'image précédente et l'image courante, pour détecter le mouvement.

## Comptage des véhicules
Enfin, le comptage des véhicules est réalisé en vérifiant la présence du centre géométrique d'un rectangle encapsulant les véhicules à l'intérieur d'une courte bande centrée sur un horizon arbitraire. L'horizon à néanmoins été choisis pour être positionné là ou les rectangles sont les moins déformés entre quelques instants lors du passage d'un véhicule. En effet, lorsque les véhicules sont long, le centre géométrique peut "revenir en arrière" et être compté plusieurs fois. À l'inverse, si on prend un horizon trop haut, on se rapproche du point de fuite, et on risque de manquer des véhicules trop proches (notamment les motocyclettes)

Dans analyse.cpp, définir TEST à 0 permet d'afficher chaque étape de traitement (au prix des performances).