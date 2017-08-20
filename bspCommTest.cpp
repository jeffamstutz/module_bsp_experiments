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

// Bulk library
#include "bulk/bulk.hpp"

#if 1
#  include <bulk/backends/thread/thread.hpp>
using environment = bulk::thread::environment;
#else
#  include <bulk/backends/mpi/mpi.hpp>
using environment = bulk::mpi::environment;
#endif

int main()
{
  environment env;

  env.spawn(env.available_processors(), [](bulk::world &world) {
    int s = world.rank();
    int p = world.active_processors();

    world.log("Hello, world %d/%d", s, p);

    auto a               = bulk::var<int>(world);
    a(world.next_rank()) = s;
    world.sync();
    // ... the local a is now updated
    world.log("%d got put %d", s, a.value());

    auto b = a(world.next_rank()).get();
    world.sync();
    // ... b.value() is now available

    // coarrays are distributed arrays, each processor has an array to which
    // other processors can write
    auto xs                  = bulk::coarray<int>(world, 10);
    xs(world.next_rank())[3] = s;

    // messages can be passed to queues that specify a tag type, and a
    // content type
    auto q = bulk::queue<int, float>(world);
    for (int t = 0; t < p; ++t) {
      q(t).send(s, 3.1415f);  // send (s, pi) to processor t
    }
    world.sync();

    // Messages are now available in q
    for (auto[tag, content] : q) {
      world.log("%d got sent %d, %f", s, tag, content);
    }
  });

  return 0;
}