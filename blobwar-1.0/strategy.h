#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "common.h"
#include "bidiarray.h"
#include "move.h"
#include <limits>



class Strategy {

private:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;
    
    //number of P1's blobs
    Sint32 nb_blob1;
    //number of P2's blobs
    Sint32 nb_blob2;
    
	
    
    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(move&);
    
    Sint32 nb_blobs_adv();

public:
        // Constructor from a current situation
    Strategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(move&))
            : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove)
    {
	nb_blob1 = 0;
	nb_blob2 = 0;
	alpha = std::numeric_limits<Sint32>::max();
	for(int i = 0 ; i < 8 ; i++){
	    for(int j = 0 ; j < 8 ; j++){
		if(_blobs.get(i,j) == 0)
		    nb_blob1++;
		if(_blobs.get(i,j) == 1)
		    nb_blob2++;
	    }
	}
    }
    
              
    
        // Copy constructor
    Strategy (const Strategy& St)
	: _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player), nb_blob1(St.nb_blob1), nb_blob2(St.nb_blob2), alpha(St.alpha)  
        {}
    
        // Destructor
    ~Strategy() {}
    
        /** 
         * Apply a move to the current state of blobs
         * Assumes that the move is valid
         */
    void applyMove (const move& mv);

        /**
         * Compute the vector containing every possible moves
         */
    vector<move>& computeValidMoves (vector<move>& valid_moves) const;

        /**
         * Estimate the score of the current state of the game
         */
    Sint32 estimateCurrentScore () const;

        /**
         * Find the best move.
         */
    void computeBestMove ();
    
    void incrBlob(Uint16 player);
    
    void decrBlob(Uint16 player);

    move& findMoveMinMax(move& mv, int profondeur);

    void change_current_player();

    move& findMoveAlphaBeta(move& mv, int profondeur);
    
    Sint32 alpha ;
};

#endif
