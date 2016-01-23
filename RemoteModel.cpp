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

#include "RemoteModel.hpp"
#include "RandomNumberGenerator.h"
#include "Utils.h"
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

std::string RemoteModel::remote_exe;

struct ParameterRecord
{
  double defval;
  double minval;
  double maxval;
  double randrange;
  char name[32];
};

// parameters from remote command
static std::vector<ParameterRecord> remote_params;

static boost::thread_specific_ptr<int> remote_fd;

static void read_size(int fd, void* buf, size_t count)
{
  size_t n = 0;
  while(n < count)
    {
      ssize_t retn = read(fd, buf, count-n);
      if(retn <= 0)
        {
          std::cerr << "Failed to read response from remote process\n";
          exit(1);
        }
      n += retn;
    }
}

void RemoteModel::initialise()
{
  const char *remote = std::getenv("REMOTE_MODEL");
  if(remote == 0)
    {
      std::cerr << "REMOTE_MODEL needs to be set\n";
      std::exit(1);
    }
  remote_exe = remote;

  int fd[2];
  if( socketpair(PF_LOCAL, SOCK_STREAM, 0, fd) != 0 )
    {
      perror("failed to create sockets");
      exit(1);
    }
  int pid = fork();
  if(pid == 0)
    {
      // am child
      close(fd[0]);
      execl(remote_exe.c_str(), remote_exe.c_str(), "params",
            boost::lexical_cast<std::string>(fd[1]).c_str(),
            (char*)0);
      perror("failed to start remote");
      exit(1);
    }
  else
    {
      // am parent
      close(fd[1]);

      int npars;
      read_size(fd[0], &npars, sizeof(npars));

      ParameterRecord rec;
      for(int i=0; i<npars; ++i)
        {
          read_size(fd[0], &rec, sizeof(rec));
          remote_params.push_back(rec);
        }

      close(fd[0]);
      waitpid(pid, 0, 0);
    }
}

boost::recursive_mutex sockmtx;

void RemoteModel::start_remote()
{
  int fd[2];
  {
    // race on fcntl without this
    boost::lock_guard<boost::recursive_mutex> lock(sockmtx);

    if( socketpair(PF_LOCAL, SOCK_STREAM, 0, fd) != 0 )
      {
        perror("failed to create sockets");
        exit(1);
      }

    // avoid propagating other sockets to children
    if(fcntl(fd[0], F_SETFD, FD_CLOEXEC) < 0)
      {
        perror("close socket on exec");
        exit(1);
      }
  }

  int pid = fork();
  if(pid == 0)
    {
      // am child
      close(fd[0]);
      execl(remote_exe.c_str(), remote_exe.c_str(), "run",
            boost::lexical_cast<std::string>(fd[1]).c_str(),
            (char*)0);
      perror("failed to start remote");
      exit(1);
    }
  else
    {
      // am parent
      close(fd[1]);
      remote_fd.reset(new int);
      *remote_fd = fd[0];
    }
}

RemoteModel::RemoteModel()
  : params(remote_params.size())
{
}

void RemoteModel::fromPrior()
{
  for(size_t i=0; i!=params.size(); ++i)
    {
      const ParameterRecord& r = remote_params[i];
      params[i] = r.minval + DNest3::randomU()*(r.maxval-r.minval);
    }
}

double RemoteModel::perturb()
{
  int which = DNest3::randInt(params.size());
  const ParameterRecord& r = remote_params[which];

  double p = params[which] + DNest3::randh()*r.randrange;
  params[which] = DNest3::mod(p-r.minval, r.maxval-r.minval)+r.minval;
  return 0.;
}

double RemoteModel::logLikelihood() const
{
  if( ! remote_fd.get() )
    start_remote();

  ssize_t size = sizeof(double)*params.size();
  ssize_t retn = write(*remote_fd, &params[0], size);
  if(retn < size)
    {
      std::cout << "Could not write to remote process\n";
      exit(1);
    }

  double like;
  read_size(*remote_fd, &like, sizeof(like));

  return like;
}

void RemoteModel::print(std::ostream& out) const
{
  for(size_t i=0; i != params.size(); ++i)
    out << params[i] << ' ';
}

std::string RemoteModel::description() const
{
  std::string retn;
  for(size_t i=0; i != remote_params.size(); ++i)
    {
      retn += remote_params[i].name;
      retn += " ";
    }
  return retn;
}
