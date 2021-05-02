/*--------------------------------------------------------------------------*/
/*-------------------- File MultiKnapsackAssignBlock.h ---------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*---------------------------- DEFINITIONS ---------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef __MultiKnapsackAssignBlock
 #define __MultiKnapsackAssignBlock 
                      /* self-identification: #endif at the end of the file */

/*--------------------------------------------------------------------------*/
/*------------------------------ INCLUDES ----------------------------------*/
/*--------------------------------------------------------------------------*/

#include "BinaryKnapsackBlock.h"

/*--------------------------------------------------------------------------*/
/*------------------------------ NAMESPACE ---------------------------------*/
/*--------------------------------------------------------------------------*/

/// namespace for the Structured Modeling System++ (SMS++)
namespace SMSpp_di_unipi_it
{

/*--------------------------------------------------------------------------*/
/*------------------------------- CLASSES ----------------------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*--------------------- CLASS MultiKnapsackAssignBlock ---------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------- GENERAL NOTES --------------------------------*/
/*--------------------------------------------------------------------------*/
/// implementation of the Block concept for the Multiple Knapsack Assignment 
/// problem
/** The MultiKnapsackAssignBlock class implements the Block concept 
* [see Block.h] for the Multiple Knapsack Assignment Problem (MKAP).
*
* The MKAP is defined on a set of N items and M knapsacks, where the items 
* are partitioned into R classes. Each Knapsack has a Capacity and each item 
* has a weight and a profit, and the objective is to select M disjoint subsets
* of items to be assigned to the knapsacks so that the total profit is 
* maximized. Each Knapsack can contain only items of the same class, whose 
* total weight must be less or equal than its Capacity.  
*
* The data of the problem consist of:
*
*   - N: the number of items
*   - M: the number of knapsacks
*   - R: the number of classes
*   
*   - W: a vector of size N s.t. W[ j ] is the weight of item j
*   - P: a vecotr of size N s.t. P[ j ] is the profit of item j
*   - K: a vector of size N s.t. K[ j ] is the class of item j
*   - C: a vector of size M s.t. C[ i ] is the capacity of knapsack i
*
* A formulation of the problem is:
* \f[
* \max \sum_{ j = 1 }^{ N } P[ j ] \sum_{ i = 1 }^{ M } X[ i , j ]
* \f]
* \f[
* \sum_{ i = 1 }^{ M } X[ i , j ] \leq 1 \quad j = 1 , \dots , N    (1)
* \f]
* \f[
* \sum_{ j \in Sk } W[ j ] X[ i , j ] \leq C[ i ] Y[ i , k ] 
*       \quad i = 1 , \dots , M \quad k = 1, \dots , R              (2)
* \f]
* \f[
* \sum_{ k = 1 }^{ R } Y[ i , k ] \leq 1 \quad i = 1 , \dots , M    (3)
* \f]
* \f[
* X[ i , j ] \in \{ 0 , 1 \} \quad i = 0 , \dots , M 
*                            \quad j = 0 , \dots , N                (4)
* \f]
* \f[
* Y[ i , k ] \in \{ 0 , 1 \} \quad i = 0 , \dots , M 
*                            \quad k = 0 , \dots , R                (5)
* \f]
*
* MultiKnapsackAssignBlock is constructed by means of R x M sub-Blocks 
* (BinaryKnapsackBlock), one for each combination of (knapsack, class) and it
* only contains the linking constraints (1) and (3). The Y[ i , k ] variables
* are treated as additional "item" of each sub-Block, having 0 as profit and
* -C[ i ] as weight. */


class MultiKnapsackAssignBlock : public Block {

/*--------------------------------------------------------------------------*/
/*----------------------- PUBLIC PART OF THE CLASS -------------------------*/
/*--------------------------------------------------------------------------*/

public:

/*--------------------------------------------------------------------------*/
/*---------------------------- PUBLIC TYPES --------------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Public types
 @{ */


/**@} ----------------------------------------------------------------------*/
/*------------------------------- FRIENDS ----------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*--------------------- PUBLIC METHODS OF THE CLASS ------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------- CONSTRUCTOR AND DESTRUCTOR -------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Constructor and Destructor
 *  @{ */

 /// constructor of MultiKnapsackAssignBlock, taking a pointer to the father 
 /** Constructor of MultiKnapsackAssignBlock. It accepts a pointer to the 
  * father Block, which can be of any type, defaulting to nullpt so that this  
  * can also be used as the void constructor. */

explicit MultiKnapsackAssignBlock( Block * father = nullptr ) : Block( father ),
                                   f_N( 0 ) , f_M( 0 ) , f_R( 0 ) , AR( 0 ) {}

/*--------------------------------------------------------------------------*/
 /// destructor of MultiKnapsackAssignBlock

virtual ~MultiKnapsackAssignBlock(){ guts_of_destructor(); }

/**@} ----------------------------------------------------------------------*/
/*-------------------------- OTHER INITIALIZATIONS -------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Other initializations
 *  @{ */

 /// loads the Multiple Knapsack Assignment instance from memory
 /** Loads the Multiple Knapsack Assignment instance from memory. 
  * The parameters are:
  *
  * - n             is the number of items
  *
  * - r             is the number of classes
  *
  * - m             is the number of knapsacks
  *
  * - Capacities    vector of Capacities, which must have size equal to m
  *
  * - Profits       vector of Profits, which must have size equal to n
  *
  * - Weights       vector of Weights, which must have size equal to n
  *
  * - Classes       vector of Classes, which must have size equal to n
  *  
  * Like load( std::istream & ), if there is any Solver attached to this
  * MultiKnapsackAssignBlock then a NBModification (the "nuclear option") is 
  * issued. */

void load( Index n , Index r , Index m , std::vector< double > && Capacities , 
           std::vector< double > && Profits , std::vector< double > && Weights, 
           Subset && Classes );

/*--------------------------------------------------------------------------*/
 /// extends Block::deserialize( netCDF::NcGroup )
 /** Extends Block::deserialize( netCDF::NcGroup ) to the specific format of
  * a MultiKnapsackAssignBlock. Besides what is managed by serialize() method  
  * of the base Block class, the group should contains the following:
  *
  * - the dimension "NItems" containing the number of items
  *
  * - the dimension "NClasses" containing the number of classes
  *
  * - the dimension "NKnapsacks" containing the number of knapsacks
  *
  * - the variable "Capacities" of type double and indexed over the dimension
  *   "NKnapsacks"; the i-th entry of the variable is assumed to contain the
  *   Capacity of the i.th Knapsack
  *
  * - the variable "Profits" of type double and indexed over the dimension
  *   "NItems"; the i-th entry of the variable is assumed to contain the 
  *   profit of the i-th item
  *
  * - the variable "Weights" of type double and indexed over the dimension
  *   "NItems"; the i-th entry of the variable is assumed to contain the 
  *   weight of th i-th item
  *
  * - the variable "Classes" of type double and indexed over the dimension
  *   "NItems"; the i-th entry of the variable is assumed to contain the 
  *   class of th i-th item  
  *
  * All dimensions and variables are mandatory. */

void deserialize( const netCDF::NcGroup & group ) override;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /// generate the static constraints of the Multiple Knapsack Assignment.
 /** Method that generates the abstract constraints of the Multiple Knapsack
  * Assignment Problem  */

void generate_abstract_constraints( Configuration * stcc = nullptr )override;

/**@} ----------------------------------------------------------------------*/
/*------- Methods for reading the data of the MultiKnapsackAssignBlock -----*/
/*--------------------------------------------------------------------------*/
/** @name Methods for reading the data of the MultiKnapsackAssignBlock
 *  @{ */

/*--------------------------------------------------------------------------*/
 /// get the sense of the Objective

int get_objective_sense() const override final { 
 return( Objective::eMax );
}

/*--------------------------------------------------------------------------*/
/// get the number of items

Index get_NItems(){ return f_N; }

/*--------------------------------------------------------------------------*/
/// get the number of classes

Index get_NClasses(){ return f_R; }

/*--------------------------------------------------------------------------*/
/// get the number of knapsacks

Index get_NKnapsacks(){ return f_M; }

/*--------------------------------------------------------------------------*/
/// given an index m get the Capacity of the m-th Knapsack

double get_Capacity( Index m ){ 
 
 if( m >= f_M )
  throw( std::invalid_argument( "invalid knapsack index" ) );  

 return v_C[ m ]; 

}

/*--------------------------------------------------------------------------*/
/// get the vector of Capacities

const std::vector< double > & get_Capacities() const { return v_C; }

/*--------------------------------------------------------------------------*/
/// given an index i get the Weight of the i-th item

double get_Weight( Index i ){ 
 
 if( i >= f_N )
  throw( std::invalid_argument( "invalid item index" ) );  

 return v_W[ i ]; 

}

/*--------------------------------------------------------------------------*/
/// get the vector of Weights

const std::vector< double > & get_Weights() const { return v_W; }

/*--------------------------------------------------------------------------*/
/// given an index i get the Profit of the i-th item

double get_Profit( Index i ){ 

 if( i >= f_N )
  throw( std::invalid_argument( "invalid item index" ) );  
 
 return v_P[ i ]; 

}

/*--------------------------------------------------------------------------*/
/// get the vector of Profits

const std::vector< double > & get_Profits() const { return v_P; }

/*--------------------------------------------------------------------------*/
/// given an index i get the class of the i-th item

Index get_Class( Index i ){ 

 if( i >= f_N )
  throw( std::invalid_argument( "invalid item index" ) );  

 return v_K[ i ]; 

}

/**@} ----------------------------------------------------------------------*/
/*--------------------- Methods for checking the Block ---------------------*/
/*--------------------------------------------------------------------------*/
/** @name Methods for checking the Block
 *  @{ */



/**@} ----------------------------------------------------------------------*/
/*----------------------- Methods for handling Solution --------------------*/
/*--------------------------------------------------------------------------*/
/** @name Methods for handling Solution
 *  @{ */


/**@} ----------------------------------------------------------------------*/
/*-------------------- Methods for handling Modification -------------------*/
/*--------------------------------------------------------------------------*/
/** @name Methods for handling Modification
 *  @{ */

/**@} ----------------------------------------------------------------------*/
/*--- METHODS FOR LOADING, PRINTING & SAVING THE MultiKnapsackAssignBlock --*/
/*--------------------------------------------------------------------------*/
/** @name Methods for loading, printing & saving the MultiKnapsackAssignBlock
 *  @{ */

/// extends Block::serialize( netCDF::NcGroup )
/** Extends Block::serialize( netCDF::NcGroup ) to the specific format of a
 * MultiKnapsackAssignBlock. 
 * See MultiKnapsackAssignBlock::deserialize(netCDF::NcGroup) for details of 
 * the format of the created netCDF group. */

void serialize( netCDF::NcGroup & group ) const override;

/**@} ----------------------------------------------------------------------*/
/*------------- METHODS FOR ADDING / REMOVING / CHANGING DATA --------------*/
/*--------------------------------------------------------------------------*/
/** @name Changing the data of the Multiple Knapsack Assignment instance */


/**@} ----------------------------------------------------------------------*/
/*-------------------- PROTECTED PART OF THE CLASS -------------------------*/
/*--------------------------------------------------------------------------*/

protected:

/*--------------------------------------------------------------------------*/
/*-------------------------- PROTECTED METHODS -----------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Protected methods for inserting and extracting
 *  @{ */

 /// print the MultiKnapsackAssignBlock on an ostream
 /** Protected method to print information about the 
 * MultiKnapsackAssignBlock */ 

void print( std::ostream & output ) const override;

/*--------------------------------------------------------------------------*/
 /// load instance from txt file  
/** Protected method for loading a MultiKnapsackAssignBlock out of 
 * std::istream */      

void load( std::istream & input ) override;

/**@} ----------------------------------------------------------------------*/
/*--------------------------- PROTECTED FIELDS  ----------------------------*/
/*--------------------------------------------------------------------------*/

Index f_N;                      ///< the number of Items
Index f_R;                      ///< the number of Classes
Index f_M;                      ///< the number of Knapsacks

std::vector< double > v_C;      ///< vector of Capacities
std::vector< double > v_P;      ///< vector of Profits
std::vector< double > v_W;      ///< vector of Weights
Subset v_K;                     ///< vector of Classes     

std::vector< Subset > v_Sk;     // (redundant but helpful)
///< vector of class subsets, each entry contains the items of one class 

unsigned char AR;               ///< bit-wise coded: what abstract is there

static constexpr unsigned char HasCns = 4;
///< third bit of AR == 1 if the Constraints has been constructed

std::vector< FRowConstraint > v_cnstX;  
///< Each item is assigned to at most one Knapsack
std::vector< FRowConstraint > v_cnstY;  
///< Each knapsack contains items of at most one class

/*--------------------------------------------------------------------------*/
/*--------------------- PRIVATE PART OF THE CLASS --------------------------*/
/*--------------------------------------------------------------------------*/

private:

/*--------------------------------------------------------------------------*/
/*-------------------------- PRIVATE METHODS -------------------------------*/
/*--------------------------------------------------------------------------*/

void guts_of_destructor();

void create_SubBlocks();

/*--------------------------------------------------------------------------*/
/*---------------------------- PRIVATE FIELDS ------------------------------*/
/*--------------------------------------------------------------------------*/

SMSpp_insert_in_factory_h; 
// insert MultiKnapsackAssignBlock in the Block factory

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

}; // end( class( MultiKnapsackAssignBlock ) )


} // end( namespace SMSpp_di_unipi_it )

#endif /* MultiKnapsackAssignBlock.h included */

/*--------------------------------------------------------------------------*/
/*------------------ End File MultiKnapsackAssignBlock.h -------------------*/
/*--------------------------------------------------------------------------*/