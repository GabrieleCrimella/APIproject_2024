# Pasticceria Management System

## Overview
This project simulates the operations of an industrial pastry shop to improve its order management system. The simulation occurs in discrete time steps, starting at time 0, with each command execution representing an instant of time.

## Elements of the Simulation

- **Ingredients**: Each ingredient is identified by a name.
- **Recipes**: Each recipe has a unique name and specifies the required quantities of ingredients in grams.
- **Inventory**: Stores ingredients with specified quantities and expiration dates.
- **Orders**: Customers place orders for pastries, which are prepared immediately if ingredients are available. If not, orders are put on hold.
- **Courier**: Periodically collects ready orders from the pastry shop, prioritizing by order arrival time and load capacity.

## Input File Format

The input file starts with a line containing two integers: the courier's periodicity and capacity. Subsequent lines contain commands with the following formats:

- `aggiungi_ricetta <recipe_name> <ingredient_name> <quantity> ...`
  - Adds a new recipe to the catalog.
  - Example: `aggiungi_ricetta meringhe_della_prozia zucchero 100 albumi 100`
  - Output: `aggiunta` or `ignorato`

- `rimuovi_ricetta <recipe_name>`
  - Removes a recipe from the catalog.
  - Example: `rimuovi_ricetta cannoncini`
  - Output: `rimossa`, `ordini in sospeso`, or `non presente`

- `rifornimento <ingredient_name> <quantity> <expiration> ...`
  - Restocks ingredients with specified quantities and expiration dates.
  - Example: `rifornimento zucchero 200 150 farina 1000 220`
  - Output: `rifornito`

- `ordine <recipe_name> <quantity>`
  - Places an order for a specified number of pastries.
  - Example: `ordine torta_paradiso 36`
  - Output: `accettato` or `rifiutato`

## Output
The program outputs the state of orders in the courier's truck at specific intervals and other relevant messages based on command execution. For example:
- Orders in the truck are printed as sequences of triples `(arrival_time, recipe_name, quantity)`.
- If the truck is empty, it prints `camioncino vuoto`.

## Requirements
- All quantities are integers greater than zero.
- Names are alphanumeric with up to 255 characters.
- Positive integer values are 32-bit encodable.

## Notes
- The preparation of orders happens instantaneously.
- Orders are loaded into the courier truck by weight, prioritizing older orders if weights are equal.
- Simulation ends after the last command is executed.

## Author
- Gabriele Crimella, developed as part of the Final Exam for Algorithms and Data Structures 2023-2024.

