//  Copyright (c) 2014 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the seventh in a series of examples demonstrating the development
// of a fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds on example six.

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/serialization/serialize.hpp>

#include <boost/shared_array.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include <typeinfo>

inline std::size_t locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np/nl);
}

struct node_data
{
public:
    node_data() : num(0)
    {}

    // Create a new (uninitialized) partition of the given size.
    node_data(std::uint64_t value) : num(value)
    {}

    // Create a partition which acts as a proxy to a part of the embedded array.
    // The proxy is assumed to refer to either the left or the right boundary
    // element.
    node_data(node_data const& base) : num(base.get_num())
    {}

    std::uint64_t get_num() const
    {
        return num;
    }

    void set_num(std::uint64_t value)
    {
        num = value;
    }


private:
    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class hpx::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & num;
    }

    std::uint64_t num;
};

std::uint64_t operator + (std::uint64_t num, node_data const& value)
{
    return num + value.get_num();
}

std::ostream& operator<<(std::ostream& os, node_data const& c)
{
    os << "{" << c.get_num() << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// This is the server side representation of the data. We expose this as a HPX
// component which allows for it to be created and accessed remotely through
// a global address (hpx::id_type).
struct computation_node_server 
: hpx::components::component_base<computation_node_server>
{
    // construct new instances
    computation_node_server() 
    {}

    computation_node_server(node_data value) : num(value)
    {}

    computation_node_server(std::uint64_t value) : num(value)
    {}

    node_data get_data() const
    {
        return num;
    }

    // Every member function which has to be invoked remotely needs to be
    // wrapped into a component action. The macro below defines a new type
    // 'get_data_action' which represents the (possibly remote) member function
    // partition::get_data().
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(computation_node_server, get_data, get_data_action);

private:
    node_data num;
};

// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<computation_node_server> computation_node_server_type;
HPX_REGISTER_COMPONENT(computation_node_server_type, computation_node_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef computation_node_server::get_data_action get_data_action;
HPX_REGISTER_ACTION(get_data_action);

///////////////////////////////////////////////////////////////////////////////
// This is a client side helper class allowing to hide some of the tedious
// boilerplate.
struct computation_node : hpx::components::client_base<computation_node, computation_node_server>
{
    typedef hpx::components::client_base<computation_node, computation_node_server> base_type;

    computation_node() {}

    computation_node(hpx::id_type where, node_data initial_value)
      : base_type(hpx::new_<computation_node_server>(hpx::colocated(where), initial_value))
    {}

    // Create new component on locality 'where' and initialize the held data
    computation_node(hpx::id_type where, std::uint64_t value, std::uint64_t value1)
      : base_type(hpx::new_<computation_node_server>(where, value))
    {}

    // Attach a future representing a (possibly remote) partition.
    computation_node(hpx::future<hpx::id_type> && id)
      : base_type(std::move(id))
    {}

    // Unwrap a future<partition> (a partition already holds a future to the
    // id of the referenced object, thus unwrapping accesses this inner future).
    computation_node(hpx::future<computation_node> && c)
      : base_type(std::move(c))
    {}

    ///////////////////////////////////////////////////////////////////////////
    // Invoke the (remote) member function which gives us access to the data.
    // This is a pure helper function hiding the async.
    hpx::future<node_data> get_data() const
    {
        computation_node_server::get_data_action act;
        return hpx::async(act, get_id());
    }
};

///////////////////////////////////////////////////////////////////////////////
struct stepper
{
    // Our data for one time step
    typedef std::vector<computation_node> space;

    static computation_node compute(computation_node const& node)
    {
        using hpx::dataflow;
        using hpx::util::unwrapped;

        return dataflow(
            hpx::launch::async,
            unwrapped(
                [node](node_data const& num)
                {
                    std::uint64_t base = num.get_num();
                    std::uint64_t results = base*base;
                    node_data node_result;
                    node_result.set_num(results);

                    std::cout << " Currently on locality: " << node.get_id()  << " ,computed value: " << results << std::endl; 
                    // The new partition_data will be allocated on the same locality
                    // as 'middle'                    
                    return computation_node(node.get_id(), node_result);
                }
            ),
            node.get_data()
        );
    }

    // do all the work on 'np' partitions, 'nx' data points each, for 'nt'
    // time steps
    std::uint64_t do_work(std::size_t np, std::size_t nx, std::size_t nt);
};

HPX_PLAIN_ACTION(stepper::compute, compute_action);

///////////////////////////////////////////////////////////////////////////////
// do all the work on 'np' partitions, 'nx' data points each, for 'nt'
// time steps
std::uint64_t stepper::do_work(std::size_t np, std::size_t nx, std::size_t nt)
{
    using hpx::dataflow;

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                    // Number of localities

    // U[t][i] is the state of position i at time t.
    std::vector<space> U(2);
    for (space& s: U)
        s.resize(np);

    // Initial conditions: f(0, i) = i
    for (std::size_t i = 0; i != np; ++i)
    {
        U[0][i] = computation_node(localities[locidx(i, np, nl)], (std::uint64_t(i+2)), (std::uint64_t(i+2)));
    }

    space const& current = U[0];
    space& next = U[1];

    compute_action act;
    for (std::size_t i = 0; i != np; ++i)
    {
        using hpx::util::placeholders::_1;
        auto Op = hpx::util::bind(act, localities[locidx(i, np, nl)], _1);  
        next[i] = dataflow(hpx::launch::async, Op, current[i]);
    }

    hpx::future<space> result = hpx::when_all(next);
    space vec_result = result.get();

    std::uint64_t sum = 0;
    for (std::size_t i = 0; i != np; ++i)
    {
        computation_node node = vec_result[i];
        hpx::shared_future<node_data> fnode = node.get_data();
        sum = sum + fnode.get().get_num();
    }
    return sum;
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t np = vm["np"].as<std::uint64_t>();   // Number of partitions.
    std::uint64_t nx = vm["nx"].as<std::uint64_t>();   // Number of grid points.
    std::uint64_t nt = vm["nt"].as<std::uint64_t>();   // Number of steps.

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                // Number of localities

    if (np < nl)
    {
        std::cout << "The number of partitions should not be smaller than "
                     "the number of localities" << std::endl;
        return hpx::finalize();
    }

    // Create the stepper object
    stepper step;

    // Measure execution time.
    std::uint64_t t = hpx::util::high_resolution_clock::now();

    // Execute nt time steps on nx grid points and print the final solution.
    std::uint64_t solution = step.do_work(np, nx, nt);
    
    std::uint64_t elapsed = hpx::util::high_resolution_clock::now() - t;

    std::cout << "solution: " << solution << std::endl;

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("results", "print generated results (default: false)")
        ("nx", value<std::uint64_t>()->default_value(1),
         "Local x dimension (of each partition)")
        ("nt", value<std::uint64_t>()->default_value(1),
         "Number of time steps")
        ("np", value<std::uint64_t>()->default_value(4),
         "Number of partitions")
        ( "no-header", "do not print out the csv header row")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}

//c++ -o ml_dis_parallel ml_dis_parallel.cpp `pkg-config --cflags --libs hpx_application_debug` -lhpx_iostreamsd -DHPX_APPLICATION_NAME=ml_dis_parallel
