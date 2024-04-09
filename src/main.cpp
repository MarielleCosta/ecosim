#define CROW_MAIN
#define CROW_STATIC_DIR "../public"

#include "crow_all.h"
#include "json.hpp"
#include <random>
#include <mutex>
#include <thread>

static const uint32_t NUM_ROWS = 15;

// Constants
const uint32_t PLANT_MAXIMUM_AGE = 10;
const uint32_t HERBIVORE_MAXIMUM_AGE = 50;
const uint32_t CARNIVORE_MAXIMUM_AGE = 80;
const uint32_t MAXIMUM_ENERGY = 200;
const uint32_t THRESHOLD_ENERGY_FOR_REPRODUCTION = 20;

// Probabilities
const double PLANT_REPRODUCTION_PROBABILITY = 0.2;
const double HERBIVORE_REPRODUCTION_PROBABILITY = 0.075;
const double CARNIVORE_REPRODUCTION_PROBABILITY = 0.025;
const double HERBIVORE_MOVE_PROBABILITY = 0.7;
const double HERBIVORE_EAT_PROBABILITY = 0.9;
const double CARNIVORE_MOVE_PROBABILITY = 0.5;
const double CARNIVORE_EAT_PROBABILITY = 1.0;

// Type definitions
enum entity_type_t
{
    empty,
    plant,
    herbivore,
    carnivore
};

struct pos_t
{
    uint32_t i;
    uint32_t j;
};

struct entity_t
{
    entity_type_t type;
    int32_t energy;
    int32_t age;
};

// Auxiliary code to convert the entity_type_t enum to a string
NLOHMANN_JSON_SERIALIZE_ENUM(entity_type_t, {
                                                {empty, " "},
                                                {plant, "P"},
                                                {herbivore, "H"},
                                                {carnivore, "C"},
                                            })

// Auxiliary code to convert the entity_t struct to a JSON object
namespace nlohmann
{
    void to_json(nlohmann::json &j, const entity_t &e)
    {
        j = nlohmann::json{{"type", e.type}, {"energy", e.energy}, {"age", e.age}};
    }
}

// Grid that contains the entities
static std::vector<std::vector<entity_t>> entity_grid;

std::mutex obj_mutex;

bool random_action(float probability) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < probability;
}

// Planta
void plant_life(int row, int col){
    entity_grid[row][col].age++;
    if(entity_grid[row][col].age > PLANT_MAXIMUM_AGE){
        entity_grid[row][col].type = empty;
        entity_grid[row][col].age = 0;
        entity_grid[row][col].energy = 0;
        return;
    }

    
}

// Herbivoro
void herbivore_life(int row, int col){

}

// Carnivoro
void carnivore_life(int row, int col){


}

int main()
{
    crow::SimpleApp app;

    // Endpoint que serve a página HTML
    CROW_ROUTE(app, "/")
    ([](crow::request &, crow::response &res)
     {
        // Retorna o conteúdo HTML
        res.set_static_file_info_unsafe("../public/index.html");
        res.end(); });

    // Endpoint que inicia a simulação, com os parâmetros estabelecidos
    CROW_ROUTE(app, "/start-simulation")
        .methods("POST"_method)([](crow::request &req, crow::response &res)
                                { 
        // Faz o parse no body do JSON
        nlohmann::json request_body = nlohmann::json::parse(req.body);

       // Valida o número total de entidades no body
        uint32_t total_entinties = (uint32_t)request_body["plants"] + (uint32_t)request_body["herbivores"] + (uint32_t)request_body["carnivores"];
        if (total_entinties > NUM_ROWS * NUM_ROWS) {
        res.code = 400;
        res.body = "Too many entities";
        res.end();
        return;
        }

        // Limpa o grid de entidades
        entity_grid.clear();
        entity_grid.assign(NUM_ROWS, std::vector<entity_t>(NUM_ROWS, { empty, 0, 0}));
        
        // Create the entities
        // <YOUR CODE HERE>
        
        int i, row, col;

        // plants
        for(i = 0; i < (uint32_t)request_body["plants"]; i++){
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, NUM_ROWS - 1 );
            row = dis(gen);
            col = dis(gen);

            while(!entity_grid[row][col].type == empty){
                row = dis(gen);
                col = dis(gen);
            }
            
            entity_grid[row][col].type = plant;
            entity_grid[row][col].age = 0; 
            std::thread plant_thread(plant_life, row, col); 
        }

        // herbivores
        for(i = 0; i < (uint32_t)request_body["herbivores"]; i++){
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, NUM_ROWS - 1);
            row = dis(gen);
            col = dis(gen);

            while(!entity_grid[row][col].type == empty){
                row = dis(gen);
                col = dis(gen);
            }
            
            entity_grid[row][col].type = herbivore;
            entity_grid[row][col].age = 0;
            entity_grid[row][col].energy = 100;
            std::thread herb_thread(herbivore_life, row, col); 
        }

        // carnivores
        for(i = 0; i < (uint32_t)request_body["carnivores"]; i++){
            static std::random_device rd; //cria item aleatório
            static std::mt19937 gen(rd()); //inicializa o gerador de aletórios
            std::uniform_int_distribution<> dis(0, NUM_ROWS - 1); //Gera número alatório entre 0 e 14
            row = dis(gen);
            col = dis(gen);

            while(!entity_grid[row][col].type == empty){
                row = dis(gen);
                col = dis(gen);
            }
            
            entity_grid[row][col].type = carnivore;
            entity_grid[row][col].age = 0;
            entity_grid[row][col].energy = 100;
            std::thread carn_thread(carnivore_life, row, col);
        }

        // Retorna o JSON que representa o grid de entidades
        nlohmann::json json_grid = entity_grid; 
        res.body = json_grid.dump();
        res.end(); });

    // Endpoint to process HTTP GET requests for the next simulation iteration
    CROW_ROUTE(app, "/next-iteration")
        .methods("GET"_method)([]()
                               {
        // Simulate the next iteration
        // Iterate over the entity grid and simulate the behaviour of each entity
        
        // <YOUR CODE HERE>
        
        // Retorna a representação do grid em JSON
        nlohmann::json json_grid = entity_grid; 
        return json_grid.dump(); });
    app.port(8080).run();

    return 0;
}