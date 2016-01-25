// Copyright 2016 Jeremy Sanders

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
