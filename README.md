# reservation-
système de gestion de réservation 
# Reservation System Project

Ce projet est une application console en langage C permettant de gérer un système de réservation simple.

## 🛠️ Fonctionnalités

- Ajouter une réservation
- Afficher les réservations existantes
- Sauvegarder les données dans un fichier texte (`reservations.txt`)
- Gérer les réservations avec vérification des conflits
- Calculer automatiquement le coût des réservations
- Générer des factures simples
- Fournir des statistiques d'utilisation
  

## 📁 Structure du projet

| Fichier / Dossier     | Description                                      |
|-----------------------|--------------------------------------------------|
| `reser.c`             | Code source principal du programme               |
| `reservations.txt`    | Fichier de sauvegarde des réservations           |
| `.vscode/`            | Configuration de l’environnement VS Code         |
| `reser.exe`           | Fichier compilé (à ne pas pousser sur GitHub)    |

## ▶️ Compilation

Pour compiler le programme :

```bash
gcc reser.c -o reser.exe
