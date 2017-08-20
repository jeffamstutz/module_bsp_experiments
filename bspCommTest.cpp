// ========================================================================== //
// The MIT License (MIT)                                                      //
//                                                                            //
// Copyright (c) 2017 Jefferson Amstutz                                       //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// in all copies or substantial portions of the Software.                     //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
// ========================================================================== //

#include <iostream>
#include <random>

// Bulk library
#include "bulk/bulk.hpp"

#if 1
#  include <bulk/backends/thread/thread.hpp>
using environment = bulk::thread::environment;
#else
#  include <bulk/backends/mpi/mpi.hpp>
using environment = bulk::mpi::environment;
#endif

// ospcommon
#include "components/ospcommon/utility/CodeTimer.h"

const int payloadSize = 100000;

struct DummyData
{
  ospcommon::byte_t values[payloadSize];
};

int main()
{
  environment env;

  env.spawn(env.available_processors(), [](bulk::world &world) {
    int rank     = world.rank();
    int numRanks = world.active_processors();

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> randomRank(0, numRanks - 1);

    auto queue = bulk::queue<DummyData>(world);

    double t0 = ospcommon::getSysTime();
    double t1 = ospcommon::getSysTime();

    size_t numReceived = 0;

    while (true) {
      double t2 = ospcommon::getSysTime();

      if (t2 - t1 > 1) {
        numReceived += queue.size();

        double rate = numReceived * payloadSize / (t2 - t0);

        std::string numBytes = ospcommon::prettyNumber(numReceived*payloadSize);
        std::string rateString = ospcommon::prettyNumber(rate);

        world.log("rank %i: received %li messages (%sbytes) in %lf secs;"
                  " that is %sB/s", rank, numReceived, numBytes.c_str(),
                  t2-t0, rateString.c_str());

        world.sync();

        t1 = ospcommon::getSysTime();
      } else {
        queue(randomRank(rng)).send(DummyData());
      }
    }
  });
}