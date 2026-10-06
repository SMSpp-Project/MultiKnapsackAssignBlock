/*--------------------------------------------------------------------------*/
/*------------------------------ File test.cpp -----------------------------*/
/*--------------------------------------------------------------------------*/
/** @file
 * Unit test of MultiKnapsackAssignBlock, which only uses the SMS++ core and
 * BinaryKnapsackBlock. On small instances built in memory, among which the
 * edge cases of one knapsack, of one class, of a class with no items and of
 * knapsacks of capacity zero, it checks that:
 *
 * - the Block factory gives a MultiKnapsackAssignBlock;
 *
 * - load() builds one BinaryKnapsackBlock per pair of a knapsack and a
 *   class, whose first item has weight minus the capacity of the knapsack
 *   and whose other ones are the items of the class;
 *
 * - the abstract representation has two groups of static Constraint, one
 *   "assign" Constraint per item, with one Variable per knapsack, and one
 *   "class" Constraint per knapsack, with one Variable per class, and its
 *   generation issues no Modification to a FakeSolver registered to the
 *   Block;
 *
 * - a second load() of the same data, from memory or from a stream with
 *   comments, gives the same Block, and so does the netCDF round trip;
 *
 * - invalid data are refused.
 *
 * The solution of the instances, by a :MILPSolver and by the Lagrangian
 * decomposition, is cross-checked in the MultiKnapsackAssignBlock suite of
 * the tests of the umbrella.
 *
 * \author Federica Di Pasquale \n
 *         Dipartimento di Informatica \n
 *         Universita' di Pisa \n
 *
 * \author Antonio Frangioni \n
 *         Dipartimento di Informatica \n
 *         Universita' di Pisa \n
 *
 * \copyright &copy; by Federica Di Pasquale, Antonio Frangioni
 */
/*--------------------------------------------------------------------------*/
/*------------------------------ INCLUDES ----------------------------------*/
/*--------------------------------------------------------------------------*/

#include <filesystem>

#include <iostream>

#include <sstream>

#include "FakeSolver.h"

#include "MultiKnapsackAssignBlock.h"

/*--------------------------------------------------------------------------*/
/*-------------------------------- USING -----------------------------------*/
/*--------------------------------------------------------------------------*/

using namespace SMSpp_di_unipi_it;

using Index = Block::Index;

/*--------------------------------------------------------------------------*/
/*------------------------------ FUNCTIONS ---------------------------------*/
/*--------------------------------------------------------------------------*/

/// an instance of the Multiple Knapsack Assignment Problem

struct Instance {
 std::string name;
 Index r;
 Index m;
 std::vector< double > C;
 std::vector< double > P;
 std::vector< double > W;
 Block::Subset K;
 };

/*--------------------------------------------------------------------------*/

static bool fail( const std::string & name , const std::string & what )
{
 std::cout << name << ": " << what << std::endl;
 return( false );
 }

/*--------------------------------------------------------------------------*/
/// loads ins into a new MultiKnapsackAssignBlock, from memory

static MultiKnapsackAssignBlock * load( const Instance & ins )
{
 auto b = new MultiKnapsackAssignBlock();
 auto C = ins.C;
 auto P = ins.P;
 auto W = ins.W;
 auto K = ins.K;
 b->load( P.size() , ins.r , ins.m , std::move( C ) , std::move( P ) ,
	  std::move( W ) , std::move( K ) );
 return( b );
 }

/*--------------------------------------------------------------------------*/
/// ins in the txt format of load( std::istream & ), with comments

static std::string to_txt( const Instance & ins )
{
 std::ostringstream s;
 s << "# " << ins.name << "\n" << ins.P.size() << " " << ins.r << " "
   << ins.m << "\n# capacities\n";
 for( auto c : ins.C )
  s << c << " ";
 s << "\n# index profit weight class\n";
 for( Index j = 0 ; j < ins.P.size() ; ++j )
  s << j << " " << ins.P[ j ] << " " << ins.W[ j ] << " " << ins.K[ j ]
    << "\n";
 return( s.str() );
 }

/*--------------------------------------------------------------------------*/
/// whether b holds exactly the data of ins, in itself and in its sub-Block

static bool same_data( MultiKnapsackAssignBlock * b , const Instance & ins ,
		       const std::string & what )
{
 const auto n = ins.P.size();
 if( ( b->get_NItems() != n ) || ( b->get_NClasses() != ins.r ) ||
     ( b->get_NKnapsacks() != ins.m ) )
  return( fail( ins.name , what + ": wrong sizes" ) );
 if( ( b->get_Capacities() != ins.C ) || ( b->get_Profits() != ins.P ) ||
     ( b->get_Weights() != ins.W ) || ( b->get_Classes() != ins.K ) )
  return( fail( ins.name , what + ": wrong data" ) );

 if( b->get_nested_Blocks().size() != ins.m * ins.r )
  return( fail( ins.name , what + ": wrong number of sub-Block" ) );

 for( Index i = 0 ; i < ins.m ; ++i )
  for( Index k = 0 ; k < ins.r ; ++k ) {
   auto bkb = b->get_knapsack( i , k );
   std::vector< double > W = { - ins.C[ i ] };
   std::vector< double > P = { 0 };
   for( Index j = 0 ; j < n ; ++j )
    if( ins.K[ j ] == k ) {
     W.push_back( ins.W[ j ] );
     P.push_back( ins.P[ j ] );
     }
   if( ( bkb->get_Weights() != W ) || ( bkb->get_Profits() != P ) ||
       ( bkb->get_Capacity() != 0 ) )
    return( fail( ins.name , what + ": wrong sub-Block ( " +
		  std::to_string( i ) + " , " + std::to_string( k ) + " )" ) );
   }

 return( true );
 }

/*--------------------------------------------------------------------------*/
/// generates the abstract representation of b with a FakeSolver attached,
/// and checks it

static bool check_abstract( MultiKnapsackAssignBlock * b ,
			    const Instance & ins )
{
 auto fake = new FakeSolver();
 b->register_Solver( fake );
 b->generate_abstract_constraints();  // generates the Variable first
 b->generate_objective();
 const auto nmods = fake->get_Modification_list().size();
 b->unregister_Solvers( true );

 if( nmods )
  return( fail( ins.name , std::to_string( nmods ) +
		" Modification issued while generating" ) );

 const auto n = ins.P.size();
 if( b->get_number_static_constraints() != 2 )
  return( fail( ins.name , "wrong number of groups of static Constraint" ) );

 auto assign = b->get_static_constraint_v< FRowConstraint >( "assign" );
 auto cls = b->get_static_constraint_v< FRowConstraint >( "class" );
 if( ( ! assign ) || ( ! cls ) || ( assign->size() != n ) ||
     ( cls->size() != ins.m ) )
  return( fail( ins.name , "wrong groups of static Constraint" ) );

 for( Index j = 0 ; j < n ; ++j ) {
  auto & c = ( *assign )[ j ];
  if( ( c.get_num_active_var() != ins.m ) || ( c.get_rhs() != 1 ) )
   return( fail( ins.name , "wrong assignment Constraint " +
		 std::to_string( j ) ) );
  // the i-th Variable is item j in the sub-Block of ( i , class of j )
  for( Index i = 0 ; i < ins.m ; ++i ) {
   auto bkb = b->get_knapsack( i , ins.K[ j ] );
   Index pos = 1;
   for( Index h = 0 ; h < j ; ++h )
    pos += ( ins.K[ h ] == ins.K[ j ] );
   if( c.get_active_var( i ) != bkb->get_Var( pos ) )
    return( fail( ins.name , "wrong Variable in assignment Constraint " +
		  std::to_string( j ) ) );
   }
  }

 for( Index i = 0 ; i < ins.m ; ++i ) {
  auto & c = ( *cls )[ i ];
  if( ( c.get_num_active_var() != ins.r ) || ( c.get_rhs() != 1 ) )
   return( fail( ins.name , "wrong class Constraint " +
		 std::to_string( i ) ) );
  for( Index k = 0 ; k < ins.r ; ++k )
   if( c.get_active_var( k ) != b->get_knapsack( i , k )->get_Var( 0 ) )
    return( fail( ins.name , "wrong Variable in class Constraint " +
		  std::to_string( i ) ) );
  }

 // no solution is there yet: every x and y reads 0
 for( Index i = 0 ; i < ins.m ; ++i ) {
  for( Index j = 0 ; j < n ; ++j )
   if( b->get_x( i , j ) != 0 )
    return( fail( ins.name , "nonzero x without a solution" ) );
  for( Index k = 0 ; k < ins.r ; ++k )
   if( b->get_y( i , k ) != 0 )
    return( fail( ins.name , "nonzero y without a solution" ) );
  }

 return( true );
 }

/*--------------------------------------------------------------------------*/
/// all the checks on ins

static bool check( const Instance & ins )
{
 try {
  auto b = Block::new_Block( "MultiKnapsackAssignBlock" );
  if( ! dynamic_cast< MultiKnapsackAssignBlock * >( b ) ) {
   delete b;
   return( fail( ins.name , "not given by the Block factory" ) );
   }
  delete b;

  std::unique_ptr< MultiKnapsackAssignBlock > mb( load( ins ) );
  if( ! ( same_data( mb.get() , ins , "load()" ) &&
	  check_abstract( mb.get() , ins ) ) )
   return( false );

  // a second load() of the same data, after the abstract representation
  // has been generated: the old one is discarded and generated again
  auto C = ins.C;
  auto P = ins.P;
  auto W = ins.W;
  auto K = ins.K;
  mb->load( P.size() , ins.r , ins.m , std::move( C ) , std::move( P ) ,
	    std::move( W ) , std::move( K ) );
  if( ! ( same_data( mb.get() , ins , "second load()" ) &&
	  check_abstract( mb.get() , ins ) ) )
   return( false );

  // load() from a stream, with comments
  std::istringstream txt( to_txt( ins ) );
  std::unique_ptr< MultiKnapsackAssignBlock > tb(
					    new MultiKnapsackAssignBlock() );
  tb->load( txt );
  if( ! ( same_data( tb.get() , ins , "load( istream )" ) &&
	  check_abstract( tb.get() , ins ) ) )
   return( false );

  // the netCDF round trip
  const auto file = std::filesystem::temp_directory_path() /
                    ( "MultiKnapsackAssignBlock_unit_test_" + ins.name +
		      ".nc4" );
  {
   netCDF::NcFile f( file.string() , netCDF::NcFile::replace );
   mb->serialize( f );
   }
  std::unique_ptr< MultiKnapsackAssignBlock > nb(
					    new MultiKnapsackAssignBlock() );
  {
   netCDF::NcFile f( file.string() , netCDF::NcFile::read );
   nb->deserialize( f );
   }
  std::filesystem::remove( file );
  if( ! ( same_data( nb.get() , ins , "netCDF round trip" ) &&
	  check_abstract( nb.get() , ins ) ) )
   return( false );
  }
 catch( const std::exception & e ) {
  return( fail( ins.name , e.what() ) );
  }

 std::cout << ins.name << ": OK" << std::endl;
 return( true );
 }

/*--------------------------------------------------------------------------*/
/// whether loading ins is refused

static bool refused( const Instance & ins )
{
 try {
  std::unique_ptr< MultiKnapsackAssignBlock > b( load( ins ) );
  }
 catch( const std::invalid_argument & ) {
  std::cout << ins.name << ": refused, OK" << std::endl;
  return( true );
  }
 return( fail( ins.name , "not refused" ) );
 }

/*--------------------------------------------------------------------------*/
/*-------------------------------- main() ----------------------------------*/
/*--------------------------------------------------------------------------*/

int main( int argc , char ** argv )
{
 const std::vector< Instance > good = {
  { "generic" , 2 , 3 , { 10 , 7 , 12 } ,
    { 6 , 5 , 8 , 3 , 9 , 4 , 7 } , { 4 , 3 , 6 , 2 , 7 , 3 , 5 } ,
    { 0 , 1 , 0 , 1 , 0 , 1 , 1 } } ,
  { "one-knapsack" , 2 , 1 , { 9 } ,
    { 5 , 4 , 6 , 2 } , { 4 , 3 , 5 , 1 } , { 0 , 1 , 1 , 0 } } ,
  { "one-class" , 1 , 3 , { 5 , 6 , 4 } ,
    { 3 , 4 , 5 , 2 , 6 } , { 2 , 3 , 4 , 1 , 5 } , { 0 , 0 , 0 , 0 , 0 } } ,
  { "empty-class" , 3 , 2 , { 8 , 6 } ,
    { 4 , 6 , 5 , 3 } , { 3 , 5 , 4 , 2 } , { 0 , 2 , 0 , 2 } } ,
  { "zero-capacities" , 2 , 2 , { 0 , 0 } ,
    { 4 , 6 , 5 } , { 3 , 5 , 4 } , { 1 , 0 , 1 } } ,
  { "no-items" , 2 , 2 , { 5 , 3 } , {} , {} , {} } };

 const std::vector< Instance > bad = {
  { "invalid-class" , 2 , 2 , { 5 , 3 } , { 4 , 6 } , { 3 , 5 } ,
    { 0 , 2 } } ,
  { "wrong-capacities" , 2 , 3 , { 5 , 3 } , { 4 , 6 } , { 3 , 5 } ,
    { 0 , 1 } } };

 bool ok = true;
 for( const auto & ins : good )
  ok = check( ins ) && ok;
 for( const auto & ins : bad )
  ok = refused( ins ) && ok;

 std::cout << ( ok ? "MultiKnapsackAssignBlock: all tests passed"
		   : "MultiKnapsackAssignBlock: some tests failed" )
	   << std::endl;

 return( ok ? 0 : 1 );
 }

/*--------------------------------------------------------------------------*/
/*---------------------------- End File test.cpp ---------------------------*/
/*--------------------------------------------------------------------------*/
