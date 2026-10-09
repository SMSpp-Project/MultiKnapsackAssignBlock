# MultiKnapsackAssignBlock

`MultiKnapsackAssignBlock` is a SMS++ `:Block` for the Multiple Knapsack
Assignment Problem (MKAP), in which N items, partitioned into R classes, are
to be placed in M knapsacks: each knapsack is given at most one class and
only holds items of that class, within its capacity, each item goes in at
most one knapsack, and the total profit of the items placed is maximized
(see, e.g., S. Kataoka, T. Yamada, "Upper and lower bounding procedures for
the multiple knapsack assignment problem", European Journal of Operational
Research 237(2), 440-447, 2014).

The problem is represented as M * R sub-`Block`, one `BinaryKnapsackBlock`
for each pair of a knapsack and a class, whose first item is the binary
variable that gives the class to the knapsack (with profit 0 and weight
minus the capacity of the knapsack) and whose other ones are the items of
the class; the `MultiKnapsackAssignBlock` itself only holds the constraints
that link them, i.e., each item in at most one knapsack and each knapsack
given at most one class. Hence, relaxing these constraints in a Lagrangian
way leaves M * R independent 0-1 knapsack problems, which any `Solver` of
`BinaryKnapsackBlock` can solve, while a `:MILPSolver` solves the whole
problem out of its abstract representation.

The data can be read from netCDF (`deserialize()`), from a text file (the
numbers of items, classes and knapsacks, the capacities, then the index, the
profit, the weight and the class of each item) or from memory (`load()`).


## Getting started

These instructions will let you build the `MultiKnapsackAssignBlock` module on
your system.

### Requirements

- The [SMS++ core library](https://gitlab.com/smspp/smspp) and its
  requirements.

- [BinaryKnapsackBlock](https://gitlab.com/smspp/binaryknapsackblock).

### Build and install with CMake

Configure and build the library with:

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

The library has the same configuration options of
[SMS++](https://gitlab.com/smspp/smspp-project/-/wikis/Customize-the-configuration).

Optionally, install the library in the system with:

```sh
cmake --install .
```

### Usage with CMake

After the library is built, you can use it in your CMake project with:

```cmake
find_package(MultiKnapsackAssignBlock)
target_link_libraries(<my_target> SMS++::MultiKnapsackAssignBlock)
```

### Build and install with makefiles

Carefully hand-crafted makefiles have also been developed for those unwilling
to use CMake. Makefiles build the executable in-source (in the same directory
tree where the code is) as opposed to out-of-source (in the copy of the
directory tree constructed in the build/ folder) and therefore it is more
convenient when having to recompile often, such as when developing/debugging
a new module, as opposed to the compile-and-forget usage envisioned by CMake.

Each executable using `MultiKnapsackAssignBlock` has to include a "main
makefile" of the module, which typically is either [makefile-c](makefile-c)
including all necessary libraries comprised the "core SMS++" one, or
[makefile-s](makefile-s) including all necessary libraries but not the "core
SMS++" one (for the common case in which this is used together with other
modules that already include them). These in turn recursively include all the
required other makefiles, hence one should only need to edit the "main
makefile" for compilation type (C++ compiler and its options) and it all should
be good to go. In case some of the external libraries are not at their default
location, it should only be necessary to create the `../extlib/makefile-paths`
out of the `extlib/makefile-default-paths-*` for your OS `*` and edit the
relevant bits (commenting out all the rest).

Check the [SMS++ installation
wiki](https://gitlab.com/smspp/smspp-project/-/wikis/Customize-the-configuration#location-of-required-libraries)
for further details.


## Data

We provide a set of instances that is used by the testers of the [tests
repo](https://gitlab.com/smspp/tests). It is not included in the repo: it is
automatically downloaded by CMake when the tests need it, but if you are not
using CMake you need to do it by hand, via

```sh
cd data
wget https://gitlab.com/api/v4/projects/26260764/packages/generic/txt/2026-10-07/txt.tgz
tar xzvf txt.tgz
```

This builds the folder `data/txt`, with 12 instances in the text format
above: 50 and 100 items, 2 classes and 4 knapsacks or 4 classes and 8
knapsacks, and profits and weights uncorrelated, weakly or strongly
correlated. They are produced by `data/make-txt`, out of the generator
`data/generate`, which draws the class of each item uniformly, its weight in
[1, 1000] and its profit as in the classical classes of the 0-1 knapsack
problem, and the capacity of each knapsack so that the knapsacks hold about
half of the total weight.


## Tests

The unit test of the module, in [test](test), only uses the SMS++ core and
`BinaryKnapsackBlock`; the suite that cross-checks a `:MILPSolver` and the
Lagrangian decomposition on the instances lives in the [tests
repo](https://gitlab.com/smspp/tests).


## Getting help

If you need support, you want to submit bugs or propose a new feature, you
can [open a new issue](https://gitlab.com/smspp/multiknapsackassignblock/-/issues/new).


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of
conduct, and the process for submitting merge requests to us.


## Authors

### Current Lead Authors

- **Antonio Frangioni**  
  Dipartimento di Informatica  
  Università di Pisa

- **Federica Di Pasquale**  
  Dipartimento di Informatica  
  Università di Pisa

- **Donato Meoli**  
  Dipartimento di Informatica  
  Università di Pisa


## License

This code is provided free of charge under the [GNU Lesser General Public
License version 3.0](https://opensource.org/licenses/lgpl-3.0.html) -
see the [LICENSE](LICENSE) file for details.


## Disclaimer

The code is currently provided free of charge under an open-source license.
As such, it is provided "*as is*", without any explicit or implicit warranty
that it will properly behave or it will suit your needs. The Authors of
the code cannot be considered liable, either directly or indirectly, for
any damage or loss that anybody could suffer for having used it. More
details about the non-warranty attached to this code are available in the
license description file.
