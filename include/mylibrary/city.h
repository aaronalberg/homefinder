// Copyright (c) 2020 [Your Name]. All rights reserved.
#ifndef FINALPROJECT_MYLIBRARY_EXAMPLE_H_
#define FINALPROJECT_MYLIBRARY_EXAMPLE_H_

#include <string>


namespace mylibrary {

struct City {
  City(const std::string& name, const double& population, const double& lat, const double& lng);
  std::string name;
  double population;
  double temperature;
  double crime_index;
  double healthcare_index;
  double lat;
  double lng;

};

}  // namespace mylibrary


#endif // FINALPROJECT_MYLIBRARY_EXAMPLE_H_
