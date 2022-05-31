## Plan de construction ##
Dans ce répertoie, se trouve tout ce qui concerne la construction physique.


## Epaisseurs des matériaux ##
* PMMA :
  * Dessus de la boite : 3 mm
* CTP peuplier :
  * Coté de la boite : 5 mm

## Paramétres des découpes laser ##
Ces paramètres sont valable pour la laser avec des miroirs propres.

| Matériau | Opération | Power | Speed | Remarque |
|----------|-----------|------|-------|----------|
| PMMA 3 mm | Cut | 20 | 75 | |
| CTP Peuplier 5 mm | Cut | 100 | 45 | |
| CTP Peuplier 5 mm | Mark | 8 | 20 | Défocalisation à +2 mm |

## Agencement des découpes ##
Le logiciel [Deepnest](https://deepnest.io/) permet d'agencer avec un minimum de perte les différentes pièces à découper.

Dans le logiciel Deepnest :
- Importer le fichier svg.
- Cliquer sur le [+] en bas de page pour ajouter une "planche"
- Donner les dimensions en millimètres de la planche (par exemple 900 x 450), puis cliqueer sur [Add]
- Cocher la pièce nouvellement créé (elle doit être tout en bas de la liste) pour qu'elle soit défini comme "Sheet".
- Lancer la génération de l'agencement [Start Nest]
- ... et attendre ...
- Cliquer sur un des éléments dans la liste *best nests so far* des "trucs" colorés pour choisir le meilleur agencement.
(Attention à bien vérifier que toutes les pièces sont bien présentes dans l'agencement proposé -> cf *parts placed*)
- Exporter le plan de découpe au format SVG.

NB : Le logiciel Deepnest ne s'arrète jamais de calculer un meilleur agencement, il faut cliquer sur le bouton **"Stop nest"**
