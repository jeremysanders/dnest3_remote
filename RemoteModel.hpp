#ifndef REMOTEMODEL_HH
#define REMOTEMODEL_HH

#include "Model.h"
#include <vector>

class RemoteModel : public DNest3::Model
{
private:

public:
  RemoteModel();

  // Generate the point from the prior
  void fromPrior();

  // Metropolis-Hastings proposals
  double perturb();

  // Likelihood function
  double logLikelihood() const;

  // Print to stream
  void print(std::ostream& out) const;

  // Return string with column information
  std::string description() const;

  static void initialise();
  static void start_remote();

private:
  std::vector<double> params;

  static std::string remote_exe;
};

#endif
