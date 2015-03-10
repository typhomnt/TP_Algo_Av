#include "strategy.h"
#include <limits>

void Strategy::applyMove (const move& mv) {
    //first check if we need to create a new blob or to move an old one
    if (((mv.ox - mv.nx)*(mv.ox - mv.nx)<=1)&&((mv.oy - mv.ny)*(mv.oy - mv.ny)<=1)) {
	//it's a copy 
	_blobs.set(mv.nx, mv.ny, _current_player);
	incrBlob( _current_player);
    }
    else{
	//it's a move
    _blobs.set(mv.ox, mv.oy, -1);
    _blobs.set(mv.nx, mv.ny, _current_player);
    }
    
    for(Sint8 i = -1 ; i < 2 ; i++)
	for(Sint8 j = -1 ; j < 2 ; j++) {
	    if (mv.nx+i < 0) continue;
	    if (mv.nx+i > 7) continue;
	    if (mv.ny+j < 0) continue;
	    if (mv.ny+j > 7) continue;
	    if ((_blobs.get(mv.nx+i, mv.ny+j)!=-1)&&(_blobs.get(mv.nx+i, mv.ny+j)!=_current_player)) {
		_blobs.set(mv.nx+i, mv.ny+j, _current_player);
		incrBlob(_current_player);
		if(_current_player == 0) {
		    decrBlob(_current_player + 1);
		} 
		else {
		    decrBlob(0);
		}
	    }
	}

}

void Strategy::incrBlob(Uint16 player){
    if(player == 0){
	nb_blob1++;
    }
    else{
	nb_blob2++;
    }
}

void Strategy::decrBlob(Uint16 player){
    if(player == 0){
	nb_blob1--;
    }
    else{
	nb_blob2--;
    }
}

Sint32 Strategy::estimateCurrentScore () const {
    if(_current_player == 0)
	return nb_blob1 - nb_blob2 ;
    else
	return nb_blob2 - nb_blob1;
}

vector<move>& Strategy::computeValidMoves (vector<move>& valid_moves) const {
    
    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) valid_moves.push_back(mv);
                    }
                }
            }
        }
    }
    return valid_moves;
}


void Strategy::change_current_player(){
    if(_current_player == 0){
	_current_player = 1;
    }
    else {
	_current_player = 0;
    }
}

void Strategy::change_enemy(){
    if(enemy){
	enemy = false;
    }
    else{
	enemy = true;
    }
}

move& Strategy::findMoveMinMax(move& mv, int profondeur){
    Sint32 score;
    Sint32 forseenScore ;
    vector<move> valid_moves;
    move best_mv_en;
    this->computeValidMoves(valid_moves);
    std::cout << "Profondeur : " << profondeur << " Nbr coups valides " << valid_moves.size() << "joueur: "  << _current_player <<std::endl ;
    if(profondeur <= 0){
	score = std::numeric_limits<Sint32>::min();
	for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
	    Strategy foresee(*this);
	    foresee.applyMove(*it);
	    forseenScore = foresee.estimateCurrentScore();
	    if(forseenScore > score){
		    score = forseenScore;
		    mv = *it;
		    std::cout << "Score : " << score << "joueur: "  << _current_player <<std::endl ; 
	    }
	    /*if(!enemy){
		if(forseenScore > score){
		    score = forseenScore;
		    mv = *it;
		}
	    }
	    else {
		if(forseenScore < score){
		    score = forseenScore;
		    mv = *it;
		}
		}*/
	}
	std::cout << "Score final : " << score << "joueur: "  << _current_player <<std::endl ; 
	return mv;
    }
    else {
	/*if(!enemy){
	    score = std::numeric_limits<Sint32>::max();
	}
	else{
	    score =  std::numeric_limits<Sint32>::max();
	}*/
	score =  std::numeric_limits<Sint32>::max();
	for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
	    Strategy foresee(*this);
	    foresee.applyMove(*it);
	    foresee.change_current_player();
	    foresee.change_enemy();
	    Strategy compute(foresee);
	    //compute.change_current_player();
	    //compute.change_enemy();
	    compute.applyMove(foresee.findMoveMinMax(best_mv_en, profondeur - 1));
	    forseenScore = compute.estimateCurrentScore();
	    std::cout << "Score en : " << forseenScore << "joueur: "  << _current_player <<std::endl ; 
	    if(forseenScore < score){
		score = forseenScore;
		std::cout << "Score origin : " << score << "joueur: "  << _current_player <<std::endl ; 
		mv = *it;
	    }
		/*if(!enemy){
		if(forseenScore < score){
		    score = forseenScore;
		    mv = *it;
		}
	    }
	    else {
		if(forseenScore < score){
		    score = forseenScore;
		    mv = *it;
		}
		}*/
	}
	std::cout << "Score final origin : " << score << "joueur: "  << _current_player <<std::endl ; 
	return mv;
    }
    
}

void Strategy::computeBestMove () {
    /* //The following code find a valid move.
    Sint32 score = this->estimateCurrentScore();
    Sint32 forseenScore ;
    vector<move> valid_moves;
    this->computeValidMoves(valid_moves);
    move best_mv;
    //_saveBestMove(*valid_moves.begin());
    for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
	//surchager =
	Strategy foresee(*this);
	foresee.applyMove(*it);
	forseenScore = foresee.estimateCurrentScore();
	if(forseenScore > score){
	    score = forseenScore;
	    best_mv = *it;
	}	
    }
    _saveBestMove(best_mv);*/
    move best_mv;
    _saveBestMove(findMoveMinMax(best_mv,0));
}

