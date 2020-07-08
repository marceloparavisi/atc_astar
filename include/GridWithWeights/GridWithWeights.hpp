#ifndef GRIDWITHWEIGHTS_HPP
#define GRIDWITHWEIGHTS_HPP

#include "../SquareGrid/SquareGrid.hpp"
#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>

struct GridWithWeights: SquareGrid {
  std::unordered_set<Pos> forests;
  GridWithWeights(int w, int h): SquareGrid(w, h) {}
  double cost(Pos from_node, Pos to_node) {
    // return 1;
    std::unordered_set<Pos>::const_iterator pos = forests.find(to_node);
    return pos != forests.end() ? pos->cost : 1;
    // return forests.find(to_node) != forests.end()? to_node.cost : 1;
  }
};

// This outputs a grid. Pass in a distances map if you want to print
// the distances, or pass in a point_to map if you want to print
// arrows that point to the parent location, or pass in a path vector
// if you want to draw the path.
inline void draw_grid(const GridWithWeights& grid, int field_width,
               std::unordered_map<Pos, double>* distances=nullptr,
               std::unordered_map<Pos, Pos>* point_to=nullptr,
               std::vector<Pos>* path=nullptr) {
  std::cout<<"\n grid: "<<grid.width<<", "<<grid.height;
  for (int y = grid.height; y != 0; --y) {
    for (int x = 0; x != grid.width; ++x) {
      Pos id {x, y};
      std::cout << std::left << std::setw(field_width);
      if (grid.walls.find(id) != grid.walls.end()) {
        std::cout << std::string(field_width, '#');
      } else if (point_to != nullptr && point_to->count(id)) {
        Pos next = (*point_to)[id];
        if (next.x == x + 1) { std::cout << "> "; }
        else if (next.x == x - 1) { std::cout << "< "; }
        else if (next.y == y + 1) { std::cout << "v "; }
        else if (next.y == y - 1) { std::cout << "^ "; }
        else { std::cout << "* "; }
      } else if (distances != nullptr && distances->count(id)) {
        std::cout << (*distances)[id];
      } else if (grid.forests.find(id) != grid.forests.end()) {
          if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
            std::cout << std::string(field_width, '&');
          } else {
            std::cout << std::string(field_width, '%');
          }
      } else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()){
        std::cout << '@';
      } else {
        std::cout << '.';
      }
    }
    std::cout << '\n';
  }
}

inline void publish_grid(const ros::Publisher& grid_pub, const GridWithWeights& grid, int field_width,  geometry_msgs::Pose currentPos,
               std::unordered_map<Pos, double>* distances=nullptr,
               std::unordered_map<Pos, Pos>* point_to=nullptr,
               std::vector<Pos>* path=nullptr) 
               
{
  
  nav_msgs::OccupancyGrid* mymap = new nav_msgs::OccupancyGrid();
  
  mymap->header.frame_id="world";
	mymap->header.stamp = ros::Time::now();
  mymap->info.width = grid.width;
	mymap->info.height = grid.height;
  mymap->info.resolution = 0.2;
  // mymap->info.resolution = grid.resolution;
	mymap->info.origin.position.x = currentPos.position.x-mymap->info.resolution*grid.width/2.0;
	mymap->info.origin.position.y = currentPos.position.y-mymap->info.resolution*grid.height/2.0;
	mymap->info.origin.position.z = 0;
	mymap->info.origin.orientation.x = 0;
	mymap->info.origin.orientation.y = 0;
	mymap->info.origin.orientation.z = 0;
	mymap->info.origin.orientation.w = 1;
  
  mymap->data.resize(mymap->info.width * mymap->info.height);
  for (int i = 0; i < mymap->data.size(); i++)
    mymap->data[i]=0;
  
  for (int y = 0; y < grid.height; ++y) {
    for (int x = 0; x < grid.width; ++x) {
      Pos id {x, y};
      // std::cerr << std::left << std::setw(field_width);
      if (grid.walls.find(id) != grid.walls.end()) 
      {
        //  std::cerr << std::string(field_width, '#');//virtual barrier
        mymap->data[x+y*grid.width] = 255;
      } 
      // else if (point_to != nullptr && point_to->count(id)) 
      // {
      //   Pos next = (*point_to)[id];
      //   if (next.x == x + 1) { std::cout << "> "; }
      //   else if (next.x == x - 1) { std::cout << "< "; }
      //   else if (next.y == y + 1) { std::cout << "v "; }
      //   else if (next.y == y - 1) { std::cout << "^ "; }
      //   else { std::cout << "* "; }
      // } else if (distances != nullptr && distances->count(id)) {
      //   std::cout << (*distances)[id];
      // } else if (grid.forests.find(id) != grid.forests.end()) {
      //     if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
      //       std::cout << std::string(field_width, '&');
      //     } else {
      //       std::cout << std::string(field_width, '%');// valor cost_map
      //     }
      // } else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()){
      //   std::cout << '@';//trajetória
      // } else {// limpo
      //   std::cout << '.';
      // }
    }
  }
  grid_pub.publish(*mymap);
  delete mymap;
}

// // This outputs a grid. Pass in a distances map if you want to print
// // the distances, or pass in a point_to map if you want to print
// // arrows that point to the parent location, or pass in a path vector
// // if you want to draw the path.
// inline void draw_grid(const GridWithWeights& grid, int field_width,
//                std::unordered_map<Pos, double>* distances=nullptr,
//                std::unordered_map<Pos, Pos>* point_to=nullptr,
//                std::vector<Pos>* path=nullptr) {
//   for (int y = 0; y != grid.height; ++y) {
//     for (int x = 0; x != grid.width; ++x) {
//       Pos id {x, y};
//       std::cout << std::left << std::setw(field_width);
//       if (grid.walls.find(id) != grid.walls.end()) {
//         std::cout << std::string(field_width, '#');
//       } else if (point_to != nullptr && point_to->count(id)) {
//         Pos next = (*point_to)[id];
//         if (next.x == x + 1) { std::cout << "> "; }
//         else if (next.x == x - 1) { std::cout << "< "; }
//         else if (next.y == y + 1) { std::cout << "v "; }
//         else if (next.y == y - 1) { std::cout << "^ "; }
//         else { std::cout << "* "; }
//       } else if (distances != nullptr && distances->count(id)) {
//         std::cout << (*distances)[id];
//       } else if (grid.forests.find(id) != grid.forests.end()) {
//           if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
//             std::cout << std::string(field_width, '&');
//           } else {
//             std::cout << std::string(field_width, '%');
//           }
//       } else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()){
//         std::cout << '@';
//       } else {
//         std::cout << '.';
//       }
//     }
//     std::cout << '\n';
//   }
// }

// // This outputs a grid. Pass in a distances map if you want to print
// // the distances, or pass in a point_to map if you want to print
// // arrows that point to the parent location, or pass in a path vector
// // if you want to draw the path.
// inline void draw_grid(const GridWithWeights& grid, int field_width,
//                std::unordered_map<Pos, double>* distances=nullptr,
//                std::unordered_map<Pos, Pos>* point_to=nullptr,
//                std::vector<Pos>* path=nullptr) {
//   for (int y = 0; y != grid.height; ++y) {
//     for (int x = 0; x != grid.width; ++x) {
//       Pos id {x, y};
//       std::cout << std::left << std::setw(field_width);
//       if (grid.walls.find(id) != grid.walls.end()) {
//         std::cout << std::string(field_width, '#');
//       } else if (grid.forests.find(id) != grid.forests.end()) {
//         std::cout << std::string(field_width, '%');
//       } else if (point_to != nullptr && point_to->count(id)) {
//         Pos next = (*point_to)[id];
//         if (next.x == x + 1) { std::cout << "> "; }
//         else if (next.x == x - 1) { std::cout << "< "; }
//         else if (next.y == y + 1) { std::cout << "v "; }
//         else if (next.y == y - 1) { std::cout << "^ "; }
//         else { std::cout << "* "; }
//       } else if (distances != nullptr && distances->count(id)) {
//         std::cout << (*distances)[id];
//       } else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
//         if (grid.forests.find(id) != grid.forests.end()){
//           std::cout << std::string(field_width, '&');
//         }else{
//           std::cout << '@';
//         }
//       } else {
//         std::cout << '.';
//       }
//     }
//     std::cout << '\n';
//   }
// }

// inline GridWithWeights make_diagram4() {
//   GridWithWeights grid(20, 20);
//   add_rect(grid, 1, 7, 4, 9);
//   typedef Pos L;
//   grid.forests = std::unordered_set<Pos> {
//     L{3, 4}, L{3, 5}, L{4, 1}, L{4, 2},
//     L{4, 3}, L{4, 4}, L{4, 5}, L{4, 6},
//     L{4, 7}, L{4, 8}, L{5, 1}, L{5, 2},
//     L{5, 3}, L{5, 4}, L{5, 5}, L{5, 6},
//     L{5, 7}, L{5, 8}, L{6, 2}, L{6, 3},
//     L{6, 4}, L{6, 5}, L{6, 6}, L{6, 7},
//     L{7, 3}, L{7, 4}, L{7, 5}
//   };
//   return grid;
// }

#endif