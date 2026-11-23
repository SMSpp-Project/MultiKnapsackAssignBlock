/*--------------------------------------------------------------------------*/
/*-------------------- File MultiKnapsackAssignBlock.h ---------------------*/
/*--------------------------------------------------------------------------*/
/** @file
 * Header file for the class MultiKnapsackAssignBlock, which implements the
 * Block concept [see Block.h] for the Multiple Knapsack Assignment Problem,
 * as a set of BinaryKnapsackBlock linked by assignment constraints.
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
/*----------------------------- DEFINITIONS --------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef __MultiKnapsackAssignBlock
 #define __MultiKnapsackAssignBlock
                      /* self-identification: #endif at the end of the file */

/*--------------------------------------------------------------------------*/
/*------------------------------ INCLUDES ----------------------------------*/
/*--------------------------------------------------------------------------*/

#include "BinaryKnapsackBlock.h"

#include "FRowConstraint.h"

/*--------------------------------------------------------------------------*/
/*------------------------------ NAMESPACE ---------------------------------*/
/*--------------------------------------------------------------------------*/

/// namespace for the Structured Modeling System++ (SMS++)
namespace SMSpp_di_unipi_it
{
/*--------------------------------------------------------------------------*/
/*-------------------------------- CLASSES ---------------------------------*/
/*--------------------------------------------------------------------------*/
/** @defgroup MultiKnapsackAssignBlock_CLASSES Classes in \
 *            MultiKnapsackAssignBlock.h
 *  @{ */

/*--------------------------------------------------------------------------*/
/*--------------------- CLASS MultiKnapsackAssignBlock ---------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------- GENERAL NOTES --------------------------------*/
/*--------------------------------------------------------------------------*/
/// a Block for the Multiple Knapsack Assignment Problem
/** The MultiKnapsackAssignBlock class implements the Block concept [see
 * Block.h] for the Multiple Knapsack Assignment Problem (MKAP). It is defined
 * on a set of N items, partitioned into R classes, and on M knapsacks; each
 * item j has a weight W[ j ], a profit P[ j ] and a class K[ j ], and each
 * knapsack i has a capacity C[ i ]. Each knapsack is given (at most) one
 * class, and only holds items of that class whose total weight is at most
 * its capacity; each item goes in at most one knapsack, and the total profit
 * of the items in the knapsacks is maximized. Denoting by S_k the items of
 * class k, the problem is
 * \f[
 *  \max \Bigl\{ \, \sum_{ i = 1 }^M \sum_{ j = 1 }^N P_j x_{ij} \;:\;
 *  \sum_{ i = 1 }^M x_{ij} \leq 1 \;\; j = 1 , \ldots , N \,,\;
 *  \sum_{ k = 1 }^R y_{ik} \leq 1 \;\; i = 1 , \ldots , M \,,\;
 *  ( x , y ) \in X \, \Bigr\}
 * \f]
 * where \f$ X \f$ is the set of the binary \f$ ( x , y ) \f$ such that
 * \f[
 *  \sum_{ j \in S_k } W_j x_{ij} \leq C_i y_{ik}
 *  \quad i = 1 , \ldots , M \,,\; k = 1 , \ldots , R \;.
 * \f]
 *
 * The problem is represented by M * R sub-Block, one BinaryKnapsackBlock for
 * each pair ( i , k ) of a knapsack and a class, in the order ( 0 , 0 ),
 * ( 0 , 1 ), ..., ( 0 , R - 1 ), ( 1 , 0 ), ... (i.e., the sub-Block of the
 * pair ( i , k ) is the ( i * R + k )-th one). The first item of the
 * sub-Block of ( i , k ) is \f$ y_{ik} \f$, with profit 0 and weight
 * \f$ - C_i \f$, and the other ones are the items of class k, in increasing
 * order of their index, the capacity of the BinaryKnapsackBlock being 0.
 * Thus, the MultiKnapsackAssignBlock itself only has the static Constraint
 * linking the sub-Block, i.e., the N "assignment" ones and the M "one class"
 * ones above, and no Variable and Objective of its own: the objective is the
 * sum of these of the sub-Block.
 *
 * The BlockConfig can select instead the "surrogate" formulation of
 * [Lalonde, Cote, Gendron, INFORMS J. on Computing 34(2), 2022], which has
 * the same optimum and a stronger Lagrangian dual [see
 * generate_abstract_variables()]: a binary variable \f$ t_j \f$ tells
 * whether item j is in some knapsack, and the problem becomes
 * \f[
 *  \max \Bigl\{ \, \sum_{ j = 1 }^N P_j t_j \;:\;
 *  t_j \leq \sum_{ i = 1 }^M x_{ij} \;\; j = 1 , \ldots , N \,,\;
 *  \sum_{ j = 1 }^N W_j t_j \leq \sum_{ i = 1 }^M C_i \,,\;
 *  \sum_{ i = 1 }^M x_{ij} \leq 1 \;\; j = 1 , \ldots , N \,,\;
 *  \sum_{ k = 1 }^R y_{ik} \leq 1 \;\; i = 1 , \ldots , M \,,\;
 *  ( x , y ) \in X \, \Bigr\}
 * \f]
 * (the surrogate knapsack constraint on the t being valid, since each item
 * with \f$ t_j = 1 \f$ is in some knapsack). The profits of the items in
 * the M * R sub-Block are then 0, and one more BinaryKnapsackBlock, the
 * last sub-Block, has the N items with variables \f$ t_j \f$ (in
 * increasing order of j), profits P, weights W and capacity
 * \f$ \sum_i C_i \f$; the MultiKnapsackAssignBlock has the N "select"
 * Constraint \f$ t_j \leq \sum_i x_{ij} \f$ as well. Relaxing all the
 * static Constraint of the MultiKnapsackAssignBlock, the bound is not worse
 * than these of both the original formulation and of the surrogate
 * knapsack alone. */

class MultiKnapsackAssignBlock : public Block
{
/*--------------------------------------------------------------------------*/
/*----------------------- PUBLIC PART OF THE CLASS -------------------------*/
/*--------------------------------------------------------------------------*/

 public:

/*--------------------------------------------------------------------------*/
/*--------------------- CONSTRUCTOR AND DESTRUCTOR -------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Constructor and Destructor
 *  @{ */

 /// constructor of MultiKnapsackAssignBlock, taking a pointer to the father
 /** Constructor of MultiKnapsackAssignBlock. It accepts a pointer to the
  * father Block, which can be of any type, defaulting to nullptr so that
  * this can also be used as the void constructor required by the Block
  * factory. */

 explicit MultiKnapsackAssignBlock( Block * father = nullptr )
  : Block( father ) , f_N( 0 ) , f_R( 0 ) , f_M( 0 ) , AR( 0 ) {}

/*--------------------------------------------------------------------------*/
 /// destructor of MultiKnapsackAssignBlock: deletes the sub-Block

 ~MultiKnapsackAssignBlock() override { guts_of_destructor(); }

/**@} ----------------------------------------------------------------------*/
/*-------------------------- OTHER INITIALIZATIONS -------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Other initializations
 *  @{ */

 /// loads the Multiple Knapsack Assignment instance from memory
 /** Loads the Multiple Knapsack Assignment instance from memory, moving the
  * data from the parameters:
  *
  * - n           is the number of items
  *
  * - r           is the number of classes
  *
  * - m           is the number of knapsacks
  *
  * - Capacities  is the vector of the capacities, of size m
  *
  * - Profits     is the vector of the profits, of size n
  *
  * - Weights     is the vector of the weights, of size n
  *
  * - Classes     is the vector of the classes, of size n, each entry being
  *               in [ 0 , r )
  *
  * Any previous instance, together with its sub-Block and its abstract
  * representation, is discarded. Like load( std::istream & ), if there is
  * any Solver attached to this MultiKnapsackAssignBlock then a
  * NBModification (the "nuclear option") is issued. */

 void load( Index n , Index r , Index m , std::vector< double > && Capacities ,
            std::vector< double > && Profits ,
            std::vector< double > && Weights , Subset && Classes );

/*--------------------------------------------------------------------------*/
 /// loads the MultiKnapsackAssignBlock out of an istream
 /** Loads the MultiKnapsackAssignBlock out of an istream, in the format
  *
  * - the number n of items, the number r of classes and the number m of
  *   knapsacks
  *
  * - the m capacities of the knapsacks
  *
  * - for each item j = 0 , ... , n - 1: its index (which is ignored), its
  *   profit, its weight and its class (in [ 0 , r ))
  *
  * the elements being separated by whitespaces; \p frmt is ignored. Any
  * previous instance is discarded, and if there is any Solver attached to
  * this MultiKnapsackAssignBlock then a NBModification is issued. */

 void load( std::istream & input , char frmt = 0 ) override;

/*--------------------------------------------------------------------------*/
 /// extends Block::deserialize( netCDF::NcGroup )
 /** Extends Block::deserialize( netCDF::NcGroup ) to the specific format of
  * a MultiKnapsackAssignBlock. Besides what is managed by
  * Block::deserialize(), the group must contain:
  *
  * - the dimension "NItems", the number of items;
  *
  * - the dimension "NClasses", the number of classes;
  *
  * - the dimension "NKnapsacks", the number of knapsacks;
  *
  * - the variable "Capacities", of type double and indexed over
  *   "NKnapsacks", the capacities of the knapsacks;
  *
  * - the variable "Profits", of type double and indexed over "NItems", the
  *   profits of the items;
  *
  * - the variable "Weights", of type double and indexed over "NItems", the
  *   weights of the items;
  *
  * - the variable "Classes", of integer type and indexed over "NItems", the
  *   classes of the items, each in [ 0 , NClasses ).
  *
  * Any previous instance is discarded, and the NBModification is issued by
  * Block::deserialize(). */

 void deserialize( const netCDF::NcGroup & group ) override;

/*--------------------------------------------------------------------------*/
 /// chooses the formulation, and generates the Variable of the sub-Block
 /** The MultiKnapsackAssignBlock has no Variable of its own: this generates
  * these of the sub-Block, which the linking Constraint use. The formulation
  * is chosen by the f_value wf of \p stvv, if it is a
  * SimpleConfiguration< int >, else of the f_static_variables_Configuration
  * of the BlockConfig, if it is one, else wf = 0:
  *
  * - wf == 0, the original formulation, with the M * R sub-Block;
  *
  * - wf == 1, the surrogate formulation [see the general notes], which sets
  *   to 0 the profits of the items of the M * R sub-Block and adds the
  *   surrogate BinaryKnapsackBlock as the last sub-Block.
  *
  * The formulation is chosen once: after the first call, the next ones do
  * nothing. */

 void generate_abstract_variables( Configuration * stvv = nullptr ) override;

/*--------------------------------------------------------------------------*/
 /// generates the linking Constraint, and these of the sub-Block
 /** Generates the N "assignment" Constraint and the M "one class"
  * Constraint, which link the sub-Block (whose Variable are generated
  * first, if they are not there yet), and then the Constraint of the
  * sub-Block. The two groups are named "assign" and "class"; the surrogate
  * formulation also has the N Constraint \f$ t_j \leq \sum_i x_{ij} \f$,
  * the group "select". No Modification is issued. */

 void generate_abstract_constraints( Configuration * stcc = nullptr )
  override;

/**@} ----------------------------------------------------------------------*/
/*------- Methods for reading the data of the MultiKnapsackAssignBlock -----*/
/*--------------------------------------------------------------------------*/
/** @name Methods for reading the data of the MultiKnapsackAssignBlock
 *  @{ */

 /// the sense of the Objective, which is to be maximized

 [[nodiscard]] int get_objective_sense( void ) const override final {
  return( Objective::eMax );
  }

/*--------------------------------------------------------------------------*/
 /// the number of items

 [[nodiscard]] Index get_NItems( void ) const { return( f_N ); }

/*--------------------------------------------------------------------------*/
 /// the number of classes

 [[nodiscard]] Index get_NClasses( void ) const { return( f_R ); }

/*--------------------------------------------------------------------------*/
 /// the number of knapsacks

 [[nodiscard]] Index get_NKnapsacks( void ) const { return( f_M ); }

/*--------------------------------------------------------------------------*/
 /// the capacity of the i-th knapsack

 [[nodiscard]] double get_Capacity( Index i ) const {
  if( i >= f_M )
   throw( std::invalid_argument( "MultiKnapsackAssignBlock::get_Capacity: "
				 "invalid knapsack" ) );
  return( v_C[ i ] );
  }

/*--------------------------------------------------------------------------*/
 /// the vector of the capacities

 [[nodiscard]] const std::vector< double > & get_Capacities( void ) const {
  return( v_C );
  }

/*--------------------------------------------------------------------------*/
 /// the weight of the j-th item

 [[nodiscard]] double get_Weight( Index j ) const {
  if( j >= f_N )
   throw( std::invalid_argument(
		      "MultiKnapsackAssignBlock::get_Weight: invalid item" ) );
  return( v_W[ j ] );
  }

/*--------------------------------------------------------------------------*/
 /// the vector of the weights

 [[nodiscard]] const std::vector< double > & get_Weights( void ) const {
  return( v_W );
  }

/*--------------------------------------------------------------------------*/
 /// the profit of the j-th item

 [[nodiscard]] double get_Profit( Index j ) const {
  if( j >= f_N )
   throw( std::invalid_argument(
		      "MultiKnapsackAssignBlock::get_Profit: invalid item" ) );
  return( v_P[ j ] );
  }

/*--------------------------------------------------------------------------*/
 /// the vector of the profits

 [[nodiscard]] const std::vector< double > & get_Profits( void ) const {
  return( v_P );
  }

/*--------------------------------------------------------------------------*/
 /// the class of the j-th item

 [[nodiscard]] Index get_Class( Index j ) const {
  if( j >= f_N )
   throw( std::invalid_argument(
		       "MultiKnapsackAssignBlock::get_Class: invalid item" ) );
  return( v_K[ j ] );
  }

/*--------------------------------------------------------------------------*/
 /// the vector of the classes

 [[nodiscard]] c_Subset & get_Classes( void ) const { return( v_K ); }

/*--------------------------------------------------------------------------*/
 /// the sub-Block of the knapsack i and of the class k

 [[nodiscard]] BinaryKnapsackBlock * get_knapsack( Index i , Index k ) const;

/*--------------------------------------------------------------------------*/
 /// whether the formulation is the surrogate one [see the general notes]

 [[nodiscard]] bool has_surrogate( void ) const { return( AR & HasSrg ); }

/*--------------------------------------------------------------------------*/
 /// the surrogate sub-Block, whose item j is \f$ t_j \f$
 /** The surrogate BinaryKnapsackBlock, the last sub-Block, whose j-th item
  * is \f$ t_j \f$; nullptr if the formulation is the original one. */

 [[nodiscard]] BinaryKnapsackBlock * get_surrogate( void ) const {
  return( ( AR & HasSrg ) ?
	  static_cast< BinaryKnapsackBlock * >( v_Block.back() ) : nullptr );
  }

/**@} ----------------------------------------------------------------------*/
/*-------------------------- Methods for R3 Blocks -------------------------*/
/*--------------------------------------------------------------------------*/
/** @name Methods for R3 Blocks
 *  @{ */

 /// gives an identical copy of the MultiKnapsackAssignBlock
 /** The only R3 Block is the copy, asked with \p r3bc == nullptr: a
  * MultiKnapsackAssignBlock with the same data, \p base if it is given
  * (which must be a MultiKnapsackAssignBlock) and a new one whose father is
  * \p father otherwise, with the same formulation, whose sub-Block are the
  * copies of these of this one [see BinaryKnapsackBlock::get_R3_Block()].
  * If the BlockConfig asks for the surrogate formulation and this has not
  * chosen it yet, this chooses it first [see
  * generate_abstract_variables()]. */

 Block * get_R3_Block( Configuration * r3bc = nullptr ,
                       Block * base = nullptr ,
                       Block * father = nullptr ) override;

/*--------------------------------------------------------------------------*/
 /// writes the solution of a copy into the Variable of this one
 /** \p R3B must be a copy given by get_R3_Block(): the solution of each of
  * its sub-Block is written into the corresponding one of this [see
  * BinaryKnapsackBlock::map_back_solution()]. */

 void map_back_solution( Block * R3B , Configuration * r3bc = nullptr ,
                         Configuration * solc = nullptr ) override;

/*--------------------------------------------------------------------------*/
 /// writes the solution of this into the Variable of a copy
 /** The reverse of map_back_solution(). */

 void map_forward_solution( Block * R3B , Configuration * r3bc = nullptr ,
                            Configuration * solc = nullptr ) override;

/**@} ----------------------------------------------------------------------*/
/*----------------------- Methods for handling Solution --------------------*/
/*--------------------------------------------------------------------------*/
/** @name Methods for handling Solution
 *  @{ */

 /// the value of \f$ x_{ij} \f$, i.e., whether item j is in knapsack i
 /** The value of \f$ x_{ij} \f$ in the solution of the sub-Block, i.e., that
  * of the item j in the sub-Block of the knapsack i and of the class of j
  * (0 if no solution is there). */

 [[nodiscard]] double get_x( Index i , Index j ) const;

/*--------------------------------------------------------------------------*/
 /// the value of \f$ y_{ik} \f$, i.e., whether knapsack i is given class k

 [[nodiscard]] double get_y( Index i , Index k ) const;

/*--------------------------------------------------------------------------*/
 /// the value of \f$ t_j \f$, i.e., whether item j is in some knapsack
 /** The value of \f$ t_j \f$ in the solution of the surrogate sub-Block;
  * in the original formulation, \f$ \sum_i x_{ij} \f$. */

 [[nodiscard]] double get_t( Index j ) const;

/**@} ----------------------------------------------------------------------*/
/*--- METHODS FOR LOADING, PRINTING & SAVING THE MultiKnapsackAssignBlock --*/
/*--------------------------------------------------------------------------*/
/** @name Methods for loading, printing & saving the MultiKnapsackAssignBlock
 *  @{ */

 /// extends Block::serialize( netCDF::NcGroup )
 /** Extends Block::serialize( netCDF::NcGroup ) to the specific format of a
  * MultiKnapsackAssignBlock [see deserialize( netCDF::NcGroup )]. */

 void serialize( netCDF::NcGroup & group ) const override;

/*--------------------------------------------------------------------------*/
 /// prints the data of the MultiKnapsackAssignBlock on an ostream

 void print( std::ostream & output , char vlvl = 0 ) const override;

/** @} ---------------------------------------------------------------------*/
/*-------------------- PROTECTED PART OF THE CLASS -------------------------*/
/*--------------------------------------------------------------------------*/

 protected:

/*--------------------------------------------------------------------------*/
/*---------------------------- PROTECTED FIELDS ----------------------------*/
/*--------------------------------------------------------------------------*/

 Index f_N;                   ///< the number of items
 Index f_R;                   ///< the number of classes
 Index f_M;                   ///< the number of knapsacks

 std::vector< double > v_C;   ///< the capacities
 std::vector< double > v_P;   ///< the profits
 std::vector< double > v_W;   ///< the weights
 Subset v_K;                  ///< the classes

 /// the items of each class, in increasing order
 std::vector< Subset > v_Sk;

 /// the position of each item among these of its class
 Subset v_pos;

 unsigned char AR;            ///< bit-wise coded: what abstract is there

 static constexpr unsigned char HasVar = 1;
 ///< first bit of AR == 1 if the Variable have been constructed

 static constexpr unsigned char HasCns = 2;
 ///< second bit of AR == 1 if the Constraint have been constructed

 static constexpr unsigned char HasSrg = 4;
 ///< third bit of AR == 1 if the formulation is the surrogate one

 /// the assignment Constraint: each item in at most one knapsack
 std::vector< FRowConstraint > v_assign;

 /// the class Constraint: each knapsack is given at most one class
 std::vector< FRowConstraint > v_class;

 /// the select Constraint of the surrogate formulation: t_j <= sum_i x_ij
 std::vector< FRowConstraint > v_select;

/*--------------------------------------------------------------------------*/
/*--------------------- PRIVATE PART OF THE CLASS --------------------------*/
/*--------------------------------------------------------------------------*/

 private:

/*--------------------------------------------------------------------------*/
/*-------------------------- PRIVATE METHODS -------------------------------*/
/*--------------------------------------------------------------------------*/

 /// discards the instance, its sub-Block and its abstract representation

 void guts_of_destructor( void );

/*--------------------------------------------------------------------------*/
 /// checks the data, then builds v_Sk, v_pos and the sub-Block

 void build( const std::string & prfx );

/*--------------------------------------------------------------------------*/
 /// the formulation that \p stvv, or else the BlockConfig, asks for

 [[nodiscard]] int asked_formulation( Configuration * stvv ) const;

/*--------------------------------------------------------------------------*/
 /// turns the original formulation into the surrogate one, if it is not yet

 void add_surrogate( void );

/*--------------------------------------------------------------------------*/
/*---------------------------- PRIVATE FIELDS ------------------------------*/
/*--------------------------------------------------------------------------*/

 SMSpp_insert_in_factory_h;  // insert it in the Block factory

/*--------------------------------------------------------------------------*/

 };  // end( class( MultiKnapsackAssignBlock ) )

/** @} end( group( MultiKnapsackAssignBlock_CLASSES ) ) */

/*--------------------------------------------------------------------------*/

}  // end( namespace SMSpp_di_unipi_it )

#endif  /* MultiKnapsackAssignBlock.h included */

/*--------------------------------------------------------------------------*/
/*------------------ End File MultiKnapsackAssignBlock.h -------------------*/
/*--------------------------------------------------------------------------*/
