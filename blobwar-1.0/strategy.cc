#include "strategy.h"



void Strategy::applyMove (const move& mv) {
    _blobs.set(mv.ox, mv.oy, -1);
    _blobs.set(mv.nx, mv.ny, _current_player);
}

Sint32 Strategy::estimateCurrentScore () const {
    return 0;
    if(_current_player == 0)
	return nb_blob1 - nb_blob2 ;
    else
	return nb_blob2 - nb_blob1;
}

vector<move>& Strategy::computeValidMoves (vector<move>& valid_moves) const {
    return valid_moves;
}

void Strategy::computeBestMove () {
    // To be improved...
    std::cout << estimateCurrentScore();
    //The following code find a valid move.
    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
}

