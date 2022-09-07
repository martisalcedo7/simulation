#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "solvers.hpp"
#include "cities.hpp"
#include "utils.hpp"

// Brute Force

BruteForce::BruteForce(Cities &cities): cities {cities}{
    solved = false;
    counter = 0;
    number_of_cities = cities.get_number_of_cities();

    path = std::vector<int>(number_of_cities, 0);
    short_path = std::vector<int>(number_of_cities-1, 0);

    for(size_t x=1; x<number_of_cities; x++){
        short_path.at(x-1)=x;
        path.at(x)=x;
    }

    best_distance = cities.total_distance(path);
    best_path = path;

    long permutations = factorial(number_of_cities-1)/2.0;
    std::cout << "Possible paths for " << number_of_cities << " cities: (" << number_of_cities << "-1)! / 2 = " << permutations << "\n";
}

void BruteForce::step(void){
    if(solved){
        return;
    }

    for(size_t x=0; x<number_of_cities-1; x++){
        path.at(x+1) = short_path.at(x);
    }

    float distance = cities.total_distance(path);

    if(distance < best_distance){
        best_distance = distance;
        best_path = path;
    }

    counter++;

    // std::cout << counter << "\n";

    while(true){
        if(!std::next_permutation(short_path.begin(), short_path.end())){
            solved = true;
            break;
        }
        if(short_path.at(0) < short_path.back()){
            break;
        }
    }
}

void BruteForce::solve(void){
    while(!solved){
        step();
        // std::cout << counter << "\n";
    }
}

std::vector<int> BruteForce::get_best_path(void){
    return best_path;
}

std::vector<int> BruteForce::get_path(void){
    return path;
}

float BruteForce::get_best_distance(void){
    return std::sqrt(best_distance);
}

bool BruteForce::is_solved(void){
    return solved;
}

long BruteForce::get_counter(void){
    return counter;
}


// SOM solver

SOM::SOM(Cities &cities, float nabla, float nabla_decay, float alpha,
         float alpha_decay, uint number_of_iterations, uint points_multiplier): cities {cities}, 
         nabla {nabla}, nabla_decay {nabla_decay}, alpha {alpha}, alpha_decay {alpha_decay},
         number_of_iterations {number_of_iterations}, points_multiplier {points_multiplier}{

    solved = false;
    counter = 0;
    number_of_cities = cities.get_number_of_cities();
    cities_location = cities.get_cities();
    number_of_points = number_of_cities * points_multiplier;

    // Initialize best path and best distance
    best_path = std::vector<int>(number_of_cities);
    for(size_t x=0; x<number_of_cities; x++){
        best_path.at(x)=x;
    }
    best_distance = cities.total_distance(best_path);

    //Create points in an ellipse
    points = std::vector<Point>(number_of_points);

    float angle_increment = 2 * M_PI / number_of_points;
    float a = cities.get_map_size().x / 2.0;
    float b = cities.get_map_size().y / 2.0;

    for(size_t x=0; x<number_of_points; x++){
        // points.at(x).x = radius * std::cos(x * angle_increment) + cities.get_map_size().x / 2.0;
        // points.at(x).y = radius * std::sin(x * angle_increment) + cities.get_map_size().y / 2.0;
        float r = (a*b)/(std::sqrt(a*a*std::sin(x * angle_increment)*std::sin(x * angle_increment)+b*b*std::cos(x * angle_increment)*std::cos(x * angle_increment)));
        points.at(x).x = r * std::cos(x * angle_increment)+ a;
        points.at(x).y = r * std::sin(x * angle_increment)+ b;
        // points.at(x).x = random_number(cities.get_map_size().x);
        // points.at(x).y = random_number(cities.get_map_size().y);
    }

    //Create neighboring matrix
    neighboring = std::vector<std::vector<float>>(number_of_points, std::vector<float>(number_of_points));
    update_neighboring();

}

void SOM::update_neighboring(void){
    for(size_t x=0; x<number_of_points; x++){
        for(size_t y=0; y<number_of_points; y++){
            uint distance = std::abs(static_cast<int>(x)-static_cast<int>(y));
            if(distance>(number_of_points/2)){
                distance = number_of_points - distance;
            }
            if(distance == 0){
                neighboring.at(x).at(y) = 1.0;
            }else{
                neighboring.at(x).at(y) = std::exp(-std::pow(distance, 2.0)/alpha);
            }
        }
    }
}

void SOM::step(void){
    if(solved){
        return;
    }

    uint city = random_number(number_of_cities-1);

    uint c_point = closest_point(city);

    for(size_t x=0; x<number_of_points; x++){
        points.at(x).x = points.at(x).x + nabla * neighboring.at(c_point).at(x) * (cities_location.at(city).x - points.at(x).x);
        points.at(x).y = points.at(x).y + nabla * neighboring.at(c_point).at(x) * (cities_location.at(city).y - points.at(x).y);
    }

    if(alpha > alpha_decay){
        alpha /=(1 + alpha_decay);
        alpha = std::max(alpha, alpha_decay);
    }
    if(nabla > nabla_decay){
        nabla /= (1 + nabla_decay);
        nabla = std::max(nabla, nabla_decay);
    }

    // std::cout << nabla << " " << alpha << "\n";

    update_neighboring();

    counter++;

    if(counter >= number_of_iterations){
        solved = true;
    }

}

void SOM::solve(void){
    while(!solved){
        step();
    }
}

std::vector<int> SOM::get_best_path(void){

    std::vector<int> path(number_of_cities, 0);
    std::vector<int> visited_cities(number_of_cities, -1);
    uint visited_cities_index = 0;

    for(size_t x=0; x<number_of_points; x++){
        float shortest_distance = distance_city_point(x, 0);
        uint closest_city = 0;
        for(size_t y=1; y<number_of_cities; y++){
            float distance = distance_city_point(x, y);
            if(distance < shortest_distance){
                shortest_distance = distance;
                closest_city = y;
            }
        }
        if (!std::count(visited_cities.begin(), visited_cities.end(), closest_city)) {
            visited_cities.at(visited_cities_index) = closest_city;
            visited_cities_index++;
        }
    }

    // print_vector(path);
    if (!std::count(visited_cities.begin(), visited_cities.end(), -1)) {
        float distance = cities.total_distance(visited_cities);
        if(distance < best_distance){
            best_distance = distance;
            best_path = visited_cities;
        }
    }

    return best_path;
}

float SOM::get_best_distance(void){
    return std::sqrt(best_distance);
}

bool SOM::is_solved(void){
    return solved;
}

long SOM::get_counter(void){
    return counter;
}

std::vector<Point> SOM::get_points(void){
    return points;
}

uint SOM::get_number_of_points(void){
    return number_of_points;
}

uint SOM::closest_point(uint city){
    uint closest_point = 0;
    float shortest_distance = distance_city_point(0, city);
    for(size_t x=1; x<number_of_points; x++){
        float distance = distance_city_point(x, city);
        if(distance < shortest_distance){
            closest_point = x;
            shortest_distance = distance;
        }
    }
    return closest_point;
}

float SOM::distance_city_point(uint point, uint city){
    return std::pow(cities_location.at(city).x - points.at(point).x, 2.0) + std::pow(cities_location.at(city).y - points.at(point).y, 2.0);
}
