/*******************GLOBAL INCLUDES*******************/

#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "card.h"

/*******************MACRO DEFINITIONS*******************/

#define _GNU_SOURCE


/**********************GLOBAL FUNCTION PROTOTYPES***************************/

// Given Function
void pretty_print(POKEMON_T* pokemon, FILE* fp_ascii); 

INDEX_T* read_Indexes(FILE* indexesFile);

POKEMON_T* read_Pokemon(FILE* pokemonFile, FILE* abilityFile, INDEX_T* index);

ABILITY_T* read_Ability(FILE* abilityFile, int ability_offset);

/*******************************MAIN FUNCTION********************************/

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("usage: ./search (0 || 1)\n");
        return -1;
    }
    if(*argv[1] != '0' && *argv[1] != '1') {
        printf("usage: ./search (0 || 1)\n");
        return -2;
    }


 /****************************OPEN BINARY FILES - MAIN FUNCTION**************************************/ 

    // Open ascii.bin
    FILE *ascii_file = NULL;

    // If the second argument is 1
    if (*argv[1] == '1') {
        
        // Open the file for reading
        ascii_file = fopen("ascii.bin", "r"); 
        
        // Check if the file opened
        if (ascii_file == NULL) {
            perror("fopen");
            return -1;
        }
        
    }

    // Open Abilities Binary File
    FILE* abiltiiesFile = fopen("abilities.bin", "rb");

    // Check if file opened
    if (abiltiiesFile == NULL)
    {
        perror("fopen");
        return -1;
    }

    // Open Pokemon Binary File
    FILE* pokemonFile = fopen("pokemon.bin", "rb");

    // Check if file opened
    if (pokemonFile == NULL)
    {
        perror("fopen");
        return -2;
    }

    // Open Index Binary File
    FILE* indexesFile = fopen("indexes.bin", "rb");    

    // Check if file opened
    if (indexesFile == NULL)
    {
        perror("fopen");
        return -3;
    }

    /**************************Read Total Number of Pokemon and Allocate Memory******************************/

    // Total number of pokemon
    size_t total_pokemons = 0;

    // Read the total number of pokemon
    fread(&total_pokemons, sizeof(size_t), 1, indexesFile);

    // Initialize and allocate memory for indexes
    INDEX_T** indexes = malloc(sizeof(INDEX_T*) * total_pokemons);

    // Check if malloc was successful
    if (indexes == NULL)
    {  
        // Close the files
        fclose(indexesFile);
        fclose(pokemonFile);
        fclose(abiltiiesFile);
        perror("malloc");
        return -4;
    }

    // Read the indexes
    for (size_t i = 0; i < total_pokemons; i++)
    {
        indexes[i] = read_Indexes(indexesFile);
    }

    // Declare a variable to store the echo status
    unsigned echo = 0;

    // Check if the input is a terminal
    if (isatty(0) == 0) {
        echo = 1;
    }

    // Initializing the line variable for the get_line function
    char *line = NULL; 
    
    // Initializing the len variable for the length of the line in the csv file
    size_t len = 0;

    // While loop to read the input from the user
    while (getline(&line, &len, stdin) != -1)
    {
        // print double arrows
        printf(">> ");

        // Remove the newline character
        line[strcspn(line, "\n")] = 0;

        // Echo the input back if echo is enabled
        if (echo)
        {
            printf("%s\n", line);
        }

        // Declare a variable to store if the pokemon was found
        int found = 0;

        // Check for the quit character
        if (line[0] == '!')
        {
            // Set found to true
            found = 1;
        }

        if (found)
        {
            // Free the indexes
            for (size_t i = 0; i < total_pokemons; i++)
            {
                free(indexes[i]->name);
                free(indexes[i]);
            }
            free(indexes);
            
            // Free the line
            free(line);

            // Close the files
            fclose(indexesFile);
            fclose(pokemonFile);
            fclose(abiltiiesFile);
            if(ascii_file) {
                fclose(ascii_file);
            }

            return 0;
        }


        // Loop through the indexes
        for (size_t i = 0; i < total_pokemons; i++)
        {   

            // If the name matches
            if (strcasecmp(indexes[i]->name, line) == 0)
            {   

                // Read the pokemon
                POKEMON_T* pokemon = read_Pokemon(pokemonFile, abiltiiesFile, indexes[i]);

                // Print the pokemon in a pretty format
                pretty_print(pokemon, *argv[1] == '1' ? ascii_file : NULL);

                // Check if the pokemon has a third ability
                if (pokemon->ability_three != NULL)
                {
                    free(pokemon->ability_three->desc);
                    free(pokemon->ability_three->name);
                    free(pokemon->ability_three);
                }

                // Check if the pokemon has a second ability
                if (pokemon->ability_two != NULL)
                {
                    free(pokemon->ability_two->desc);
                    free(pokemon->ability_two->name);
                    free(pokemon->ability_two);
                }

                // Free pokemon ability one
                free(pokemon->ability_one->desc);
                free(pokemon->ability_one->name);
                free(pokemon->ability_one);

                // Free the pokemon
                free(pokemon->name);
                free(pokemon);
                
                // Set found to true
                found = 1;
            }
        }

        // If the pokemon was not found
        if (!found)
        {
            printf("%s not found!\n", line);
        }  
    }    

    // Free the indexes
    for (size_t i = 0; i < total_pokemons; i++)
    {
        free(indexes[i]->name);
        free(indexes[i]);
    }
    free(indexes);
    
    // Free the line
    free(line);

    // Close the files
    fclose(indexesFile);
    fclose(pokemonFile);
    fclose(abiltiiesFile);
    if(ascii_file) {
        fclose(ascii_file);
    }

    return 0;

}




INDEX_T* read_Indexes(FILE* indexesFile)
{
    INDEX_T* indexes = malloc(sizeof(INDEX_T));

    // Check if malloc was successful
    if (indexes == NULL)
    {
        perror("malloc");
        return NULL;
    }

    size_t index_count = 0;


    char* name = NULL;

    // Read the length of the name
    fread(&index_count, sizeof(size_t), 1, indexesFile);

    // Allocate string plus null terminator
    name = realloc(name, index_count + 1);
    
    // Read the name
    fread(name, 1, index_count, indexesFile);

    // Null terminate the string
    name[index_count] = '\0';

    // Set the name of the index
    indexes->name = strdup(name);

    // Read the offset
    fread(&indexes->offset, sizeof(off_t), 1, indexesFile);

    // Free the name
    free(name);

    return indexes;
}


POKEMON_T* read_Pokemon(FILE* pokemonFile, FILE* abilityFile, INDEX_T* index)
{
    POKEMON_T* pokemon = malloc(sizeof(POKEMON_T));

    // seek to the offset
    fseek(pokemonFile, index->offset, SEEK_SET);

    // Name from index
    pokemon->name = strdup(index->name);

    // Read the id
    fread(&pokemon->id, sizeof(unsigned int), 1, pokemonFile);

    // Read the type
    fread(&pokemon->type, sizeof(int32_t), 1, pokemonFile);

    // Read the subtype
    fread(&pokemon->subtype, sizeof(int32_t), 1, pokemonFile);

    // Declare a variable to store the ability offset
    int64_t ability_offset;

    // Read the ability offsets
    fread(&ability_offset, sizeof(ability_offset), 1, pokemonFile);

    // Read the ability offsets
    pokemon->ability_one = read_Ability(abilityFile, ability_offset);

    // Read the ability offsets
    fread(&ability_offset, sizeof(ability_offset), 1, pokemonFile);

    if (ability_offset == __INT64_MAX__)
    {
        pokemon->ability_two = NULL;
    }
    else{
        // Read the ability offsets
        pokemon->ability_two = read_Ability(abilityFile, ability_offset);
    }

    // Read the ability offsets
    fread(&ability_offset, sizeof(ability_offset), 1, pokemonFile);

    if (ability_offset == __INT64_MAX__)
    {
        pokemon->ability_three = NULL;
    }
    else
    {
    
        // Read the ability offsets
        pokemon->ability_three = read_Ability(abilityFile, ability_offset);
    }

    // Read the base experience
    fread(&pokemon->base_experience, sizeof(unsigned int), 1, pokemonFile);

    // Read the weight
    fread(&pokemon->weight, sizeof(float), 1, pokemonFile);

    // Read the height
    fread(&pokemon->height, sizeof(float), 1, pokemonFile);

    // Read the hp
    fread(&pokemon->hp, sizeof(unsigned int), 1, pokemonFile);

    // Read the attack
    fread(&pokemon->attack, sizeof(unsigned int), 1, pokemonFile);

    // Read the defense
    fread(&pokemon->defense, sizeof(unsigned int), 1, pokemonFile);

    // Read the special attack
    fread(&pokemon->special_attack, sizeof(unsigned int), 1, pokemonFile);

    // Read the special defense
    fread(&pokemon->special_defense, sizeof(unsigned int), 1, pokemonFile);

    // Read the speed
    fread(&pokemon->speed, sizeof(unsigned int), 1, pokemonFile);

    // Read the offset
    fread(&pokemon->offset, sizeof(off_t), 1, pokemonFile);

    return pokemon;
}

ABILITY_T* read_Ability(FILE* abilityFile, int ability_offset)
{
    ABILITY_T* ability = malloc(sizeof(ABILITY_T));

    // memory leak fix
    if(ability == NULL) {
        perror("malloc");
        return NULL;
    }

    ability->name = NULL; // Initialize to NULL before using realloc.
    ability->desc = NULL; // Initialize to NULL before using realloc.

    // seek to the offset
    fseek(abilityFile, ability_offset, SEEK_SET);

    // Read the length of the name
    size_t name_len = 0;
    fread(&name_len, sizeof(size_t), 1, abilityFile);

    // Allocate string plus null terminator
    ability->name = malloc(name_len + 1);

    // memory leak fix
    if(ability->name == NULL) {
        perror("malloc");
        free(ability);
        return NULL;
    }

    // Read the name
    fread(ability->name, 1, name_len, abilityFile);
    ability->name[name_len] = '\0'; // Null-terminate the string

    // Read the length of the description
    size_t desc_len = 0;
    fread(&desc_len, sizeof(size_t), 1, abilityFile);

    // Allocate string plus null terminator
    ability->desc = malloc(desc_len + 1);

    // memory leak fix
    if(ability->desc == NULL) {
        perror("malloc");
        free(ability->name);
        free(ability);
        return NULL;
    }

    // Read the description
    fread(ability->desc, 1, desc_len, abilityFile);
    ability->desc[desc_len] = '\0'; // Null-terminate the string

    return ability;
}

void pretty_print(POKEMON_T* pokemon, FILE* fp_ascii) {
    // PICTURE ////////////
    // if fp_ascii is null, that means its not supposed to display the art
    if(fp_ascii) {
        size_t data_len = 0;
        fseek(fp_ascii, pokemon->offset, SEEK_SET);
        fread(&data_len, sizeof(data_len), 1, fp_ascii);
        char *data_buf = malloc(data_len + 1);
        fread(data_buf, 1, data_len, fp_ascii);
        data_buf[data_len] = 0;
        printf("%s\n", data_buf);
        free(data_buf);
    }

    // ID, NAME & TYPE ////////////
    // print name & pokedex #
    printf("#%.03d ", pokemon->id);
    printf("%s\n", pokemon->name);
    // print type (and subtype if applicable)
    printf("\033[1m%s\033[0m", type_str[pokemon->type]);
    if (pokemon->subtype != NONE) {
        printf(" / \033[1m%s\033[0m\n", type_str[pokemon->subtype]);
    } else {
        printf("\n");
    }

    // ABILITIES ////////////
    // print abilities in a nice list
    printf("\n\033[1mAbilities:\033[0m");
    printf("\n1. %s\n", pokemon->ability_one->name);
    printf("   %s\n", pokemon->ability_one->desc);

    if(pokemon->ability_two){
        // sometimes the second ability is hidden
        if(pokemon->ability_three == NULL) {
            printf("\033[3m");
        }
        printf("2. %s", pokemon->ability_two->name);
        if(pokemon->ability_three == NULL) {
            printf(" (hidden)");
        }
        printf("\n   %s", pokemon->ability_two->desc);

        printf("\033[0m\n");
    }
    // third ability if it exists is guaranteed to be hidden. make italic
    if(pokemon->ability_three) {
        printf("\033[3m");
        printf("3. %s (hidden)\n", pokemon->ability_three->name);
        printf("   %s", pokemon->ability_three->desc);
        printf("\033[0m\n");
    }

    // remaining stat block
    printf("\nHP: %d\t\t| %.2fkg\n", pokemon->hp, pokemon->weight);
    if (pokemon->attack > 99) {
        printf("Atk: %d\t| %.2fm\n", pokemon->attack, pokemon->height);
    } else {
        printf("Atk: %d\t\t| %.2fm\n", pokemon->attack, pokemon->height);
    }
    if (pokemon->defense > 99) {
        printf("Def: %d\t|\n", pokemon->defense);
    } else {
        printf("Def: %d\t\t|\n", pokemon->defense);
    }
    printf("Sp. Atk: %d\t|\n", pokemon->special_attack);
    printf("Sp. Def: %d\t|\n", pokemon->special_defense);
    printf("Speed: %d\t| XP: %d\n\n", pokemon->speed, pokemon->base_experience);
}
