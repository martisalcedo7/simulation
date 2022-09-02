#ifndef SOLVERS_HPP
#define SOLVERS_HPP

#include "cities.hpp"
#include "SFML/Graphics.hpp"
#include "utils.hpp"

//Solvers

class BruteForce{

    private:
        Cities cities;
        uint number_of_cities;
        std::vector<int> best_path;
        float best_distance;
        long counter;
        bool solved;

        //This solver
        std::vector<int> path;
        std::vector<int> short_path;

    public:

        BruteForce(Cities &cities);

        void step(void);
        void solve(void);

        std::vector<int> get_best_path(void);
        std::vector<int> get_path(void);
        float get_best_distance(void);
        bool is_solved(void);
        long get_counter(void);
    
};

class SOM{

    private:
        Cities cities;
        uint number_of_cities;
        std::vector<int> best_path;
        float best_distance;
        long counter;
        bool solved;

        //This solver
        // std::vector<Point> points;

    public:

        SOM(Cities &cities);

        void step(void);
        void solve(void);

        std::vector<int> get_best_path(void);
        float get_best_distance(void);
        bool is_solved(void);
        long get_counter(void);
    
};

#endif