/*--------------------------------------------------------------------------*/
/*------------------- File MultiKnapsackAssignBlock.cpp --------------------*/
/*--------------------------------------------------------------------------*/
/** @file
 * Implementation of the MultiKnapsackAssignBlock class.
 *
 * \author Antonio Frangioni \n
 *         Dipartimento di Informatica \n
 *         Universita' di Pisa \n
 *
 * \author Federica Di Pasquale \n
 *         Dipartimento di Informatica \n
 *         Universita' di Pisa \n
 *
 * \author Donato Meoli \n
 *         Dipartimento di Informatica \n
 *         Universita' di Pisa \n
 *
 * \copyright &copy; by Antonio Frangioni, Federica Di Pasquale,
 *            Donato Meoli
 */
/*--------------------------------------------------------------------------*/
/*----------------------------- IMPLEMENTATION -----------------------------*/
/*--------------------------------------------------------------------------*/
/*------------------------------ INCLUDES ----------------------------------*/
/*--------------------------------------------------------------------------*/

#include "MultiKnapsackAssignBlock.h"

#include "LinearFunction.h"

/*--------------------------------------------------------------------------*/
/*------------------------- NAMESPACE AND USING ----------------------------*/
/*--------------------------------------------------------------------------*/

using namespace SMSpp_di_unipi_it;

/*--------------------------------------------------------------------------*/
/*----------------------------- STATIC MEMBERS -----------------------------*/
/*--------------------------------------------------------------------------*/

// register MultiKnapsackAssignBlock in the Block factory

SMSpp_insert_in_factory_cpp_1( MultiKnapsackAssignBlock );

/*--------------------------------------------------------------------------*/
/*----------------- METHODS OF MultiKnapsackAssignBlock --------------------*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::load( Index n , Index r , Index m ,
				     std::vector< double > && Capacities ,
				     std::vector< double > && Profits ,
				     std::vector< double > && Weights ,
				     Subset && Classes )
{
 static const std::string _prfx = "MultiKnapsackAssignBlock::load: ";

 if( Capacities.size() != m )
  throw( std::invalid_argument( _prfx + "Capacities of the wrong size" ) );
 if( Profits.size() != n )
  throw( std::invalid_argument( _prfx + "Profits of the wrong size" ) );
 if( Weights.size() != n )
  throw( std::invalid_argument( _prfx + "Weights of the wrong size" ) );
 if( Classes.size() != n )
  throw( std::invalid_argument( _prfx + "Classes of the wrong size" ) );

 guts_of_destructor();  // discard the previous instance, if any

 f_N = n;
 f_R = r;
 f_M = m;
 v_C = std::move( Capacities );
 v_P = std::move( Profits );
 v_W = std::move( Weights );
 v_K = std::move( Classes );

 build( _prfx );

 if( anyone_there() )
  add_Modification( std::make_shared< NBModification >( this ) );

 }  // end( MultiKnapsackAssignBlock::load( memory ) )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::load( std::istream & input , char frmt )
{
 static const std::string _prfx = "MultiKnapsackAssignBlock::load: ";

 guts_of_destructor();  // discard the previous instance, if any

 if( ! ( input >> eatcomments >> f_N ) )
  throw( std::invalid_argument( _prfx +
				"error reading the number of items" ) );
 if( ! ( input >> eatcomments >> f_R ) )
  throw( std::invalid_argument( _prfx +
				"error reading the number of classes" ) );
 if( ! ( input >> eatcomments >> f_M ) )
  throw( std::invalid_argument( _prfx +
				"error reading the number of knapsacks" ) );

 v_C.resize( f_M );
 for( auto & c : v_C )
  if( ! ( input >> eatcomments >> c ) )
   throw( std::invalid_argument( _prfx + "error reading the capacities" ) );

 v_P.resize( f_N );
 v_W.resize( f_N );
 v_K.resize( f_N );
 for( Index j = 0 ; j < f_N ; ++j ) {
  Index item;  // the index of the item, which is ignored
  if( ! ( input >> eatcomments >> item >> v_P[ j ] >> v_W[ j ] >> v_K[ j ] ) )
   throw( std::invalid_argument( _prfx + "error reading item " +
				 std::to_string( j ) ) );
  }

 build( _prfx );

 if( anyone_there() )
  add_Modification( std::make_shared< NBModification >( this ) );

 }  // end( MultiKnapsackAssignBlock::load( istream ) )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::deserialize( const netCDF::NcGroup & group )
{
 static const std::string _prfx = "MultiKnapsackAssignBlock::deserialize: ";

 guts_of_destructor();  // discard the previous instance, if any

 const auto get_dim = [ & ]( const char * name ) -> Index {
  auto d = group.getDim( name );
  if( d.isNull() )
   throw( std::invalid_argument( _prfx + name + " dimension is required" ) );
  return( d.getSize() );
  };

 f_N = get_dim( "NItems" );
 f_R = get_dim( "NClasses" );
 f_M = get_dim( "NKnapsacks" );

 const auto get_var = [ & ]( const char * name , auto & v , Index size ) {
  v.resize( size );
  auto nv = group.getVar( name );
  if( nv.isNull() )
   throw( std::invalid_argument( _prfx + name + " are required" ) );
  if( size )
   nv.getVar( v.data() );
  };

 get_var( "Capacities" , v_C , f_M );
 get_var( "Profits" , v_P , f_N );
 get_var( "Weights" , v_W , f_N );
 get_var( "Classes" , v_K , f_N );

 build( _prfx );

 Block::deserialize( group );  // this issues the NBModification

 }  // end( MultiKnapsackAssignBlock::deserialize )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::generate_abstract_variables(
						      Configuration * stvv )
{
 if( AR & HasVar )  // the Variable are there already
  return;           // nothing to do

 Block::generate_abstract_variables( stvv );  // these of the sub-Block

 AR |= HasVar;

 }  // end( MultiKnapsackAssignBlock::generate_abstract_variables )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::generate_abstract_constraints(
						      Configuration * stcc )
{
 if( AR & HasCns )  // the Constraint are there already
  return;           // nothing to do

 // the linking Constraint use the Variable of the sub-Block
 generate_abstract_variables();

 // the class Constraint: sum_k y_{ik} <= 1 for each knapsack i - - - - - - -

 v_class.resize( f_M );
 for( Index i = 0 ; i < f_M ; ++i ) {
  LinearFunction::v_coeff_pair coeffs( f_R );
  for( Index k = 0 ; k < f_R ; ++k )
   coeffs[ k ] = { get_knapsack( i , k )->get_Var( 0 ) , 1 };

  v_class[ i ].set_lhs( - Inf< RowConstraint::RHSValue >() , eNoMod );
  v_class[ i ].set_rhs( 1 , eNoMod );
  v_class[ i ].set_function( new LinearFunction( std::move( coeffs ) ) ,
			     eNoMod );
  }

 // the assignment Constraint: sum_i x_{ij} <= 1 for each item j - - - - - -

 v_assign.resize( f_N );
 for( Index j = 0 ; j < f_N ; ++j ) {
  LinearFunction::v_coeff_pair coeffs( f_M );
  for( Index i = 0 ; i < f_M ; ++i )
   coeffs[ i ] = { get_knapsack( i , v_K[ j ] )->get_Var( v_pos[ j ] + 1 ) ,
		   1 };

  v_assign[ j ].set_lhs( - Inf< RowConstraint::RHSValue >() , eNoMod );
  v_assign[ j ].set_rhs( 1 , eNoMod );
  v_assign[ j ].set_function( new LinearFunction( std::move( coeffs ) ) ,
			      eNoMod );
  }

 add_static_constraint( v_assign , "assign" );
 add_static_constraint( v_class , "class" );

 Block::generate_abstract_constraints( stcc );  // these of the sub-Block

 AR |= HasCns;

 }  // end( MultiKnapsackAssignBlock::generate_abstract_constraints )

/*--------------------------------------------------------------------------*/
/*------- Methods for reading the data of the MultiKnapsackAssignBlock -----*/
/*--------------------------------------------------------------------------*/

BinaryKnapsackBlock * MultiKnapsackAssignBlock::get_knapsack( Index i ,
							       Index k ) const
{
 if( ( i >= f_M ) || ( k >= f_R ) )
  throw( std::invalid_argument(
	       "MultiKnapsackAssignBlock::get_knapsack: invalid knapsack or "
	       "class" ) );

 return( static_cast< BinaryKnapsackBlock * >( v_Block[ i * f_R + k ] ) );

 }  // end( MultiKnapsackAssignBlock::get_knapsack )

/*--------------------------------------------------------------------------*/
/*----------------------- Methods for handling Solution --------------------*/
/*--------------------------------------------------------------------------*/

double MultiKnapsackAssignBlock::get_x( Index i , Index j ) const
{
 if( j >= f_N )
  throw( std::invalid_argument(
			  "MultiKnapsackAssignBlock::get_x: invalid item" ) );

 return( get_knapsack( i , v_K[ j ] )->get_x( v_pos[ j ] + 1 ) );
 }

/*--------------------------------------------------------------------------*/

double MultiKnapsackAssignBlock::get_y( Index i , Index k ) const
{
 return( get_knapsack( i , k )->get_x( 0 ) );
 }

/*--------------------------------------------------------------------------*/
/*--- METHODS FOR LOADING, PRINTING & SAVING THE MultiKnapsackAssignBlock --*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::serialize( netCDF::NcGroup & group ) const
{
 Block::serialize( group );

 auto n = group.addDim( "NItems" , f_N );
 auto r = group.addDim( "NClasses" , f_R );
 auto m = group.addDim( "NKnapsacks" , f_M );

 auto c = group.addVar( "Capacities" , netCDF::NcDouble() , m );
 auto p = group.addVar( "Profits" , netCDF::NcDouble() , n );
 auto w = group.addVar( "Weights" , netCDF::NcDouble() , n );
 auto k = group.addVar( "Classes" , netCDF::NcUint() , n );

 if( f_M )
  c.putVar( v_C.data() );
 if( f_N ) {
  p.putVar( v_P.data() );
  w.putVar( v_W.data() );
  k.putVar( v_K.data() );
  }

 }  // end( MultiKnapsackAssignBlock::serialize )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::print( std::ostream & output ,
				      char vlvl ) const
{
 output << "MultiKnapsackAssignBlock with " << f_N << " items, " << f_R
	<< " classes and " << f_M << " knapsacks" << std::endl;
 if( ! vlvl )
  return;

 output << "capacities:";
 for( auto c : v_C )
  output << " " << c;
 output << std::endl << "item\tprofit\tweight\tclass" << std::endl;
 for( Index j = 0 ; j < f_N ; ++j )
  output << j << "\t" << v_P[ j ] << "\t" << v_W[ j ] << "\t" << v_K[ j ]
	 << std::endl;

 }  // end( MultiKnapsackAssignBlock::print )

/*--------------------------------------------------------------------------*/
/*-------------------------- PRIVATE METHODS -------------------------------*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::guts_of_destructor( void )
{
 // the Function of the linking Constraint are emptied before the
 // sub-Block, whose Variable they use, are deleted
 Constraint::clear( v_assign );
 Constraint::clear( v_class );
 reset_static_constraints();

 for( auto b : v_Block )
  delete b;
 v_Block.clear();

 v_Sk.clear();
 v_pos.clear();
 AR = 0;

 }  // end( MultiKnapsackAssignBlock::guts_of_destructor )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::build( const std::string & prfx )
{
 v_Sk.assign( f_R , Subset() );
 v_pos.resize( f_N );
 for( Index j = 0 ; j < f_N ; ++j ) {
  if( v_K[ j ] >= f_R )
   throw( std::invalid_argument( prfx + "invalid class of item " +
				 std::to_string( j ) ) );
  v_pos[ j ] = v_Sk[ v_K[ j ] ].size();
  v_Sk[ v_K[ j ] ].push_back( j );
  }

 // one BinaryKnapsackBlock for each pair ( knapsack i , class k ), whose
 // first item is y_{ik}, with profit 0 and weight - C_i, and the other ones
 // are the items of class k, the capacity being 0
 v_Block.reserve( f_M * f_R );
 for( Index i = 0 ; i < f_M ; ++i )
  for( Index k = 0 ; k < f_R ; ++k ) {
   const auto & items = v_Sk[ k ];
   std::vector< double > W( items.size() + 1 );
   std::vector< double > P( items.size() + 1 );
   W[ 0 ] = - v_C[ i ];
   P[ 0 ] = 0;
   for( Index h = 0 ; h < items.size() ; ++h ) {
    W[ h + 1 ] = v_W[ items[ h ] ];
    P[ h + 1 ] = v_P[ items[ h ] ];
    }

   auto bkb = new BinaryKnapsackBlock( this );
   bkb->load( items.size() + 1 , 0 , std::move( W ) , std::move( P ) );
   v_Block.push_back( bkb );
   }

 }  // end( MultiKnapsackAssignBlock::build )

/*--------------------------------------------------------------------------*/
/*----------------- End File MultiKnapsackAssignBlock.cpp ------------------*/
/*--------------------------------------------------------------------------*/
