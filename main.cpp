#include <iostream>
#include "Start.h"
#include "RemoteModel.hpp"

int main(int argc, char** argv)
{
  RemoteModel::initialise();

#ifndef DNest3_No_Boost
  DNest3::MTSampler<RemoteModel> sampler =
    DNest3::setup_mt<RemoteModel>(argc, argv);
#else
  DNest3::Sampler<RemoteModel> sampler =
    DNest3::setup<RemoteModel>(argc, argv);
#endif

  sampler.run();
  return 0;
}
