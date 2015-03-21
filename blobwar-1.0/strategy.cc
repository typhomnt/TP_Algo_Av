#include "strategy.h"
void Strategy::applyMove (const move& mv) {
    //first check if we need to create a new blob or to move an old one
    //On pourrait utiliser la valeur absolue
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
	nb_blobs1++;
    }
    else{
	nb_blobs2++;
    }
}

void Strategy::decrBlob(Uint16 player){
    if(player == 0){
	nb_blobs1--;
    }
    else{
	nb_blobs2--;
    }
}

Sint32 Strategy::estimateCurrentScore () const {
    if(_current_player == 0)
	return nb_blobs1 - nb_blobs2 ;
    else
	return nb_blobs2 - nb_blobs1;
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

Sint32 Strategy::nb_blobs_adv(){
    if(_current_player == 0)
        return nb_blobs2;
    else
        return nb_blobs1;
}


move& Strategy::findMoveAlphaBeta(move& mv, int profondeur){
    Sint32 score;
    Sint32 forseenScore ;
    vector<move> valid_moves;
    move best_mv_en;
    this->computeValidMoves(valid_moves);
    if(profondeur <= 0){
	score = std::numeric_limits<Sint32>::min();
	for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
	    Strategy foresee(*this);
	    foresee.applyMove(*it);
	    forseenScore = foresee.estimateCurrentScore();
	    if(this->alpha <= forseenScore){
		mv = *it ;
		return mv;
	    }
	    if(forseenScore > score){
		score = forseenScore;
		this->alpha = score;
		mv = *it;
	    }
	}
	return mv;
    }
    else {
	score =  std::numeric_limits<Sint32>::max();
	for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
	    Strategy foresee(*this);
	    foresee.applyMove(*it);
	    foresee.change_current_player();
	    foresee.applyMove(foresee.findMoveAlphaBeta(best_mv_en, profondeur - 1));
	    if(this->alpha < foresee.alpha)
		break;
	    this->alpha = foresee.alpha;
	    forseenScore = foresee.estimateCurrentScore();
	    if(forseenScore < score){
		score = forseenScore;
		mv = *it;
	    }
	}
	return mv;
    }
}
    
bool inf(int x, int y){
    return x < y;
}

bool sup(int x, int y){
    return x > y;
}

Sint32 Strategy::nb_blobs(Uint16 player){
    if(player == 0){
        return nb_blobs1;
    } else {
        return nb_blobs2;
    }
}

Sint32 Strategy::min_max(int prof, Uint16 tour){
    bool (*better_score)(int, int);
    Sint32 best_score;
    if (tour == _current_player){
        better_score = &sup;
        best_score = numeric_limits<Sint32>::min();
    } else {
        better_score = &inf;
        best_score = numeric_limits<Sint32>::max();
    }

    if(prof == 0){
        if ((tour == _current_player && nb_blobs(_current_player) == 0) || 
                (tour != _current_player && nb_blobs((_current_player+1)%2) == 0)) {
            return best_score;
        }

        return estimateCurrentScore();

    } else {
        vector<move> valid_moves;
        this->computeValidMoves(valid_moves);

        if(valid_moves.empty()){
            if ((tour == _current_player && nb_blobs1 == 0) || 
                (tour != _current_player && nb_blobs2 == 0)) {
                return best_score;
            } else {
                return this->min_max(prof-1, (tour+1)%2);
            }
        }

        for(vector<move>::iterator curr_move = valid_moves.begin() ; curr_move != valid_moves.end() ; ++curr_move){
            Sint32 curr_score;

            Strategy foresee(*this);
            foresee.applyMove(*curr_move);
            curr_score = foresee.min_max(prof-1, (tour+1)%2);
            if(better_score(curr_score, best_score)){
                best_score = curr_score;
            }
        }

        return best_score;

    }

}

std::list< std::vector<int> > Strategy::apply_move_saving_mods(move& mv){
    std::list< std::vector<int> > l;
    return l;
}

move& Strategy::findMoveMinMax(move& mv, int prof){
    Sint16 tour = _current_player;
    bool (*better_score)(int, int) = &sup;
    Sint32 best_score = numeric_limits<Sint32>::min();

    vector<move> valid_moves;
    this->computeValidMoves(valid_moves);

    for(vector<move>::iterator curr_move = valid_moves.begin() ; curr_move != valid_moves.end() ; ++curr_move){
        Sint32 curr_score;

        Strategy foresee(*this);
        foresee.applyMove(*curr_move);
        curr_score = foresee.min_max(prof-1, (tour+1)%2);
        if(better_score(curr_score, best_score)){
            best_score = curr_score;
            mv = *curr_move;
        }
    }

    return mv;



}

/*
move& Strategy::findMoveMinMax(move& mv, int profondeur){
    Sint32 score;
    Sint32 forseenScore ;
    Sint32 nb_blobs_opponent;
    vector<move> valid_moves;
    move best_mv_en;
    this->computeValidMoves(valid_moves);
    //std::cout << "Profondeur : " << profondeur << " Nbr coups valides " << valid_moves.size() << "joueur: "  << _current_player <<std::endl ;
    if(profondeur <= 0){
        score = std::numeric_limits<Sint32>::min();
        nb_blobs_opponent =  std::numeric_limits<Sint32>::max();
        for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
            Strategy foresee(*this);
            foresee.applyMove(*it);
            forseenScore = foresee.estimateCurrentScore();
            if(forseenScore > score || (forseenScore == score && foresee.nb_blobs_adv() < nb_blobs_opponent)){
                score = forseenScore;
                nb_blobs_opponent = foresee.nb_blobs_adv();
                mv = *it;
            }
            //std::cout << "Score : " << score << "joueur: "  << _current_player <<std::endl ; 
        }
    //std::cout << "Score final : " << score << "joueur: "  << _current_player <<std::endl ; 
    return mv;
    }
    else {
        score =  std::numeric_limits<Sint32>::max();
        nb_blobs_opponent =  std::numeric_limits<Sint32>::min();
        for(vector<move>::iterator it = valid_moves.begin() ; it != valid_moves.end() ; ++it){
            Strategy foresee(*this);
            foresee.applyMove(*it);
            foresee.change_current_player();
            foresee.applyMove(foresee.findMoveMinMax(best_mv_en, profondeur - 1));
            forseenScore = foresee.estimateCurrentScore();
            //std::cout << "Score en : " << forseenScore << "joueur: "  << _current_player <<std::endl ; 
            if(forseenScore < score || (forseenScore == score && foresee.nb_blobs_adv() > nb_blobs_opponent)){
                score = forseenScore;
                nb_blobs_opponent = foresee.nb_blobs_adv();
                //std::cout << "Score origin : " << score << "joueur: "  << _current_player <<std::endl ; 
                mv = *it;
            }
        }
        //std::cout << "Score final origin : " << score << "joueur: "  << _current_player <<std::endl ; 
        return mv;
    }

}
*/

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
    int profondeur = 1;
    while(true){
	std::cout << "Profondeur : " << profondeur << std::endl ;
	_saveBestMove(findMoveMinMax(best_mv,profondeur));
	profondeur++;
    }
}

