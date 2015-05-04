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

void Strategy::apply_relative_move (Uint16 player, const move& mv){
    //first check if we need to create a new blob or to move an old one
    //On pourrait utiliser la valeur absolue
    if (((mv.ox - mv.nx)*(mv.ox - mv.nx)<=1)&&((mv.oy - mv.ny)*(mv.oy - mv.ny)<=1)) {
        //it's a copy 
        _blobs.set(mv.nx, mv.ny, player);
        incrBlob( player);
    }
    else{
        //it's a move
        _blobs.set(mv.ox, mv.oy, -1);
        _blobs.set(mv.nx, mv.ny, player);
    }

    for(Sint8 i = -1 ; i < 2 ; i++)
        for(Sint8 j = -1 ; j < 2 ; j++) {
            if (mv.nx+i < 0) continue;
            if (mv.nx+i > 7) continue;
            if (mv.ny+j < 0) continue;
            if (mv.ny+j > 7) continue;
            if ((_blobs.get(mv.nx+i, mv.ny+j)!=-1)&&(_blobs.get(mv.nx+i, mv.ny+j)!=player)) {
                _blobs.set(mv.nx+i, mv.ny+j, player);
                incrBlob(player);
                if(player == 0) {
                    decrBlob(player + 1);
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


vector<move>& Strategy::compute_relative_valid_moves (Sint16 player, vector<move>& valid_moves) const {

    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) player) {
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

Sint32 Strategy::alpha_beta(int prof, Uint16 tour,Sint32 alpha, Sint32 beta){
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

        bool no_valid_moves = true;
        move mv(0,0,0,0);
        //iterate on starting position
        for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
            for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
                if (_blobs.get(mv.ox, mv.oy) == (int) tour) {
                    //iterate on possible destinations
                    for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                        for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                            if (_holes.get(mv.nx, mv.ny)) continue;
                            if (_blobs.get(mv.nx, mv.ny) == -1){
                                no_valid_moves = false;
                                Sint32 curr_score;

                                Strategy foresee(*this);
                                foresee.apply_relative_move(tour, mv);
                                curr_score = foresee.alpha_beta(prof-1, (tour+1)%2,alpha,beta);

                                if(better_score(curr_score, best_score)){
                                    best_score = curr_score;
                                    if(tour == _current_player){
                                        if(curr_score > alpha)
                                            alpha = curr_score;
                                        if(alpha >= beta)
                                            return curr_score;
                                    }
                                    else{
                                        if(curr_score < beta)
                                            beta = curr_score;
                                        if(alpha >= beta)
                                            return curr_score;
                                    }
                                }

                            }
                        }
                    }
                }
            }
        }
        if(no_valid_moves){
            if ((tour == _current_player && nb_blobs(_current_player) == 0) || 
                    (tour != _current_player && nb_blobs((_current_player+1)%2) == 0)) {
                return best_score;
            } else {
                Strategy foresee(*this);
                return foresee.alpha_beta(prof-1, (tour+1)%2,alpha,beta);
            }
        }
        return best_score;
    }
}



move& Strategy::findMoveAlphaBeta(move& best_mv, int prof){

    Sint16 tour = _current_player;
    bool (*better_score)(int, int) = &sup;
    Sint32 best_score = numeric_limits<Sint32>::min();
    Sint32 alpha = numeric_limits<Sint32>::min();
    Sint32 beta = numeric_limits<Sint32>::max();

    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) tour) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1){
                            Sint32 curr_score;

                            Strategy foresee(*this);
                            foresee.apply_relative_move(tour, mv);
                            curr_score = foresee.alpha_beta(prof-1, (tour+1)%2,alpha,beta);
                            if(better_score(curr_score, best_score)){
                                best_score = curr_score;
                                best_mv = mv;
                            }
                        }
                    }
                }
            }
        }
    }
    return best_mv;



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
        this->compute_relative_valid_moves(tour, valid_moves);

        if(valid_moves.empty()){
            if ((tour == _current_player && nb_blobs(_current_player) == 0) || 
                    (tour != _current_player && nb_blobs((_current_player+1)%2) == 0)) {
                return best_score;
            } else {
                Strategy foresee(*this);
                return foresee.min_max(prof-1, (tour+1)%2);
            }
        }

        #pragma omp parallel for
        for(vector<move>::iterator curr_move = valid_moves.begin() ; curr_move != valid_moves.end() ; ++curr_move){
            Sint32 curr_score;

            Strategy foresee(*this);
            foresee.apply_relative_move(tour, *curr_move);
            curr_score = foresee.min_max(prof-1, (tour+1)%2);
            #pragma omp critical(update_best_score)
            {
                if(better_score(curr_score, best_score)){
                    best_score = curr_score;
                }
            }
        }

        return best_score;

    }

}

move& Strategy::findMoveMinMax(move& mv, int prof){
    Sint16 tour = _current_player;
    bool (*better_score)(int, int) = &sup;
    Sint32 best_score = numeric_limits<Sint32>::min();

    vector<move> valid_moves;
    this->compute_relative_valid_moves(tour, valid_moves);

    #pragma omp parallel for
    for(vector<move>::iterator curr_move = valid_moves.begin() ; curr_move != valid_moves.end() ; ++curr_move){
        Sint32 curr_score;

        Strategy foresee(*this);
        foresee.apply_relative_move(tour, *curr_move);
        curr_score = foresee.min_max(prof-1, (tour+1)%2);
        #pragma omp critical(update_best_score_root)
        {
            if(better_score(curr_score, best_score)){
                best_score = curr_score;
                mv = *curr_move;
            }
        }
    }

    return mv;

}


void Strategy::computeBestMove () {
    move best_mv;
    int profondeur = 1;
    while(true){
        _saveBestMove(findMoveAlphaBeta(best_mv,profondeur));
        profondeur++;
    }
}

