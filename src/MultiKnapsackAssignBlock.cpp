/*--------------------------------------------------------------------------*/
/*-------------------- File MultiKnapsackAssignBlock.cpp -------------------*/
/*--------------------------------------------------------------------------*/

#include "MultiKnapsackAssignBlock.h"

/*--------------------------------------------------------------------------*/
/*--------------------------- NAMESPACE AND USING --------------------------*/
/*--------------------------------------------------------------------------*/

using namespace SMSpp_di_unipi_it;

/*--------------------------------------------------------------------------*/
/*---------------------------------- TYPES ---------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*-------------------------------- FUNCTIONS -------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*----------------------------- STATIC MEMBERS -----------------------------*/
/*--------------------------------------------------------------------------*/

// register MultiKnapsackAssignBlock to the Block factory

SMSpp_insert_in_factory_cpp_1( MultiKnapsackAssignBlock );

/*--------------------------------------------------------------------------*/
/*----------------- METHODS OF MultiKnapsackAssignBlock --------------------*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::load( Index n , Index r , Index m , 
                                     std::vector< double > && Capacities , 
                                     std::vector< double > && Profits , 
                                     std::vector< double > && Weights , 
                                     Subset && Classes ){

 // sanity checks

 if( Capacities.size() != m )
  throw( std::invalid_argument( "Vector of Capacities of the wrong size!" ) );
 
 if( Profits.size() != n )  
  throw( std::invalid_argument( "Vector of Profits of the wrong size!" ) );

 if( Weights.size() != n )  
  throw( std::invalid_argument( "Vector of Weights of the wrong size!" ) );

 if( Classes.size() != n )  
  throw( std::invalid_argument( "Vector of Classes of the wrong size!" ) );

 if( f_N )                      // clear previous instances (if any)
  guts_of_destructor();
 
 f_N = n;
 f_R = r;
 f_M = m;

 v_C = std::move( Capacities );
 v_P = std::move( Profits );
 v_W = std::move( Weights );
 v_K = std::move( Classes );

 v_Sk.resize( f_R );

 for( Index i = 0 ; i < f_N ; i++ ){
  
  Index k = v_K[ i ];       // class of the item i 

  if( k >= f_R )
   throw( std::invalid_argument( "Invalid class for item " + 
                                 std::to_string( i ) ) );  

  v_Sk[ k ].push_back( i );
 
 }

 // create and load sub-Blocks (BinaryKnapsackBlock)
 create_SubBlocks();

 // Modification - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  

 if( anyone_there() )
  add_Modification( std::make_shared< NBModification >( this ) ); 


} // end( MultiKnapsackAssignBlock::load( memory ) )

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::deserialize( const netCDF::NcGroup & group ){

 if( f_N )                      // clear previous instances (if any)
  guts_of_destructor();

 // read problem data

 // get dimensions: f_N, f_R and f_M - - - - - - - - - - - - - - - - - - - -

 netCDF::NcDim n = group.getDim( "NItems" );
 if( n.isNull() )
  throw( std::logic_error( "NItems dimension is required" ) );
 f_N = n.getSize();

 netCDF::NcDim r = group.getDim( "NClasses" );
 if( r.isNull() )
  throw( std::logic_error( "NClasses dimension is required" ) );
 f_R = r.getSize();

 netCDF::NcDim m = group.getDim( "NKnapsacks" );
 if( m.isNull() )
  throw( std::logic_error( "NKnapsacks dimension is required" ) );
 f_M = m.getSize();

 // get capacities, weights, profits and classes - - - - - - - - - - - - - - 

 netCDF::NcVar c = group.getVar( "Capacities" );
 if( c.isNull() )
  throw( std::logic_error( "Capacities are required" ) ); 

 v_C.resize( f_M );
 c.getVar( v_C.data() );


 netCDF::NcVar p = group.getVar( "Profits" );
 if( p.isNull() )
  throw( std::logic_error( "Profits are required" ) ); 

 v_P.resize( f_N );
 p.getVar( v_P.data() );


 netCDF::NcVar w = group.getVar( "Weights" );
 if( w.isNull() )
  throw( std::logic_error( "Weights are required" ) ); 

 v_W.resize( f_N );
 w.getVar( v_W.data() );


 netCDF::NcVar k = group.getVar( "Classes" );
 if( k.isNull() )
  throw( std::logic_error( "Classes are required" ) ); 

 v_K.resize( f_N );
 k.getVar( v_K.data() );

 v_Sk.resize( f_R );

 for( Index i = 0 ; i < f_N ; i++ ){
  
  Index k = v_K[ i ];       // class of the item i 

  if( k >= f_R )
   throw( std::logic_error( "Invalid class for item " + 
                            std::to_string( i ) ) );  

  v_Sk[ k ].push_back( i );
 
 }

 // create and load sub-Blocks (BinaryKnapsackBlock)
 create_SubBlocks();

 // call the method of Block
 // inside this the NBModification, the "nuclear option",  is issued
 Block::deserialize( group );

} // end( MultiKnapsackAssignBlock::deserialize )


/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::generate_abstract_constraints( 
    Configuration * stcc ){

 if( AR & HasCns )   // nothing to do
  return;

 // generate v_cnstY - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

 v_cnstY.resize( f_M );

 for( Index m = 0 ; m < f_M ; m++ ){    // for each knapsack
  
  LinearFunction::v_coeff_pair y( f_R );

  for( Index k = 0 ; k < f_R ; k++ ){   // for each class
   auto bkb = static_cast< BinaryKnapsackBlock * >( v_Block[ m * f_R + k ] );
   y[ k ].first = bkb->get_Var( 0 ); 
   y[ k ].second = 1;
  }

  v_cnstY[ m ].set_function( new LinearFunction( std::move( y ) , 0 ) );
  v_cnstY[ m ].set_rhs( 1 );
  v_cnstY[ m ].set_lhs( - Inf<double>() );
 
  add_static_constraint( v_cnstY[ m ] );

 }

 // generate v_cnstX - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 v_cnstX.resize( f_N );

 for( Index i = 0 ; i < f_N ; i++ ){    // for each item
  
  LinearFunction::v_coeff_pair x( f_M );

  for( Index m = 0 ; m < f_M ; m++ ){   // for each knapsack
   
   Index k = v_K[ i ];                  // class of item i
   
   auto bkb = static_cast< BinaryKnapsackBlock * >( v_Block[ m * f_R + k ] );
   
   Index s = 0;                         // index of the variable in the
   while( v_Sk[ k ][ s ] != i )         // sub-Block 
    s++;

   x[ m ].first = bkb->get_Var( s + 1 ); 
   x[ m ].second = 1;
  
  } // end( for each knapsack )

  v_cnstX[ i ].set_function( new LinearFunction( std::move( x ) , 0 ) );
  v_cnstX[ i ].set_rhs( 1 );
  v_cnstX[ i ].set_lhs( - Inf<double>() );
 
  add_static_constraint( v_cnstX[ i ] );
    
 } // end( for each item )


 // call the base class method
 Block::generate_abstract_constraints();  

 AR |= HasCns;

}

/*--------------------------------------------------------------------------*/
/*--------------------- Methods for checking the Block ---------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*----------------------- Methods for handling Solution --------------------*/
/*--------------------------------------------------------------------------*/

bool MultiKnapsackAssignBlock::get_x( Index i , Index j ){

 if( j >= f_N )
  throw( std::invalid_argument( "invalid item" ) );

 if( i >= f_M )
  throw( std::invalid_argument( "invalid knapsack" ) );

 Index k = v_K[ j ];   // get the class of item j

 auto bkb = static_cast< BinaryKnapsackBlock * >( v_Block[ i * f_R + k ] );
 
 Index s = 0;                         // index of the variable in the
 while( v_Sk[ k ][ s ] != j )         // sub-Block 
  s++;

 return bkb->get_x( s + 1 );

}

/*--------------------------------------------------------------------------*/

bool MultiKnapsackAssignBlock::get_y( Index i , Index k ){

 if( k >= f_R )
  throw( std::invalid_argument( "invalid class" ) );

 if( i >= f_M )
  throw( std::invalid_argument( "invalid knapsack" ) );

 auto bkb = static_cast< BinaryKnapsackBlock * >( v_Block[ i * f_R + k ] );

 return bkb->get_x( 0 );

}



/*--------------------------------------------------------------------------*/
/*--- METHODS FOR LOADING, PRINTING & SAVING THE MultiKnapsackAssignBlock --*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::serialize( netCDF::NcGroup & group ) const {

 // call the method of Block
 Block::serialize( group ); 

 // MultiKnapsackAssignBlock data

 netCDF::NcDim n = group.addDim( "NItems" , f_N );

 netCDF::NcDim r = group.addDim( "NClasses" , f_R );

 netCDF::NcDim m = group.addDim( "NKnapsacks" , f_M );

 ( group.addVar( "Capacities" , netCDF::NcDouble() , m ) ).putVar( v_C.data() ); 

 ( group.addVar( "Profits" , netCDF::NcDouble() , n ) ).putVar( v_P.data() );
 
 ( group.addVar( "Weights" , netCDF::NcDouble() , n ) ).putVar( v_W.data() );

 ( group.addVar( "Classes" , netCDF::NcDouble() , n ) ).putVar( v_K.data() );

 
}// end( MultiKnapsackAssignBlock::serialize )

/*--------------------------------------------------------------------------*/
/*------------- METHODS FOR ADDING / REMOVING / CHANGING DATA --------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*------------ METHODS FOR LOADING, PRINTING & SAVING THE Block ------------*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::print( std::ostream & output , 
                                      char vlvl ) const {

 output << "MultiKnapsackAssignBlock" << std::endl;

 output << "Number of items: " << f_N << std::endl;

 output << "Number of classes: " << f_R << std::endl;

 output << "Number of knapsacks: " << f_M << std::endl;

 output << "Capacities of the knapsacks:" << std::endl;
 for( auto c : v_C )
  output << c << " ";
 output << std::endl;

 output << "Item\tProfit\tWeight\tClass" << std::endl;

 for( Index i = 0 ; i < f_N ; i++ ){
  output << i << "\t" << v_P[ i ] << "\t" << v_W[ i ] << "\t" 
       << v_K[ i ] << std::endl;  
 }

 output << std::endl; 

}

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::load( std::istream & input , char frmt ){

 if( f_N )              // erase previous instance, if any
  guts_of_destructor(); 


 // read problem data
 if( !( input >> f_N ) )
  throw( std::invalid_argument( "error reading number of items" ) );

 if( !( input >> f_R ) )
  throw( std::invalid_argument( "error reading number of classes" ) );

 if( !( input >> f_M ) )
  throw( std::invalid_argument( "error reading number of knapsacks" ) );

 v_C.resize( f_M );

 for( Index i = 0 ; i < f_M ; i++ ){
  if( !( input >> v_C[ i ] ) )
   throw( std::invalid_argument( "error reading Capacities" ) );
 }

 v_P.resize( f_N );
 v_W.resize( f_N );
 v_K.resize( f_N );
 v_Sk.resize( f_R );

 for( Index i = 0 ; i < f_N ; i++ ){
  
  Index item;         // skip redundant index of the item

  if( !( input >> item ) )
   throw( std::invalid_argument( "error reading item" ) );

  if( !( input >> v_P[ i ] ) )
   throw( std::invalid_argument( "error reading Profits" ) );

  if( !( input >> v_W[ i ] ) )
   throw( std::invalid_argument( "error reading Weights" ) );

  if( !( input >> v_K[ i ] ) )
   throw( std::invalid_argument( "error reading Classes" ) );

  if( v_K[ i ] >= f_R )
   throw( std::invalid_argument( "Invalid class for item " + 
                    std::to_string( i ) ) ); 

  v_Sk[ v_K[ i ] ].push_back( i );
 
 }

 // create and load sub-Blocks (BinaryKnapsackBlock)
 create_SubBlocks();

 // Modification - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

 if( anyone_there() )
  add_Modification( std::make_shared< NBModification >( this ) ); 

}

/*--------------------------------------------------------------------------*/
/*-------------------------- PROTECTED METHODS -----------------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*-------------------------- PRIVATE METHODS -------------------------------*/
/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::guts_of_destructor(){
 
 // clear the constraint
 for( auto & c : v_cnstX )
  c.clear();
 v_cnstX.clear();

 for( auto & c : v_cnstY )
  c.clear();
 v_cnstY.clear();
 
 // delete sub-Blocks
 Index n_Blocks = v_Block.size();
 
 for( Index i = 0 ; i < n_Blocks ; i++ )
  delete v_Block[ i ];

 v_Block.clear();

 // explicitly reset Constraint
 reset_static_constraints();

 AR = 0;

 for( auto & s : v_Sk )
  s.clear();
 v_Sk.clear(); 

}

/*--------------------------------------------------------------------------*/

void MultiKnapsackAssignBlock::create_SubBlocks(){

// create M x R sub-Blocks (BinaryKnapsackBlock).
// For each class create M BinaryKnapsackBlock containing all the items of the
// class + an "item" corresponding to the Y[ i , k ] variable that appears in 
// constraints (2). This "item" has 0 as profit and -C[ i ] as weight, and it
// will be the first item of each sub-Block.

 if( ! v_Block.empty() ){               // delete previous sub-Blocks (if any)
  
  Index n_Blocks = v_Block.size();
 
  for( Index i = 0 ; i < n_Blocks ; i++ )
   delete v_Block[ i ];

  v_Block.clear();
 }

// create and load sub-Blocks - - - - - - - - - - - - - - - - - - - - - - - - -

 for( Index m = 0 ; m < f_M ; m++ ){  // for each knapsack
  
  for( Index k = 0 ; k < f_R ; k++ ){   // for each class

   auto bkb = new BinaryKnapsackBlock( this );
    
   std::vector< double > W = { -v_C[ m ] }; // Initialize profits and weights
   std::vector< double > P = { 0 };         // with the added "item"
                                        
   Index n = v_Sk[ k ].size() + 1;          // number of items of the sub-block

   for( auto i : v_Sk[ k ] ){
    W.push_back( v_W[ i ] );
    P.push_back( v_P[ i ] );
   }  

   bkb->load( n , 0 , std::move( W ) , std::move( P ) );

   v_Block.push_back( bkb );

  } // end( for each class )

 } // end( for each knapsack )
 
} // end( create_SubBlocks() )


/*--------------------------------------------------------------------------*/
/*----------------- End File MultiKnapsackAssignBlock.cpp ------------------*/
/*--------------------------------------------------------------------------*/